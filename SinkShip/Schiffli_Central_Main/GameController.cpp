#include "GameController.h"
#include "AppConfig.h"
#include <cstdio>
#include <cstring>

GameController::GameController(GameState& gameState,
                               DisplayManager& displayMgr,
                               TouchManager& touchMgr,
                               BleLink& bleLink)
  : game(gameState),
    display(displayMgr),
    touch(touchMgr),
    ble(bleLink),
    dirtyFullRedraw(true) {
}

void GameController::begin() {
  game.resetAll();
  game.setStatusMessage("Bereit fuer Suche");
  dirtyFullRedraw = true;
}

void GameController::enterGameScreen() {
  game.screen = UiScreen::GAME;
  game.gameOpened = true;
  game.resetBoardOnly();
  game.initiatorBegins = ble.isInitiator();

  if (game.initiatorBegins) {
    game.setStatusMessage("Du suchst -> du beginnst spaeter");
  } else {
    game.setStatusMessage("Gegner suchte -> Gegner beginnt spaeter");
  }

  dirtyFullRedraw = true;
}

void GameController::startNewConnectedGame() {
  game.resetBoardOnly();
  game.screen = UiScreen::GAME;
  game.gameOpened = true;
  game.initiatorBegins = ble.isInitiator();
  game.setStatusMessage("Neues Spiel: Schiffe setzen");
  dirtyFullRedraw = true;
}

void GameController::processAttackFromEnemy(uint8_t x, uint8_t y) {
  if (x >= GRID_SIZE || y >= GRID_SIZE) return;

  Packet resp{};
  resp.type = PKT_RESULT;
  resp.x = x;
  resp.y = y;
  resp.count = 0;

  uint8_t state = game.ownGrid[y][x];

  if (state == 0 || state == 4) {
    game.ownGrid[y][x] = 4;
    display.drawOwnCell(x, y, 4);

    resp.value = RES_WATER;
    ble.send(resp);

    game.setPlayerTurn();
    game.attackArmed = false;
    if (!game.gameOver) game.setStatusMessage("Gegner traf Wasser");
    display.redrawBottomBar(game);
    return;
  }

  uint8_t shipId = game.getOwnShipIdAt(x, y);

  if (state == 1) {
    game.ownGrid[y][x] = 2;
    display.drawOwnCell(x, y, 2);
    game.registerOwnHit(shipId);

    if (game.isOwnShipSunk(shipId)) {
      SunkCells cells;
      game.collectOwnShipCells(shipId, cells);

      for (uint8_t i = 0; i < cells.count; i++) {
        game.ownGrid[cells.y[i]][cells.x[i]] = 3;
        display.drawOwnCell(cells.x[i], cells.y[i], 3);
      }

      resp.value = RES_SUNK;
      resp.count = cells.count;
      for (uint8_t i = 0; i < cells.count && i < 5; i++) {
        resp.payload[i * 2]     = cells.x[i];
        resp.payload[i * 2 + 1] = cells.y[i];
      }

      game.markOwnShipSunk(game.getOwnShipLengthById(shipId));
      ble.send(resp);

      game.setEnemyTurn();
      game.setStatusMessage("Eigenes Schiff versenkt");
    } else {
      resp.value = RES_HIT;
      ble.send(resp);

      game.setEnemyTurn();
      game.setStatusMessage("Gegner traf Schiff");
    }

    if (game.allOwnShipsSunk()) {
      game.gameOver = true;
      game.phase = GamePhase::FINISHED;
      game.setStatusMessage("Du hast verloren");
    }

    display.redrawBottomBar(game);
    if (game.statusVisible) display.redrawStatusPanel(game);
    return;
  }

  if (state == 2 || state == 3) {
    resp.value = (state == 3) ? RES_SUNK : RES_HIT;

    if (state == 3) {
      SunkCells cells;
      game.collectOwnShipCells(shipId, cells);
      resp.count = cells.count;
      for (uint8_t i = 0; i < cells.count && i < 5; i++) {
        resp.payload[i * 2]     = cells.x[i];
        resp.payload[i * 2 + 1] = cells.y[i];
      }
    }

    ble.send(resp);
    game.setEnemyTurn();
    game.setStatusMessage("Bereits getroffen");
    display.redrawBottomBar(game);
  }
}

