# ESP8266 Wireless Sensor Network (WSN)

This project implements a wireless sensor network using ESP8266 boards. A sensor node reads temperature and humidity from a DHT11 sensor and sends the data to a sink node over ESP-NOW. The sink receives each packet, updates the live dashboard, and writes every reading to a CSV log stored in the ESP8266 flash memory through LittleFS.

## Project structure

- [sensor node/esp2866_hdt.ino](sensor%20node/esp2866_hdt.ino) – sensor node code
- [sink/esp8266_sink.ino](sink/esp8266_sink.ino) – sink node and web dashboard code
- [sink/Storage.h](sink/Storage.h) – LittleFS-based CSV logger for storing received sensor measurements

## Features

- DHT11 temperature and humidity sensing
- ESP-NOW wireless communication between sensor node and sink
- Periodic transmissions every 5 seconds
- Web dashboard served by the sink on a local access point
- JSON endpoint for live telemetry data
- Local CSV logging to flash storage using LittleFS on the sink
- Persistent record of measurements for later inspection or debugging

## Hardware

### Sensor node

- ESP8266 board (NodeMCU/ESP-12E compatible)
- DHT11 sensor
- 5V power source

### Sink

- ESP8266 board
- Wi-Fi access point enabled on the sink
- Optional local monitoring via browser

## Wiring

### DHT11 to ESP8266

Connect the DHT11 data pin to GPIO D2 on the sensor node.

Typical wiring:

- DHT11 VCC -> 3.3V or 5V (depending on module)
- DHT11 GND -> GND
- DHT11 DATA -> D2

## Software architecture

### Sensor node

The sensor node:

1. Initializes Wi-Fi in station mode
2. Configures ESP-NOW
3. Adds the sink as a peer
4. Reads DHT11 values every 5 seconds
5. Sends a packet containing:
   - node ID
   - temperature
   - humidity
   - packet counter

### Sink

The sink:

1. Starts in AP + STA mode
2. Creates a Wi-Fi access point named WSN-SINK-dadna
3. Initializes ESP-NOW as a receiver
4. Collects incoming sensor packets
5. Saves each received reading to /measurements.csv using LittleFS
6. Serves a webpage at the root URL
7. Exposes live sensor data through /data as JSON

## Required libraries

Install these libraries in the Arduino IDE or Arduino CLI:

- ESP8266WiFi
- ESP8266WebServer
- espnow
- DHT11
- LittleFS (built into ESP8266 core)

## Setup instructions

### 1. Install the ESP8266 core

In the Arduino IDE:

- Go to File > Preferences
- Add the ESP8266 board manager URL
- Install the ESP8266 package from Boards Manager

### 2. Open the project files

- Open [sensor node/esp2866_hdt.ino](sensor%20node/esp2866_hdt.ino)
- Open [sink/esp8266_sink.ino](sink/esp8266_sink.ino)

### 3. Configure the sink address

In the sensor node code, the sink MAC address is defined as:

```cpp
uint8_t sinkAddress[] = {0x84, 0x0D, 0x8E, 0xA5, 0x37, 0x7B};
```

Replace it with the actual MAC address of the sink ESP8266 board.

### 4. Upload the code

Upload the sensor node sketch to the sensor ESP8266 and the sink sketch to the sink ESP8266.

### 5. Connect to the sink

After the sink boots, connect to the access point:

- SSID: WSN-SINK-dadna
- Password: WSN12345

Then open the browser and visit:

```text
http://192.168.4.1/
```

or the IP shown in the serial monitor.

## Data flow

```text
DHT11 -> ESP8266 sensor node -> ESP-NOW -> ESP8266 sink -> Web UI / JSON endpoint
                                                   |
                                                   v
                                             measurements.csv
                                             (LittleFS on flash)
```

## Endpoints

### Root page

```text
http://<sink_ip>/
```

Displays:

- current temperature
- current humidity
- connection status
- packet counter
- age of last received data

### JSON data

```text
http://<sink_ip>/data
```

Returns JSON like:

```json
{"valid":true,"temperature":24.3,"humidity":48.5,"counter":12,"age":2}
```

## LittleFS storage

The sink includes [sink/Storage.h](sink/Storage.h), which stores each received sensor packet in a CSV file named measurements.csv. This is saved inside the ESP8266 flash filesystem using LittleFS, so the data remains on the board instead of being lost when the program runs or the device reboots.

The CSV file is created in the filesystem at:

```text
/measurements.csv
```

This file is stored on the ESP8266 flash memory, not on the computer. The sink calls WSNStorage::begin() during setup and WSNStorage::append() every time a packet is received.

The CSV file columns are:

```text
elapsed_seconds,node_id,temperature_c,humidity_percent,packet_counter
```

## Storage behavior

This project stores measurements locally on the sink ESP8266, not on a computer or cloud service.

- Storage type: LittleFS filesystem on the ESP8266 flash
- File name: /measurements.csv
- Purpose: keep a historical log of temperature, humidity, timestamps, and packet counters
- How it works: every ESP-NOW packet received by the sink is appended to the CSV immediately
- Use case: monitoring, debugging, and later analysis of sensor data

## Troubleshooting

### No sensor data appears

- Check the DHT11 wiring
- Confirm the sink MAC address matches the actual sink board
- Verify both ESP8266 devices are powered correctly
- Check the serial monitor for errors

### Web page not loading

- Connect to the sink access point first
- Confirm the sink is running the web server code
- Verify the IP address printed in the serial monitor

### ESP-NOW initialization fails

- Ensure both boards use compatible ESP8266 settings
- Confirm the board type and library versions are correct
- Try resetting the device and re-uploading the sketch

## Notes

This is a simple educational and prototype-oriented wireless sensor network. It is well suited for monitoring environmental conditions in a local lab or small-scale IoT deployment.

