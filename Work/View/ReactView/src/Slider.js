import Slider from '@mui/material/Slider';
import React, {useEffect, useState} from 'react';
import {Box, Stack} from "@mui/material";

export default function TimeSlider({setTimeApp, dateFromApp, timeFromApp, maxfromApp}) {
    const [time, setTime] = React.useState(timeFromApp);

    const handleChange = (event, newValue) => {
        setTime(newValue)
        setTimeApp(time)
    };

    const formatDateFromApp = (date) => {
        const year = date.getFullYear();
        const month = (date.getMonth() + 1).toString().padStart(2, '0'); // Months are 0-indexed
        const day = date.getDate().toString().padStart(2, '0');
        const hour = date.getHours().toString().padStart(2, '0');
        const minute = date.getMinutes().toString().padStart(2, '0');
        const second = date.getSeconds().toString().padStart(2, '0');

        return `${day}/${month}/${year} ${hour}:${minute}:${second}`
    }

    return (
        <div>
            <h1 style={{color: "white"}}>{ formatDateFromApp(dateFromApp) }</h1>
            <Box sx={{ width: 200 }}>
                <Stack spacing={2} direction="row" sx={{ mb: 1 }} alignItems="center">
                    <Slider  min={0} max={maxfromApp} value={timeFromApp} onChange={handleChange} />
                </Stack>
            </Box>
        </div>
    );
}