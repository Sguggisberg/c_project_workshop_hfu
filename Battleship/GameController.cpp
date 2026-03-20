#include "GameController.h"

GameController::GameController()
  : placedShips_(0), placementHorizontal_(true),
    localReady_(false), remoteReady_(false), myTurn_(false), pendingAttack_(false),
    finished_(false), iWon_(false), phase_(PHASE_PLACING) {
  status_ = "Starte...";
}

void GameController::begin() {
  randomSeed(analogRead(A0) + millis());
  resetAll();
}

void GameController::clearEnemyBoard() {
  for (uint8_t y = 0; y < Config::GRID_SIZE; ++y) {
    for (uint8_t x = 0; x < Config::GRID_SIZE; ++x) {
      enemyBoard_[y][x] = ENEMY_UNKNOWN;
    }
  }
}

void GameController::resetAll() {
  clearPlacement();
  clearEnemyBoard();
  localReady_ = false;
  remoteReady_ = false;
  myTurn_ = false;
  pendingAttack_ = false;
  finished_ = false;
  iWon_ = false;
  queuedAttack_ = {MSG_NONE, 0, 0, 0};
  phase_ = PHASE_PLACING;
  status_ = "Schiffe setzen";
}

void GameController::clearPlacement() {
  for (uint8_t y = 0; y < Config::GRID_SIZE; ++y) {
    for (uint8_t x = 0; x < Config::GRID_SIZE; ++x) {
      myBoard_[y][x] = CELL_EMPTY;
    }
  }
  for (uint8_t i = 0; i < Config::SHIP_COUNT; ++i) {
    ships_[i] = {0, 0, Config::SHIP_LENGTHS[i], true};
    shipPlaced_[i] = false;
  }
  placedShips_ = 0;
  placementHorizontal_ = true;
  localReady_ = false;
  remoteReady_ = false;
  myTurn_ = false;
  pendingAttack_ = false;
  finished_ = false;
  iWon_ = false;
  phase_ = PHASE_PLACING;
  queuedAttack_ = {MSG_NONE, 0, 0, 0};
}

bool GameController::insideBoard(uint8_t x, uint8_t y) const {
  return x < Config::GRID_SIZE && y < Config::GRID_SIZE;
}

bool GameController::canPlaceShip(const Ship& ship) const {
  for (uint8_t i = 0; i < ship.len; ++i) {
    uint8_t x = ship.x + (ship.horizontal ? i : 0);
    uint8_t y = ship.y + (ship.horizontal ? 0 : i);
    if (!insideBoard(x, y) || myBoard_[y][x] != CELL_EMPTY) {
      return false;
    }
  }
  return true;
}

bool GameController::placeShip(const Ship& ship) {
  if (!canPlaceShip(ship)) return false;
  for (uint8_t i = 0; i < ship.len; ++i) {
    uint8_t x = ship.x + (ship.horizontal ? i : 0);
    uint8_t y = ship.y + (ship.horizontal ? 0 : i);
    myBoard_[y][x] = CELL_SHIP;
  }
  return true;
}

void GameController::rebuildBoardFromShips() {
  for (uint8_t y = 0; y < Config::GRID_SIZE; ++y) {
    for (uint8_t x = 0; x < Config::GRID_SIZE; ++x) {
      myBoard_[y][x] = CELL_EMPTY;
    }
  }
  for (uint8_t i = 0; i < placedShips_; ++i) {
    placeShip(ships_[i]);
    shipPlaced_[i] = true;
  }
  for (uint8_t i = placedShips_; i < Config::SHIP_COUNT; ++i) {
    shipPlaced_[i] = false;
  }
}

uint8_t GameController::currentPlacementLength() const {
  if (placedShips_ >= Config::SHIP_COUNT) return 0;
  return Config::SHIP_LENGTHS[placedShips_];
}

bool GameController::placementHorizontal() const {
  return placementHorizontal_;
}

uint8_t GameController::placedShipCount() const {
  return placedShips_;
}

bool GameController::allShipsPlaced() const {
  return placedShips_ >= Config::SHIP_COUNT;
}

