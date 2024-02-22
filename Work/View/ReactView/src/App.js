import React, {useEffect, useState} from 'react';
import DeckGL from '@deck.gl/react';
import {MVTLayer} from '@deck.gl/geo-layers';
import {StaticMap} from "react-map-gl";

const TRAJECTORY_URL = 'http://localhost:7800/public.linesimpl/{z}/{x}/{y}.pbf';
const MAP_STYLE = 'https://basemaps.cartocdn.com/gl/positron-nolabels-gl-style/style.json';

const INITIAL_VIEW_STATE = {
  longitude: 4.383406,
  latitude: 50.815338,
  zoom: 11,
  minZoom: 0,
  maxZoom: 23,
};

function App() {

  const [trajectoryParameter, setTrajectoryParameter] = useState('');
  const [trajectoryMMSIParameter, setTrajectoryMMSIParameter] = useState('-1');
  const [trajectoryURL, setTrajectoryURL] = useState('');
  const [trajectoryLayerVisibility, setTrajectoryLayerVisibility] = useState(true);


  const layers = [
    new MVTLayer({
      id: 'trajectoryLayer',
      data: trajectoryURL,
      minZoom: 0,
      maxZoom: 22,
      getLineColor: (d) => {
        console.log(d);
        if (d.properties.sim === 'S'){
          return [255,0,0]
        }  else {
          return [0,0,255]
        }
      },
      getLineWidth : 2,
      lineWidthMinPixels : 2,
      visible: trajectoryLayerVisibility,
    })
  ];

  // Handle input changes and button clicks
  const handleTrajectoryInputChange = (e) => setTrajectoryParameter(e.target.value);
  const handleTrajectoryMMSIInputChange = (e) => setTrajectoryMMSIParameter(e.target.value);
  const toggleTrajectoryLayerVisibility = () => setTrajectoryLayerVisibility(!trajectoryLayerVisibility);
  const updateTrajectoryParameter = () => setTrajectoryURL(TRAJECTORY_URL + `?s=` + trajectoryParameter + '&mmsi_=' + trajectoryMMSIParameter );




  return (
      <div style={{ position: 'relative', height: '100vh' }}>
        <DeckGL
            layers={layers}
            initialViewState={INITIAL_VIEW_STATE}
            controller={true}
        >
          <StaticMap mapStyle={MAP_STYLE} />
        </DeckGL>
        <div style={{ position: 'absolute', top: 0, left: 0, padding: '10px' }}>

          <div style={{marginTop: '10px'}}>
            <input type="number" value={trajectoryParameter} onChange={handleTrajectoryInputChange}/>
            <button onClick={updateTrajectoryParameter}>Update s</button>
            <input type="number" value={trajectoryMMSIParameter} onChange={handleTrajectoryMMSIInputChange}/>
            <button onClick={updateTrajectoryParameter}>Update MMSI</button>

            <button onClick={toggleTrajectoryLayerVisibility}>
              {trajectoryLayerVisibility ? 'Hide trajectory Layer' : 'Show trajecotry Layer'}
            </button>
          </div>

        </div>
      </div>
  );
}

export default App;