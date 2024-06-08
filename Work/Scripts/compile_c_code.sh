#!/bin/bash

# A script to compile the C code

source creditentials
export PGPASSWORD=$password


cd ../BuildScript
./change_file.sh
./compile_mobility.sh $1



#dropdb -h $host -p $port -U $dbowner $database
#createdb -h $host -p $port -U $dbowner $database
#psql -h $host -p $port -U $dbowner  -d $database -c 'CREATE EXTENSION MobilityDB CASCADE'

cd ../C
gcc -Wall -g -I/usr/local/include -I/usr/include/postgresql -o ../bin/ais_stream ais_stream_db.c -L/usr/local/lib -lmeos -lpq
gcc -Wall -g -I/usr/local/include -I/usr/include/postgresql -o ../bin/benchmark_db benchmark_db.c -L/usr/local/lib  -lpq
gcc -Wall -g -I/usr/local/include -I/usr/include/postgresql -o ../bin/online_squish online_squish_e.c -L/usr/local/lib -lmeos -lpq
#./online_squish
