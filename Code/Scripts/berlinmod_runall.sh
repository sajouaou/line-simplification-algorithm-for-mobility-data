#!/bin/bash
# A script to run the BerlinMOD generator with my personnal sql


source creditentials
scalefactor=0.005

while getopts ":s:" option; do
case $option in
  s)
     scalefactor=${OPTARG}
    ;;
esac
done
export PGPASSWORD=$password

./berlinmod_setup.sh
psql -h $host -p $port -U $dbowner -d $database -c "select deliveries_generate(scaleFactor := ${scalefactor})"

