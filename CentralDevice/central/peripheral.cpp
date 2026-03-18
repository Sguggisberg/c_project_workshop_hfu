#include "peripheral.h"

BLEService GameService(GAME_SERVICE);  // BLE LED Service
// BLE LED Switch Characteristic - custom 128-bit UUID, read and writable by central
BLECharacteristic RequestCharacteristic(GAME_REQUEST_CHARACTERSITIC_UUID, BLERead | BLENotify | BLEWrite,
                                        sizeof(Message), true);
BLECharacteristic ResponseCharacteristic(GAME_RESPONSE_CHARACTERSITIC_UUID, BLERead | BLENotify | BLEWrite,
                                         sizeof(Message), true);


void setupPeripheral() {
  // begin initialization
  if (!BLE.begin()) {
    Serial.println("starting Bluetooth® Low Energy failed!");

    while (true) {
    }
  }
  // set advertised local name and service UUID:
  BLE.setLocalName("Game Device");
  BLE.setAdvertisedService(GameService);
  // add the characteristic to the service
  GameService.addCharacteristic(RequestCharacteristic);
  GameService.addCharacteristic(ResponseCharacteristic);
  // add service
  BLE.addService(GameService);
  // start advertising
  BLE.advertise();
  Serial.println("BLE Game Peripheral, waiting for connections....");
}

void loopPeripheral() {
  // listen for BLE peripherals to connect:
  BLEDevice central = BLE.central();
  if (!central) {
    Serial.println("BLE no central");
  }
  // if a central is connected to peripheral:
  if (central) {
    Serial.print("Connected to central: ");
    // print the central's MAC address:
    Serial.println(central.address());

    // while the central is still connected to peripheral:
    while (central.connected()) {

      Serial.println("Write message");
      if (sendMessage(RequestCharacteristic, BOMB_ATTACK, 3, 7)) {
        Serial.println("Send successful");
      }

      if (ResponseCharacteristic.written()) {

        if (central.discoverAttributes()) {
          Serial.println("discoverAttributtes successful");
        } else {
          Serial.println("discoverAttributes failed");
        }

        Message received;
        int result = receiveMessage(ResponseCharacteristic, received);
        //int result = ResponseCharacteristic.readValue(&received, sizeof(Message));

        Serial.print("Received: ");
        Serial.println(received.type);
        Serial.println(received.x);
        Serial.println(received.y);

        if (received.type == )
      }
      delay(500);
    }
  }
}