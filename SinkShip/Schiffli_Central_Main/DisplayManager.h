#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include "GameState.h"
#include "BleLink.h"

class DisplayManager {
public:
  DisplayManager(Adafruit_ILI9341& display);

  void begin();
  void drawFatal(const char* text);

  void drawLobby(const GameState& game, const BleLink& ble);
  void drawInviteDialog(const char* title, const char* line1, const char* line2);
  void drawGame(const GameState& game);

  void redrawBottomBar(const GameState& game);
  void redrawStatusPanel(const GameState& game);
  void redrawAttackButton(bool active);
  void redrawPlacementButtons(const GameState& game);

  void drawOwnCell(uint8_t gx, uint8_t gy, uint8_t state);
  void drawEnemyCell(uint8_t gx, uint8_t gy, uint8_t state);

private:
  Adafruit_ILI9341& tft;

  void drawButton(int x, int y, int w, int h, const char* text,
                  uint16_t frame, uint16_t textColor, uint16_t fill);
  void drawGridFrame(int x, int y, const char* title);
  void drawStatusButton(bool active);
  void drawRestartButton();
  void drawQuitButton();
  void drawBottomText(const GameState& game);
  void drawStatusPanelInternal(const GameState& game);
  uint16_t ownColor(uint8_t state) const;
};

#endif