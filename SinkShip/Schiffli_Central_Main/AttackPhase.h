#ifndef ATTACK_PHASE_H
#define ATTACK_PHASE_H

#include <Arduino.h>

static const uint8_t ATTACK_GRID_SIZE = 8;
static const uint8_t MAX_SUNK_CELLS   = 5;

enum AttackResponseType : uint8_t {
  RESP_WATER = 0,
  RESP_HIT   = 1,
  RESP_SUNK  = 2
};

struct SunkInfo {
  uint8_t count;
  uint8_t x[MAX_SUNK_CELLS];
  uint8_t y[MAX_SUNK_CELLS];
};

class AttackPhase {
public:
  AttackPhase();

  void reset();

  bool canAttack(uint8_t x, uint8_t y) const;
  bool createAttackCommand(uint8_t x, uint8_t y, char* outBuffer, size_t outSize);

  bool parseAttackCommand(const char* input, uint16_t& moveNumber, uint8_t& x, uint8_t& y);
  bool parseResponseCommandExtended(const char* input, uint16_t& moveNumber, uint8_t& response, bool& repeatTurn, SunkInfo& sunkInfo);

  uint16_t getLastMoveNumber() const;

private:
  bool _enemyShots[ATTACK_GRID_SIZE][ATTACK_GRID_SIZE];
  uint16_t _nextMoveNumber;
};

#endif