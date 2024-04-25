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
CREATE TABLE IF NOT EXISTS precResult(   mmsi integer,
                                         trip tgeompoint,
                                         s_trip tgeompoint,
                                         lambda float,
                                         frechet float,
                                         hausdorff float,
                                         dTimeWarp float,
                                         nbPoint int,
                                         nbSimPoint int,
                                         duration interval,
                                         simpl text);
RAISE INFO '-----------------------BEGIN SIMPLIFY------------------------------------';

SELECT clock_timestamp() INTO startTime;

CASE simpl then
when 'MINDIST' then
SELECT minDistSimplify(trip,lambda) into s_trip;
when 'DOUGLAS' then
SELECT DouglasPeuckerSimplify(trip,lambda,sync) into s_trip;
else
SELECT SquishESimplify(trip,lambda,sync) into s_trip;
end case;

SELECT clock_timestamp() INTO endTime;
RAISE INFO '----------------------END SIMPLIFY----------------------------------';

INSERT INTO precResult VALUES (
                                  trip,
                                  s_trip,
                                  lambda,
                                  frechetDistance(trip,s_trip),
                                  hausdorffDistance(trip,s_trip),
                                  dynTimeWarpDistance(trip,s_trip),
                                  numInstants(trip),
                                  numInstants(s_trip),
                                  endTime - startTime,
                                  simpl);
RAISE INFO '----------------------INSERTED----------------------------------';
END;
$$ LANGUAGE plpgsql;

--DROP TABLE precResult;
--SELECT precBenchmarkSQ(trip,0.1) from aistrips;
--SELECT * from precResult;