void GameController::processResultFromEnemy(const Packet& p) {
  if (p.x >= GRID_SIZE || p.y >= GRID_SIZE) return;

  game.attackArmed = false;
  display.redrawAttackButton(false);

  if (p.value == RES_WATER) {
    game.setEnemyCell(p.x, p.y, ENEMY_WATER);
    display.drawEnemyCell(p.x, p.y, ENEMY_WATER);
    game.setEnemyTurn();
    game.setStatusMessage("Wasser");
  } else if (p.value == RES_HIT) {
    game.setEnemyCell(p.x, p.y, ENEMY_HIT);
    display.drawEnemyCell(p.x, p.y, ENEMY_HIT);
    game.setPlayerTurn();
    game.setStatusMessage("Treffer");
  } else if (p.value == RES_SUNK) {
    uint8_t len = p.count;
    if (p.count > 0) {
      for (uint8_t i = 0; i < p.count && i < 5; i++) {
        uint8_t sx = p.payload[i * 2];
        uint8_t sy = p.payload[i * 2 + 1];
        game.setEnemyCell(sx, sy, ENEMY_SUNK);
        display.drawEnemyCell(sx, sy, ENEMY_SUNK);
      }
    } else {
      game.setEnemyCell(p.x, p.y, ENEMY_SUNK);
      display.drawEnemyCell(p.x, p.y, ENEMY_SUNK);
      len = 1;
    }

    game.markEnemyShipSunk(len);
    game.setPlayerTurn();
    game.setStatusMessage("Versenkt");
  }

  if (game.allEnemyShipsSunk()) {
    game.gameOver = true;
    game.phase = GamePhase::FINISHED;
    game.setStatusMessage("Du hast gewonnen");
  }

  display.redrawBottomBar(game);
  if (game.statusVisible) display.redrawStatusPanel(game);
}

void GameController::requestRestart() {
  if (!ble.isConnected()) return;

  Packet p = makeSimplePacket(PKT_RESTART_REQ);
  ble.send(p);

  game.setStatusMessage("Neustart angefragt");
  display.redrawBottomBar(game);
}

void GameController::acceptRestart() {
  Packet p = makeSimplePacket(PKT_RESTART_ACCEPT);
  ble.send(p);

  game.restartPending = false;
  startNewConnectedGame();
}

void GameController::declineRestart() {
  Packet p = makeSimplePacket(PKT_RESTART_DECLINE);
  ble.send(p);

  game.restartPending = false;
  game.screen = UiScreen::GAME;
  game.setStatusMessage("Neustart abgelehnt");
  dirtyFullRedraw = true;
}

void GameController::quitGame() {
  Packet p = makeSimplePacket(PKT_QUIT);
  ble.send(p);
  ble.disconnect();

  game.resetAll();
  game.setStatusMessage("Spiel beendet / Verbindung getrennt");
  dirtyFullRedraw = true;
}

