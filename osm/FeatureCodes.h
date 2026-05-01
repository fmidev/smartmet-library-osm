// ======================================================================
/*!
 * \brief Mapping from OSM place=* tag values to GeoNames-style feature codes.
 *
 * Keeping the same vocabulary the geonames engine already uses means
 * downstream code that filters or prioritizes by feature (autocomplete
 * ranking, plugin feature filters) keeps working unchanged.
 */
// ======================================================================

#pragma once

#include <string>

namespace SmartMet
{
namespace Osm
{
// Returns a GeoNames feature code for the given OSM place=* value, or an
// empty string for unrecognized values.
std::string featureCodeForPlaceTag(const std::string& placeTag);

// Best-guess population when the OSM node has no population= tag. Order of
// magnitude is what matters: autocomplete ranking is population-driven and
// these defaults keep cities ranked above villages.
int defaultPopulationForPlaceTag(const std::string& placeTag);

}  // namespace Osm
}  // namespace SmartMet
