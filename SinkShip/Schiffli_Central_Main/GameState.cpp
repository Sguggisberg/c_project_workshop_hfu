#include "GameState.h"
#include <cstring>
#include <cstdio>

const uint8_t GameState::shipLengths[GameState::SHIP_COUNT] = {5, 4, 3, 3, 2};

GameState::GameState() {
  resetAll();
}

void GameState::initShipCounters() {
  ownShips[0] = {2, 1};
  ownShips[1] = {3, 2};
  ownShips[2] = {4, 1};
  ownShips[3] = {5, 1};

  enemyShips[0] = {2, 1};
  enemyShips[1] = {3, 2};
  enemyShips[2] = {4, 1};
  enemyShips[3] = {5, 1};
}

void GameState::resetBoardOnly() {
  for (uint8_t y = 0; y < GRID_LEN; y++) {
    for (uint8_t x = 0; x < GRID_LEN; x++) {
      ownGrid[y][x] = 0;
      enemyGrid[y][x] = ENEMY_UNKNOWN;
      ownShipId[y][x] = 0;
    }
  }

  for (uint8_t i = 0; i < SHIP_COUNT; i++) {
    ownShipHits[i] = 0;
  }

  initShipCounters();

  horizontal = true;
  currentShip = 0;
  myReady = false;
  opponentReady = false;
  attackArmed = false;
  gameOver = false;
  enemySelected = false;
  selectedEnemyX = 0;
  selectedEnemyY = 0;

  phase = GamePhase::PLACE_SHIPS;
  turn = TurnOwner::PLAYER;
  updatePlacementMessage();
}

void GameState::resetAll() {
  screen = UiScreen::LOBBY;
  phase = GamePhase::PLACE_SHIPS;
  turn = TurnOwner::PLAYER;

  statusVisible = false;
  quitRequested = false;
  restartRequested = false;
  gameOpened = false;
  invitePending = false;
  restartPending = false;
  connectionAccepted = false;
  initiatorBegins = false;

  resetBoardOnly();
}

bool GameState::isPlayerTurn() const {
  return turn == TurnOwner::PLAYER;
}

void GameState::setPlayerTurn() {
  turn = TurnOwner::PLAYER;
}

void GameState::setEnemyTurn() {
  turn = TurnOwner::ENEMY;
}

bool GameState::allShipsPlaced() const {
  return currentShip >= SHIP_COUNT;
}

bool GameState::canPlaceShip(uint8_t x, uint8_t y, uint8_t len, bool hor) const {
  if (hor) {
    if (x + len > GRID_LEN) return false;
    for (uint8_t i = 0; i < len; i++) {
      if (ownGrid[y][x + i] != 0) return false;
    }
  } else {
    if (y + len > GRID_LEN) return false;
    for (uint8_t i = 0; i < len; i++) {
      if (ownGrid[y + i][x] != 0) return false;
    }
  }
  return true;
}

void GameState::placeShip(uint8_t x, uint8_t y, uint8_t len, bool hor) {
  uint8_t id = currentShip + 1;

  if (hor) {
    for (uint8_t i = 0; i < len; i++) {
      ownGrid[y][x + i] = 1;
      ownShipId[y][x + i] = id;
    }
  } else {
    for (uint8_t i = 0; i < len; i++) {
      ownGrid[y + i][x] = 1;
      ownShipId[y + i][x] = id;
    }
  }
}

void GameState::clearEnemyView() {
  for (uint8_t y = 0; y < GRID_LEN; y++) {
    for (uint8_t x = 0; x < GRID_LEN; x++) {
      enemyGrid[y][x] = ENEMY_UNKNOWN;
    }
  }
}

void GameState::setEnemyCell(uint8_t x, uint8_t y, uint8_t state) {
  if (x >= GRID_LEN || y >= GRID_LEN) return;
  enemyGrid[y][x] = state;
}

uint8_t GameState::getOwnShipIdAt(uint8_t x, uint8_t y) const {
  if (x >= GRID_LEN || y >= GRID_LEN) return 0;
  return ownShipId[y][x];
}

uint8_t GameState::getOwnShipLengthById(uint8_t id) const {
  if (id == 0 || id > SHIP_COUNT) return 0;
  return shipLengths[id - 1];
}

void GameState::registerOwnHit(uint8_t shipId) {
  if (shipId == 0 || shipId > SHIP_COUNT) return;
  uint8_t idx = shipId - 1;
  if (ownShipHits[idx] < shipLengths[idx]) {
    ownShipHits[idx]++;
  }
}

bool GameState::isOwnShipSunk(uint8_t shipId) const {
  if (shipId == 0 || shipId > SHIP_COUNT) return false;
  uint8_t idx = shipId - 1;
  return ownShipHits[idx] >= shipLengths[idx];
}

void GameState::collectOwnShipCells(uint8_t shipId, SunkCells& cells) const {
  cells.count = 0;

  if (shipId == 0 || shipId > SHIP_COUNT) return;

  for (uint8_t y = 0; y < GRID_LEN; y++) {
    for (uint8_t x = 0; x < GRID_LEN; x++) {
      if (ownShipId[y][x] == shipId && cells.count < 5) {
        cells.x[cells.count] = x;
        cells.y[cells.count] = y;
        cells.count++;
      }
    }
  }
}

bool GameState::allEnemyShipsSunk() const {
  for (uint8_t i = 0; i < SHIP_TYPE_COUNT; i++) {
    if (enemyShips[i].anzahlLebend > 0) return false;
  }
  return true;
}

bool GameState::allOwnShipsSunk() const {
  for (uint8_t i = 0; i < SHIP_TYPE_COUNT; i++) {
    if (ownShips[i].anzahlLebend > 0) return false;
  }
  return true;
}

void GameState::markEnemyShipSunk(uint8_t len) {
  for (uint8_t i = 0; i < SHIP_TYPE_COUNT; i++) {
    if (enemyShips[i].groesse == len && enemyShips[i].anzahlLebend > 0) {
      enemyShips[i].anzahlLebend--;
      return;
    }
  }
}

void GameState::markOwnShipSunk(uint8_t len) {
  for (uint8_t i = 0; i < SHIP_TYPE_COUNT; i++) {
    if (ownShips[i].groesse == len && ownShips[i].anzahlLebend > 0) {
      ownShips[i].anzahlLebend--;
      return;
    }
  }
}

const char* GameState::turnText() const {
  return isPlayerTurn() ? "Du bist dran" : "Gegner ist dran";
}

const char* GameState::directionText() const {
  return horizontal ? "Horizontal" : "Vertikal";
}

const char* GameState::finishedText() const {
  return gameOver ? "Ja" : "Nein";
}

void GameState::setStatusMessage(const char* text) {
  strncpy(statusMessage, text, sizeof(statusMessage) - 1);
  statusMessage[sizeof(statusMessage) - 1] = '\0';
}

void GameState::updatePlacementMessage() {
  if (allShipsPlaced()) {
    setStatusMessage("Alle Schiffe gesetzt -> OK");
    return;
  }

  char buf[64];
  snprintf(buf, sizeof(buf), "Setze Schiff %u (%u), %s",
           currentShip + 1,
           shipLengths[currentShip],
           directionText());
  setStatusMessage(buf);
}