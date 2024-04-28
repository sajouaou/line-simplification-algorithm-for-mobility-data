SELECT mmsi, ST_AsGeoJSON(trip::geometry) as trip FROM aistrips where mmsi in ($mmsi)

-- Orchestra Cities Map (plugin)