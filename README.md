# Edge AI-Based Predictive Maintenance using ESP32 and TensorFlow Lite Micro

An Edge AI-powered predictive maintenance system that continuously monitors the health of rotating machinery using **temperature** and **vibration** sensors. The system performs **real-time anomaly detection directly on an ESP32** using a TensorFlow Lite Micro Autoencoder and visualizes the results through **MQTT**, **Node-RED**, **InfluxDB**, and **Grafana**.

---

## Features

- Real-time motor health monitoring
- Edge AI inference using TensorFlow Lite Micro
- Autoencoder-based anomaly detection
- Temperature monitoring using NTC Thermistor
- Vibration monitoring using MPU6050
- MQTT-based wireless communication
- Node-RED data processing
- InfluxDB time-series storage
- Grafana live dashboard
- Sliding window anomaly score smoothing
- Hysteresis-based stable anomaly detection
- Low-cost Industrial IoT solution

---

## System Architecture

```
                 +----------------------+
                 |      DC Motor        |
                 +----------+-----------+
                            |
          +-----------------+-----------------+
          |                                   |
          ▼                                   ▼
+--------------------+              +--------------------+
| NTC Thermistor     |              | MPU6050 IMU        |
| Temperature Sensor |              | Vibration Sensor   |
+--------------------+              +--------------------+
          |                                   |
          +-----------------+-----------------+
                            |
                            ▼
                  +------------------+
                  |      ESP32       |
                  | Edge AI Inference|
                  +------------------+
                            |
            TensorFlow Lite Micro Autoencoder
                            |
              Reconstruction Error Calculation
                            |
           Sliding Window + Hysteresis Logic
                            |
                            ▼
                    MQTT Publisher
                            |
                            ▼
                  Mosquitto MQTT Broker
                            |
                            ▼
                       Node-RED
                            |
                            ▼
                       InfluxDB
                            |
                            ▼
                  Grafana Dashboard
```

---

## Project Workflow

1. Read temperature using an NTC thermistor.
2. Read vibration using the MPU6050 accelerometer.
3. Calibrate sensor readings.
4. Extract vibration magnitude.
5. Normalize sensor data.
6. Perform Edge AI inference using TensorFlow Lite Micro.
7. Calculate reconstruction error.
8. Smooth anomaly score using a sliding window.
9. Detect anomalies using hysteresis thresholds.
10. Publish telemetry using MQTT.
11. Store data in InfluxDB through Node-RED.
12. Visualize real-time motor health using Grafana.

---

## Hardware Used

| Component | Description |
|----------|-------------|
| ESP32 DevKit | Edge AI Controller |
| MPU6050 | 3-Axis Accelerometer & Gyroscope |
| NTC Thermistor (10K) | Temperature Sensor |
| 10KΩ Resistor | Voltage Divider |
| Breadboard | Circuit Assembly |
| Jumper Wires | Connections |
| DC Motor | Test Machine |

---

## Software Stack

| Software | Purpose |
|----------|---------|
| Arduino IDE | ESP32 Firmware Development |
| TensorFlow Lite Micro | Edge AI Inference |
| Mosquitto | MQTT Broker |
| Node-RED | Data Processing |
| InfluxDB | Time-Series Database |
| Grafana | Dashboard Visualization |

---

## Technologies Used

- Embedded Systems
- TinyML
- Edge AI
- TensorFlow Lite Micro
- Internet of Things (IoT)
- MQTT
- Node-RED
- InfluxDB
- Grafana
- Arduino Framework
- ESP32
- C++

---

## Repository Structure

```
Motor-Anomaly-Detection-ESP32/
│
├── README.md
├── LICENSE
├── .gitignore
├── requirements.md
│
├── assets/
│   ├── architecture.png
│   ├── dashboard.png
│   └── node-red-flow.png
│
├── Firmware/
│   ├── ESP32_AI_Monitor.ino
│   ├── model_data.h
│   └── libraries_used.txt
│
├── Node-RED/
│   ├── flow.json
│   ├── node-red-flow.png
│   └── README.md
│
└── Grafana/
    ├── dashboard.json
    ├── dashboard.png
    └── README.md
```

---

## Dashboard

The Grafana dashboard provides live monitoring of:

- Motor Health Status
- Current Temperature
- Current Vibration
- Current AI Anomaly Score
- Temperature Trend
- Vibration Trend
- Average Anomaly Score
- Motor Health Gauge

### Dashboard Preview

> Replace the image below with your dashboard screenshot.

![Grafana Dashboard](assets/dashboard.png)

---

## Node-RED Flow

Node-RED acts as the middleware between the ESP32 and InfluxDB.

Functions performed:

- MQTT Subscription
- Data Processing
- Payload Formatting
- Database Storage
- Data Routing

### Flow Preview

> Replace the image below with your Node-RED flow screenshot.

![Node-RED Flow](assets/node-red-flow.png)

---

## Edge AI Model

The project uses an **Autoencoder Neural Network** deployed on the ESP32 using **TensorFlow Lite Micro**.

### Input Features

- Temperature
- Vibration

### Output

- Reconstructed Input

### Anomaly Detection

The reconstruction error is used as the anomaly score.

A higher reconstruction error indicates abnormal motor behavior.

---

## MQTT Topics

| Topic | Description |
|--------|-------------|
| motor/temp | Temperature |
| motor/vibration | Vibration |
| motor/raw_mse | Raw AI Score |
| motor/avg_mse | Averaged AI Score |
| motor/status | Motor Health Status |

---

## Key Features Implemented

- ESP32 Sensor Interfacing
- MPU6050 Calibration
- NTC Temperature Measurement
- TensorFlow Lite Micro Deployment
- Autoencoder Inference
- Sliding Window Filtering
- Hysteresis Decision Logic
- MQTT Communication
- Node-RED Integration
- InfluxDB Storage
- Grafana Visualization
- Real-Time Monitoring

---

## Future Improvements

- Support multiple motors
- Cloud integration (AWS IoT / Azure IoT)
- LSTM-based anomaly detection
- Mobile application
- OTA firmware updates
- Additional sensors (Current, Acoustic, Bearing Temperature)

---

## Requirements

See **requirements.md** for the required software, libraries, and hardware.

---

## License

This project is licensed under the **MIT License**.

---

## Author

**Mitanshu Dhameliya**

Bachelor of Engineering (Electronics & Communication Engineering)

Nirma University

---

## Acknowledgements

This project was developed as part of the Bachelor's Final Year Project and demonstrates the integration of **Embedded Systems**, **TinyML**, **Industrial IoT**, and **Edge AI** for predictive maintenance of rotating machinery.
