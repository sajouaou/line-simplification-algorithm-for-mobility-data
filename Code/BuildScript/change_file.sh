#!/bin/bash

cd MobilityDB
git reset --hard HEAD~1
cd ../

cat ../C/point_dict.c  >> MobilityDB/meos/src/general/temporal_analytics.c
cat ../C/priority_dict.c  >> MobilityDB/meos/src/general/temporal_analytics.c
cat ../C/priority_queue.c  >> MobilityDB/meos/src/general/temporal_analytics.c
cat ../C/squish_e.c  >> MobilityDB/meos/src/general/temporal_analytics.c


cat ../Scripts/online_simplification.c  >> MobilityDB/mobilitydb/src/point/tpoint_analytics.c
cat ../Scripts/online_simplification.sql >> MobilityDB/mobilitydb/sql/point/076_tpoint_analytics.in.sql

from="Temporal *temporal_simplify_max_dist(const Temporal *temp, double eps_dist, bool synchronized);"
to="Temporal *temporal_simplify_max_dist(const Temporal *temp, double eps_dist, bool synchronized); Temporal *temporal_simplify_sqe(const Temporal *temp, double eps_dist, bool synchronized);"
file="MobilityDB/meos/include/meos.h"
sudo sed -i "s/${from}/${to}/g" $file