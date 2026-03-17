#include <ArduinoBLE.h>
#include <stdbool.h>
#include "central.h"
#include "message.h"

void log(BLEDevice peripheral);

void setup_central() {

  while (!Serial)
    ;
  BLE.begin();
  Serial.println("Starting as Central");
  BLE.scanForUuid("19b10000-e8f2-537e-4f6c-d104768a1214");
}

void loop_central() {
  // check if a peripheral has been discovered
  BLEDevice peripheral = BLE.available();
  if (peripheral) {
    log(peripheral);
    BLE.stopScan();
    controlLed(peripheral);
    BLE.scanForUuid("19b10000-e8f2-537e-4f6c-d104768a1214");
  }
}

void controlLed(BLEDevice peripheral) {
  Serial.println("Connecting ...");
  if (peripheral.connect()) {
    Serial.println("Connected");
  } else {
    Serial.println("Failed to connect!");
    return;
  }
  // discover peripheral attributes
  Serial.println("Discovering attributes ...");
  if (peripheral.discoverAttributes()) {
    Serial.println("Attributes discovered");
  } else {
    Serial.println("Attribute discovery failed!");
    peripheral.disconnect();
    return;
  }
  BLECharacteristic LEDCharacteristic = peripheral.characteristic("19b10001-e8f2-537e-4f6c-d104768a1214");
  if (!LEDCharacteristic) {
    Serial.println("Peripheral does not have LED characteristic!");
    peripheral.disconnect();
    return;
  }
  while (peripheral.connected()) {
    if (LEDCharacteristic.canRead()) {
      Message value;
      LEDCharacteristic.readValue(&value, sizeof(Message));
      Serial.println("Wert: ");
      Serial.println(value.type);
      Serial.println(value.x);
      Serial.println(value.y);
      Serial.println("Ende");
    }
    delay(500);
  }
  Serial.println("Peripheral disconnected");
}

void log(BLEDevice peripheral) {
  if (peripheral) {
    Serial.print("Found ");
    Serial.print(peripheral.address());
    Serial.print(" '");
    Serial.print(peripheral.localName());
    Serial.print("' ");
    Serial.print(peripheral.advertisedServiceUuid());
    Serial.println();
  }
}