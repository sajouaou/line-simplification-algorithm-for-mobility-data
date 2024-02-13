#!/bin/bash
# A script to run the BerlinMOD generator with my personnal sql

source creditentials
export PGPASSWORD=$password

psql -h $host -p $port -U $dbowner  -d $database -f sql/*