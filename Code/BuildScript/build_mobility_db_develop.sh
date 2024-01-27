#!/bin/bash


#./install_dependance.sh
git clone --branch develop https://github.com/MobilityDB/MobilityDB



./change_file.sh


mkdir MobilityDB/build
cd MobilityDB/build
cmake ..
make
sudo make install
cd ../../
#rm -r --force MobilityDB
./setup_postgresql.sh