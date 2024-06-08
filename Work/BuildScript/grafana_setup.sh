#!/bin/bash

# This script setup and install grafana

# Note These are instructions for Grafana Enterprise Edition (via APT repository),
# which they recommend. It includes all the Open Source features and can also use
# Enterprise features if you have a License.

# Setup Grafana Keys
sudo apt-get install -y apt-transport-https
sudo apt-get install -y software-properties-common wget
sudo wget -q -O /usr/share/keyrings/grafana.key https://apt.grafana.com/gpg.key

# Add repository for stable releases
echo "deb [signed-by=/usr/share/keyrings/grafana.key] https://apt.grafana.com stable main" | sudo tee -a /etc/apt/sources.list.d/grafana.list

sudo apt-get update

# Install the latest Enterprise release:
sudo apt-get install grafana-enterprise