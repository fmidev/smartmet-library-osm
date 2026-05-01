# smartmet-library-osm

C++ library for reading OpenStreetMap-derived place data (cities, towns,
villages, hamlets) from a local mirror. Intended as a backend alternative to
`smartmet-library-locus` (GeoNames-derived) for use by
`smartmet-engine-geonames` in deployments where OSM data is preferred — for
example, foreign installs of the FMI weather app where GeoNames is known to
be inaccurate.

## Status

Day-1 prototype: file backend (TSV) is the only working `DataSource`. The
PostgreSQL `DataSource` is declared and stubbed; methods throw
`Fmi::Exception("not implemented")`. The schema for the eventual Postgres
backend is checked in at `schema/osm_places.sql` for documentation.

## Architecture

A `DataSource` is a load-time interface. The geonames engine asks for places,
countries, and a hash for change detection; everything else lives in the
engine's existing in-memory indices (KD-trees for spatial search, ternary
trees for autocomplete). After startup, runtime queries do not touch this
library.

```
class DataSource {
  virtual Places      readPlaces()      = 0;
  virtual Countries   readCountries()   = 0;
  virtual std::string dataHash() const  = 0;
};

class FileDataSource     : public DataSource { /* implemented */ };
class PostgresDataSource : public DataSource { /* stub */       };
```

The `defaultKeywords()` free function in `Keywords.h` materializes the
`cities` and `populated` keyword sets from a `Places` vector. Keep this
out of the `DataSource` interface so different deployments can plug in
their own keyword definitions without changing the data source.

## Place data shape

```
struct Place {
  long        id;          // synthetic stable id
  std::string name;        // primary OSM name tag (likely native script)
  std::string iso2;        // ISO 3166-1 alpha-2 country code
  std::string feature;     // GeoNames-style feature code (PPLA, PPL, PPLA4, PPLX, ...)
  double      lon, lat;
  int         population;  // -1 if unknown
  std::string timezone;    // IANA timezone
  std::string admin1;      // empty in the prototype
};
```

OSM `place=*` tags are mapped to GeoNames feature codes so existing engine
logic (feature-based filtering, prioritization) keeps working unchanged:
`city → PPLA`, `town → PPL`, `village → PPLA4`, `hamlet → PPLX`.

## Tools

`tools/fetch_osm_places.py` queries the Overpass API for one country's
populated places and writes a TSV file in the format `FileDataSource`
expects. Example:

```
python3 tools/fetch_osm_places.py UA test/data/ua-places.tsv
```

## Test data

`test/data/` ships pre-fetched TSVs for Finland (`fi-places.tsv`, ~10k
places) and Ukraine (`ua-places.tsv`, ~30k places). Ukraine is the
primary motivating use case; Finland is included as a control where the
GeoNames database is the production source of truth.
