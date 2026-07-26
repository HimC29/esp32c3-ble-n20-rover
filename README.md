# esp32c3-ble-n20-motor

An open source tiny ESP32 C3 Super Mini car that is light and fast controlled by a website using BLE.

> This project is still in development and is not stable to use yet.

## Components:
- 1x ESP32 C3 Super Mini 
- 2x N20 Motor 500RPM 5V-6V
- 2x 34mm Wheel for GA12 N20
- 1x TB6612 Motor Driver (ROB-14450)
- 1x SX1308 Boost Convertor
- 1x 3.7V LiPo 1000mAh 102040 Battery
- TP4056 USB-C w/ Protection
And passive components:
- 3x 0.1uF Ceramic Capacitors
- 1x 100uF Electrolytic Capacitors
- 1x IN5819 Diode
- Extra stuff like wires, base for car, 3rd ball wheel, etc.

## Wiring
Schematics can be found in [here](schematics/).

## How to create
1. Gather all components and wire everything up according to the [schematics](schematics/).
2. Flash the firmware to your ESP32-C3:
   - Open the `firmware/` folder in PlatformIO.
   - Connect your ESP32-C3 via USB.
   - Run `pio run -t upload`.
3. Power on the rover, then open the controller website: **https://himc29.github.io/esp32c3-ble-n20-rover/**
   - **Note:** Web Bluetooth only works in Chromium-based browsers (Chrome, Edge). It is not supported on Safari or iOS at all.
   - If you'd rather self-host the controller instead, the `controller/` folder contains the site — serve it with any static file server. Web Bluetooth requires HTTPS (or `localhost`), so a plain local network address won't work without extra setup.
4. Tap **Connect** and select your device from the browser's Bluetooth picker.
5. Drive using the on-screen controls.

## License
Project is free and open sourced under the MIT License.
