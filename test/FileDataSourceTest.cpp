// ======================================================================
/*!
 * \brief Tests for FileDataSource using the bundled fi-places.tsv and
 *        ua-places.tsv fixtures in test/data.
 *
 * Uses the same lightweight tframe.h convention the rest of SmartMet uses.
 */
// ======================================================================

#include "CountryInfo.h"
#include "FeatureCodes.h"
#include "FileDataSource.h"
#include "Keywords.h"

#include <regression/tframe.h>

#include <algorithm>
#include <iostream>
#include <map>
#include <set>
#include <string>

using namespace SmartMet::Osm;

namespace FileDataSourceTest
{
const std::string kDataDir = "data";

void load_fi()
{
  FileDataSource ds(kDataDir);
  Places places = ds.readPlaces();
  if (places.empty())
    TEST_FAILED("expected non-empty places list");

  // Both fixture countries should be loaded.
  std::set<std::string> isos;
  for (const auto& p : places)
    isos.insert(p.iso2);
  if (isos.count("FI") == 0)
    TEST_FAILED("Finland data missing");
  if (isos.count("UA") == 0)
    TEST_FAILED("Ukraine data missing");

  TEST_PASSED();
}

void timezones_filled()
{
  FileDataSource ds(kDataDir);
  for (const auto& p : ds.readPlaces())
  {
    if (p.timezone.empty())
      TEST_FAILED("place with empty timezone: " + p.name + " (" + p.iso2 + ")");
  }
  TEST_PASSED();
}

void feature_codes_mapped()
{
  FileDataSource ds(kDataDir);
  std::map<std::string, int> counts;
  for (const auto& p : ds.readPlaces())
    counts[p.feature]++;
  // We expect at least PPLA (city) and PPL (town) and PPLA4 (village) and PPLX (hamlet).
  for (const char* code : {"PPLA", "PPL", "PPLA4", "PPLX"})
    if (counts[code] == 0)
      TEST_FAILED(std::string("no places mapped to feature ") + code);
  TEST_PASSED();
}

void kyiv_is_present()
{
  FileDataSource ds(kDataDir);
  auto places = ds.readPlaces();
  auto it = std::find_if(places.begin(), places.end(), [](const Place& p) {
    return p.iso2 == "UA" && p.feature == "PPLA" && p.population > 1000000;
  });
  if (it == places.end())
    TEST_FAILED("expected to find Kyiv (UA, PPLA, pop > 1M)");
  if (it->timezone != "Europe/Kyiv")
    TEST_FAILED("expected Kyiv timezone Europe/Kyiv, got " + it->timezone);
  TEST_PASSED();
}

void countries_listed()
{
  FileDataSource ds(kDataDir);
  auto countries = ds.readCountries();
  std::set<std::string> codes;
  for (const auto& c : countries)
    codes.insert(c.iso2);
  if (codes.count("FI") == 0 || codes.count("UA") == 0)
    TEST_FAILED("expected FI and UA in countries list");
  TEST_PASSED();
}

void keywords_materialize()
{
  FileDataSource ds(kDataDir);
  auto kw = defaultKeywords(ds.readPlaces());
  if (kw["cities"].empty())
    TEST_FAILED("expected non-empty 'cities' keyword set");
  if (kw["populated"].empty())
    TEST_FAILED("expected non-empty 'populated' keyword set");
  if (kw["populated"].size() <= kw["cities"].size())
    TEST_FAILED("'populated' should be larger than 'cities'");
  TEST_PASSED();
}

void hash_is_stable()
{
  FileDataSource ds(kDataDir);
  const auto h1 = ds.dataHash();
  const auto h2 = ds.dataHash();
  if (h1 != h2)
    TEST_FAILED("dataHash() should be stable across calls when files unchanged");
  if (h1.empty())
    TEST_FAILED("dataHash() should be non-empty");
  TEST_PASSED();
}

class tests : public tframe::tests
{
  virtual const char* error_message_prefix() const { return "\n\t"; }

  void test(void)
  {
    TEST(load_fi);
    TEST(timezones_filled);
    TEST(feature_codes_mapped);
    TEST(kyiv_is_present);
    TEST(countries_listed);
    TEST(keywords_materialize);
    TEST(hash_is_stable);
  }
};

}  // namespace FileDataSourceTest

int main(void)
{
  std::cout << std::endl << "FileDataSource tester" << std::endl << "=====================" << std::endl;
  FileDataSourceTest::tests t;
  return t.run();
}
