#!/bin/bash

# A script to run the BerlinMOD generator with my personnal sql
source creditentials
export PGPASSWORD=$password


#rm -rf ../BuildScript/MobilityDB/build
#mkdir ../BuildScript/MobilityDB/build
cd ../BuildScript
#./change_file.sh
cd ../BuildScript/MobilityDB/build
#cmake .. -DMEOS=on
#cmake ..
make install




dropdb -h $host -p $port -U $dbowner $database
createdb -h $host -p $port -U $dbowner $database
psql -h $host -p $port -U $dbowner  -d $database -c 'CREATE EXTENSION MobilityDB CASCADE'

psql -h $host -p $port -U $dbowner -d $database -c "SELECT asText(SquishESimplify(tgeompoint '[Point(1 1 1)@2000-01-01,Point(2 2 2)@2000-01-02, Point(3 3 3)@2000-01-04, Point(5 5 5)@2000-01-05)', sqrt(3)))"
psql -h $host -p $port -U $dbowner -d $database -c "SELECT asText(minDistSimplify(tgeompoint '[Point(1 1 1)@2000-01-01,Point(2 2 2)@2000-01-02, Point(3 3 3)@2000-01-04, Point(5 5 5)@2000-01-05)', sqrt(3)))"
psql -h $host -p $port -U $dbowner -d $database -c "SELECT asText(DouglasPeuckerSimplify(tgeompoint '[Point(1 1 1)@2000-01-01,Point(2 2 2)@2000-01-02, Point(3 3 3)@2000-01-04, Point(5 5 5)@2000-01-05)', sqrt(3)))"


cd ../../../C
#gcc -Wall -g -I/usr/local/include -I/usr/include/postgresql -o ais_stream asi_stream_db.c -L/usr/local/lib -lmeos -lpq
./ais_stream

psql -h $host -p $port -U $dbowner -d $database -c "SELECT NumInstants(SquishESimplify(trip, sqrt(3) )) from AISTrips"

#psql -h $host -p $port -U $dbowner -d $database -c "SELECT asText(DouglasPeuckerSimplify(trip, sqrt(3))) from AISTrips"

