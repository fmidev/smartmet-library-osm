// ======================================================================
/*!
 * \brief Plain data types for OpenStreetMap-derived places.
 */
// ======================================================================

#pragma once

#include <map>
#include <string>
#include <vector>

namespace SmartMet
{
namespace Osm
{
struct Place
{
  long id = 0;
  std::string name;
  std::string iso2;
  std::string feature;
  double lon = 0.0;
  double lat = 0.0;
  int population = -1;
  std::string timezone;
  std::string admin1;
};

using Places = std::vector<Place>;

struct Country
{
  std::string iso2;
  std::string name;
};

using Countries = std::vector<Country>;

using KeywordMap = std::map<std::string, std::vector<long>>;

}  // namespace Osm
}  // namespace SmartMet
