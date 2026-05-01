#include "Keywords.h"

namespace SmartMet
{
namespace Osm
{
namespace
{
bool isCityFeature(const std::string& f)
{
  return f == "PPLC" || f == "PPLA" || f == "PPL";
}

bool isPopulatedFeature(const std::string& f)
{
  return f == "PPLC" || f == "PPLA" || f == "PPL" || f == "PPLA2" || f == "PPLA3" ||
         f == "PPLA4" || f == "PPLX";
}
}  // namespace

KeywordMap defaultKeywords(const Places& places)
{
  KeywordMap kw;
  auto& cities = kw["cities"];
  auto& populated = kw["populated"];
  cities.reserve(places.size() / 4);
  populated.reserve(places.size());

  for (const auto& p : places)
  {
    if (isPopulatedFeature(p.feature))
      populated.push_back(p.id);
    if (isCityFeature(p.feature))
      cities.push_back(p.id);
  }
  return kw;
}

}  // namespace Osm
}  // namespace SmartMet
