// ======================================================================
/*!
 * \brief Materialize the default keyword sets ("cities", "populated") from
 *        a Places vector.
 *
 * Kept out of the DataSource interface so deployments can swap in their own
 * keyword definitions without touching the data layer.
 */
// ======================================================================

#pragma once

#include "Place.h"

namespace SmartMet
{
namespace Osm
{
// "cities"    : feature in {PPLC, PPLA, PPL}
// "populated" : feature in {PPLC, PPLA, PPL, PPLA2, PPLA3, PPLA4, PPLX}
KeywordMap defaultKeywords(const Places& places);

}  // namespace Osm
}  // namespace SmartMet
