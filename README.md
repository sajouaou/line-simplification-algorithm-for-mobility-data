SQUISH-E line simplification algorithm for MobilityDB
==================================

<img src="doc/images/mobilitydb-logo.svg" width="200" alt="MobilityDB Logo" />

[MobilityDB](https://github.com/ULB-CoDE-WIT/MobilityDB) is an open source software program that adds support for temporal and spatio-temporal objects to the [PostgreSQL](https://www.postgresql.org/) database and its spatial extension [PostGIS](http://postgis.net/).

This repository contains code and the documentation for running an implementation of SQUISH-E into [MobilityDB](https://github.com/ULB-CoDE-WIT/MobilityDB) using AIS Data.

Warning
-------------
In order to begin with this project you can configure our scripts (in the folder scripts of the repository) in order to be compliant 
with your internal configuration. Make sure to not let your password visible in deployment and change it. Here for an educational 
purpose passwords are simple but do not take those scripts as secure. 


How to use
-------------

## Dependency
As a requirement you need [MobilityDB-BerlinMOD](https://github.com/MobilityDB/MobilityDB-BerlinMOD) to run the requests.
You can use my personnal script for installation of the dependency.

## Data generation

In order to generate data with our conceptual model you can use this command with parameter s to define the scalefactor

* Change directory to the scripts one 

        cd Work/Scripts/berlinmod_script
* Run the scripts with a defined scalefactor (0.005 if not defined)

        ./berlinmod_runall.sh -s $scalefactor

## AIS Data
The requests and queries work using AIS data you need to download and push it in the database. 