#include "GameController.h"
#include "message.h"
#include <cstdio>
#include <cstring>

StartSpieler MacVergleich(const uint8_t mac1[6], const uint8_t mac2[6]) {
  for (int i = 0; i < 6; i++) {
    if (mac1[i] < mac2[i]) return MAC1_BEGINNT;
    if (mac1[i] > mac2[i]) return MAC2_BEGINNT;
  }
  return MAC2_BEGINNT;
}

GameController::GameController(GameState& gameState,
                               DisplayManager& displayMgr,
                               TouchManager& touchMgr)
  : game(gameState),
    display(displayMgr),
    touch(touchMgr),
    playMode(MODE_PLACE_SHIPS),
    attackModeArmed(false),
    lastTouchDown(false),
    lastAttackX(255),
    lastAttackY(255) {
}

void GameController::begin() {
  Serial.begin(115200);
  delay(150);

  game.init();
  display.begin();
  touch.begin();

  game.clearEnemyView();
  game.updatePlacementMessage();

  display.drawInitialScreen(game);
}

bool GameController::punktInRechteck(int px, int py, int rx, int ry, int rw, int rh) const {
  return (px >= rx && px < rx + rw && py >= ry && py < ry + rh);
}

void GameController::updatePanelsIfVisible() {
  display.redrawBottomBar(game);
  if (game.statusSichtbar) {
    display.redrawStatusPanel(game);
  }
}

void GameController::finalizeIfGameEnded(const char* text) {
  if (text != nullptr) {
    game.setStatusMessage(text);
  }

  if (game.allEnemyShipsSunk()) {
    game.spielBeendet = true;
    game.spielStatus = SPIEL_BEENDET;
    playMode = MODE_WAIT_RESPONSE;
    attackModeArmed = false;
    game.setStatusMessage("Du hast gewonnen");
  } else if (game.allOwnShipsSunk()) {
    game.spielBeendet = true;
    game.spielStatus = SPIEL_BEENDET;
    playMode = MODE_WAIT_RESPONSE;
    attackModeArmed = false;
    game.setStatusMessage("Du hast verloren");
  }

  display.redrawAttackButton(false);
  updatePanelsIfVisible();
}

void GameController::bearbeiteSchiffPlatzierungTouch() {
  int sx, sy;
  bool touched = touch.touchZuScreen(sx, sy);

  if (!touched) {
    lastTouchDown = false;
    return;
  }
  if (lastTouchDown) return;
  lastTouchDown = true;

  if (touch.isQuitButton(sx, sy)) {
    statusPhase.quit(game);
    display.redrawStatusPanel(game);
    display.redrawBottomBar(game);
    return;
  }

  if (touch.isStatusButton(sx, sy)) {
    statusPhase.toggle(game);
    display.redrawStatusPanel(game);
    display.redrawBottomBar(game);
    return;
  }

  if (punktInRechteck(sx, sy, BTN_DIR_X, BTN_DIR_Y, BTN_DIR_W, BTN_DIR_H)) {
    game.horizontal = !game.horizontal;
    game.updatePlacementMessage();
    display.redrawPlacementButtons(game);
    display.redrawStatusPanel(game);
    display.redrawBottomBar(game);
    return;
  }

  if (punktInRechteck(sx, sy, BTN_OK_X, BTN_OK_Y, BTN_OK_W, BTN_OK_H)) {
    if (game.alleSchiffeGesetzt()) {
      game.spielStatus = SPIEL_LAEUFT;
      game.setPlayerTurn();
      playMode = MODE_ATTACK;
      attackModeArmed = false;
      game.setStatusMessage("Angriff starten");
      display.redrawAttackButton(false);
      display.redrawBottomBar(game);
    } else {
      game.updatePlacementMessage();
      display.redrawBottomBar(game);
    }
    return;
  }

  uint8_t feldX, feldY;
  if (!touch.screenZuEigenemFeld(sx, sy, feldX, feldY)) return;
  if (game.alleSchiffeGesetzt()) return;

  uint8_t laenge = GameState::schiffLaengen[game.aktuellesSchiff];

  if (!game.kannSchiffPlatzieren(feldX, feldY, laenge, game.horizontal)) {
    game.setStatusMessage("Passt nicht / ueberlappt");
    display.redrawBottomBar(game);
    return;
  }

  game.setzeSchiff(feldX, feldY, laenge, game.horizontal);

  if (game.horizontal) {
    for (uint8_t i = 0; i < laenge; i++) {
      display.drawOwnCell(feldX + i, feldY, 1);
    }
  } else {
    for (uint8_t i = 0; i < laenge; i++) {
      display.drawOwnCell(feldX, feldY + i, 1);
    }
  }

  game.aktuellesSchiff++;
  game.updatePlacementMessage();
  display.redrawBottomBar(game);
}

