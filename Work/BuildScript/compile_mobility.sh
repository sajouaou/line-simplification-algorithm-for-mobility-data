#!/bin/bash

#This file compile and remove previous build file it receive as parameter r or m
# r for remove it removes previous build file
# m for meos it build the meos library for C

can_remove=false
is_meos=false
while getopts "rm" option; do
case $option in
  r)
     can_remove=true
    ;;

  m)
     can_remove=true
     is_meos=true
    ;;

esac
done

if  $can_remove  ;then
  rm -r --force MobilityDB/build
  mkdir MobilityDB/build
  cd MobilityDB/build

  if  $is_meos ;then
    cmake .. -DMEOS=on
  else
    cmake ..
  fi

else
  cd MobilityDB/build
fi

make
sudo make install
cd ../../