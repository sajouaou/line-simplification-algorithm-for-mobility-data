#!/bin/bash


#./install_dependance.sh
#git clone --branch develop https://github.com/MobilityDB/MobilityDB

cat ../C/point_dict.c  >> MobilityDB/meos/src/general/temporal_analytics.c
cat ../C/priority_dict.c  >> MobilityDB/meos/src/general/temporal_analytics.c
cat ../C/priority_queue.c  >> MobilityDB/meos/src/general/temporal_analytics.c
cat ../C/squish_e.c  >> MobilityDB/meos/src/general/temporal_analytics.c


cat ../Scripts/online_simplification.c  >> MobilityDB/mobilitydb/src/point/tpoint_analytics.c
cat ../Scripts/online_simplification.sql >> MobilityDB/mobilitydb/sql/point/076_tpoint_analytics.in.sql


mkdir MobilityDB/build
cd MobilityDB/build
cmake ..
make
sudo make install
cd ../../
#rm -r --force MobilityDB
./setup_postgresql.sh