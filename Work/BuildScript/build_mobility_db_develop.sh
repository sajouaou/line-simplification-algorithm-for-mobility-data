#!/bin/bash

#This script build mobility and add the implementation of SQUISH-E into mobilityDB

#./install_dependance.sh
git clone --branch develop https://github.com/MobilityDB/MobilityDB



./change_file.sh

./compile_mobility.sh -r

./setup_postgresql.sh