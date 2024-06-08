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

### Dependency
As a requirement you need [MobilityDB-BerlinMOD](https://github.com/MobilityDB/MobilityDB-BerlinMOD) to run the requests.
You can use my personnal script for installation of the dependency.

### Data generation

In order to generate data with our conceptual model you can use this command with parameter s to define the scalefactor

* Change directory to the scripts one 

        cd Work/Scripts/berlinmod_script
* Run the scripts with a defined scalefactor (0.005 if not defined)

        ./berlinmod_runall.sh -s $scalefactor

### AIS Data
The requests and queries work using AIS data you need to download and push it in the database. 

### Repository Structure 


```md
Data/       -- This directory contains all the data files used in the project.
Thesis/     -- This directory contains the thesis documents and related files.
Work/       -- This directory is the main workspace for the project and contains the following subdirectories
├── BuildScript/    -- Contains build scripts for the project.
├── C/              -- Contains C language source code files for testing and generation.
├── PythonCode/     -- Contains Python scripts and Proof of Concept. 
├── Scripts/        -- Contains various scripts used in the project.
│ ├── berlinmod_script/     -- Scripts specific to BerlinMOD.
│ ├── sql/                  -- SQL scripts for database operations.
│ ├── credentials           -- credentials files that contains user and db info for all scripts
├── SquishE/        -- Contains the implementation of the SQUISH-E algorithm.
└── View/           -- Contains files related to the project's user interface and visualization.
│ ├──Dashboard/             -- Contains dashboard-related files.
│ ├──ReactView/             -- Contains files for the React front-end.
│ └── pgtileserv/           -- Contains files for the pgtileserv service.
```