#!/bin/bash

#This script install the dependance for MobilityDB and change the corresponding file

wget --quiet -O - https://www.postgresql.org/media/keys/ACCC4CF8.asc | sudo apt-key add -
sudo apt-get update
sudo apt install build-essential cmake libproj-dev libjson-c-dev postgresql-14-postgis-3 libgeos-dev libgsl-dev
sudo apt install postgresql-server-dev-14

from="#shared_preload_libraries = ''"
to="shared_preload_libraries = 'postgis-3'"
file="/etc/postgresql/14/main/postgresql.conf"
sudo sed -i "s/${from}/${to}/g" $file

from="#max_locks_per_transaction = 64"
to="max_locks_per_transaction = 128"
sudo sed -i "s/${from}/${to}/g" $file
