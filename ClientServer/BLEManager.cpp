#include "BLEManager.h"

void BLEManager::begin() {
  BLE.begin();
  BLE.scanForUuid("180C");
  delay(2000);

  BLEDevice dev = BLE.available();

  if (dev) {
    role = CLIENT;
    Serial.println("CLIENT");
  } else {
    role = SERVER;
    Serial.println("SERVER");
  }

  if (role == SERVER) {
    BLE.setLocalName("RP2040_SERVER");
    BLE.setAdvertisedService(service);
    service.addCharacteristic(characteristic);
    BLE.addService(service);
    BLE.advertise();
  }
}

void BLEManager::update() {
  if (role == SERVER) runServer();
  else if (role == CLIENT) runClient();
}

void BLEManager::runServer() {
  BLEDevice central = BLE.central();
  if (central) {
    if (characteristic.written()) {
      Message msg;
      characteristic.readValue((uint8_t*)&msg, sizeof(msg));

      Serial.println("Server received");

      msg.type = 2;
      characteristic.writeValue((uint8_t*)&msg, sizeof(msg));
    }
  }
}

void BLEManager::runClient() {
  static BLEDevice dev;

  if (!dev) {
    dev = BLE.available();
    if (dev) {
      BLE.stopScan();
      if (dev.connect()) {
        Serial.println("Connected to server");
        BLECharacteristic remoteChar = dev.characteristic("2A56");
        remoteChar.subscribe();
      }
    }
  }

  if (dev && dev.connected()) {
    BLECharacteristic remoteChar = dev.characteristic("2A56");

    Message msg = {1, 1, 5, 6};
    remoteChar.writeValue((uint8_t*)&msg, sizeof(msg));
    delay(2000);

    if (remoteChar.valueUpdated()) {
      Message resp;
      remoteChar.readValue((uint8_t*)&resp, sizeof(resp));
      Serial.println("Client received response");
    }
  }
}
