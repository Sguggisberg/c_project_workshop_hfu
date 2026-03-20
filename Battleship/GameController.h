#pragma once

#include "Config.h"
#include "GameTypes.h"

class GameController {
public:
  GameController();

  void begin();
  void resetAll();
  void randomizeShips();
  void clearPlacement();
  bool placeCurrentShip(uint8_t x, uint8_t y);
  bool removeLastPlacedShip();
  void togglePlacementOrientation();

  bool markLocalReady();
  bool isLocalReady() const;
  void setRemoteReady(bool ready);
  bool isRemoteReady() const;
  bool isGameReady() const;
  bool allShipsPlaced() const;

  bool canAttack() const;
  bool hasPendingAttack() const;
  bool queueAttack(uint8_t x, uint8_t y);
  Packet takeQueuedAttack();
  void clearPendingAttack();

  Packet handleIncomingAttack(uint8_t x, uint8_t y);
  void applyAttackResult(uint8_t x, uint8_t y, AttackResult result);

  void setStatus(const char* text);
  void setStatus(const String& text);
  String statusString() const;

  void setMyTurn(bool turn);
  bool isMyTurn() const;
  GamePhase phase() const;
  bool isFinished() const;
  bool didIWin() const;

  CellState myCell(uint8_t x, uint8_t y) const;
  EnemyCellState enemyCell(uint8_t x, uint8_t y) const;

  uint8_t currentPlacementLength() const;
  bool placementHorizontal() const;
  uint8_t placedShipCount() const;

private:
  bool placeShip(const Ship& ship);
  bool canPlaceShip(const Ship& ship) const;
  bool insideBoard(uint8_t x, uint8_t y) const;
  int findShipAt(uint8_t x, uint8_t y) const;
  bool isShipSunk(int shipIndex) const;
  bool areAllShipsSunk() const;
  void rebuildBoardFromShips();
  void clearEnemyBoard();
  void updateReadinessState();

  CellState myBoard_[Config::GRID_SIZE][Config::GRID_SIZE];
  EnemyCellState enemyBoard_[Config::GRID_SIZE][Config::GRID_SIZE];
  Ship ships_[Config::SHIP_COUNT];
  bool shipPlaced_[Config::SHIP_COUNT];
  uint8_t placedShips_;
  bool placementHorizontal_;

  bool localReady_;
  bool remoteReady_;
  bool myTurn_;
  bool pendingAttack_;
  bool finished_;
  bool iWon_;
  Packet queuedAttack_;
  String status_;
  GamePhase phase_;
};