bool GameController::placeCurrentShip(uint8_t x, uint8_t y) {
  if (finished_ || localReady_ || allShipsPlaced()) return false;
  Ship ship = {x, y, currentPlacementLength(), placementHorizontal_};
  if (!placeShip(ship)) {
    status_ = "Platzierung ungueltig";
    return false;
  }
  ships_[placedShips_] = ship;
  shipPlaced_[placedShips_] = true;
  ++placedShips_;

  if (allShipsPlaced()) {
    status_ = "Alle Schiffe gesetzt";
  } else {
    status_ = String("Setze Schiff Laenge ") + currentPlacementLength();
  }
  return true;
}

bool GameController::removeLastPlacedShip() {
  if (placedShips_ == 0 || localReady_) return false;
  --placedShips_;
  shipPlaced_[placedShips_] = false;
  rebuildBoardFromShips();
  status_ = String("Rueckgaengig - Schiff Laenge ") + currentPlacementLength();
  return true;
}

void GameController::togglePlacementOrientation() {
  if (!localReady_ && !finished_) {
    placementHorizontal_ = !placementHorizontal_;
    status_ = placementHorizontal_ ? "Horizontal" : "Vertikal";
  }
}

void GameController::randomizeShips() {
  clearPlacement();
  bool done = false;
  while (!done) {
    rebuildBoardFromShips();
    placedShips_ = 0;
    for (uint8_t y = 0; y < Config::GRID_SIZE; ++y) {
      for (uint8_t x = 0; x < Config::GRID_SIZE; ++x) {
        myBoard_[y][x] = CELL_EMPTY;
      }
    }

    done = true;
    for (uint8_t i = 0; i < Config::SHIP_COUNT; ++i) {
      bool placed = false;
      for (uint16_t tries = 0; tries < 250 && !placed; ++tries) {
        bool horizontal = random(0, 2) == 0;
        uint8_t len = Config::SHIP_LENGTHS[i];
        uint8_t maxX = horizontal ? (Config::GRID_SIZE - len) : (Config::GRID_SIZE - 1);
        uint8_t maxY = horizontal ? (Config::GRID_SIZE - 1) : (Config::GRID_SIZE - len);
        Ship s = {
          static_cast<uint8_t>(random(0, maxX + 1)),
          static_cast<uint8_t>(random(0, maxY + 1)),
          len,
          horizontal
        };
        if (placeShip(s)) {
          ships_[i] = s;
          shipPlaced_[i] = true;
          ++placedShips_;
          placed = true;
        }
      }
      if (!placed) {
        done = false;
        break;
      }
    }
  }
  placementHorizontal_ = true;
  localReady_ = false;
  remoteReady_ = false;
  myTurn_ = false;
  pendingAttack_ = false;
  finished_ = false;
  iWon_ = false;
  phase_ = PHASE_PLACING;
  status_ = "Tippe READY";
}

bool GameController::markLocalReady() {
  if (finished_ || !allShipsPlaced()) return false;
  localReady_ = true;
  updateReadinessState();
  return true;
}

bool GameController::isLocalReady() const { return localReady_; }
void GameController::setRemoteReady(bool ready) { remoteReady_ = ready; updateReadinessState(); }
bool GameController::isRemoteReady() const { return remoteReady_; }

void GameController::updateReadinessState() {
  if (finished_) return;
  if (localReady_ && remoteReady_) {
    phase_ = PHASE_PLAYING;
    status_ = myTurn_ ? "Du bist dran" : "Gegner ist dran";
  } else if (localReady_) {
    phase_ = PHASE_WAIT_READY;
    status_ = "Warte auf Gegner";
  } else {
    phase_ = PHASE_PLACING;
  }
}

bool GameController::isGameReady() const {
  return localReady_ && remoteReady_ && !finished_;
}

bool GameController::canAttack() const {
  return isGameReady() && myTurn_ && !pendingAttack_ && !finished_;
}

bool GameController::hasPendingAttack() const { return pendingAttack_; }

bool GameController::queueAttack(uint8_t x, uint8_t y) {
  if (!canAttack() || !insideBoard(x, y)) return false;
  if (enemyBoard_[y][x] != ENEMY_UNKNOWN) {
    status_ = "Feld schon benutzt";
    return false;
  }
  queuedAttack_ = {MSG_ATTACK, x, y, 0};
  pendingAttack_ = true;
  status_ = "Angriff senden";
  return true;
}

Packet GameController::takeQueuedAttack() { return queuedAttack_; }

void GameController::clearPendingAttack() {
  pendingAttack_ = false;
  queuedAttack_ = {MSG_NONE, 0, 0, 0};
}

