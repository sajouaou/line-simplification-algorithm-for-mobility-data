Line Simplification Algorithm DashBoard
==================================
<img src="../../../doc/images/mobilitydb-logo.svg" width="200" alt="MobilityDB Logo" />

This folder contains the various queries that can be implemented on grafana


How to use
-------------

First of all you need to install Grafana in order to have access to the Dashboard

## Create DashBoard

* Run Grafana Service

         sudo service grafana-server start
* Navigate into the grafana local server

         localhost:3000
* Set a new password (The default one is admin/admin)
* Add PostgreSQL DataSource

  <img src="../../../doc/images/datasource.jpg" alt="Configuration Datasource" />
  <img src="../../../doc/images/add_new_datasource.jpg"  alt="Adding Datasource" />
* Create a new DashBoard

  <img src="../../../doc/images/new_dashboard.jpg"  alt="New DashBoard" />


## Add Panel from sql request

In order to use the request in this folder for the dashboard you need to :

* Add a New Panel

  <img src="../../../doc/images/add_panel.jpg"  alt="Add a New Panel" />
* Select your PostgreSQL data source
* Switch from builder to code

  <img src="../../../doc/images/panel_to_code.jpg"  alt="Add a New Panel" />
* Put Inside the code from sql file
* Check comment in sql file to know which type of panel