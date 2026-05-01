-- =====================================================================
-- Schema for the local OSM-derived place mirror.
--
-- This file documents the target schema for PostgresDataSource (which is
-- currently stubbed). The shape mirrors the TSV format consumed by
-- FileDataSource, with a synthetic primary key and a PostGIS geography
-- column for nearest-neighbour queries.
--
-- Loaded by an external importer (planned: smartmet-osm-importer, not in
-- this repo) that reads .osm.pbf country extracts via osmium and emits
-- INSERTs / UPDATEs against these tables.
-- =====================================================================

CREATE EXTENSION IF NOT EXISTS postgis;

CREATE TABLE IF NOT EXISTS osm_places (
    id            BIGSERIAL PRIMARY KEY,
    osm_type      CHAR(1) NOT NULL,                 -- 'n','w','r' (info only)
    osm_id        BIGINT  NOT NULL,                 -- source osm id (info only;
                                                    --   not stable across imports)
    name          TEXT    NOT NULL,                 -- primary OSM name tag
    iso2          CHAR(2) NOT NULL,
    feature       TEXT    NOT NULL,                 -- GeoNames-style code
                                                    --   (PPLA, PPL, PPLA4, PPLX, ...)
    place_type    TEXT    NOT NULL,                 -- raw place=* value (debug)
    admin1        TEXT,                             -- containing admin_level=4 name
    lon           DOUBLE PRECISION NOT NULL,
    lat           DOUBLE PRECISION NOT NULL,
    geog          GEOGRAPHY(POINT, 4326) NOT NULL,
    population    INTEGER,
    elevation     INTEGER,
    timezone      TEXT NOT NULL,
    last_modified TIMESTAMPTZ NOT NULL DEFAULT now()
);

CREATE INDEX IF NOT EXISTS osm_places_geog_gix ON osm_places USING GIST(geog);
CREATE INDEX IF NOT EXISTS osm_places_iso_feat ON osm_places(iso2, feature);
CREATE INDEX IF NOT EXISTS osm_places_name     ON osm_places(name);

-- Translations. Empty in the day-1 prototype; populated from name:<lang>=*
-- tags by the importer once translation support is enabled.
CREATE TABLE IF NOT EXISTS osm_alternate_names (
    place_id BIGINT REFERENCES osm_places(id) ON DELETE CASCADE,
    language TEXT,
    name     TEXT NOT NULL,
    PRIMARY KEY (place_id, language)
);

-- Country reference data. The library has a built-in fallback table; this
-- exists for deployments that want to override or extend it.
CREATE TABLE IF NOT EXISTS osm_countries (
    iso2 CHAR(2) PRIMARY KEY,
    name TEXT NOT NULL
);

CREATE TABLE IF NOT EXISTS osm_alternate_countries (
    iso2     CHAR(2),
    language TEXT,
    name     TEXT NOT NULL,
    PRIMARY KEY (iso2, language)
);

-- Materialized keyword sets. The importer populates this from rules
-- (e.g. "cities" = feature in (PPLC, PPLA, PPL)) so the engine can read
-- a flat (keyword, place_id) table.
CREATE TABLE IF NOT EXISTS osm_keywords (
    keyword  TEXT,
    place_id BIGINT REFERENCES osm_places(id) ON DELETE CASCADE,
    PRIMARY KEY (keyword, place_id)
);

CREATE INDEX IF NOT EXISTS osm_keywords_place_id ON osm_keywords(place_id);
