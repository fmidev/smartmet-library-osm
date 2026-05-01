#include "PostgresDataSource.h"

#include <macgyver/Exception.h>

#include <utility>

namespace SmartMet
{
namespace Osm
{
PostgresDataSource::PostgresDataSource(std::string host,
                                       std::string database,
                                       std::string user,
                                       std::string password,
                                       int port)
    : itsHost(std::move(host)),
      itsDatabase(std::move(database)),
      itsUser(std::move(user)),
      itsPassword(std::move(password)),
      itsPort(port)
{
}

Places PostgresDataSource::readPlaces()
{
  throw Fmi::Exception(BCP, "PostgresDataSource::readPlaces: not implemented");
}

Countries PostgresDataSource::readCountries()
{
  throw Fmi::Exception(BCP, "PostgresDataSource::readCountries: not implemented");
}

std::string PostgresDataSource::dataHash() const
{
  throw Fmi::Exception(BCP, "PostgresDataSource::dataHash: not implemented");
}

}  // namespace Osm
}  // namespace SmartMet