void GameController::handleBlePackets() {
  Packet p{};
  while (ble.receive(p)) {
    switch (p.type) {
      case PKT_INVITE:
        game.invitePending = true;
        game.screen = UiScreen::INVITE_DIALOG;
        game.setStatusMessage("Verbindungsanfrage erhalten");
        dirtyFullRedraw = true;
        break;

      case PKT_INVITE_ACCEPT:
        game.connectionAccepted = true;
        game.screen = UiScreen::LOBBY;
        game.setStatusMessage("Verbunden - bereit zum Start");
        dirtyFullRedraw = true;
        break;

      case PKT_INVITE_DECLINE:
        game.connectionAccepted = false;
        game.screen = UiScreen::LOBBY;
        game.setStatusMessage("Verbindung abgelehnt");
        dirtyFullRedraw = true;
        ble.disconnect();
        break;

      case PKT_OPEN_GAME:
        game.connectionAccepted = true;
        game.setStatusMessage("Spiel wird gestartet...");
        enterGameScreen();
        break;

      case PKT_READY:
        game.opponentReady = true;

        // WICHTIG:
        // Wenn ich selbst noch NICHT ready bin, darf ich WEITER Schiffe setzen.
        if (game.myReady) {
          game.phase = GamePhase::RUNNING;
          if (game.initiatorBegins) game.setPlayerTurn();
          else game.setEnemyTurn();
          game.setStatusMessage(game.isPlayerTurn() ? "Spiel laeuft: Du beginnst" : "Spiel laeuft: Gegner beginnt");
        } else {
          // NICHT auf WAIT_READY wechseln!
          // Gegner ist bereit, aber ich darf weiter platzieren.
          game.phase = GamePhase::PLACE_SHIPS;
          game.setStatusMessage("Gegner ist bereit - setze deine Schiffe");
        }

        dirtyFullRedraw = true;
        break;

      case PKT_ATTACK:
        if (game.phase == GamePhase::RUNNING && !game.gameOver) {
          processAttackFromEnemy(p.x, p.y);
        }
        break;

      case PKT_RESULT:
        if (game.phase == GamePhase::RUNNING && !game.gameOver) {
          processResultFromEnemy(p);
        }
        break;

      case PKT_RESTART_REQ:
        game.restartPending = true;
        game.screen = UiScreen::RESTART_DIALOG;
        game.setStatusMessage("Neustart angefragt");
        dirtyFullRedraw = true;
        break;

      case PKT_RESTART_ACCEPT:
        startNewConnectedGame();
        game.setStatusMessage("Neustart angenommen");
        break;

      case PKT_RESTART_DECLINE:
        game.setStatusMessage("Neustart abgelehnt");
        display.redrawBottomBar(game);
        break;

      case PKT_QUIT:
        ble.disconnect();
        game.resetAll();
        game.setStatusMessage("Gegner hat das Spiel beendet");
        dirtyFullRedraw = true;
        break;

      default:
        break;
    }
  }

  if (!ble.isConnected() && game.screen != UiScreen::LOBBY) {
    game.resetAll();
    game.setStatusMessage("Verbindung verloren");
    dirtyFullRedraw = true;
  }
}

void GameController::handleLobbyTouch(int sx, int sy) {
  if (touch.isInside(sx, sy, BTN_SEARCH_X, BTN_SEARCH_Y, BTN_SEARCH_W, BTN_SEARCH_H)) {
    ble.startScan();
    game.setStatusMessage("Suche laeuft...");
    dirtyFullRedraw = true;
    return;
  }

  if (touch.isInside(sx, sy, BTN_CONNECT_X, BTN_CONNECT_Y, BTN_CONNECT_W, BTN_CONNECT_H)) {
    if (!ble.hasFoundDevice()) {
      game.setStatusMessage("Kein Spieler gefunden");
      dirtyFullRedraw = true;
      return;
    }

    if (!ble.connectToFound()) {
      game.setStatusMessage("Verbinden fehlgeschlagen");
      dirtyFullRedraw = true;
      return;
    }

    Packet p = makeSimplePacket(PKT_INVITE);
    ble.send(p);

    game.connectionAccepted = false;
    game.setStatusMessage("Gegner angefragt...");
    dirtyFullRedraw = true;
    return;
  }

  if (touch.isInside(sx, sy, BTN_TO_GAME_X, BTN_TO_GAME_Y, BTN_TO_GAME_W, BTN_TO_GAME_H)) {
    if (!(ble.isConnected() && game.connectionAccepted)) {
      game.setStatusMessage("Noch keine bestaetigte Verbindung");
      dirtyFullRedraw = true;
      return;
    }

    Packet p = makeSimplePacket(PKT_OPEN_GAME);
    ble.send(p);

    game.setStatusMessage("Spiel wird gestartet...");
    enterGameScreen();
    return;
  }
}

void GameController::handleInviteDialogTouch(int sx, int sy) {
  if (touch.isInside(sx, sy, DIALOG_ACCEPT_X, DIALOG_ACCEPT_Y, DIALOG_ACCEPT_W, DIALOG_ACCEPT_H)) {
    Packet p = makeSimplePacket(PKT_INVITE_ACCEPT);
    ble.send(p);

    game.invitePending = false;
    game.connectionAccepted = true;
    game.screen = UiScreen::LOBBY;
    game.setStatusMessage("Einladung angenommen");
    dirtyFullRedraw = true;
    return;
  }

  if (touch.isInside(sx, sy, DIALOG_DECLINE_X, DIALOG_DECLINE_Y, DIALOG_DECLINE_W, DIALOG_DECLINE_H)) {
    Packet p = makeSimplePacket(PKT_INVITE_DECLINE);
    ble.send(p);

    game.invitePending = false;
    game.connectionAccepted = false;
    game.screen = UiScreen::LOBBY;
    game.setStatusMessage("Einladung abgelehnt");
    ble.disconnect();
    dirtyFullRedraw = true;
  }
}

