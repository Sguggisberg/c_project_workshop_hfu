#pragma once

#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <XPT2046_Touchscreen.h>
#include "Config.h"
#include "GameController.h"
#include "BLEManager.h"

struct TouchAction {
  bool rotatePressed;
  bool mixPressed;
  bool undoPressed;
  bool readyPressed;
  bool resetPressed;
  bool myBoardPressed;
  bool enemyBoardPressed;
  uint8_t myX;
  uint8_t myY;
  uint8_t enemyX;
  uint8_t enemyY;
};

enum UIScreenMode {
  UI_SCREEN_WAITING,
  UI_SCREEN_GAME
};

class UIManager {
public:
  UIManager();

  void begin();
  void draw(const GameController& game, const BLEManager& ble);
  TouchAction readTouch();

private:
  UIScreenMode decideScreenMode(const BLEManager& ble) const;
  void resetCaches();
  void drawBackground();
  void drawHeader(const GameController& game, const BLEManager& ble);
  void drawPanelTitles();
  void drawBoards(const GameController& game);
  void drawStaticLayout();
  void drawWaitingScreen(const GameController& game, const BLEManager& ble);
  void drawGameScreen(const GameController& game, const BLEManager& ble);
  void clearHeaderArea();
  void clearFooterArea();
  void updateCachedState(const GameController& game, const BLEManager& ble);
  bool myBoardChanged(const GameController& game) const;
  bool enemyBoardChanged(const GameController& game) const;
  bool headerChanged(const GameController& game, const BLEManager& ble) const;
  bool buttonsChanged(const GameController& game, const BLEManager& ble) const;
  bool footerChanged(const GameController& game) const;
  void drawMyBoard(const GameController& game);
  void drawEnemyBoard(const GameController& game);
  void drawButtons(const GameController& game, const BLEManager& ble);
  void drawFooter(const GameController& game);
  void drawButton(int x, int y, int w, int h, uint16_t fill, const char* label, bool enabled);
  void drawBadge(int x, int y, int w, int h, uint16_t fill, const String& text, uint16_t textColor);
  bool pointInRect(int x, int y, int rx, int ry, int rw, int rh) const;
  int mapTouchX(int rawX) const;
  int mapTouchY(int rawY) const;

  Adafruit_ILI9341 tft_;
  XPT2046_Touchscreen ts_;
  unsigned long lastTouchAt_;
  bool firstDraw_;
  UIScreenMode currentScreen_;

  CellState cachedMyBoard_[Config::GRID_SIZE][Config::GRID_SIZE];
  EnemyCellState cachedEnemyBoard_[Config::GRID_SIZE][Config::GRID_SIZE];
  String cachedConnectionLabel_;
  String cachedStatus_;
  bool cachedPlacementHorizontal_;
  uint8_t cachedPlacedShipCount_;
  bool cachedLocalReady_;
  bool cachedRemoteReady_;
  bool cachedMyTurn_;
  bool cachedFinished_;
  bool cachedDidIWin_;
  bool cachedBleReady_;
};
