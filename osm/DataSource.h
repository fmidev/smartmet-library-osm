// ======================================================================
/*!
 * \brief Abstract place-data source.
 *
 * Implementations:
 *   - FileDataSource     (TSV files; implemented)
 *   - PostgresDataSource (osm_places schema; stubbed)
 *
 * Engine usage is load-once: the engine calls the read* methods at startup,
 * builds its own in-memory indices, and never touches the source again at
 * request time. dataHash() is polled periodically by the engine's auto-reload
 * loop to detect when the source data has changed.
 */
// ======================================================================

#pragma once

#include "Place.h"

#include <string>

namespace SmartMet
{
namespace Osm
{
class DataSource
{
 public:
  virtual ~DataSource() = default;

  virtual Places readPlaces() = 0;
  virtual Countries readCountries() = 0;

  // Stable across reads when the underlying data is unchanged. Used by the
  // engine to decide whether a reload is needed.
  virtual std::string dataHash() const = 0;
};

}  // namespace Osm
}  // namespace SmartMet
