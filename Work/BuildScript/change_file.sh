#!/bin/bash

cd MobilityDB
git reset
git checkout .
cd ../

cat ../SquishE/meos/src/point_dict.c  >> MobilityDB/meos/src/general/temporal_analytics.c
cat ../SquishE/meos/src/priority_dict.c  >> MobilityDB/meos/src/general/temporal_analytics.c
cat ../SquishE/meos/src/priority_queue.c  >> MobilityDB/meos/src/general/temporal_analytics.c
cat ../SquishE/meos/src/squish_e.c  >> MobilityDB/meos/src/general/temporal_analytics.c


cat ../SquishE/mobilitydb/src/online_simplification.c  >> MobilityDB/mobilitydb/src/point/tpoint_analytics.c
cat ../SquishE/mobilitydb/sql/online_simplification.sql >> MobilityDB/mobilitydb/sql/point/076_tpoint_analytics.in.sql

to="Temporal *temporal_simplify_sqe(const Temporal *temp, double eps_dist, bool synchronized);"
file="MobilityDB/meos/include/meos.h"
sudo sed -i "1581i${to}" $file