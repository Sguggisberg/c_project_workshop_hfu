#ifndef GAME_STATE_H
#define GAME_STATE_H

#include <Arduino.h>
#include "AppConfig.h"

enum class TurnOwner {
  PLAYER,
  ENEMY
};

enum SpielStatus {
  SCHIFFE_PLATZIEREN,
  WARTE_AUF_GEGNER,
  SPIEL_LAEUFT,
  SPIEL_BEENDET
};

enum EnemyCellState {
  ENEMY_UNKNOWN = 0,
  ENEMY_WATER   = 1,
  ENEMY_HIT     = 2,
  ENEMY_SUNK    = 3
};

struct ShipStatusInfo {
  uint8_t groesse;
  uint8_t anzahlLebend;
};

class GameState {
public:
  static constexpr uint8_t GRID_LEN = GRID_SIZE;
  static constexpr uint8_t SHIP_TYPE_COUNT = 4;
  static constexpr uint8_t SHIP_COUNT = 5;
  static const uint8_t schiffLaengen[SHIP_COUNT];

  GameState();

  void init();

  bool isPlayerTurn() const;
  void setPlayerTurn();
  void setEnemyTurn();

  void showStatus();
  void hideStatus();
  void toggleStatus();
  void requestQuit();

  const char* getTurnText() const;
  const char* getSpielBeendetText() const;
  const char* getDirectionText() const;

  void setStatusMessage(const char* text);
  void updatePlacementMessage();

  void clearEnemyView();
  bool alleSchiffeGesetzt() const;
  bool kannSchiffPlatzieren(uint8_t startX, uint8_t startY, uint8_t laenge, bool horizontal) const;
  void setzeSchiff(uint8_t startX, uint8_t startY, uint8_t laenge, bool horizontal);
  void setEnemyCellState(uint8_t x, uint8_t y, uint8_t state);

  void markEnemyShipSunk(uint8_t shipLength);

  TurnOwner amZug;
  SpielStatus spielStatus;

  bool spielBeendet;
  bool quitAngefordert;
  bool statusSichtbar;
  bool horizontal;

  uint8_t aktuellesSchiff;
  char statusMeldung[64];

  uint8_t meinFeld[GRID_LEN][GRID_LEN];
  uint8_t gegnerFeld[GRID_LEN][GRID_LEN];

  ShipStatusInfo eigeneSchiffe[SHIP_TYPE_COUNT];
  ShipStatusInfo gegnerSchiffe[SHIP_TYPE_COUNT];

private:
  void initShipStatus();
};

#endif