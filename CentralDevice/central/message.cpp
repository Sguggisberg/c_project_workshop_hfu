
#include "message.h"

int sendMessage(BLECharacteristic& characteristic, MessageType type, uint8_t x, uint8_t y) {
  Message message;
  message.type = type;
  message.recevierId[0] = '\0';
  message.senderId[0] = '\0';
  message.x = x;
  message.y = y;

  return characteristic.writeValue(&message, sizeof(Message), false);
}

int receiveMessage(BLECharacteristic& characteristic, Message& message)
{
  return characteristic.readValue(&message, sizeof(Message));
}