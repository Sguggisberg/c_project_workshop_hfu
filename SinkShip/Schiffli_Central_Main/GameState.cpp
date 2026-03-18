#include "GameState.h"
#include <cstring>
#include <cstdio>

const uint8_t GameState::schiffLaengen[GameState::SHIP_COUNT] = {5, 4, 3, 3, 2};

GameState::GameState() {
  init();
}

void GameState::init() {
  amZug = TurnOwner::PLAYER;
  spielStatus = SCHIFFE_PLATZIEREN;
  spielBeendet = false;
  quitAngefordert = false;
  statusSichtbar = false;
  horizontal = true;
  aktuellesSchiff = 0;

  for (uint8_t y = 0; y < GRID_LEN; y++) {
    for (uint8_t x = 0; x < GRID_LEN; x++) {
      meinFeld[y][x] = 0;
      gegnerFeld[y][x] = ENEMY_UNKNOWN;
    }
  }

  initShipStatus();
  updatePlacementMessage();
}

void GameState::initShipStatus() {
  eigeneSchiffe[0] = {2, 1};
  eigeneSchiffe[1] = {3, 2};
  eigeneSchiffe[2] = {4, 1};
  eigeneSchiffe[3] = {5, 1};

  gegnerSchiffe[0] = {2, 1};
  gegnerSchiffe[1] = {3, 2};
  gegnerSchiffe[2] = {4, 1};
  gegnerSchiffe[3] = {5, 1};
}

bool GameState::isPlayerTurn() const {
  return amZug == TurnOwner::PLAYER;
}

void GameState::setPlayerTurn() {
  amZug = TurnOwner::PLAYER;
}

void GameState::setEnemyTurn() {
  amZug = TurnOwner::ENEMY;
}

void GameState::showStatus() {
  statusSichtbar = true;
}

void GameState::hideStatus() {
  statusSichtbar = false;
}

void GameState::toggleStatus() {
  statusSichtbar = !statusSichtbar;
}

void GameState::requestQuit() {
  quitAngefordert = true;
  spielBeendet = true;
  spielStatus = SPIEL_BEENDET;
  setStatusMessage("Spiel beendet");
}

const char* GameState::getTurnText() const {
  return isPlayerTurn() ? "Du bist dran" : "Gegner ist dran";
}

const char* GameState::getSpielBeendetText() const {
  return spielBeendet ? "Ja" : "Nein";
}

const char* GameState::getDirectionText() const {
  return horizontal ? "Horizontal" : "Vertikal";
}

void GameState::setStatusMessage(const char* text) {
  strncpy(statusMeldung, text, sizeof(statusMeldung) - 1);
  statusMeldung[sizeof(statusMeldung) - 1] = '\0';
}

void GameState::updatePlacementMessage() {
  if (alleSchiffeGesetzt()) {
    setStatusMessage("Alle Schiffe gesetzt -> OK");
    return;
  }

  char msg[64];
  snprintf(msg, sizeof(msg), "Setze Schiff %u (%u), %s",
           aktuellesSchiff + 1,
           schiffLaengen[aktuellesSchiff],
           getDirectionText());
  setStatusMessage(msg);
}

void GameState::clearEnemyView() {
  for (uint8_t y = 0; y < GRID_LEN; y++) {
    for (uint8_t x = 0; x < GRID_LEN; x++) {
      gegnerFeld[y][x] = ENEMY_UNKNOWN;
    }
  }
}

bool GameState::alleSchiffeGesetzt() const {
  return aktuellesSchiff >= SHIP_COUNT;
}

bool GameState::kannSchiffPlatzieren(uint8_t startX, uint8_t startY, uint8_t laenge, bool istHorizontal) const {
  if (istHorizontal) {
    if (startX + laenge > GRID_LEN) return false;
    for (uint8_t i = 0; i < laenge; i++) {
      if (meinFeld[startY][startX + i] != 0) return false;
    }
  } else {
    if (startY + laenge > GRID_LEN) return false;
    for (uint8_t i = 0; i < laenge; i++) {
      if (meinFeld[startY + i][startX] != 0) return false;
    }
  }
  return true;
}

void GameState::setzeSchiff(uint8_t startX, uint8_t startY, uint8_t laenge, bool istHorizontal) {
  if (istHorizontal) {
    for (uint8_t i = 0; i < laenge; i++) {
      meinFeld[startY][startX + i] = 1;
    }
  } else {
    for (uint8_t i = 0; i < laenge; i++) {
      meinFeld[startY + i][startX] = 1;
    }
  }
}

void GameState::setEnemyCellState(uint8_t x, uint8_t y, uint8_t state) {
  if (x >= GRID_LEN || y >= GRID_LEN) return;
  gegnerFeld[y][x] = state;
}

void GameState::markEnemyShipSunk(uint8_t shipLength) {
  for (uint8_t i = 0; i < SHIP_TYPE_COUNT; i++) {
    if (gegnerSchiffe[i].groesse == shipLength) {
      if (gegnerSchiffe[i].anzahlLebend > 0) {
        gegnerSchiffe[i].anzahlLebend--;
      }
      return;
    }
  }
}