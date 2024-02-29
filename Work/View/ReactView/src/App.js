import React, {useEffect, useState} from 'react';
import DeckGL from '@deck.gl/react';
import {MVTLayer, TripsLayer} from '@deck.gl/geo-layers';
import {StaticMap} from "react-map-gl";
import TimeSlider from "./Slider";

const TRAJECTORY_URL = 'http://localhost:7800/public.linesimplanim/{z}/{x}/{y}.pbf';
const MAP_STYLE = 'https://basemaps.cartocdn.com/gl/positron-nolabels-gl-style/style.json';
var RGBvalues = (function() {

  var _hex2dec = function(v) {
    return parseInt(v, 16)
  };

  var _splitHEX = function(hex) {
    var c;
    if (hex.length === 4) {
      c = (hex.replace('#','')).split('');
      return [
         _hex2dec((c[0] + c[0])),
         _hex2dec((c[1] + c[1])),
        _hex2dec((c[2] + c[2]))
      ];
    } else {
      return [
        _hex2dec(hex.slice(1,3)),
         _hex2dec(hex.slice(3,5)),
         _hex2dec(hex.slice(5))
      ];
    }
  };

  var _splitRGB = function(rgb) {
    var c = (rgb.slice(rgb.indexOf('(')+1, rgb.indexOf(')'))).split(',');
    var flag = false, obj;
    c = c.map(function(n,i) {
      return (i !== 3) ? parseInt(n, 10) : flag = true, parseFloat(n);
    });
    obj = {
      r: c[0],
      g: c[1],
      b: c[2]
    };
    if (flag) obj.a = c[3];
    return obj;
  };

  var color = function(col) {
    var slc = col.slice(0,1);
    if (slc === '#') {
      return _splitHEX(col);
    } else if (slc.toLowerCase() === 'r') {
      return _splitRGB(col);
    } else {
      console.log('!Ooops! RGBvalues.color('+col+') : HEX, RGB, or RGBa strings only');
    }
  };

  return {
    color: color
  };
}());


const INITIAL_VIEW_STATE = {
  longitude: 4.383406,
  latitude: 50.815338,
  zoom: 11,
  minZoom: 0,
  maxZoom: 23,
};

