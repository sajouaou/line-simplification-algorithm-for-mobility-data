CREATE OR REPLACE FUNCTION perfBenchmarkSQ(
	trip tgeompoint, -- Path on what the benchmark is running
	lambda float,
	N int DEFAULT 10, --Number of Iterations
	sync boolean DEFAULT TRUE, -- SED
    simpl text DEFAULT 'SQUISH-E' -- SQUISH-E / MINDIST / DOUGLAS
)
RETURNS VOID AS $$
DECLARE
  -- Loop variable
i int;
  -- Start and end time of the execution
  startTime timestamptz; endTime timestamptz;
  itStartTime timestamptz; itEndTime timestamptz;

BEGIN
SELECT clock_timestamp() INTO startTime;
FOR i in 1..N LOOP
SELECT clock_timestamp() INTO itStartTime;

CASE simpl then
when 'MINDIST' then
PERFORM minDistSimplify(trip,lambda);
when 'DOUGLAS' then
PERFORM DouglasPeuckerSimplify(trip,lambda,sync);
else
PERFORM SquishESimplify(trip,lambda,sync);
end case;

SELECT clock_timestamp() INTO itEndTime;
INSERT INTO benchResult VALUES (i,lambda,numInstants(trip),itEndTime - itStartTime,simpl);
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
SELECT c.trip FROM
                  (SELECT MAX(points) as maxp FROM tripGenerated as b WHERE points <= N ) as a,tripGenerated as c WHERE points = maxp
    into trip;

t = t + numInstants(trip)*deliveryTime;
FOR i IN 1..N-numInstants(trip) LOOP
    t = t + deliveryTime;
	trip = appendInstant(trip, tgeompoint_inst(endvalue(instantn(trip,i)), t));
END LOOP;

INSERT INTO tripGenerated VALUES (numInstants(trip),trip)  on conflict (points) do nothing;
return trip;
END;
$$ LANGUAGE plpgsql;


CREATE  OR REPLACE FUNCTION perfBenchmarkSQ(
    __points integer[],
    _lambda float[],
	N int DEFAULT 10, --Number of Iterations
	sync boolean DEFAULT TRUE, -- SED
    simpl text DEFAULT 'SQUISH-E' -- SQUISH-E / MINDIST / DOUGLAS
)
RETURNS VOID AS $$
DECLARE
_points integer;
lambda float;
trip tgeompoint;
BEGIN
CREATE TABLE IF NOT EXISTS benchResult(i int,lambda float,nbPoint int,duration interval,simpl text);
CREATE TABLE IF NOT EXISTS tripGenerated(points int PRIMARY KEY,trip tgeompoint);
INSERT INTO tripGenerated VALUES (2,tgeompoint '[Point(1 1 1)@2000-01-01,Point(2 2 2)@2000-01-02]') on conflict (points) do nothing;


FOREACH _points in array __points LOOP
SELECT generateTrip(_points) into trip;
FOREACH lambda in array _lambda LOOP
    	PERFORM perfBenchmarkSQ(trip,lambda,N,sync,simpl);
END LOOP;
END LOOP;
END;
$$ LANGUAGE plpgsql;



--select perfBenchmarkSQ('{100,1000,10000,100000}'::integer[],'{0.01,0.25,0.5,0.75,1}'::float[] ,simpl:= 'DOUGLAS');

--SELECT AVG(duration) as average,nbpoint,lambda
--FROM public.benchresult
--WHERE simpl like 'DOUGLAS'
--GROUP BY nbpoint,lambda ORDER BY nbpoint,average;
