DROP FUNCTION precBenchmarkSQ;
CREATE OR REPLACE FUNCTION precBenchmarkSQ(
	trip tgeompoint, -- Path on what the benchmark is running
	lambda float,
	sync boolean DEFAULT TRUE -- SED
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
CREATE TABLE IF NOT EXISTS precResult(
                                         trip tgeompoint,
                                         s_trip tgeompoint,
                                         lambda float,
                                         frechet float,
                                         hausdorff float,
                                         dTimeWarp float,
                                         nbPoint int,
                                         nbSimPoint int,
                                         duration interval);

SELECT clock_timestamp() INTO startTime;
SELECT SquishESimplify(trip,lambda,sync) into s_trip;
SELECT clock_timestamp() INTO endTime;
INSERT INTO precResult VALUES (
                                  trip,
                                  s_trip,
                                  lambda,
                                  frechetDistance(trip,s_trip),
                                  hausdorffDistance(trip,s_trip),
                                  dynTimeWarpDistance(trip,s_trip),
                                  numInstants(trip),
                                  numInstants(s_trip),
                                  itEndTime - itStartTime);
END;
$$ LANGUAGE plpgsql;






--DROP TABLE precResult;
--SELECT precBenchmarkSQ(trip,0.1) from aistrips;
--SELECT * from precResult;