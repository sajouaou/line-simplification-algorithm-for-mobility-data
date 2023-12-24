#!/bin/bash
# A script to run the BerlinMOD generator with my personnal sql

source creditentials
export PGPASSWORD=$password

rm -rf ../BuildScript/MobilityDB/build
mkdir ../BuildScript/MobilityDB/build

#cd ../BuildScript
#./change_file.sh
cd ../BuildScript/MobilityDB/build
cmake .. -DMEOS=on
#cmake ..
make install




#dropdb -h $host -p $port -U $dbowner $database
#createdb -h $host -p $port -U $dbowner $database
#psql -h $host -p $port -U $dbowner  -d $database -c 'CREATE EXTENSION MobilityDB CASCADE'

cd ../../../C
gcc -Wall -g -I/usr/local/include -I/usr/include/postgresql -o asi_stream asi_stream_db.c -L/usr/local/lib -lmeos -lpq
#gcc -Wall -g -I/usr/local/include -I/usr/include/postgresql -o online_squish online_squish_e.c -L/usr/local/lib -lmeos -lpq
#./online_squish
