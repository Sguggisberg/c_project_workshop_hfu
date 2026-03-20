#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <Arduino.h>

const char GAME_SERVICE_UUID[] = "19b10000-e8f2-537e-4f6c-d104768a1215";
const char GAME_RX_UUID[]      = "19b10001-e8f2-537e-4f6c-d104768a1215";
const char GAME_TX_UUID[]      = "19b10002-e8f2-537e-4f6c-d104768a1215";
const char GAME_DEVICE_NAME[]  = "Schiffli-RP2040";

enum PacketType : uint8_t {
  PKT_NONE = 0,
  PKT_INVITE,
  PKT_INVITE_ACCEPT,
  PKT_INVITE_DECLINE,
  PKT_OPEN_GAME,
  PKT_READY,
  PKT_ATTACK,
  PKT_RESULT,
  PKT_RESTART_REQ,
  PKT_RESTART_ACCEPT,
  PKT_RESTART_DECLINE,
  PKT_QUIT
};

enum AttackResult : uint8_t {
  RES_WATER = 0,
  RES_HIT   = 1,
  RES_SUNK  = 2
};

struct Packet {
  uint8_t type;
  uint8_t x;
  uint8_t y;
  uint8_t value;
  uint8_t count;
  uint8_t payload[10];
};

inline Packet makeSimplePacket(PacketType type) {
  Packet p{};
  p.type = static_cast<uint8_t>(type);
  return p;
}

#endif