#ifndef MESSAGE_H
#define MESSAGE_H

#include <Arduino.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stddef.h>

const char GAME_SERVICE[]="19b10000-e8f2-537e-4f6c-d104768a1214";
const char GAME_REQUEST_CHARACTERSITIC_UUID[] = "19b10001-e8f2-537e-4f6c-d104768a1214";
const char GAME_RESPONSE_CHARACTERSITIC_UUID[] = "19b10002-e8f2-537e-4f6c-d104768a1214";

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

#endif