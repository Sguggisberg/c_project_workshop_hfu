#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include "GameState.h"

class DisplayManager {
public:
  DisplayManager(Adafruit_ILI9341& display, uint8_t touchCsPin);

  void begin();
  void drawInitialScreen(const GameState& game);

  void redrawBottomBar(const GameState& game);
  void redrawStatusPanel(const GameState& game);
  void redrawPlacementButtons(const GameState& game);
  void redrawAttackButton(bool active);

  void drawOwnCell(uint8_t gx, uint8_t gy, uint8_t state);
  void drawEnemyWater(uint8_t gx, uint8_t gy);
  void drawEnemyHit(uint8_t gx, uint8_t gy);
  void drawEnemySunkCell(uint8_t gx, uint8_t gy);

private:
  Adafruit_ILI9341& tft;
  uint8_t touchCs;

  void selectDisplay();
  void drawBaseScreen(const GameState& game);
  void drawGridFrame(int startX, int startY, const char* titel, uint8_t gridSize);
  void drawStatusButton(bool active);
  void drawQuitButton();
  void drawStatusPanel(const GameState& game);
  void drawBottomText(const GameState& game);
  void drawButton(int x, int y, int w, int h, const char* text,
                  uint16_t frame, uint16_t textColor, uint16_t fill);
  uint16_t farbeFuerMeinFeld(uint8_t state) const;
};

#endif