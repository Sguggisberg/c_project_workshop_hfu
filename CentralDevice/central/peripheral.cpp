#include "peripheral.h"

BLEDevice central;
BLECharacteristic messageCharacteristic;

void setupPeripheral() {
  central = BLE.central();
  //if (!isCentral())

  // Central does not exits, create it
  BLE.begin();
  Serial.println("BLE Peripheral");
  // Create game Service
  BLEService gameService("19B10000-E8F2-537E-4F6C-D104768A1214");  // BLE LED Service
  // BLE LED Switch Characteristic - custom 128-bit UUID, read and writable by central
  BLECharacteristic messageCharacteristic("19B10001-E8F2-537E-4F6C-D104768A1214", BLEWrite, sizeof(Message), true);
  BLE.setAdvertisedService(gameService);
  // add the characteristic to the service
  gameService.addCharacteristic(messageCharacteristic);
  // add service
  BLE.addService(gameService);
  // start advertising
  BLE.advertise();
}

void loopPeripheral() {

  Message message = {
    BOMB_ATTACK,
    {},
    {},
    16,
    7,
  };

  String senderId = getMacAddress();
  memcpy(message.senderId, senderId.c_str(), senderId.length());
  String receiverId = central.address();
  memcpy(message.recevierId, receiverId.c_str(), receiverId.length());

  while (central.connected()) {
    messageCharacteristic.writeValue(&message, sizeof(Message));
    delay(500);
  }
}