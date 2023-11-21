#!/bin/bash

sudo apt install wget ca-certificates
wget --quiet -O - https://www.postgresql.org/media/keys/ACCC4CF8.asc | sudo apt-key add -
sudo apt update
sudo apt install postgresql-14-pgrouting
sudo apt install osm2pgrouting
sudo apt install osm2pgsql
sudo apt-get update && sudo apt-get upgrade