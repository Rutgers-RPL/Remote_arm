# Remote Arm

Monorepo for the ESP32-based remote arming system. Uses an ESP32 as a master device to receive commands over BLE and, if needed, relay commands to two other ESP32s using ESP-NOW.

## Required Packages

In the Boards Manager, install the "esp32" boards package by "Espressif Systems" in Arduino IDE to deploy this.

## Important Notes

The size of the "booster_bay_master" project is too large for the default partition scheme of the ESP32. To solve this, follow these steps:

1. First, go to `Tools` > `Board` > `esp32` and select "ESP32 Dev Module"
2. Then, go to `Tools` > `Partition Scheme` and select "Huge App"

After following the above steps, there shouldn't be any issue with deploying the "booster_bay_master" project
