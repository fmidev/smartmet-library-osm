// ======================================================================
/*!
 * \brief DataSource that reads OSM places from a local PostgreSQL/PostGIS
 *        mirror — STUB ONLY.
 *
 * The schema this is intended to query is documented in
 * schema/osm_places.sql. None of the methods are implemented yet; all
 * three throw Fmi::Exception("not implemented"). The class exists so the
 * shape of a future implementation is fixed before anyone needs it, and
 * so the engine's OsmBackend can switch from FileDataSource to this with
 * no API change.
 */
// ======================================================================

#pragma once

#include "DataSource.h"

#include <string>

namespace SmartMet
{
namespace Osm
{
class PostgresDataSource : public DataSource
{
 public:
  PostgresDataSource(std::string host,
                     std::string database,
                     std::string user,
                     std::string password,
                     int port = 5432);

  Places readPlaces() override;
  Countries readCountries() override;
  std::string dataHash() const override;

 private:
  std::string itsHost;
  std::string itsDatabase;
  std::string itsUser;
  std::string itsPassword;
  int itsPort;
};

}  // namespace Osm
}  // namespace SmartMet
