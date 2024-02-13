CREATE OR REPLACE FUNCTION perfBenchmarkSQ(
	trip tgeompoint, -- Path on what the benchmark is running
	lambda float,
	N int DEFAULT 10, --Number of Iterations
	sync boolean DEFAULT TRUE -- SED
)
RETURNS VOID AS $$
DECLARE
  -- Loop variable
i int;
  -- Start and end time of the execution
  startTime timestamptz; endTime timestamptz;
  itStartTime timestamptz; itEndTime timestamptz;

BEGIN
CREATE TABLE IF NOT EXISTS benchResult(i int,lambda float,nbPoint int,duration interval);
SELECT clock_timestamp() INTO startTime;
FOR i in 1..N LOOP
SELECT clock_timestamp() INTO itStartTime;
PERFORM SquishESimplify(trip,lambda,sync);
SELECT clock_timestamp() INTO itEndTime;
INSERT INTO benchResult VALUES (i,lambda,numInstants(trip),itEndTime - itStartTime);
END LOOP;
SELECT clock_timestamp() INTO endTime;
RAISE INFO '----------------------------------------------------------------------';
  RAISE INFO 'Execution started at %', startTime;
  RAISE INFO 'Execution finished at %', endTime;
  RAISE INFO 'Execution time %', endTime - startTime;

END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION generateTrip(N int)
    RETURNS tgeompoint AS $$
DECLARE
trip tgeompoint;
  -- Loop variables
  i int;
  -- Current timestamp
  t timestamptz;
  -- Time servicing a customer
  deliveryTime interval;
BEGIN
SELECT clock_timestamp() INTO t;
deliveryTime = interval '1 min';
SELECT tgeompoint '[Point(1 1 1)@2000-01-01,Point(2 2 2)@2000-01-02]' into trip;
FOR i IN 1..N-2 LOOP
    t = t + deliveryTime;
	trip = appendInstant(trip, tgeompoint_inst(endvalue(instantn(trip,i)), t));
END LOOP;
return trip;
END;
$$ LANGUAGE plpgsql;



CREATE  OR REPLACE FUNCTION perfBenchmarkSQ(
    _points integer[],
    _lambda float[],
	N int DEFAULT 10, --Number of Iterations
	sync boolean DEFAULT TRUE -- SED
)
RETURNS VOID AS $$
DECLARE
points integer;
lambda float;
trip tgeompoint;
BEGIN

FOREACH points in array _points LOOP
SELECT generateTrip(points) into trip;
FOREACH lambda in array _lambda LOOP
    	PERFORM perfBenchmarkSQ(trip,lambda,N,sync);
END LOOP;
END LOOP;
END;
$$ LANGUAGE plpgsql;


-- select perfBenchmarkSQ('{100,1000,10000,100000}'::integer[],'{0.01,0.25,0.5,0.75,1}'::float[] );