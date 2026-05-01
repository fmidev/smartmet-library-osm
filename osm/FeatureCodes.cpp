#include "FeatureCodes.h"

namespace SmartMet
{
namespace Osm
{
std::string featureCodeForPlaceTag(const std::string& placeTag)
{
  if (placeTag == "city")
    return "PPLA";
  if (placeTag == "town")
    return "PPL";
  if (placeTag == "village")
    return "PPLA4";
  if (placeTag == "hamlet")
    return "PPLX";
  if (placeTag == "suburb")
    return "PPLX";
  if (placeTag == "municipality")
    return "ADM3";
  return {};
}

int defaultPopulationForPlaceTag(const std::string& placeTag)
{
  if (placeTag == "city")
    return 50000;
  if (placeTag == "town")
    return 5000;
  if (placeTag == "village")
    return 500;
  if (placeTag == "hamlet")
    return 50;
  return -1;
}

}  // namespace Osm
}  // namespace SmartMet