void GameController::handleRestartDialogTouch(int sx, int sy) {
  if (touch.isInside(sx, sy, DIALOG_ACCEPT_X, DIALOG_ACCEPT_Y, DIALOG_ACCEPT_W, DIALOG_ACCEPT_H)) {
    acceptRestart();
    return;
  }

  if (touch.isInside(sx, sy, DIALOG_DECLINE_X, DIALOG_DECLINE_Y, DIALOG_DECLINE_W, DIALOG_DECLINE_H)) {
    declineRestart();
    return;
  }
}

void GameController::handleGameTouch(int sx, int sy) {
  if (touch.isInside(sx, sy, STATUS_BUTTON_X, STATUS_BUTTON_Y, STATUS_BUTTON_W, STATUS_BUTTON_H)) {
    game.statusVisible = !game.statusVisible;
    dirtyFullRedraw = true;
    return;
  }

  if (touch.isInside(sx, sy, RESTART_BUTTON_X, RESTART_BUTTON_Y, RESTART_BUTTON_W, RESTART_BUTTON_H)) {
    requestRestart();
    return;
  }

  if (touch.isInside(sx, sy, QUIT_BUTTON_X, QUIT_BUTTON_Y, QUIT_BUTTON_W, QUIT_BUTTON_H)) {
    quitGame();
    return;
  }

  if (game.phase == GamePhase::PLACE_SHIPS) {
    // Wenn ich selbst bereits ready bin, darf ich NICHT mehr platzieren,
    // aber der Gegner darf auf seinem Gerät unabhängig weitermachen.
    if (game.myReady) {
      game.setStatusMessage("Du bist bereit - warte auf Gegner");
      display.redrawBottomBar(game);
      return;
    }

    if (touch.isInside(sx, sy, BTN_DIR_X, BTN_DIR_Y, BTN_DIR_W, BTN_DIR_H)) {
      game.horizontal = !game.horizontal;
      game.updatePlacementMessage();
      dirtyFullRedraw = true;
      return;
    }

    if (touch.isInside(sx, sy, BTN_OK_X, BTN_OK_Y, BTN_OK_W, BTN_OK_H)) {
      if (!game.allShipsPlaced()) {
        game.setStatusMessage("Noch nicht alle Schiffe gesetzt");
        display.redrawBottomBar(game);
        return;
      }

      game.myReady = true;
      Packet p = makeSimplePacket(PKT_READY);
      ble.send(p);

      if (game.opponentReady) {
        game.phase = GamePhase::RUNNING;
        if (game.initiatorBegins) game.setPlayerTurn();
        else game.setEnemyTurn();
        game.setStatusMessage(game.isPlayerTurn() ? "Spiel laeuft: Du beginnst" : "Spiel laeuft: Gegner beginnt");
      } else {
        game.phase = GamePhase::WAIT_READY;
        game.setStatusMessage("Bereit - warte auf Gegner");
      }

      dirtyFullRedraw = true;
      return;
    }

    uint8_t gx, gy;
    if (touch.getOwnGridCell(sx, sy, gx, gy)) {
      if (game.allShipsPlaced()) return;

      uint8_t len = GameState::shipLengths[game.currentShip];
      if (!game.canPlaceShip(gx, gy, len, game.horizontal)) {
        game.setStatusMessage("Passt nicht / ueberlappt");
        display.redrawBottomBar(game);
        return;
      }

      game.placeShip(gx, gy, len, game.horizontal);

      if (game.horizontal) {
        for (uint8_t i = 0; i < len; i++) {
          display.drawOwnCell(gx + i, gy, 1);
        }
      } else {
        for (uint8_t i = 0; i < len; i++) {
          display.drawOwnCell(gx, gy + i, 1);
        }
      }

      game.currentShip++;
      game.updatePlacementMessage();

      // Falls Gegner schon ready ist, aber ich noch am Platzieren bin,
      // soll diese Info sichtbar bleiben.
      if (game.opponentReady && !game.allShipsPlaced()) {
        game.setStatusMessage("Gegner ist bereit - setze deine Schiffe");
      }

      display.redrawBottomBar(game);
    }
    return;
  }

  if (game.phase == GamePhase::WAIT_READY) {
    game.setStatusMessage("Warte auf Gegner");
    display.redrawBottomBar(game);
    return;
  }

  if (game.phase == GamePhase::RUNNING) {
    if (touch.isInside(sx, sy, ATTACK_BTN_X, ATTACK_BTN_Y, ATTACK_BTN_W, ATTACK_BTN_H)) {
      if (game.gameOver) {
        game.setStatusMessage("Spiel beendet");
        display.redrawBottomBar(game);
        return;
      }

      if (!game.isPlayerTurn()) {
        game.setStatusMessage("Gegner ist dran");
        display.redrawBottomBar(game);
        return;
      }

      game.attackArmed = !game.attackArmed;
      game.setStatusMessage(game.attackArmed ? "Zielfeld waehlen" : "Attacke abgebrochen");
      display.redrawAttackButton(game.attackArmed);
      display.redrawBottomBar(game);
      return;
    }

    if (game.attackArmed && game.isPlayerTurn()) {
      uint8_t gx, gy;
      if (touch.getEnemyGridCell(sx, sy, gx, gy)) {
        if (game.enemyGrid[gy][gx] != ENEMY_UNKNOWN) {
          game.setStatusMessage("Feld bereits benutzt");
          display.redrawBottomBar(game);
          return;
        }

        game.selectedEnemyX = gx;
        game.selectedEnemyY = gy;
        game.enemySelected = true;

        Packet p{};
        p.type = PKT_ATTACK;
        p.x = gx;
        p.y = gy;
        ble.send(p);

        char buf[64];
        snprintf(buf, sizeof(buf), "Angriff: %u,%u", gx, gy);
        game.setStatusMessage(buf);
        game.attackArmed = false;
        display.redrawAttackButton(false);
        display.redrawBottomBar(game);
      }
    }
    return;
  }
}

