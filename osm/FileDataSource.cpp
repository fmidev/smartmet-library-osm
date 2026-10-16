#include "FileDataSource.h"

#include "CountryInfo.h"
#include "FeatureCodes.h"

#include <macgyver/Exception.h>

#include <algorithm>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <optional>
#include <set>
#include <sstream>
#include <sys/stat.h>
#include <utility>

namespace fs = std::filesystem;

namespace SmartMet
{
namespace Osm
{
namespace
{
constexpr const char* kSuffix = "-places.tsv";

std::vector<std::string> splitTabs(const std::string& line)
{
  std::vector<std::string> out;
  std::string field;
  for (char c : line)
  {
    if (c == '\t')
    {
      out.push_back(std::move(field));
      field.clear();
    }
    else
    {
      field.push_back(c);
    }
  }
  out.push_back(std::move(field));
  return out;
}

int parseIntOrEmpty(const std::string& s, int fallback)
{
  if (s.empty())
    return fallback;
  try
  {
    return std::stoi(s);
  }
  catch (...)
  {
    return fallback;
  }
}

double parseDoubleOrThrow(const std::string& s, const char* what)
{
  try
  {
    return std::stod(s);
  }
  catch (...)
  {
    throw Fmi::Exception(BCP, "FileDataSource: bad numeric value")
        .addParameter("field", what)
        .addParameter("value", s);
  }
}
// The column indices of a place file, -1 for missing columns
struct PlaceColumns
{
  int osmId;
  int place;
  int name;
  int iso2;
  int lon;
  int lat;
  int pop;
  int tz;
  int adm;
};

PlaceColumns findPlaceColumns(const std::vector<std::string>& cols)
{
  auto colIndex = [&](const char* name) -> int
  {
    for (std::size_t i = 0; i < cols.size(); ++i)
      if (cols[i] == name)
        return static_cast<int>(i);
    return -1;
  };

  PlaceColumns c{};
  c.osmId = colIndex("osm_id");
  c.place = colIndex("place");
  c.name = colIndex("name");
  c.iso2 = colIndex("iso2");
  c.lon = colIndex("lon");
  c.lat = colIndex("lat");
  c.pop = colIndex("population");
  c.tz = colIndex("timezone");
  c.adm = colIndex("admin1");
  return c;
}

// Parse a place from the fields of a line. Returns nothing if the place is not accepted.
std::optional<Place> parsePlace(const std::vector<std::string>& f, const PlaceColumns& c, long id)
{
  const int nf = static_cast<int>(f.size());

  Place p;
  p.id = id;
  p.name = f[c.name];
  p.iso2 = f[c.iso2];
  const std::string& placeTag = f[c.place];
  p.feature = featureCodeForPlaceTag(placeTag);
  if (p.feature.empty())
    return {};
  p.lon = parseDoubleOrThrow(f[c.lon], "lon");
  p.lat = parseDoubleOrThrow(f[c.lat], "lat");
  p.population = (c.pop >= 0 && nf > c.pop)
                     ? parseIntOrEmpty(f[c.pop], defaultPopulationForPlaceTag(placeTag))
                     : defaultPopulationForPlaceTag(placeTag);
  p.timezone = (c.tz >= 0 && nf > c.tz && !f[c.tz].empty()) ? f[c.tz]
                                                            : CountryInfo::defaultTimezone(p.iso2);
  p.admin1 = (c.adm >= 0 && nf > c.adm) ? f[c.adm] : std::string{};
  // We swallow osm_id; only the synthetic id is exposed.

  if (p.name.empty() || p.iso2.empty())
    return {};

  return p;
}

}  // namespace

FileDataSource::FileDataSource(std::string directory) : itsDirectory(std::move(directory))
{
  if (!fs::is_directory(itsDirectory))
    throw Fmi::Exception(BCP, "FileDataSource: not a directory")
        .addParameter("path", itsDirectory);
}

std::vector<std::string> FileDataSource::tsvFiles() const
{
  std::vector<std::string> files;
  for (const auto& entry : fs::directory_iterator(itsDirectory))
  {
    if (!entry.is_regular_file())
      continue;
    const auto name = entry.path().filename().string();
    const std::string suffix = kSuffix;
    if (name.size() >= suffix.size() &&
        name.compare(name.size() - suffix.size(), suffix.size(), suffix) == 0)
    {
      files.push_back(entry.path().string());
    }
  }
  std::sort(files.begin(), files.end());
  return files;
}

Places FileDataSource::readPlaces()
{
  Places out;
  long nextId = 1;

  for (const auto& path : tsvFiles())
  {
    std::ifstream in(path);
    if (!in)
      throw Fmi::Exception(BCP, "FileDataSource: cannot open").addParameter("path", path);

    std::string header;
    if (!std::getline(in, header))
      continue;
    const PlaceColumns c = findPlaceColumns(splitTabs(header));

    if (c.name < 0 || c.iso2 < 0 || c.lon < 0 || c.lat < 0 || c.place < 0)
      throw Fmi::Exception(BCP, "FileDataSource: missing required columns")
          .addParameter("path", path);

    std::string line;
    while (std::getline(in, line))
    {
      if (line.empty())
        continue;
      auto f = splitTabs(line);
      if (static_cast<int>(f.size()) <= c.lat)
        continue;

      auto p = parsePlace(f, c, nextId++);
      if (p)
        out.push_back(std::move(*p));
    }
  }
  return out;
}

Countries FileDataSource::readCountries()
{
  // Derive the country list from the iso2 codes that appear in the loaded
  // places. Names come from the built-in CountryInfo table.
  std::set<std::string> codes;
  for (const auto& path : tsvFiles())
  {
    std::ifstream in(path);
    if (!in)
      continue;
    std::string header;
    std::getline(in, header);
    auto cols = splitTabs(header);
    int iIso2 = -1;
    for (std::size_t i = 0; i < cols.size(); ++i)
      if (cols[i] == "iso2")
        iIso2 = static_cast<int>(i);
    if (iIso2 < 0)
      continue;
    std::string line;
    while (std::getline(in, line))
    {
      auto f = splitTabs(line);
      if (static_cast<int>(f.size()) > iIso2 && !f[iIso2].empty())
        codes.insert(f[iIso2]);
    }
  }
  Countries out;
  out.reserve(codes.size());
  for (const auto& iso2 : codes)
    out.push_back({iso2, CountryInfo::countryName(iso2)});
  return out;
}

std::string FileDataSource::dataHash() const
{
  // Combine each file's path, size, and mtime. The engine only needs this
  // value to be stable when nothing has changed, and to differ otherwise.
  std::ostringstream s;
  for (const auto& path : tsvFiles())
  {
    struct stat st {};
    if (::stat(path.c_str(), &st) == 0)
    {
      s << path << ':' << st.st_size << ':' << st.st_mtime << ';';
    }
    else
    {
      s << path << ":missing;";
    }
  }
  return s.str();
}

}  // namespace Osm
}  // namespace SmartMet
