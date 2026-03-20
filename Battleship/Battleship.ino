#include "Config.h"
#include "GameTypes.h"
#include "GameController.h"
#include "BLEManager.h"
#include "UIManager.h"

GameController game;
BLEManager ble;
UIManager ui;

unsigned long lastUiDrawAt = 0;

void sendReadyState() {
  Packet p = {MSG_READY, 0, 0, 1};
  if (!ble.sendRequest(p)) {
    game.setStatus("READY senden fehlgeschlagen");
  }
}

void sendReset() {
  Packet p = {MSG_RESET, 0, 0, 0};
  ble.sendRequest(p);
}

void processBlePackets() {
  while (ble.hasIncomingRequest()) {
    Packet packet = ble.popIncomingRequest();
    switch (packet.type) {
      case MSG_READY:
        game.setRemoteReady(packet.value != 0);
        if (game.isLocalReady() && game.isRemoteReady() && ble.role() == ROLE_CENTRAL) {
          game.setMyTurn(true);
        }
        break;

      case MSG_ATTACK: {
        Packet response = game.handleIncomingAttack(packet.x, packet.y);
        ble.sendResponse(response);
        break;
      }

      case MSG_RESET:
        game.resetAll();
        ble.resetSession();
        break;

      default:
        break;
    }
  }

  while (ble.hasIncomingResponse()) {
    Packet packet = ble.popIncomingResponse();
    if (packet.type == MSG_RESULT) {
      game.applyAttackResult(packet.x, packet.y, static_cast<AttackResult>(packet.value));
    }
  }
}

void handleTouch() {
  TouchAction action = ui.readTouch();

  if (action.rotatePressed && !game.isLocalReady()) {
    game.togglePlacementOrientation();
  }

  if (action.mixPressed && !game.isLocalReady()) {
    game.randomizeShips();
  }

  if (action.undoPressed && !game.isLocalReady()) {
    if (!game.removeLastPlacedShip()) {
      game.setStatus("Nichts zum Undo");
    }
  }

  if (action.myBoardPressed && !game.isLocalReady()) {
    game.placeCurrentShip(action.myX, action.myY);
  }

  if (action.readyPressed && !game.isLocalReady()) {
    if (!game.allShipsPlaced()) {
      game.setStatus("Erst alle Schiffe setzen");
    } else if (!ble.isReadyForTraffic()) {
      game.setStatus("BLE noch nicht bereit");
    } else {
      game.markLocalReady();
      sendReadyState();
    }
  }

  if (action.resetPressed) {
    game.resetAll();
    ble.resetSession();
    sendReset();
  }

  if (action.enemyBoardPressed) {
    if (game.queueAttack(action.enemyX, action.enemyY)) {
      Packet attack = game.takeQueuedAttack();
      if (!ble.sendRequest(attack)) {
        game.clearPendingAttack();
        game.setStatus("Angriff senden fehlgeschlagen");
      }
    }
  }
}

void setup() {
  Serial.begin(115200);
  delay(300);

  game.begin();
  ui.begin();

  if (!ble.begin()) {
    game.setStatus("BLE Fehler");
  } else {
    game.setStatus("Setze Schiffe");
  }

  ui.draw(game, ble);
}

void loop() {
  ble.update();
  processBlePackets();
  handleTouch();

  if (millis() - lastUiDrawAt >= Config::UI_REFRESH_MS) {
    lastUiDrawAt = millis();
    ui.draw(game, ble);
  }
}
