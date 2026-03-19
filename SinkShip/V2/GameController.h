#ifndef GAME_CONTROLLER_H
#define GAME_CONTROLLER_H

#include <Arduino.h>
#include "GameState.h"
#include "DisplayManager.h"
#include "TouchManager.h"
#include "Phase1.h"
#include "AttackPhase.h"
#include "StatusPhase.h"

enum PlayMode {
  MODE_PLACE_SHIPS,
  MODE_ATTACK,
  MODE_WAIT_RESPONSE
};

class GameController {
public:
  GameController(GameState& gameState,
                 DisplayManager& displayMgr,
                 TouchManager& touchMgr);

  void begin();
  void update();

private:
  GameState& game;
  DisplayManager& display;
  TouchManager& touch;

  AttackPhase attackPhase;
  StatusPhase statusPhase;

  PlayMode playMode;
  bool attackModeArmed;
  bool lastTouchDown;
  uint8_t lastAttackX;
  uint8_t lastAttackY;

  bool punktInRechteck(int px, int py, int rx, int ry, int rw, int rh) const;
  void bearbeiteSchiffPlatzierungTouch();
  void bearbeiteAttackTouch();
  void bearbeiteSerielleAntwort();

  void updatePanelsIfVisible();
  void finalizeIfGameEnded(const char* text);
  void verarbeiteGegnerAngriff(uint16_t moveNumber, uint8_t ax, uint8_t ay);
};

#endif