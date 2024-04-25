#!/bin/bash

source creditentials
export PGPASSWORD=$password


cd ../BuildScript
./change_file.sh
./compile_mobility.sh -r
./compile_mobility.sh -m


cd ../C
gcc -Wall -g -I/usr/local/include -I/usr/include/postgresql -o ../bin/ais_stream ais_stream_db.c -L/usr/local/lib -lmeos -lpq
gcc -Wall -g -I/usr/local/include -I/usr/include/postgresql -o ../bin/benchmark_db benchmark_db.c -L/usr/local/lib  -lpq
gcc -Wall -g -I/usr/local/include -I/usr/include/postgresql -o ../bin/online_squish online_squish_e.c -L/usr/local/lib -lmeos -lpq

dropdb -h $host -p $port -U $dbowner $database
createdb -h $host -p $port -U $dbowner $database
psql -h $host -p $port -U $dbowner  -d $database -c 'CREATE EXTENSION MobilityDB CASCADE'


cd ../bin
#gcc -Wall -g -I/usr/local/include -I/usr/include/postgresql -o ais_stream ais_stream_db.c -L/usr/local/lib -lmeos -lpq
./ais_stream
./online_squish

cd ../Scripts
./load_sql_function.sh


psql -h $host -p $port -U $dbowner -d $database -c "SELECT asText(SquishESimplify(tgeompoint '[Point(1 1 1)@2000-01-01,Point(2 2 2)@2000-01-02, Point(3 3 3)@2000-01-04, Point(5 5 5)@2000-01-05)', 0.5))"
psql -h $host -p $port -U $dbowner -d $database -c "SELECT asText(minDistSimplify(tgeompoint '[Point(1 1 1)@2000-01-01,Point(2 2 2)@2000-01-02, Point(3 3 3)@2000-01-04, Point(5 5 5)@2000-01-05)', sqrt(3)))"
psql -h $host -p $port -U $dbowner -d $database -c "SELECT asText(DouglasPeuckerSimplify(tgeompoint '[Point(1 1 1)@2000-01-01,Point(2 2 2)@2000-01-02, Point(3 3 3)@2000-01-04, Point(5 5 5)@2000-01-05)', sqrt(3)))"



psql -h $host -p $port -U $dbowner -d $database -c "SELECT NumInstants(SquishESimplify(trip, 0.5 )) from AISTrips"
