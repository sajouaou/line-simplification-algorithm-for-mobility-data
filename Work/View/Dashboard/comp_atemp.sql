SELECT mmsi,
       CASE
           WHEN simpl = 'SQUISH-E' THEN 'red'
           WHEN simpl = 'DOUGLAS' THEN 'yellow'
           WHEN simpl = 'MINDIST' THEN 'green'
           ELSE 'grey'
           END AS simpl
        ,percentage,lambda,aTemp
FROM (
         SELECT * , CAST(nbSimPoint as float)/CAST(nbPoint as float) as percentage FROM precresulttr
     ) as t
WHERE percentage >= 0.1 and percentage <= 0.99 and mmsi in ($mmsi)
GROUP BY mmsi,simpl,percentage,lambda,frechet,hausdorff,dtimeWarp,aTemp

-- Scatter (plugin)