int GameController::findShipAt(uint8_t x, uint8_t y) const {
  for (uint8_t i = 0; i < Config::SHIP_COUNT; ++i) {
    const Ship& ship = ships_[i];
    for (uint8_t j = 0; j < ship.len; ++j) {
      uint8_t sx = ship.x + (ship.horizontal ? j : 0);
      uint8_t sy = ship.y + (ship.horizontal ? 0 : j);
      if (sx == x && sy == y) return i;
    }
  }
  return -1;
}

bool GameController::isShipSunk(int shipIndex) const {
  if (shipIndex < 0 || shipIndex >= Config::SHIP_COUNT) return false;
  const Ship& ship = ships_[shipIndex];
  for (uint8_t i = 0; i < ship.len; ++i) {
    uint8_t x = ship.x + (ship.horizontal ? i : 0);
    uint8_t y = ship.y + (ship.horizontal ? 0 : i);
    if (myBoard_[y][x] != CELL_HIT) return false;
  }
  return true;
}

bool GameController::areAllShipsSunk() const {
  for (uint8_t i = 0; i < Config::SHIP_COUNT; ++i) {
    if (!isShipSunk(i)) return false;
  }
  return true;
}

Packet GameController::handleIncomingAttack(uint8_t x, uint8_t y) {
  Packet response = {MSG_RESULT, x, y, RES_INVALID};
  if (!insideBoard(x, y) || finished_) return response;

  if (myBoard_[y][x] == CELL_EMPTY) {
    myBoard_[y][x] = CELL_MISS;
    response.value = RES_WATER;
    myTurn_ = true;
    status_ = "Du bist dran";
    return response;
  }

  if (myBoard_[y][x] == CELL_MISS || myBoard_[y][x] == CELL_HIT) {
    response.value = RES_INVALID;
    return response;
  }

  myBoard_[y][x] = CELL_HIT;
  int shipIndex = findShipAt(x, y);
  bool sunk = isShipSunk(shipIndex);
  bool lost = areAllShipsSunk();

  if (lost) {
    response.value = RES_WIN;
    finished_ = true;
    iWon_ = false;
    myTurn_ = false;
    phase_ = PHASE_FINISHED;
    status_ = "Verloren";
  } else if (sunk) {
    response.value = RES_SUNK;
    myTurn_ = false;
    status_ = "Versenkt";
  } else {
    response.value = RES_HIT;
    myTurn_ = false;
    status_ = "Treffer";
  }
  return response;
}

void GameController::applyAttackResult(uint8_t x, uint8_t y, AttackResult result) {
  if (!insideBoard(x, y)) return;
  pendingAttack_ = false;

  switch (result) {
    case RES_WATER:
      enemyBoard_[y][x] = ENEMY_MISS;
      myTurn_ = false;
      status_ = "Wasser - Gegner dran";
      break;
    case RES_HIT:
      enemyBoard_[y][x] = ENEMY_HIT;
      myTurn_ = true;
      status_ = "Treffer - weiter";
      break;
    case RES_SUNK:
      enemyBoard_[y][x] = ENEMY_SUNK;
      myTurn_ = true;
      status_ = "Versenkt - weiter";
      break;
    case RES_WIN:
      enemyBoard_[y][x] = ENEMY_SUNK;
      finished_ = true;
      iWon_ = true;
      myTurn_ = false;
      phase_ = PHASE_FINISHED;
      status_ = "Gewonnen";
      break;
    default:
      status_ = "Antwort ungueltig";
      break;
  }
}

void GameController::setStatus(const char* text) { status_ = text; }
void GameController::setStatus(const String& text) { status_ = text; }
String GameController::statusString() const { return status_; }

void GameController::setMyTurn(bool turn) {
  myTurn_ = turn;
  if (isGameReady() && !finished_) {
    status_ = myTurn_ ? "Du bist dran" : "Gegner ist dran";
  }
}

bool GameController::isMyTurn() const { return myTurn_; }
GamePhase GameController::phase() const { return phase_; }
bool GameController::isFinished() const { return finished_; }
bool GameController::didIWin() const { return iWon_; }
CellState GameController::myCell(uint8_t x, uint8_t y) const { return myBoard_[y][x]; }
EnemyCellState GameController::enemyCell(uint8_t x, uint8_t y) const { return enemyBoard_[y][x]; }
