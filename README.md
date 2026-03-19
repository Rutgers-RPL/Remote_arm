# Remote Arm

Monorepo for the ESP32-based remote arming system. Uses an ESP32 as a master device to receive commands over BLE and, if needed, relay commands to two other ESP32s using ESP-NOW.

## NOTE

Due to issues with connecting over BLE, the "single_board_bluetooth_classic" project is the most updated project here. Note that the bluetooth classic version of the project can only be used on Android. Ignore the information about the partition scheme and MAC Addresses when using the "single_board" project. The information about the USB to UART driver might still apply depending on your situation.

## Required Packages

In the Boards Manager, install the "esp32" boards package by "Espressif Systems" in Arduino IDE to deploy this.

## Important Notes

### Partion Scheme

The size of the "booster_bay_master" sketch is too large for the default partition scheme of the ESP32. To solve this, follow these steps:

1. First, go to `Tools` > `Board` > `esp32` and select "ESP32 Dev Module"
2. Then, go to `Tools` > `Partition Scheme` and select "Huge App"

After following the above steps, there shouldn't be any issue with deploying the "booster_bay_master" sketch.

### MAC Addresses

To communicate with the ignition bay and nose cone boards using ESP-NOW, the MAC addresses of these boards must be known. A board's MAC address can be found using the "get_mac_address" sketch, which will print that board's MAC address in normal text as well as in a hex array.

### USB to UART Driver

If you encounter issues with uploading code, consider installing the [USB to UART Driver](https://www.silabs.com/software-and-tools/usb-to-uart-bridge-vcp-drivers). This will likely solve your problem if it is the result of a "pySerial" error.