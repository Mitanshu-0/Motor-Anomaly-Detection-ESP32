# Node-RED Flow

This folder contains the Node-RED flow used in the **Edge AI-Based Predictive Maintenance System**. Node-RED acts as the middleware between the ESP32 edge device and the InfluxDB database, enabling real-time data processing and visualization.

---

## Overview

The ESP32 publishes sensor readings and AI inference results to an MQTT broker. Node-RED subscribes to these topics, processes the incoming data, and stores it in InfluxDB for visualization in Grafana.

---

## Workflow

```
ESP32
   │
   ▼
MQTT Broker (Mosquitto)
   │
   ▼
Node-RED
   │
   ▼
Data Processing
   │
   ▼
InfluxDB
   │
   ▼
Grafana Dashboard
```

---

## Flow Preview

> Replace the image below with your exported Node-RED flow screenshot.

![Node-RED Flow](node-red-flow.png)

---

## MQTT Topics

The ESP32 publishes the following MQTT topics:

| Topic | Description |
|--------|-------------|
| `motor/temp` | Motor temperature (°C) |
| `motor/vibration` | Motor vibration magnitude |
| `motor/raw_mse` | Raw reconstruction error from the AI model |
| `motor/avg_mse` | Sliding window average anomaly score |
| `motor/status` | Motor health status (0 = Normal, 1 = Anomaly) |

---

## Data Processing

The Node-RED flow performs the following operations:

- Subscribes to MQTT topics published by the ESP32.
- Collects temperature, vibration, and AI anomaly data.
- Converts incoming payloads to numeric values.
- Combines all sensor data into a single telemetry record.
- Writes processed data to InfluxDB.
- Enables real-time visualization through Grafana.

---

## Files

| File | Description |
|------|-------------|
| `flow.json` | Exported Node-RED flow |
| `node-red-flow.png` | Screenshot of the Node-RED flow |

---

## Importing the Flow

1. Open Node-RED.
2. Click **Menu → Import**.
3. Select **flow.json**.
4. Configure the MQTT broker if required.
5. Configure the InfluxDB connection.
6. Deploy the flow.

---

## Software Used

- Node-RED
- Mosquitto MQTT Broker
- InfluxDB
- Grafana

---

## Purpose

This Node-RED flow enables seamless communication between the ESP32 edge device and the visualization platform by providing:

- Real-time MQTT communication
- Data formatting and processing
- Time-series database integration
- Reliable data pipeline for dashboard visualization

---

## Project

**Edge AI-Based Predictive Maintenance using ESP32 and TensorFlow Lite Micro**

This project performs real-time motor health monitoring using temperature and vibration sensors, detects anomalies using an Autoencoder deployed on the ESP32, and visualizes the results using MQTT, Node-RED, InfluxDB, and Grafana.
