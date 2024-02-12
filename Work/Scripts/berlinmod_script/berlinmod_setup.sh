#!/bin/bash
# A script to setup the BerlinMOD generator


source ../creditentials
datapath=../../Data
scriptpath=../../Data/MobilityDB-BerlinMOD/BerlinMOD

export PGPASSWORD=$password


dropdb -h $host -p $port -U $dbowner $database
createdb -h $host -p $port -U $dbowner $database

psql -h $host -p $port -U $dbowner  -d $database -c 'CREATE EXTENSION hstore'
psql -h $host -p $port -U $dbowner  -d $database -c 'CREATE EXTENSION MobilityDB CASCADE'
psql -h $host -p $port -U $dbowner  -d $database -c 'CREATE EXTENSION pgRouting'

osm2pgrouting -h $host -p $port -U $dbowner -W $password -f $datapath/brussels.osm --dbname $database -c $scriptpath/mapconfig.xml
osm2pgsql -c -H $host -P $port -U $dbowner -d $database $datapath/brussels.osm

#pg_dump -h $host -p $port -U $dbowner -W -F t brussels > brussels.tar
#pg_restore --dbname=$database --create -U $dbowner --verbose $datapath/brussels.tar

psql -h $host -p $port -U $dbowner -d $database -f $scriptpath/brussels_preparedata.sql
psql -h $host -p $port -U $dbowner -d $database -f $scriptpath/berlinmod_datagenerator.sql
psql -h $host -p $port -U $dbowner -d $database -f $scriptpath/deliveries_datagenerator.sql



#psql -h $host -p $port -U $dbowner -d $database -f $scriptpath/tests.sql
