#ifndef GAME_STATE_H
#define GAME_STATE_H

#include <Arduino.h>
#include "AppConfig.h"
#include "Protocol.h"

enum class TurnOwner : uint8_t {
  PLAYER = 0,
  ENEMY  = 1
};

enum class UiScreen : uint8_t {
  LOBBY = 0,
  INVITE_DIALOG,
  RESTART_DIALOG,
  GAME
};

enum class GamePhase : uint8_t {
  PLACE_SHIPS = 0,
  WAIT_READY,
  RUNNING,
  FINISHED
};

enum EnemyCellState : uint8_t {
  ENEMY_UNKNOWN = 0,
  ENEMY_WATER   = 1,
  ENEMY_HIT     = 2,
  ENEMY_SUNK    = 3
};

struct ShipStatusInfo {
  uint8_t groesse;
  uint8_t anzahlLebend;
};

struct SunkCells {
  uint8_t count;
  uint8_t x[5];
  uint8_t y[5];
};

class GameState {
public:
  static constexpr uint8_t GRID_LEN = GRID_SIZE;
  static constexpr uint8_t SHIP_COUNT = 5;
  static constexpr uint8_t SHIP_TYPE_COUNT = 4;
  static const uint8_t shipLengths[SHIP_COUNT];

  GameState();

  void resetAll();
  void resetBoardOnly();

  bool isPlayerTurn() const;
  void setPlayerTurn();
  void setEnemyTurn();

  bool allShipsPlaced() const;
  bool canPlaceShip(uint8_t x, uint8_t y, uint8_t len, bool horizontal) const;
  void placeShip(uint8_t x, uint8_t y, uint8_t len, bool horizontal);

  void clearEnemyView();
  void setEnemyCell(uint8_t x, uint8_t y, uint8_t state);

  uint8_t getOwnShipIdAt(uint8_t x, uint8_t y) const;
  uint8_t getOwnShipLengthById(uint8_t id) const;
  void registerOwnHit(uint8_t shipId);
  bool isOwnShipSunk(uint8_t shipId) const;
  void collectOwnShipCells(uint8_t shipId, SunkCells& cells) const;

  bool allEnemyShipsSunk() const;
  bool allOwnShipsSunk() const;

  void markEnemyShipSunk(uint8_t len);
  void markOwnShipSunk(uint8_t len);

  const char* turnText() const;
  const char* directionText() const;
  const char* finishedText() const;

  void setStatusMessage(const char* text);
  void updatePlacementMessage();

  UiScreen screen;
  GamePhase phase;
  TurnOwner turn;

  bool horizontal;
  bool statusVisible;
  bool quitRequested;
  bool restartRequested;
  bool gameOpened;
  bool myReady;
  bool opponentReady;
  bool invitePending;
  bool restartPending;
  bool attackArmed;
  bool gameOver;
  bool connectionAccepted;
  bool initiatorBegins;

  uint8_t currentShip;
  uint8_t selectedEnemyX;
  uint8_t selectedEnemyY;
  bool enemySelected;

  char statusMessage[64];

  uint8_t ownGrid[GRID_LEN][GRID_LEN];
  uint8_t enemyGrid[GRID_LEN][GRID_LEN];

  uint8_t ownShipId[GRID_LEN][GRID_LEN];
  uint8_t ownShipHits[SHIP_COUNT];

  ShipStatusInfo ownShips[SHIP_TYPE_COUNT];
  ShipStatusInfo enemyShips[SHIP_TYPE_COUNT];

private:
  void initShipCounters();
};

#endif