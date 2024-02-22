CREATE OR REPLACE
FUNCTION public.linesimpl(
            z integer, x integer, y  integer,s float,mmsi integer)
RETURNS bytea
AS $$
    WITH bounds AS (
        SELECT ST_TileEnvelope(z,x,y) as geom
    ),
	trips_ AS (
		SELECT * from aistrips as a where a.mmsi = mmsi or mmsi = -1
	)
	,
    vals AS (
        SELECT sim,asMVTGeom(transform(trip,3857), (bounds.geom)::stbox)
            as geom_times
        FROM (Select 'F' as sim , * from trips_ UNION ALL (Select 'S' as sim,mmsi,SquishESimplify(trip, s) from trips_)) as ego, bounds
    ),
    mvtgeom AS (
        SELECT (geom_times).geom, (geom_times).times,sim
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