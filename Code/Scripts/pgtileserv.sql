CREATE OR REPLACE
FUNCTION public.trip(
            z integer, x integer, y  integer,s float)
RETURNS bytea
AS $$
    WITH bounds AS (
        SELECT ST_TileEnvelope(z,x,y) as geom
    ),
    vals AS (
        SELECT mmsi, asMVTGeom(transform(SquishESimplify(trip,s),3857), (bounds.geom)::stbox)
            as geom_times, asMVTGeom(transform(trip,3857), (bounds.geom)::stbox) as origin
        FROM aistrips, bounds
    ),
    mvtgeom AS (
        SELECT mmsi,(geom_times).geom,(geom_times).times
        FROM vals
        UNION ALL
        SELECT mmsi,(origin).geom,(origin).times
        FROM vals
    )
SELECT ST_AsMVT(mvtgeom) FROM mvtgeom
                                  $$
    LANGUAGE 'sql'
STABLE
PARALLEL SAFE;

CREATE OR REPLACE
FUNCTION public.tripOriginal(
            z integer, x integer, y  integer)
RETURNS bytea
AS $$
    WITH bounds AS (
        SELECT ST_TileEnvelope(z,x,y) as geom
    ),
    vals AS (
        SELECT mmsi, asMVTGeom(transform(trip,3857), (bounds.geom)::stbox)
            as geom_times
        FROM aistrips, bounds
    ),
    mvtgeom AS (
        SELECT (geom_times).geom, (geom_times).times
        FROM vals
    )
SELECT ST_AsMVT(mvtgeom) FROM mvtgeom
                                  $$
    LANGUAGE 'sql'
STABLE
PARALLEL SAFE;