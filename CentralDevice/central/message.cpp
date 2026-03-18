
#include "message.h"

int sendMessage(BLECharacteristic& characteristic, MessageType type, uint8_t x, uint8_t y) {
  Message message;
  message.type = BOMB_ATTACK;
  message.x = 7;
  message.y = 8;

  return characteristic.writeValue(&message, sizeof(Message), false);
}

int receiveMessage(BLECharacteristic& characteristic, Message& message)
{
  return characteristic.readValue(&message, sizeof(Message));
}