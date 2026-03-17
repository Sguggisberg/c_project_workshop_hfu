#ifndef MESSAGE_H
#define MESSAGE_H
#include <stddef.h>

const size_t MAC_ADDR_LENGTH = 6;
typedef uint8_t MacAddress[MAC_ADDR_LENGTH];

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
  MacAddress receiverId;
  uint8_t x;  // Only necessary for moves
  uint8_t y;  // Only necessary for moves
};


#endif