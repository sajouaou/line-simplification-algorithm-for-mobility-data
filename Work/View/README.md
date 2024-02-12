# Visualization Component

## Overview

This part of the project deals with the visualization interface, which is supported by `pgtileserver` and a React-based frontend. Follow the instructions below to get the visualization component up and running on your system.

## Prerequisites

Before you start, ensure you have the following installed:
- SQL function implemented (SQUISH-E,Visualisation,etc..) see folder Script/sql
- Node.js and npm are installed (for React).
- Access to a terminal or command line interface.

## Getting Started

To launch the visualization program, you will need to run two services: `pgtileserver` and the React application. This will require the use of two separate terminal windows or Linux shells.

### Starting `pgtileserver`

1. Open your terminal and change the directory to the `pgtileserver` folder:

```shell
cd pgtileserv
./run_pgtileserv.sh
```
pgtileserver should now be up and serving tiles at the designated port.

### Launching the `React Application`

Open another terminal session to start the React frontend.

Navigate to the React application directory:

```shell
cd ReactView
npm start
```

The React application will compile and should automatically open in your default web browser.

### Using the Visualization

With both pgtileserver and the React application running, you can interact with the visualization through the web interface. The pgtileserver needs to be operational for the React application to successfully retrieve and display map tiles.

### Support

For any assistance or if you encounter issues, please raise an issue in the project's repository or contact the maintainers directly.

Thank you for using the visualization component of our project!