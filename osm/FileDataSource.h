// ======================================================================
/*!
 * \brief DataSource that reads OSM places from TSV files in a directory.
 *
 * The directory is expected to contain one or more files matching
 * "*-places.tsv". Files are tab-separated UTF-8 with a header row. The
 * required columns are:
 *
 *   osm_id  place  name  name_en  iso2  lon  lat  population  timezone  admin1
 *
 * Empty population fields default to a value derived from the place tag.
 * Empty timezone fields default to the country's primary timezone.
 *
 * Synthetic Place::id values are sequence-assigned per file load to be
 * stable across re-reads of the same file set.
 */
// ======================================================================

#pragma once

#include "DataSource.h"

#include <string>
#include <vector>

namespace SmartMet
{
namespace Osm
{
class FileDataSource : public DataSource
{
 public:
  explicit FileDataSource(std::string directory);

  Places readPlaces() override;
  Countries readCountries() override;
  std::string dataHash() const override;

 private:
  std::vector<std::string> tsvFiles() const;

  std::string itsDirectory;
};

}  // namespace Osm
}  // namespace SmartMet
