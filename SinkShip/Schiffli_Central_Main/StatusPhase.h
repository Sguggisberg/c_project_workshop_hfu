#ifndef STATUS_PHASE_H
#define STATUS_PHASE_H

#include "GameState.h"

class StatusPhase {
public:
  StatusPhase();

  void enter(GameState& game);
  void exit(GameState& game);
  void toggle(GameState& game);
  void quit(GameState& game);
};

#endif