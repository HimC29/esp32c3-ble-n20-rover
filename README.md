# esp32c3-ble-n20-rover

An open source tiny ESP32 C3 Super Mini car that is light and fast controlled by a website using BLE.

> This project is still in development and is not stable to use yet.

## Components List

### Core Electronics & Modules
* **1x** ESP32-C3 SuperMini (`U1`)
* **1x** TB6612FNG Motor Driver Module (ROB-14450) (`U2`)
* **1x** MT3608 DC-DC Step-Up Boost Converter Module (`U3`)
* **1x** TP4056 USB-C LiPo Charger Module w/ Protection (`U4`)

### Power & Actuators
* **1x** 3.7V 1000mAh LiPo Battery (`BT1`) (102040)
* **2x** N20 Micro Gear Motors 500RPM 5V-6V (`M1`, `M2`)
* **1x** SPST Toggle / Power Switch (`SW1`)

### Passives & Discrete Components
* **3x** 0.1µF Ceramic Capacitors (`C2`, `C3`, `C4`)
* **1x** 100µF Electrolytic Capacitor (`C1`)
* **2x** 100kΩ Resistors (`R1`, `R2`)
* **1x** Schottky Diode (`D1`) (1N5819)

### Mechanical & Hardware
* **2x** 34mm Wheels (for GA12 N20 shaft)
* **1x** Ball Caster Wheel (3rd support)
* **1x** Chassis / Rover Frame
* Connecting wires, headers, and hardware fasteners

## Wiring
Schematics can be found in [here](schematics/).

## Assembly & Setup
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