function App({
               trailLength = 180,
               animationSpeed = 5
             }) {

  const [trajectoryMMSIParameter, setTrajectoryMMSIParameter] = useState('-1');
  const [trajectoryURL, setTrajectoryURL] = useState('');
  const [trajectoryLayerVisibility, setTrajectoryLayerVisibility] = useState(true);

  const [trajectoryParameter, setTrajectoryParameter] = useState(['-1']);
  const [color, setColor] = useState(["#e66465"]);
  const [algo, setAlgo] = useState(['-1']);

  const [time, setTime] = useState(0);
  const [animation] = useState({});
  const [isAnimated,setIsAnimated] = useState(false)
  const [minTimestamp, setMinTimestamp] = useState(0);
  const [maxTimestamp, setMaxTimestamp] = useState(0);
  const [minTimeURL, setMinTimeURL] = useState('');
  const [maxTimeURL, setMaxTimeURL] = useState('');
  const [loopLength,setLoopLength] = useState(1);

  function convertDateTime(input) {
    // Parse the input string into a Date object
    const date = new Date(input);

    // Inline function to pad numbers to two digits
    const pad = number => number < 10 ? '0' + number : number.toString();

    // Construct each part of the format using pad function inline
    const year = date.getFullYear();
    const month = pad(date.getMonth() + 1); // getMonth() is zero-indexed
    const day = pad(date.getDate());
    const hours = pad(date.getHours());
    const minutes = pad(date.getMinutes());
    const seconds = '00'; // Seconds are not in the input, assumed to be '00'

    // Combine parts into the desired format with the timezone offset +01
    return `${year}-${month}-${day} ${hours}:${minutes}:${seconds}`
  }

  const animate = () => {
    if (isAnimated){
      setTime(t => (t + animationSpeed) % loopLength);
      animation.id = window.requestAnimationFrame(animate);
    }
  };

  useEffect(
      () => {
        console.log("useEffect : ",isAnimated);
        if  (isAnimated){
          animation.id = window.requestAnimationFrame(animate);
        }
        return () => window.cancelAnimationFrame(animation.id);
      },
      [animation,isAnimated]
  );

  const onTileLoad = (tile) => {
    const features = [];
    console.log("--- Begin Load ----");
    console.log(tile);
    console.log("---------------")
    if (tile.content && tile.content.length > 0) {
      console.log("--- If passed ----");
      for (const feature of tile.content) {
        console.log("--- For If passed ----");
        const ts = feature.properties.times;
        const ts_final = ts.substring(1, ts.length - 1).split(",").map(t => parseInt(t, 10)-minTimestamp);

        // slice Multi into individual features
        if (feature.geometry.type === "MultiLineString") {
          console.log("--- If For If passed ----");
          let index = 0;
          for (const coords of feature.geometry.coordinates) {
            console.log("--- For If For If passed ----");
            const ts_segment = ts_final.slice(index, index + coords.length)
            features.push({
              ...feature,
              geometry: {type: "LineString", coordinates: coords},
              // properties: {...feature.properties, timestamps: ts_segment}
              properties: {timestamps: ts_segment}
            });
            console.log("--- PUUUSH ----");

            index = coords.length;
          }
        } else {
          console.log("--- Other Push ----");
          // features.push({...feature, properties: {...feature.properties, timestamps: ts_final}});
          features.push({...feature, properties: {tripid: feature.properties.tripid, timestamps: ts_final}});
        }
      }
    }
    tile.content = features;
  };





  const layers = [
    new MVTLayer({
      id: 'trajectoryLayer',
      data : trajectoryURL,
      minZoom: 0,
      maxZoom: 23,
      lineWidthMinPixels: 200,
      currentTime: time, // it has to be here, not inside the TripsLayer
      // loadOptions: {mode: 'no-cors'},
      renderSubLayers: props => {
        return new TripsLayer(props, {
          data: props.data,
          getPath: d => d.geometry.coordinates,
          getTimestamps: d => d.properties.timestamps,
          getColor: (d) => {
            console.log("----------------  ",d.properties.mmsi," ---------------");
            console.log(d.properties.index);
            console.log(d.properties.size);
            return RGBvalues.color(color[d.properties.index]);
          },
          opacity: 0.5,
          widthMinPixels: 200,
          caprounded: true,
          trailLength,
        });
      }
    }),

    new MVTLayer({
      id: 'trajectoryLayer2',
      data: trajectoryURL,
      minZoom: 0,
      maxZoom: 22,
      getLineColor: (d) => {
        console.log("----------------  ",d.properties.mmsi,"  FIX LAYER ---------------")
        console.log(d.properties.index);
        console.log(d.properties.size);

        return RGBvalues.color(color[d.properties.index]);
      },
      getLineWidth : 2,
      lineWidthMinPixels : 2,
      visible: trajectoryLayerVisibility,
    })

  ];



  // Handle input changes and button clicks

  const handleTrajectoryInputChange = (index, value) => {
    // Mettre à jour la valeur de lambda à l'index donné
    const newLambdaArray = [...trajectoryParameter];
    newLambdaArray[index] = value;
    setTrajectoryParameter(newLambdaArray);
  };

  const handleColor = (index, value) => {
    // Mettre à jour la couleur à l'index donné
    const newColorArray = [...color];
    newColorArray[index] = value;
    setColor(newColorArray);
  };

  const handleAlgo = (index, value) => {
    // Mettre à jour la couleur à l'index donné
    const newAlgoArray = [...algo];
    newAlgoArray[index] = value;
    setAlgo(newAlgoArray);
  };
  const handleTrajectoryMMSIInputChange = (e) => setTrajectoryMMSIParameter(e.target.value);
  const toggleTrajectoryLayerVisibility = () => setTrajectoryLayerVisibility(!trajectoryLayerVisibility);
  const updateTrajectoryParameter = () => {
    // Construire la chaîne des valeurs de s
    const sValues = trajectoryParameter.join(',');
    const aValues = algo.join(',');
    const start = convertDateTime(minTimeURL);
    const end = convertDateTime(maxTimeURL);

    // Mettre à jour la trajectoire avec toutes les valeurs

    setTrajectoryURL(TRAJECTORY_URL + `?s={${sValues}}&mmsi_=${trajectoryMMSIParameter}&algo={${aValues}}&p_start=${start}&p_end=${end}`);
  };

  const removeTrajectory = (index) => {
    // Retirer l'élément associé à l'index de chaque array
    const newLambdaArray = [...trajectoryParameter];
    newLambdaArray.splice(index, 1);
    setTrajectoryParameter(newLambdaArray);

    const newColorArray = [...color];
    newColorArray.splice(index, 1);
    setColor(newColorArray);

    const newAlgoArray = [...algo];
    newAlgoArray.splice(index, 1);
    setAlgo(newAlgoArray);
  };

  const addTrajectory =  () => {
    // Ajouter un élément vide à chaque array
    setTrajectoryParameter([...trajectoryParameter, '']);
    setColor([...color, '#000000']); // couleur par défaut
    setAlgo([...algo,'SQUISH-E']);
  };

  const startAnimation = () => {
    setTime(0);
    if (!isAnimated){
      setIsAnimated(true);
    }
  }


  const updateIsAnimated = () => setIsAnimated(!isAnimated);
  const handleMinTimeURLInputChange = (e) => setMinTimeURL(e.target.value);
  const handleMaxTimeURLInputChange = (e) => setMaxTimeURL(e.target.value);
  const  updateBothTimestamp = () => {
    console.log("UPDATE BOTH");
    if (minTimeURL !== '' && maxTimeURL !== '' ) {

      updateTrajectoryParameter();
      setMinTimestamp(new Date(minTimeURL).getTime()/1000);
      setMaxTimestamp(new Date(maxTimeURL).getTime()/1000);
      console.log(minTimestamp,maxTimestamp);
      setLoopLength(maxTimestamp-minTimestamp);
      updateTrajectoryParameter();
    }
    console.log('Loop Length : ',loopLength);
  };

  const setTimeFromSlider = (newTime) => {
    setTime(newTime)
  }


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

          <div>
            <TimeSlider
                setTimeApp={setTimeFromSlider}
                timeFromApp={time}
                maxfromApp={loopLength}
                dateFromApp={new Date((minTimestamp+time)*1000)}
            />
          </div>

          {trajectoryParameter.map((lambda, index) => (
              <div key={index}>
                {
                  index === 0 ? (
                      <>
                        <input
                            type="number"
                            value={trajectoryMMSIParameter}
                            onChange={handleTrajectoryMMSIInputChange}
                        />
                        <button onClick={updateTrajectoryParameter}>Refresh</button>
                        <input
                            type="color"
                            value={color[index]}
                            onChange={(e) => handleColor(index, e.target.value)}
                        />
                        <button onClick={toggleTrajectoryLayerVisibility}>
                          {trajectoryLayerVisibility ? 'Hide trajectory Layer' : 'Show trajecotry Layer'}
                        </button>
                      </>
                  ) : (
                      <>
                        <input
                            type="number"
                            value={lambda}
                            onChange={(e) => handleTrajectoryInputChange(index, e.target.value)}
                        />
                        <input
                            type="color"
                            value={color[index]}
                            onChange={(e) => handleColor(index, e.target.value)}
                        />
                        <input
                            type="text"
                            value={algo[index]}
                            list="algos"
                            autocomplete="off"
                            onChange={(e) => handleAlgo(index, e.target.value)}
                        />
                        <button onClick={() => removeTrajectory(index)}>-</button>
                      </>
                  )
                }


              </div>
          ))}



          <div>
            <input type="datetime-local" value={minTimeURL} onChange={handleMinTimeURLInputChange} />
            <input type="datetime-local" value={maxTimeURL} onChange={handleMaxTimeURLInputChange} />
            <button onClick={updateBothTimestamp}>Update Life Time</button>
          </div>
          <div>
            <button onClick={startAnimation}>Start Animation</button>
            <button onClick={updateIsAnimated}>
              {isAnimated ? 'Stop Animation' : 'Replay Animation'}
            </button>
          </div>

          <datalist id="algos" autocomplete="off" mlns="http://www.w3.org/1999/xhtml">
            <option>DOUGLAS</option>
            <option>SQUISH-E</option>
            <option>MINDIST</option>
          </datalist>
          <button onClick={addTrajectory}>
            +
          </button>
        </div>
      </div>
  );
}

export default App;