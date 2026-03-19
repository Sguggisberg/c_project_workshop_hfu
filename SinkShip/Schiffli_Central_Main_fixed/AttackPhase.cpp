#include "AttackPhase.h"
#include <stdio.h>
#include <string.h>

AttackPhase::AttackPhase() {
  reset();
}

void AttackPhase::reset() {
  for (uint8_t y = 0; y < ATTACK_GRID_SIZE; y++) {
    for (uint8_t x = 0; x < ATTACK_GRID_SIZE; x++) {
      _enemyShots[x][y] = false;
    }
  }
  _nextMoveNumber = 1;
}

bool AttackPhase::canAttack(uint8_t x, uint8_t y) const {
  if (x >= 8 || y >= 8) return false;
  return !_enemyShots[x][y];
}

bool AttackPhase::createAttackCommand(uint8_t x, uint8_t y, char* outBuffer, size_t outSize) {
  if (outBuffer == nullptr || outSize == 0) return false;

  if (x >= 8 || y >= 8) {
    snprintf(outBuffer, outSize, "invalid");
    return false;
  }

  if (_enemyShots[x][y]) {
    snprintf(outBuffer, outSize, "already used");
    return false;
  }

  uint16_t moveNumber = _nextMoveNumber++;
  _enemyShots[x][y] = true;

  snprintf(outBuffer, outSize, "move %u attack %u,%u", moveNumber, x, y);
  return true;
}

bool AttackPhase::parseAttackCommand(const char* input, uint16_t& moveNumber, uint8_t& x, uint8_t& y) {
  if (input == nullptr) return false;

  unsigned int mv = 0;
  unsigned int tx = 0;
  unsigned int ty = 0;

  int parsed = sscanf(input, "move %u attack %u,%u", &mv, &tx, &ty);
  if (parsed != 3) return false;
  if (tx >= 8 || ty >= 8) return false;

  moveNumber = (uint16_t)mv;
  x = (uint8_t)tx;
  y = (uint8_t)ty;
  return true;
}

bool AttackPhase::parseResponseCommandExtended(const char* input, uint16_t& moveNumber, uint8_t& response, bool& repeatTurn, SunkInfo& sunkInfo) {
  sunkInfo.count = 0;

  if (input == nullptr) return false;

  unsigned int mv = 0;
  unsigned int resp = 0;
  unsigned int rep = 0;

  int parsed = sscanf(input, "move %u resp %u repeat %u", &mv, &resp, &rep);
  if (parsed != 3) return false;
  if (resp > 2) return false;

  moveNumber = (uint16_t)mv;
  response = (uint8_t)resp;
  repeatTurn = (rep != 0);

  if (response != RESP_SUNK) return true;

  const char* sunkPtr = strstr(input, "sunk ");
  if (sunkPtr == nullptr) return true;

  sunkPtr += 5;

  while (*sunkPtr != '\0' && sunkInfo.count < MAX_SUNK_CELLS) {
    unsigned int sx = 0;
    unsigned int sy = 0;
    int consumed = 0;

    if (sscanf(sunkPtr, "%u,%u%n", &sx, &sy, &consumed) == 2) {
      if (sx < 8 && sy < 8) {
        sunkInfo.x[sunkInfo.count] = (uint8_t)sx;
        sunkInfo.y[sunkInfo.count] = (uint8_t)sy;
        sunkInfo.count++;
      }

      sunkPtr += consumed;
      if (*sunkPtr == ';') {
        sunkPtr++;
      } else {
        break;
      }
    } else {
      break;
    }
  }

  return true;
}

uint16_t AttackPhase::getLastMoveNumber() const {
  return (_nextMoveNumber == 0) ? 0 : (_nextMoveNumber - 1);
}