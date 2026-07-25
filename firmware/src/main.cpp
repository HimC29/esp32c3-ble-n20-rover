/* esp32c3-ble-n20-rover
 * An open source tiny ESP32 C3 Super Mini car that is light and fast controlled by a website using
   BLE.
 * Open sourced under MIT LICENSE.
 */

#include <Arduino.h>
#include <NimBLEDevice.h>

/* Pin definitions */
/* This pin reads the voltage of the battery through a voltage divider to calculate the percentage
   of the battery.
 */
const uint8_t BAT_VOLT_PIN = 0; 

/* Pins related to motor driver. */
const uint8_t AIN1_PIN = 10;
const uint8_t AIN2_PIN = 1;
const uint8_t BIN1_PIN = 3;
const uint8_t BIN2_PIN = 4;
const uint8_t PWMA_PIN = 5;
const uint8_t PWMB_PIN = 6;
const uint8_t STBY_PIN = 7;

/* UUID of BLE service. */
const char* SERVICE_UUID = "91fb9ba5-7485-4dc1-9bac-a9889bc524b9";
/* UUID of characteristic. */
const char* CHARACTERISTIC_UUID = "e942f3ad-99c0-48a0-94ad-43792af38a88";

NimBLEServer* pServer = nullptr;
NimBLECharacteristic* pCharacteristic = nullptr;

/* Create a class that inherits from NimBLECharacteristicCallbacks for BLE callbacks. */
class Callbacks : public NimBLECharacteristicCallbacks {
    /* Override the builtin onWrite function from NimBLECharacteristicCallbacks. */
    void onWrite(NimBLECharacteristic* pCharacteristic, NimBLEConnInfo& connInfo) override {
        /* This runs automatically whenever the client writes to this characteristic. */
        std::string value = pCharacteristic->getValue(); /* Get bytes sent by the client. */

        if (value.length() != 4) {
            Serial.println("Invalid packet length");
            return;
        }

        uint8_t leftDir    = value[0];
        uint8_t leftSpeed  = value[1];
        uint8_t rightDir   = value[2];
        uint8_t rightSpeed = value[3];
        
        Serial.printf(
            "L: dir=%d speed=%d | R: dir=%d speed=%d\n",
            leftDir, leftSpeed, rightDir, rightSpeed
        );
    }
};

class ServerCallbacks : public NimBLEServerCallbacks {
    void onConnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo) override {
        Serial.println("Client connected!");
    }

    void onDisconnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo, int reason) override {
        Serial.printf("Client disconnected, reason: %d\n", reason);
    }
};

void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("Starting");

    /* Initialize BLE with the name esp32c3-ble-n20-rover. */
    NimBLEDevice::init("esp32c3-ble-n20-rover");
    Serial.println("BLE INIT");
    /* Create BLE server */
    pServer = NimBLEDevice::createServer();
    pServer->setCallbacks(new ServerCallbacks());
    Serial.println("SERVER CREATE");
    /* Create service */
    NimBLEService* pService = pServer->createService(SERVICE_UUID);
    Serial.println("SERVICE CREATE");
    
    pCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID,
        NIMBLE_PROPERTY::WRITE_NR /* WRITE_NR means client can write bytes without response. */
    );
    Serial.println("CHARACTERISTIC CREATE");
    /* Attach callback to pCharacteristic */
    pCharacteristic->setCallbacks(new Callbacks());

    /* Activate the service */
    pService->start();
    Serial.println("SERVICE START");
    /* Advertise the service */
    NimBLEAdvertising* pAdvertising = NimBLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->enableScanResponse(true);
    NimBLEDevice::startAdvertising();
    Serial.println("ADVERTISE START");
}

void loop() {
    
}
