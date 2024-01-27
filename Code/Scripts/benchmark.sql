DROP FUNCTION perfBenchmarkSQ;
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