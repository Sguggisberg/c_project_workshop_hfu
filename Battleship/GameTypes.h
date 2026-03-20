#pragma once

#include <Arduino.h>

enum MessageType : uint8_t {
  MSG_NONE = 0,
  MSG_READY = 1,
  MSG_ATTACK = 2,
  MSG_RESULT = 3,
  MSG_RESET = 4
};

enum AttackResult : uint8_t {
  RES_UNKNOWN = 0,
  RES_WATER = 1,
  RES_HIT = 2,
  RES_SUNK = 3,
  RES_WIN = 4,
  RES_INVALID = 5
};

enum Role : uint8_t {
  ROLE_UNDECIDED = 0,
  ROLE_PERIPHERAL = 1,
  ROLE_CENTRAL = 2
};

enum CellState : uint8_t {
  CELL_EMPTY = 0,
  CELL_SHIP = 1,
  CELL_HIT = 2,
  CELL_MISS = 3
};

enum EnemyCellState : uint8_t {
  ENEMY_UNKNOWN = 0,
  ENEMY_MISS = 1,
  ENEMY_HIT = 2,
  ENEMY_SUNK = 3
};

enum GamePhase : uint8_t {
  PHASE_PLACING = 0,
  PHASE_WAIT_READY = 1,
  PHASE_PLAYING = 2,
  PHASE_FINISHED = 3
};

struct Packet {
  uint8_t type;
  uint8_t x;
  uint8_t y;
  uint8_t value;
};

struct Ship {
  uint8_t x;
  uint8_t y;
  uint8_t len;
  bool horizontal;
};
