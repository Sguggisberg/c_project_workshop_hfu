#include "message.h"

BLEDevice SysPeripheral;

int sendMessage(BLECharacteristic& characteristic, MessageType type, uint8_t x, uint8_t y) {
  Serial.print("BLE sendMessage: type: ");
  Serial.print(type);
  Serial.print(", x: ");
  Serial.print(x);
  Serial.print(", y: ");
  Serial.println(y);

  if (!characteristic) {
    Serial.println("Send message failed: characteristic not ready");
    return 0;
  }

  Message message{};
  message.type = type;
  message.recevierId[0] = '\0';
  message.senderId[0] = '\0';
  message.x = x;
  message.y = y;

  int res = characteristic.writeValue(&message, sizeof(Message), false);
  if (res) {
    Serial.println("Send message successful");
  } else {
    Serial.println("Send message failed");
  }

  return res;
}

int receiveMessage(BLECharacteristic& characteristic, Message& message) {
  if (!characteristic) {
    return 0;
  }

  bool hasNewValue = false;

  if (characteristic.written()) {
    hasNewValue = true;
    Serial.println("Source: local write");
  }

  if (!hasNewValue && characteristic.valueUpdated()) {
    hasNewValue = true;
    Serial.println("Source: remote notification");
  }

  if (!hasNewValue) {
    return 0;
  }

  int res = characteristic.readValue(&message, sizeof(Message));
  if (res == sizeof(Message)) {
    Serial.print("Message type: ");
    Serial.println(message.type);
    Serial.print("x: ");
    Serial.println(message.x);
    Serial.print("y: ");
    Serial.println(message.y);
    return 1;
  }

  Serial.print("Read message failed, bytes: ");
  Serial.println(res);
  return 0;
}

bool foundPeripheral() {
  BLE.scanForUuid(GAME_SERVICE_UUID);

  for (int i = 0; i < 15; i++) {
    BLE.poll();
    BLEDevice peripheral = BLE.available();

    if (peripheral) {
      Serial.print("Found peripheral: ");
      Serial.println(peripheral.address());
      BLE.stopScan();
      return true;
    }

    Serial.println("Peripheral not found");
    delay(200);
  }

  BLE.stopScan();
  return false;
}
