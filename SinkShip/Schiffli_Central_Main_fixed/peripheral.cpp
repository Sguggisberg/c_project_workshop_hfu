#include "peripheral.h"

BLEService GameService(GAME_SERVICE_UUID);
BLECharacteristic RequestCharacteristic(GAME_REQUEST_CHARACTERSITIC_UUID, BLERead | BLENotify | BLEWrite,
                                        sizeof(Message), true);
BLECharacteristic ResponseCharacteristic(GAME_RESPONSE_CHARACTERSITIC_UUID, BLERead | BLENotify | BLEWrite,
                                         sizeof(Message), true);

void setupPeripheral() {
  SysRequestCharacteristic = RequestCharacteristic;
  SysResponseCharacteristic = ResponseCharacteristic;

  Message empty{};
  RequestCharacteristic.writeValue(&empty, sizeof(empty));
  ResponseCharacteristic.writeValue(&empty, sizeof(empty));

  BLE.setLocalName(GAME_DEVICE_NAME);
  BLE.setAdvertisedService(GameService);
  GameService.addCharacteristic(RequestCharacteristic);
  GameService.addCharacteristic(ResponseCharacteristic);
  BLE.addService(GameService);
  BLE.advertise();
  Serial.println("BLE Game Peripheral, waiting for connections....");
}

void loopPeripheral() {
  BLEDevice central = BLE.central();
  if (!central) {
    return;
  }

  Serial.print("Connected to central: ");
  Serial.println(central.address());

  bool requestSent = false;

  while (central.connected()) {
    BLE.poll();

    if (!requestSent) {
      Serial.println("Write message");
      if (sendMessage(RequestCharacteristic, ATTACK, 3, 7)) {
        Serial.println("Send successful");
        requestSent = true;
      }
    }

    Message received{};
    if (receiveMessage(ResponseCharacteristic, received)) {
      Serial.print("Received: ");
      Serial.println(received.type);
      Serial.println(received.x);
      Serial.println(received.y);
      requestSent = false;
    }

    delay(20);
  }

  Serial.println("Central disconnected");
}
