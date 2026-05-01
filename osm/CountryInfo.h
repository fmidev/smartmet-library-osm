// ======================================================================
/*!
 * \brief Built-in iso2 -> country-name and iso2 -> default-timezone tables.
 *
 * OSM nodes don't carry timezone tags, and a country table isn't part of the
 * dump. Both are looked up from compile-time tables here. Multi-timezone
 * countries (US, RU, CA, AU, BR, ...) get a representative default; for
 * those a polygon-based timezone lookup will need to replace this — see the
 * TODO at the top of the .cpp file.
 */
// ======================================================================

#pragma once

#include <string>

namespace SmartMet
{
namespace Osm
{
namespace CountryInfo
{
// Returns English country name for the given ISO 3166-1 alpha-2 code, or
// the iso2 code itself if unknown.
std::string countryName(const std::string& iso2);

// Returns IANA timezone for the given iso2, or "UTC" if unknown.
std::string defaultTimezone(const std::string& iso2);

}  // namespace CountryInfo
}  // namespace Osm
}  // namespace SmartMet
