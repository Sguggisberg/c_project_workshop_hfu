
#include "message.h"

int sendMessage(BLECharacteristic& characteristic, MessageType type, uint8_t x, uint8_t y) {
  Serial.print("BLE sendMessage: type: ");
  Serial.print(type);
  Serial.print(", x: ");
  Serial.print(x);
  Serial.print(", y: ");
  Serial.println(y);

  Message message;
  message.type = type;
  message.recevierId[0] = '\0';
  message.senderId[0] = '\0';
  message.x = x;
  message.y = y;

  int res = characteristic.writeValue(&message, sizeof(Message), false);
  if (res) {
    Serial.println("Send message successful");
  }

  return res;
}

int receiveMessage(BLECharacteristic& characteristic, Message& message) {
  Serial.println("BLE receiveMessage:");
  bool written = characteristic.written();
  Serial.print("Written: ");
  Serial.println(written);
  if (written) {
    Serial.println("BLE written");
    int res = characteristic.readValue(&message, sizeof(Message));
    if (res > 0) {
      Serial.println(message.type);
    }
  }
  return 0;
}

bool foundPeripheral() {
  //BLE.scanForName(GAME_DEVICE_NAME);
  BLE.scanForUuid(GAME_SERVICE_UUID);

  for (int i = 0; i < 10; i++) {

    BLEDevice peripheral = BLE.available();

    if (peripheral) {

      Serial.print("Found peripheral: ");
      Serial.println(peripheral.address());

      return true;
    }
    Serial.println("Peripheral not found");
    
    delay(200);
  }
  return false;
}