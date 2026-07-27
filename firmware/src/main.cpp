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

/* This pin detects spike in current draw from the motor driver using a shunt resistor so that it
 * can shut off the motor before it causes any hardware damage.
 */
const uint8_t CURR_SENSE_PIN = 1;

/* Pins related to motor driver. */
const uint8_t AIN1_PIN = 10;
const uint8_t AIN2_PIN = 20;
const uint8_t BIN1_PIN = 3;
const uint8_t BIN2_PIN = 4;
const uint8_t PWMA_PIN = 5;
const uint8_t PWMB_PIN = 6;
/* STBY pin is a pin on the motor driver. When it is pulled LOW, it turns off all motors 
 * We pull it high so that it turns on.
 */
const uint8_t STBY_PIN = 7;

unsigned long lastPacketMs = 0; /* Store when the last packet was recieved. */
const unsigned long SAFETY_TIMEOUT_MS = 500; /* Stop if no packet recieved within SAFETY_TIMEOUT_MS
                                                ms */

void stopMotors() {
    digitalWrite(AIN1_PIN, LOW);
    digitalWrite(AIN2_PIN, LOW);
    digitalWrite(BIN1_PIN, LOW);
    digitalWrite(BIN2_PIN, LOW);
    analogWrite(PWMA_PIN, 0);
    analogWrite(PWMB_PIN, 0);
}

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
        
        if (leftSpeed == 0) {
            digitalWrite(AIN1_PIN, LOW);
            digitalWrite(AIN2_PIN, LOW);
        } else if (leftDir == 1) {
            digitalWrite(AIN1_PIN, LOW);
            digitalWrite(AIN2_PIN, HIGH);
        } else {
            digitalWrite(AIN1_PIN, HIGH);
            digitalWrite(AIN2_PIN, LOW);
        }

        if (rightSpeed == 0) {
            digitalWrite(BIN1_PIN, LOW);
            digitalWrite(BIN2_PIN, LOW);
        } else if (rightDir == 1) {
            digitalWrite(BIN1_PIN, LOW);
            digitalWrite(BIN2_PIN, HIGH);
        } else {
            digitalWrite(BIN1_PIN, HIGH);
            digitalWrite(BIN2_PIN, LOW);
        }
        
        analogWrite(PWMA_PIN, leftSpeed);
        analogWrite(PWMB_PIN, rightSpeed);

        lastPacketMs = millis();
    }
};

class ServerCallbacks : public NimBLEServerCallbacks {
    void onConnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo) override {
        Serial.println("Client connected!");
        digitalWrite(STBY_PIN, HIGH);
        stopMotors();
        lastPacketMs = millis();
    }

    void onDisconnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo, int reason) override {
        Serial.printf("Client disconnected, reason: %d\n", reason);
        stopMotors();
        digitalWrite(STBY_PIN, LOW);
        NimBLEDevice::startAdvertising();
    }
};

void setup() {
    Serial.begin(115200);
    Serial.println("Starting");

    pinMode(BAT_VOLT_PIN, INPUT);
    pinMode(CURR_SENSE_PIN, INPUT);
    pinMode(AIN1_PIN, OUTPUT);
    pinMode(AIN2_PIN, OUTPUT);
    pinMode(BIN1_PIN, OUTPUT);
    pinMode(BIN2_PIN, OUTPUT);
    pinMode(PWMA_PIN, OUTPUT);
    pinMode(PWMB_PIN, OUTPUT);
    pinMode(STBY_PIN, OUTPUT);

    /* Set PWM frequency to 20kHz to remove motor whining sounds */
    analogWriteFrequency(PWMA_PIN, 20000);
    analogWriteFrequency(PWMB_PIN, 20000);
    /* Set 8-bit resolution for PWM (0-255) */
    analogWriteResolution(PWMA_PIN, 8);
    analogWriteResolution(PWMB_PIN, 8);

    stopMotors();
    digitalWrite(STBY_PIN, LOW);
    
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
    /* If connected but no packets arrive for > SAFETY_TIMEOUT_MS ms, force stop the motors */
    if (pServer->getConnectedCount() > 0 && lastPacketMs > 0) {
        if (millis() - lastPacketMs > SAFETY_TIMEOUT_MS) {
            stopMotors();
        }
    }
    /* Give FreeRTOS time to manage system tasks */
    vTaskDelay(pdMS_TO_TICKS(10));
}
