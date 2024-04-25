select
frechetDistance(trip,c_trip) -  frechetDistance(trip,s_trip) as dFrechet,
hausdorffDistance(trip,c_trip) -  hausdorffDistance(trip,s_trip) as dHausdorff,
dynTimeWarpDistance(trip,c_trip) -  dynTimeWarpDistance(trip,s_trip) as dDtw,
(integral(trip <-> c_trip)/ (24 * 3600 * 1e6))  -  (integral(trip <-> s_trip)/ (24 * 3600 * 1e6)) as sTemp,
    numInstants(trip),
    numInstants(c_trip)-numInstants(s_trip),
    mmsi
from (select
          transform(trip,25832) as trip,
          transform(SquishESimplify(trip,0.01),25832) as c_trip,
          mmsi,
          lambda from aistripssq where mmsi = 257136000 and lambda = 1) as a ,
     (select transform(trip,25832) as s_trip from aistripssq where mmsi = 257136000 and lambda = 0.01) as b;