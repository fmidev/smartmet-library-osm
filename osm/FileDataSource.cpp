#include "FileDataSource.h"

#include "CountryInfo.h"
#include "FeatureCodes.h"

#include <macgyver/Exception.h>

#include <algorithm>
#include <cstdlib>
#include <filesystem>
#include <fstream>
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
    auto cols = splitTabs(header);

    auto colIndex = [&](const char* name) -> int
    {
      for (std::size_t i = 0; i < cols.size(); ++i)
        if (cols[i] == name)
          return static_cast<int>(i);
      return -1;
    };

    const int iOsmId = colIndex("osm_id");
    const int iPlace = colIndex("place");
    const int iName = colIndex("name");
    const int iIso2 = colIndex("iso2");
    const int iLon = colIndex("lon");
    const int iLat = colIndex("lat");
    const int iPop = colIndex("population");
    const int iTz = colIndex("timezone");
    const int iAdm = colIndex("admin1");

    if (iName < 0 || iIso2 < 0 || iLon < 0 || iLat < 0 || iPlace < 0)
      throw Fmi::Exception(BCP, "FileDataSource: missing required columns")
          .addParameter("path", path);

    std::string line;
    while (std::getline(in, line))
    {
      if (line.empty())
        continue;
      auto f = splitTabs(line);
      if (static_cast<int>(f.size()) <= iLat)
        continue;

      Place p;
      p.id = nextId++;
      p.name = f[iName];
      p.iso2 = f[iIso2];
      const std::string placeTag = f[iPlace];
      p.feature = featureCodeForPlaceTag(placeTag);
      if (p.feature.empty())
        continue;
      p.lon = parseDoubleOrThrow(f[iLon], "lon");
      p.lat = parseDoubleOrThrow(f[iLat], "lat");
      p.population = (iPop >= 0 && static_cast<int>(f.size()) > iPop)
                         ? parseIntOrEmpty(f[iPop], defaultPopulationForPlaceTag(placeTag))
                         : defaultPopulationForPlaceTag(placeTag);
      p.timezone = (iTz >= 0 && static_cast<int>(f.size()) > iTz && !f[iTz].empty())
                       ? f[iTz]
                       : CountryInfo::defaultTimezone(p.iso2);
      p.admin1 = (iAdm >= 0 && static_cast<int>(f.size()) > iAdm) ? f[iAdm] : std::string{};
      // We swallow osm_id; only the synthetic id is exposed.
      (void)iOsmId;

      if (p.name.empty() || p.iso2.empty())
        continue;

      out.push_back(std::move(p));
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
