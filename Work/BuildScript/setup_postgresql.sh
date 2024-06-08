#!/bin/bash

# This file change the user settings of postgres from the credentials File

source ../Scripts/creditentials
sudo service postgresql start
sudo -u postgres psql -c "ALTER USER postgres PASSWORD '${password}';"
sudo service postgresql stop
