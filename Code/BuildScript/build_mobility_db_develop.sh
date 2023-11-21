#!/bin/bash


./install_dependance.sh
git clone --branch develop https://github.com/MobilityDB/MobilityDB
cat ../Scripts/online_simplification.c  >> MobilityDB/mobilitydb/src/point/tpoint_analytics.c
cat ../Scripts/online_simplification.sql >> MobilityDB/mobilitydb/sql/point/076_tpoint_analytics.in.sql
mkdir MobilityDB/build
cd MobilityDB/build
cmake ..
make
sudo make install
cd ../../
rm -r --force MobilityDB
./setup_postgresql.sh