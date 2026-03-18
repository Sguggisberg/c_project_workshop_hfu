#ifndef MESSAGE_H
#define MESSAGE_H

#include <Arduino.h>
#include <ArduinoBLE.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stddef.h>

const size_t MAC_ADDR_LENGTH = 18;
typedef char MacAddress[MAC_ADDR_LENGTH];

enum MessageType {
  START,
  PLAY,
  BOMB_ATTACK,
  SPY_ATTACK,
  HIT,
  MISSED,
  STOP,
};

struct Message {
  MessageType type;
  MacAddress senderId;
  MacAddress recevierId;
  uint8_t x;  // Only necessary for moves
  uint8_t y;  // Only necessary for moves
};

String getMacAddress();

int sendMessage(BLECharacteristic& characteristic, MessageType type, uint8_t x, uint8_t y);
int receiveMessage(BLECharacteristic& characteristic, Message& message);

#endif