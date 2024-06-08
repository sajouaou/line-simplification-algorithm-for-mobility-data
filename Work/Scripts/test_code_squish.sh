#!/bin/bash

# A script to test SQUISH-E it resets the database
source creditentials
export PGPASSWORD=$password



cd ../BuildScript
./change_file.sh
./compile_mobility.sh $1




dropdb -h $host -p $port -U $dbowner $database
createdb -h $host -p $port -U $dbowner $database
psql -h $host -p $port -U $dbowner  -d $database -c 'CREATE EXTENSION MobilityDB CASCADE'

cd ../Scripts
./load_sql_function.sh

psql -h $host -p $port -U $dbowner -d $database -c "SELECT asText(SquishESimplify(tgeompoint '[Point(1 1 1)@2000-01-01,Point(2 2 2)@2000-01-02, Point(3 3 3)@2000-01-04, Point(5 5 5)@2000-01-05)', 0.5))"
psql -h $host -p $port -U $dbowner -d $database -c "SELECT asText(minDistSimplify(tgeompoint '[Point(1 1 1)@2000-01-01,Point(2 2 2)@2000-01-02, Point(3 3 3)@2000-01-04, Point(5 5 5)@2000-01-05)', sqrt(3)))"
psql -h $host -p $port -U $dbowner -d $database -c "SELECT asText(DouglasPeuckerSimplify(tgeompoint '[Point(1 1 1)@2000-01-01,Point(2 2 2)@2000-01-02, Point(3 3 3)@2000-01-04, Point(5 5 5)@2000-01-05)', sqrt(3)))"


cd ../bin
#gcc -Wall -g -I/usr/local/include -I/usr/include/postgresql -o ais_stream ais_stream_db.c -L/usr/local/lib -lmeos -lpq
./ais_stream

psql -h $host -p $port -U $dbowner -d $database -c "SELECT NumInstants(SquishESimplify(trip, 0.5 )) from AISTrips"

#psql -h $host -p $port -U $dbowner -d $database -c "SELECT asText(DouglasPeuckerSimplify(trip, sqrt(3))) from AISTrips"