void GameController::bearbeiteAttackTouch() {
  int sx, sy;
  bool touched = touch.touchZuScreen(sx, sy);

  if (!touched) {
    lastTouchDown = false;
    return;
  }
  if (lastTouchDown) return;
  lastTouchDown = true;

  if (touch.isQuitButton(sx, sy)) {
    statusPhase.quit(game);
    display.redrawStatusPanel(game);
    display.redrawBottomBar(game);
    return;
  }

  if (touch.isStatusButton(sx, sy)) {
    statusPhase.toggle(game);
    display.redrawStatusPanel(game);
    display.redrawBottomBar(game);
    return;
  }

  if (punktInRechteck(sx, sy, ATTACK_BTN_X, ATTACK_BTN_Y, ATTACK_BTN_W, ATTACK_BTN_H)) {
    if (game.spielBeendet) {
      game.setStatusMessage("Spiel beendet");
      updatePanelsIfVisible();
      return;
    }

    if (!game.isPlayerTurn()) {
      attackModeArmed = false;
      game.setStatusMessage("Gegner ist dran");
      display.redrawAttackButton(false);
      updatePanelsIfVisible();
      return;
    }

    if (playMode == MODE_ATTACK) {
      attackModeArmed = true;
      game.setStatusMessage("Zielfeld waehlen");
      display.redrawAttackButton(true);
      display.redrawBottomBar(game);
    }
    return;
  }

  if (!(playMode == MODE_ATTACK && attackModeArmed && game.isPlayerTurn())) return;

  uint8_t gx, gy;
  if (!touch.getEnemyGridCell(sx, sy, gx, gy)) return;

  char outBuffer[64];
  if (attackPhase.createAttackCommand(gx, gy, outBuffer, sizeof(outBuffer))) {
    char msg[64];
    snprintf(msg, sizeof(msg), "Angriff: %u,%u", gx, gy);

    lastAttackX = gx;
    lastAttackY = gy;
    attackModeArmed = false;
    playMode = MODE_WAIT_RESPONSE;

  if (sendMessage(SysRequestCharacteristic, ATTACK, gx, gy)) {
      Serial.print("Send attack");
      Serial.print(gx);
      Serial.print(", ");
      Serial.println(gy);
    }

    game.setStatusMessage(msg);
    display.redrawAttackButton(false);
    display.redrawBottomBar(game);

    Serial.println(outBuffer);
  } else {
    attackModeArmed = false;
    game.setStatusMessage(outBuffer);
    display.redrawAttackButton(false);
    display.redrawBottomBar(game);
  }
}

