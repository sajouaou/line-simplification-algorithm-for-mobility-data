DROP FUNCTION precBenchmarkSQ;
CREATE OR REPLACE FUNCTION precBenchmarkSQ(
	trip tgeompoint, -- Path on what the benchmark is running
	mmsi integer,
	lambda float,
	sync boolean DEFAULT TRUE, -- SED
    simpl text DEFAULT 'SQUISH-E'
)
RETURNS VOID AS $$
DECLARE
	-- Loop variable
i int;
	-- Simplified Path
	s_trip tgeompoint;
	-- Start and end time of the execution
	startTime timestamptz; endTime timestamptz;
	itStartTime timestamptz; itEndTime timestamptz;

BEGIN
CREATE TABLE IF NOT EXISTS precResulttr(   mmsi integer,
                                         lambda float,
                                         frechet float,
                                         hausdorff float,
                                         dTimeWarp float,
                                         aTemp float,
                                         nbPoint int,
                                         nbSimPoint int,
                                         duration interval,
                                         simpl text);

--CREATE TABLE IF NOT EXISTS precResult(   mmsi integer,
--                                           lambda float,
--                                           frechet float,
--                                           hausdorff float,
--                                           dTimeWarp float,
 --                                          aTemp float,
 --                                          nbPoint int,
 --                                          nbSimPoint int,
--                                           duration interval,
--                                           simpl text);
RAISE INFO '-----------------------BEGIN SIMPLIFY------------------------------------';

SELECT clock_timestamp() INTO startTime;

CASE simpl then
when 'MINDIST' then
RAISE INFO '----------------------MINDIST SIMPLIFY----------------------------------';
SELECT minDistSimplify(trip,lambda) into s_trip;
when 'DOUGLAS' then
RAISE INFO '----------------------DOUGLAS SIMPLIFY----------------------------------';
SELECT DouglasPeuckerSimplify(trip,lambda,sync) into s_trip;
else
RAISE INFO '----------------------SQUISH SIMPLIFY----------------------------------';
RAISE INFO '%',lambda;
SELECT SquishESimplify(trip,lambda,sync) into s_trip;
end case;
SELECT clock_timestamp() INTO endTime;
RAISE INFO '----------------------END SIMPLIFY----------------------------------';

--INSERT INTO precResult VALUES (   mmsi,
--                                  lambda,
--                                  frechetDistance(trip,s_trip),
--                                  hausdorffDistance(trip,s_trip),
--                                  dynTimeWarpDistance(trip,s_trip),
--                                  integral(trip <-> s_trip)/ (24 * 3600 * 1e6),
--                                  numInstants(trip),
--                                  numInstants(s_trip),
--                                  endTime - startTime,
--                                  simpl);

SELECT transform(trip,25832) into trip;
SELECT transform(s_trip,25832) into s_trip;
INSERT INTO precResulttr VALUES (   mmsi,
                                  lambda,
                                  frechetDistance(trip,s_trip),
                                  hausdorffDistance(trip,s_trip),
                                  dynTimeWarpDistance(trip,s_trip),
                                  integral(trip <-> s_trip)/ (24 * 3600 * 1e6),
                                  numInstants(trip),
                                  numInstants(s_trip),
                                  endTime - startTime,
                                  simpl);
RAISE INFO '----------------------INSERTED----------------------------------';
END;
$$ LANGUAGE plpgsql;

--DROP TABLE precResult;
--DROP TABLE precResulttr;
--SELECT precBenchmarkSQ(trip,0.1) from aistrips;
--SELECT * from precResult;



--SELECT SquishESimplify(trip,1,TRUE) FROM (select * from aistrips where mmsi = 257136000) as t2;
--DELETE FROM precResulttr
--WHERE simpl LIKE '%DOUGLAS%';

SELECT precBenchmarkSQ(trip,mmsi,num)
FROM aistrips , (
    SELECT 0.001 + (0.001 * (n-1)) AS num
    FROM (
             SELECT ROW_NUMBER() OVER () AS n
             FROM generate_series(1, 1000)  -- Génère les nombres de 1 à 100
         ) AS numbered_rows
) AS numbers;

--SELECT precBenchmarkSQ(trip,mmsi,num,simpl := 'MINDIST')
--FROM aistrips , (
--SELECT 0.0000002 * POW(10, 1 + ((n-1)/999.0)*6.0) AS num
--FROM generate_series(1, 1000) AS n
--) AS numbers;


--SELECT precBenchmarkSQ(trip,mmsi,num,simpl := 'DOUGLAS')
--FROM aistrips , (
--    SELECT 0.0000002 * POW(10, 1 + ((n-1)/999.0)*6.0) AS num
--    FROM generate_series(1, 1000) AS n
--) AS numbers;

--SELECT numInstants(trip),numInstants(DouglasPeuckerSimplify(trip,0.0000002)) from aistrips
