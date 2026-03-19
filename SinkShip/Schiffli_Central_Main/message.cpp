
#include "message.h"

int sendMessage(BLECharacteristic& characteristic, MessageType type, uint8_t x, uint8_t y) {
  Message message;
  message.type = BOMB_ATTACK;
  message.recevierId[0] = '\0';
  message.senderId[0] = '\0';
  message.x = 7;
  message.y = 8;

  return characteristic.writeValue(&message, sizeof(Message), false);
}

int receiveMessage(BLECharacteristic& characteristic, Message& message)
{
  bool written = characteristic.written();
  if (written)
    return characteristic.readValue(&message, sizeof(Message));
  return 0;
}

bool isPeripheral()
{
  BLE.scanForName(GAME_DEVICE_NAME);

  BLEDevice peripheral = BLE.available();

  if (peripheral) {

    Serial.print("Found peripheral: "); 
    Serial.println(peripheral.address());

    return true;
  }

  return false;
}