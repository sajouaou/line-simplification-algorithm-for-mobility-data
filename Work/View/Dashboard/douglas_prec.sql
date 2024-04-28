SELECT mmsi,simpl,percentage,lambda,frechet,hausdorff,dtimeWarp,aTemp
FROM (
         SELECT * , CAST(nbSimPoint as float)/CAST(nbPoint as float) as percentage FROM precresulttr
     ) as t
WHERE percentage >= 0.1 and percentage <= 0.99 and
    mmsi in ($mmsi) and simpl LIKE 'DOUGLAS'
GROUP BY mmsi,simpl,percentage,lambda,frechet,hausdorff,dtimeWarp,aTemp

-- XY CHART