void GameController::verarbeiteGegnerAngriff(uint16_t moveNumber, uint8_t ax, uint8_t ay) {
  char responseBuffer[128];
  uint8_t cellState = game.meinFeld[ay][ax];

  if (cellState == 0 || cellState == 4) {
    game.meinFeld[ay][ax] = 4;
    display.drawOwnCell(ax, ay, 4);

    snprintf(responseBuffer, sizeof(responseBuffer),
             "move %u resp %u repeat %u",
             moveNumber, RESP_WATER, 0);

    Serial.println(responseBuffer);

    game.setPlayerTurn();
    playMode = MODE_ATTACK;
    attackModeArmed = false;
    game.setStatusMessage("Gegner traf Wasser");
    finalizeIfGameEnded(nullptr);
    return;
  }

  uint8_t shipId = game.getOwnShipIdAt(ax, ay);

  if (cellState == 1) {
    game.meinFeld[ay][ax] = 2;
    game.registerOwnShipHitById(shipId);

    if (game.isOwnShipSunkById(shipId)) {
      SunkInfo sunkInfo;
      game.collectOwnShipCells(shipId, sunkInfo);

      for (uint8_t i = 0; i < sunkInfo.count; i++) {
        game.meinFeld[sunkInfo.y[i]][sunkInfo.x[i]] = 3;
        display.drawOwnCell(sunkInfo.x[i], sunkInfo.y[i], 3);
      }

      game.markOwnShipSunk(game.getOwnShipLengthById(shipId));

      int written = snprintf(responseBuffer, sizeof(responseBuffer),
                             "move %u resp %u repeat %u sunk ",
                             moveNumber, RESP_SUNK, 1);

      for (uint8_t i = 0; i < sunkInfo.count && written > 0 && written < (int)sizeof(responseBuffer); i++) {
        written += snprintf(responseBuffer + written,
                            sizeof(responseBuffer) - written,
                            "%u,%u%s",
                            sunkInfo.x[i],
                            sunkInfo.y[i],
                            (i + 1 < sunkInfo.count) ? ";" : "");
      }

      Serial.println(responseBuffer);

      game.setEnemyTurn();
      playMode = MODE_WAIT_RESPONSE;
      attackModeArmed = false;
      game.setStatusMessage("Eigenes Schiff versenkt");
      finalizeIfGameEnded(nullptr);
      return;
    }

    display.drawOwnCell(ax, ay, 2);

    snprintf(responseBuffer, sizeof(responseBuffer),
             "move %u resp %u repeat %u",
             moveNumber, RESP_HIT, 1);

    Serial.println(responseBuffer);

    game.setEnemyTurn();
    playMode = MODE_WAIT_RESPONSE;
    attackModeArmed = false;
    game.setStatusMessage("Gegner traf Schiff");
    finalizeIfGameEnded(nullptr);
    return;
  }

  if (cellState == 2) {
    display.drawOwnCell(ax, ay, 2);

    snprintf(responseBuffer, sizeof(responseBuffer),
             "move %u resp %u repeat %u",
             moveNumber, RESP_HIT, 1);

    Serial.println(responseBuffer);

    game.setEnemyTurn();
    playMode = MODE_WAIT_RESPONSE;
    attackModeArmed = false;
    game.setStatusMessage("Bereits getroffen");
    finalizeIfGameEnded(nullptr);
    return;
  }

  if (cellState == 3) {
    SunkInfo sunkInfo;
    game.collectOwnShipCells(shipId, sunkInfo);

    int written = snprintf(responseBuffer, sizeof(responseBuffer),
                           "move %u resp %u repeat %u sunk ",
                           moveNumber, RESP_SUNK, 1);

    for (uint8_t i = 0; i < sunkInfo.count && written > 0 && written < (int)sizeof(responseBuffer); i++) {
      written += snprintf(responseBuffer + written,
                          sizeof(responseBuffer) - written,
                          "%u,%u%s",
                          sunkInfo.x[i],
                          sunkInfo.y[i],
                          (i + 1 < sunkInfo.count) ? ";" : "");
    }

    Serial.println(responseBuffer);

    game.setEnemyTurn();
    playMode = MODE_WAIT_RESPONSE;
    attackModeArmed = false;
    game.setStatusMessage("Bereits versenkt");
    finalizeIfGameEnded(nullptr);
    return;
  }
}

