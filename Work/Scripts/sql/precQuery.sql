SELECT
frechetDistance(trip,s_trip) as sFrechet,
hausdorffDistance(trip,s_trip) as sHausdorff,
dynTimeWarpDistance(trip,s_trip) as sdtw,
integral(trip <-> s_trip)/ (24 * 3600 * 1e6) as stemp,

--frechetDistance(trip,c_trip) as cFrechet,
--hausdorffDistance(trip,c_trip) as cHausdorff,
--dynTimeWarpDistance(trip,c_trip) as cdtw,
--integral(trip <-> c_trip)/ (24 * 3600 * 1e6) as ctemp,

--frechetDistance(trip,c_trip) -  frechetDistance(trip,s_trip) as dFrechet,
--hausdorffDistance(trip,c_trip) -  hausdorffDistance(trip,s_trip) as dHausdorff,
--dynTimeWarpDistance(trip,c_trip) -  dynTimeWarpDistance(trip,s_trip) as dDtw,
--(integral(trip <-> c_trip)/ (24 * 3600 * 1e6))  -  (integral(trip <-> s_trip)/ (24 * 3600 * 1e6)) as sTemp,

numInstants(trip) as points,
numInstants(c_trip) as cPoints,
numInstants(s_trip) as sPoints

from
    (select
         transform(trip,25832) as trip,
         transform(SquishESimplify(trip,0.80383),25832) as s_trip,
         transform(MinDistSimplify(trip,0.0002),25832) as c_trip
     from (select * from aistrips where mmsi = 257136000) as t2
    ) as t;

--select DouglasPeuckerSimplify(trip,0.05) as c_trip
--	from aistrips where mmsi = 257136000;

--select SquishESimplify(trip,1) as s_trip
--from aistrips where mmsi = 257136000;

--SELECT NumInstants(SquishESimplify(trip, 0.5 )) from AISTrips


--select * from aistrips;
