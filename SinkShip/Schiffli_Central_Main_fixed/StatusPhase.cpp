#include "StatusPhase.h"

StatusPhase::StatusPhase() {
}

void StatusPhase::enter(GameState& game) {
  game.showStatus();
  game.setStatusMessage("Status eingeblendet");
}

void StatusPhase::exit(GameState& game) {
  game.hideStatus();
  game.setStatusMessage("Status ausgeblendet");
}

void StatusPhase::toggle(GameState& game) {
  game.toggleStatus();
  game.setStatusMessage(game.statusSichtbar ? "Status eingeblendet" : "Status ausgeblendet");
}

void StatusPhase::quit(GameState& game) {
  game.requestQuit();
}