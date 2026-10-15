# Smart Distance Monitoring IoT System

## Project Overview

The Smart Distance Monitoring IoT System uses an ESP32 and an HC-SR04 ultrasonic sensor to measure distance. The ESP32 sends readings through Wi-Fi and MQTT to a browser dashboard.

## Objectives

- Measure distance using an ultrasonic sensor.
- Publish readings over MQTT.
- Display sensor readings on a browser dashboard.
- Retry Wi-Fi and MQTT connections after disconnection.
- Temporarily buffer readings while offline.
- Consider power consumption, reliability and security.

## System Architecture

**HC-SR04 Sensor → ESP32 → Wi-Fi → MQTT Broker → Browser Dashboard**

The ESP32 also controls an LED and stores a limited number of readings in RAM while MQTT is unavailable.

See `system-diagram.md` for the architecture diagram.

## Components

- ESP32 DevKit
- HC-SR04 ultrasonic sensor
- LED
- 220-ohm resistor
- Jumper wires
- Wokwi simulator

## Pin Connections

| Component | ESP32 Pin |
|---|---|
| HC-SR04 VCC | 5V / VIN |
| HC-SR04 GND | GND |
| HC-SR04 TRIG | GPIO 5 |
| HC-SR04 ECHO | GPIO 18 |
| LED anode (+) | GPIO 2 through 220-ohm resistor |
| LED cathode (-) | GND |

For physical ESP32 hardware, protect the 3.3V GPIO input from the sensor's 5V ECHO signal with a suitable voltage divider.

## Software and Tools

- Arduino C++
- Wokwi ESP32 Simulator
- PubSubClient library
- Wi-Fi
- MQTT
- HTML, CSS and JavaScript
- MQTT over WebSocket

## How to Run

1. Open the ESP32 circuit in Wokwi.
2. Connect the sensor and LED according to the pin table.
3. Add the PubSubClient library.
4. Run the simulation and open Serial Monitor.
5. Verify Wi-Fi and MQTT connection messages.
6. Open `dashboard/index.html` using VS Code Live Server.
7. Verify that incoming sensor readings appear on the dashboard.

## Reliability

The firmware retries Wi-Fi and MQTT connections. Up to 20 valid readings can be temporarily stored in RAM. If the buffer is full, the oldest reading is removed. The buffer is lost when the device restarts.

## Power Considerations

Use a suitable power supply for the ESP32 and sensor. The LED uses a current-limiting resistor. For battery operation, measure actual power consumption and consider reducing sensor sampling frequency or using appropriate sleep modes.

## Security Considerations

This prototype uses a public MQTT broker for demonstration. Do not publish private or sensitive information. A real deployment should use an authenticated MQTT broker, encrypted connections and protected credentials. Do not commit passwords, private keys or access tokens to GitHub.

## Repository Structure

```text
firmware/
  sketch.ino
  libraries.txt
dashboard/
  index.html
system-diagram.md
README.md
```

## Author

Mathavi E

## Testing Evidence

Add the Wokwi simulation link and screenshots of successful MQTT publishing and dashboard readings after testing. Update this section with actual results before submission.
