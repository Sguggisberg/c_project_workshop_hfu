#ifndef GAME_TYPES_H
#define GAME_TYPES_H

#include <Arduino.h>

// ---------------- FELDWERTE ----------------
const uint8_t LEER    = 0;
const uint8_t SCHIFF  = 1;
const uint8_t TREFFER = 2;
const uint8_t WASSER  = 3;

// ---------------- SPIELZUSTAND ----------------
enum SpielStatus {
  SCHIFFE_PLATZIEREN,
  WARTE_AUF_GEGNER,
  SPIEL_LAEUFT
};

enum PlayMode {
  MODE_PLACE_SHIPS,
  MODE_ATTACK,
  MODE_WAIT_RESPONSE
};

enum EnemyCellState : uint8_t {
  ENEMY_UNKNOWN = 0,
  ENEMY_WATER   = 1,
  ENEMY_HIT     = 2,
  ENEMY_SUNK    = 3
};

#endif