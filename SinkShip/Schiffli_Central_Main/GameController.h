#ifndef GAME_CONTROLLER_H
#define GAME_CONTROLLER_H

#include "GameState.h"
#include "DisplayManager.h"
#include "TouchManager.h"
#include "BleLink.h"
#include "Protocol.h"

class GameController {
public:
  GameController(GameState& gameState,
                 DisplayManager& displayMgr,
                 TouchManager& touchMgr,
                 BleLink& bleLink);

  void begin();
  void update();

private:
  GameState& game;
  DisplayManager& display;
  TouchManager& touch;
  BleLink& ble;

  bool dirtyFullRedraw;

  void handleBlePackets();
  void handleTouch();
  void handleLobbyTouch(int sx, int sy);
  void handleInviteDialogTouch(int sx, int sy);
  void handleRestartDialogTouch(int sx, int sy);
  void handleGameTouch(int sx, int sy);

  void enterGameScreen();
  void startNewConnectedGame();

  void processAttackFromEnemy(uint8_t x, uint8_t y);
  void processResultFromEnemy(const Packet& p);

  void requestRestart();
  void acceptRestart();
  void declineRestart();

  void quitGame();

  void redrawIfNeeded();
};

#endif