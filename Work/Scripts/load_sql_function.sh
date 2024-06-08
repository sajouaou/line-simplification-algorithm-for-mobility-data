#!/bin/bash

# A script to load the SQL into the database

source creditentials
export PGPASSWORD=$password

psql -h $host -p $port -U $dbowner  -d $database -f sql/benchmark.sql
psql -h $host -p $port -U $dbowner  -d $database -f sql/pgtileserv.sql
psql -h $host -p $port -U $dbowner  -d $database -f sql/prec.sql