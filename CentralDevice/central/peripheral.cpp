#include "peripheral.h"

BLECharacteristic messageCharacteristic;

void setupPeripheral() {
  //if (!isCentral())
  Serial.println("setupPeripheral");
  // Central does not exits, create it
  if (!BLE.begin()) {
    Serial.println("starting Bluetooth® Low Energy failed!");
  }
  Serial.println("BLE goot central");
  BLE.setLocalName("Player Device");
  // Create game Service
  BLEService gameService("419d3655-282c-4a0e-9b7d-fbed8e928581");  // BLE LED Service
  // BLE LED Switch Characteristic - custom 128-bit UUID, read and writable by central
  messageCharacteristic = BLECharacteristic("b7a73424-a7d9-4aff-958d-04f4612d461b", BLEWrite, sizeof(Message), true);
  BLE.setAdvertisedService(gameService);
  // add the characteristic to the service
  gameService.addCharacteristic(messageCharacteristic);
  // add service
  BLE.addService(gameService);
  // start advertising
  BLE.advertise();
}

void loopPeripheral() {

  Serial.println("loopPeripheral");

  Message message = {
    BOMB_ATTACK,
    {},
    {},
    16,
    7,
  };

  String senderId = getMacAddress();
  strcpy(message.senderId, senderId.c_str());
  BLEDevice central = BLE.central();
  if (central) {
    Serial.println("Connected to central");
  }
  else {
    Serial.println("No central");
  }
  String receiverId = central.address();
  Serial.println(receiverId);
  Serial.println(receiverId.length());
  strcpy(message.recevierId, receiverId.c_str());

  Serial.print("Connected: ");
  Serial.println(central.connected());

  while (central.connected()) {
    Serial.println("WriteValue: ");
    Serial.print("Write: ");
    Serial.println(message.senderId);
    Serial.println(messageCharacteristic.uuid());
    messageCharacteristic.writeValue(&message, sizeof(Message), false);
    Serial.print("Written: ");
    Serial.println(message.senderId);
    delay(500);
  }
}