# SPDX-FileCopyrightText: Copyright (C) ARDUINO SRL (http://www.arduino.cc)
#
# SPDX-License-Identifier: MPL-2.0

import math
from datetime import datetime
from collections import deque
from arduino.app_bricks.web_ui import WebUI
from arduino.app_utils import App, Bridge

got_data = False
# Fixed-size rolling window — holds last N readings
MAX_READINGS = 100
que = deque(maxlen=MAX_READINGS)

def record(val1, val2):
    global que
    
    # Push a new measurement. Oldest auto-evicted when full.
    ts = datetime.now()
    que.append((ts.isoformat(), val1, val2))

def latest(n=10):
    global que
    
    # Return n most recent readings.
    return list(que)[-n:]

def snapshot():
    global que
    
    # Full copy as a list.
    return list(que)


def record_pm_values(pm25: float, pm10: float):
    global got_data
    record(pm25, pm10)
    # Will make these debug statements with config shortly...
    print("record_pm_values", round(pm25, 2), "and", round(pm10, 2))
    ts, a, b = que[-1]          # newest
    got_data = True

def on_get_fake():
    if got_data:
        print("Returning resources...")
        ts, p25, p10 = que[-1]
        return {"ts": ts, "pm25": p25, "pm10": p10}
    else:
        return {"ts": 0, "pm25": 1, "pm10": 2}

# Get data and format for the UI graphs defined in JS
def make_series(is25, meas):
    if not meas:
        return []

    points = []
    for ts, a, b in meas:
        t = datetime.fromisoformat(ts)
        epoch_ms = t.timestamp() * 1000   # matches JS's new Date().getTime()
        points.append({
            "time": epoch_ms,
            "value": a if is25 else b,
        })
    return points

def get_graph_configs():
    meas = snapshot()
    if not meas:
        max_b = 10
        max_a = 13
    else:
        # PM 10 max
        max_b = max(row[2] for row in meas)
        # PM 2.5 max
        max_a = max(row[1] for row in meas)

    # Returns the full list of panel graphs. Called on every request; live data,
    # Defines the graphs for the JS code
    return [
        {
            "title": "PM 2.5",
            "meta": "PM2.5 < 12 µg/m^3 of particles that are 2.5 micrometers or smaller and no spikes > 35",
            "color": "#4fd1c5",
            "xLabel": f"{datetime.now().strftime('%Y-%m-%d')}",
            "yLabel": "PM2.5 (µg/m^3)",
            "yMax": max_a if max_a > 13 else 13,
            "xUnit": "min",
            "data": make_series(True, meas),
        },
        {
            "title": "PM 10",
            "meta": "PM10 < 55 µg/m^3 of particles that are 10 micrometers or smaller",
            "color": "#f6ad55",
            "xLabel": "Time (h:m)",
            "yLabel": "PM10 (µg/m^3)",
            "yMax": max_b if max_b > 15 else 15,
            "xUnit": "min",
            "data": make_series(False, meas),
        },
    ]

# API exposed to the web page
def on_get_graphs():
    return get_graph_configs()

ui = WebUI()
ui.expose_api("GET", "/api/graphs", on_get_graphs)

print("Registering 'record_sensor_samples' callback.")
Bridge.provide("record_pm_values", record_pm_values)

print("Starting App...")
App.run()