void GameController::handleTouch() {
  int sx, sy;
  if (!touch.getTouchPressedEdge(sx, sy)) return;

  switch (game.screen) {
    case UiScreen::LOBBY:
      handleLobbyTouch(sx, sy);
      break;

    case UiScreen::INVITE_DIALOG:
      handleInviteDialogTouch(sx, sy);
      break;

    case UiScreen::RESTART_DIALOG:
      handleRestartDialogTouch(sx, sy);
      break;

    case UiScreen::GAME:
      handleGameTouch(sx, sy);
      break;
  }
}

void GameController::redrawIfNeeded() {
  if (!dirtyFullRedraw) return;

  switch (game.screen) {
    case UiScreen::LOBBY:
      display.drawLobby(game, ble);
      break;

    case UiScreen::INVITE_DIALOG:
      display.drawLobby(game, ble);
      display.drawInviteDialog("Verbindungsanfrage", "Ein Gegner moechte sich", "verbinden.");
      break;

    case UiScreen::RESTART_DIALOG:
      display.drawGame(game);
      display.drawInviteDialog("Neustart-Anfrage", "Der Gegner moechte", "neu starten.");
      break;

    case UiScreen::GAME:
      display.drawGame(game);
      break;
  }

  dirtyFullRedraw = false;
}

void GameController::update() {
  handleBlePackets();

  if (game.screen == UiScreen::LOBBY && ble.hasFoundDevice() && !ble.isConnected()) {
    if (strcmp(game.statusMessage, "Spieler gefunden - bereit zum Verbinden") != 0 &&
        strcmp(game.statusMessage, "Gegner angefragt...") != 0 &&
        strcmp(game.statusMessage, "Verbinden fehlgeschlagen") != 0) {
      game.setStatusMessage("Spieler gefunden - bereit zum Verbinden");
      dirtyFullRedraw = true;
    }
  }

  handleTouch();
  redrawIfNeeded();
}