void GameController::bearbeiteSerielleAntwort() {
  static char incoming[128];
  static uint8_t idx = 0;
  Message received;
  
   while (Serial.available()) {
    char c = (char)Serial.read();
    if (c == '\r') continue;

    if (c == '\n') {
      incoming[idx] = '\0';
      idx = 0;

      if (incoming[0] == '\0') return;

      if (strcmp(incoming, "start") == 0) {
        game.spielStatus = SPIEL_LAEUFT;
        game.setPlayerTurn();
        playMode = MODE_ATTACK;
        attackModeArmed = false;
        game.setStatusMessage("Spiel gestartet");
        display.redrawAttackButton(false);
        updatePanelsIfVisible();
        return;
      }

      uint16_t moveNumber = 0;
      uint8_t ax = 0;
      uint8_t ay = 0;
      Message received;

      if (receiveMessage(SysRequestCharacteristic, received))
      {
        ax = received.x;
        ay = received.y;
      }
  
      if (attackPhase.parseAttackCommand(incoming, moveNumber, ax, ay)) {
        if (!game.spielBeendet) {
          game.spielStatus = SPIEL_LAEUFT;
          verarbeiteGegnerAngriff(moveNumber, ax, ay);
        }
        return;
      }

      uint8_t response = 0;
      switch (received.type)
      {
        case HIT:
          response = RESP_HIT;
          break;
        case WATER:
          response = RESP_WATER;
          break;
        case SUNK:
          response = RESP_SUNK;
          break;
      }
      bool repeatTurn = false;
      SunkInfo sunkInfo;

      if (attackPhase.parseResponseCommandExtended(incoming, moveNumber, response, repeatTurn, sunkInfo)) {
        if (lastAttackX >= GRID_SIZE || lastAttackY >= GRID_SIZE) {
          game.setStatusMessage("Antwort ohne Angriff");
          updatePanelsIfVisible();
          return;
        }

        if (response == RESP_WATER) {
          game.setEnemyCellState(lastAttackX, lastAttackY, ENEMY_WATER);
          display.drawEnemyWater(lastAttackX, lastAttackY);
          game.setStatusMessage("Wasser");
        } else if (response == RESP_HIT) {
          game.setEnemyCellState(lastAttackX, lastAttackY, ENEMY_HIT);
          display.drawEnemyHit(lastAttackX, lastAttackY);
          game.setStatusMessage("Treffer");
        } else if (response == RESP_SUNK) {
          uint8_t sunkLen = 1;

          if (sunkInfo.count > 0) {
            sunkLen = sunkInfo.count;
            for (uint8_t i = 0; i < sunkInfo.count; i++) {
              game.setEnemyCellState(sunkInfo.x[i], sunkInfo.y[i], ENEMY_SUNK);
              display.drawEnemySunkCell(sunkInfo.x[i], sunkInfo.y[i]);
            }
          } else {
            game.setEnemyCellState(lastAttackX, lastAttackY, ENEMY_SUNK);
            display.drawEnemySunkCell(lastAttackX, lastAttackY);
          }

          game.markEnemyShipSunk(sunkLen);
          game.setStatusMessage("Versenkt");
        }

        attackModeArmed = false;
        display.redrawAttackButton(false);

        if (repeatTurn) {
          game.setPlayerTurn();
          playMode = MODE_ATTACK;
        } else {
          game.setEnemyTurn();
          playMode = MODE_WAIT_RESPONSE;
        }

        finalizeIfGameEnded(nullptr);
        return;
      }

      if (strcmp(incoming, "already used") == 0) {
        playMode = MODE_ATTACK;
        game.setPlayerTurn();
        game.setStatusMessage("Bereits benutzt");
        display.redrawAttackButton(false);
        updatePanelsIfVisible();
        return;
      }

      game.setStatusMessage("Unbekannte Nachricht");
      updatePanelsIfVisible();
      return;
    }

    if (idx < sizeof(incoming) - 1) {
      incoming[idx++] = c;
    }
  }
}

void GameController::update() {
  bearbeiteSerielleAntwort();

  if (game.spielStatus == SCHIFFE_PLATZIEREN) {
    bearbeiteSchiffPlatzierungTouch();
  } else if (game.spielStatus == SPIEL_LAEUFT) {
    bearbeiteAttackTouch();
  } else {
    int sx, sy;
    bool touched = touch.touchZuScreen(sx, sy);

    if (!touched) {
      lastTouchDown = false;
      return;
    }
    if (lastTouchDown) return;
    lastTouchDown = true;

    if (touch.isQuitButton(sx, sy)) {
      statusPhase.quit(game);
      display.redrawStatusPanel(game);
      display.redrawBottomBar(game);
    } else if (touch.isStatusButton(sx, sy)) {
      statusPhase.toggle(game);
      display.redrawStatusPanel(game);
      display.redrawBottomBar(game);
    }
  }
}