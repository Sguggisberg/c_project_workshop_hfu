#include "UIManager.h"
#include <string.h>

namespace {
  constexpr uint16_t COL_BG = 0x0841;
  constexpr uint16_t COL_PANEL = 0x10A2;
  constexpr uint16_t COL_PANEL_TOP = 0x18E3;
  constexpr uint16_t COL_TEXT = ILI9341_WHITE;
  constexpr uint16_t COL_MUTED = 0xBDF7;
  constexpr uint16_t COL_BORDER = 0x632C;
  constexpr uint16_t COL_SHIP = 0x4B3F;
  constexpr uint16_t COL_GRID = 0x5ACB;
  constexpr uint16_t COL_MISS = 0xC618;
  constexpr uint16_t COL_HIT = 0xF800;
  constexpr uint16_t COL_SUNK = 0xFD20;
  constexpr uint16_t COL_ACCENT = 0x055D;
  constexpr uint16_t COL_SUCCESS = 0x1D66;
  constexpr uint16_t COL_WARNING = 0xFBE0;
  constexpr uint16_t COL_DANGER = 0xF800;
  constexpr uint16_t COL_DISABLED = 0x52AA;
}

UIManager::UIManager()
  : tft_(Config::TFT_CS, Config::TFT_DC, Config::TFT_RST),
    ts_(Config::TOUCH_CS, Config::TOUCH_IRQ),
    lastTouchAt_(0), firstDraw_(true), currentScreen_(UI_SCREEN_WAITING),
    cachedConnectionLabel_(""), cachedStatus_(""),
    cachedPlacementHorizontal_(false), cachedPlacedShipCount_(255),
    cachedLocalReady_(false), cachedRemoteReady_(false), cachedMyTurn_(false),
    cachedFinished_(false), cachedDidIWin_(false), cachedBleReady_(false) {
  for (uint8_t y = 0; y < Config::GRID_SIZE; ++y) {
    for (uint8_t x = 0; x < Config::GRID_SIZE; ++x) {
      cachedMyBoard_[y][x] = static_cast<CellState>(255);
      cachedEnemyBoard_[y][x] = static_cast<EnemyCellState>(255);
    }
  }
}

void UIManager::begin() {
  tft_.begin();
  tft_.setRotation(1);
  tft_.setTextWrap(false);
  tft_.fillScreen(ILI9341_BLACK);
  delay(20);
  tft_.fillScreen(COL_BG);
  ts_.begin();
  ts_.setRotation(1);
}


void UIManager::resetCaches() {
  for (uint8_t y = 0; y < Config::GRID_SIZE; ++y) {
    for (uint8_t x = 0; x < Config::GRID_SIZE; ++x) {
      cachedMyBoard_[y][x] = static_cast<CellState>(255);
      cachedEnemyBoard_[y][x] = static_cast<EnemyCellState>(255);
    }
  }
  cachedConnectionLabel_ = "";
  cachedStatus_ = "";
  cachedPlacementHorizontal_ = false;
  cachedPlacedShipCount_ = 255;
  cachedLocalReady_ = false;
  cachedRemoteReady_ = false;
  cachedMyTurn_ = false;
  cachedFinished_ = false;
  cachedDidIWin_ = false;
  cachedBleReady_ = false;
}

UIScreenMode UIManager::decideScreenMode(const BLEManager& ble) const {
  if (ble.role() == ROLE_PERIPHERAL && !ble.isConnected()) {
    return UI_SCREEN_WAITING;
  }
  return UI_SCREEN_GAME;
}

void UIManager::drawBackground() {
  tft_.fillScreen(COL_BG);
  tft_.fillRoundRect(6, 6, 308, 46, 12, COL_PANEL_TOP);
  tft_.fillRoundRect(6, 54, 150, 128, 12, COL_PANEL);
  tft_.fillRoundRect(164, 54, 150, 128, 12, COL_PANEL);
  tft_.fillRect(12, 80, 120, 120, ILI9341_BLACK);
  tft_.fillRect(188, 80, 120, 120, ILI9341_BLACK);
  tft_.fillRoundRect(6, 184, 308, 50, 12, COL_PANEL);
  tft_.drawRoundRect(6, 6, 308, 46, 12, COL_BORDER);
  tft_.drawRoundRect(6, 54, 150, 128, 12, COL_BORDER);
  tft_.drawRoundRect(164, 54, 150, 128, 12, COL_BORDER);
  tft_.drawRoundRect(6, 184, 308, 50, 12, COL_BORDER);
}

void UIManager::drawStaticLayout() {
  drawBackground();
  drawPanelTitles();
}

void UIManager::clearHeaderArea() {
  tft_.fillRoundRect(8, 8, 304, 42, 12, COL_PANEL_TOP);
  tft_.drawRoundRect(6, 6, 308, 46, 12, COL_BORDER);
}

void UIManager::clearFooterArea() {
  tft_.fillRoundRect(8, 186, 304, 46, 12, COL_PANEL);
  tft_.drawRoundRect(6, 184, 308, 50, 12, COL_BORDER);
}

void UIManager::drawHeader(const GameController& game, const BLEManager& ble) {
  tft_.fillRect(12, 12, 150, 26, COL_PANEL_TOP);
  tft_.fillRect(176, 12, 128, 24, COL_PANEL_TOP);
  tft_.fillRect(176, 26, 128, 16, COL_PANEL_TOP);

  tft_.setTextColor(COL_TEXT, COL_PANEL_TOP);
  tft_.setTextSize(2);
  tft_.setCursor(14, 16);
  tft_.print("Battleship BLE");

  tft_.setTextSize(1);
  tft_.setTextColor(COL_MUTED, COL_PANEL_TOP);
  tft_.setCursor(180, 14);
  tft_.print(ble.connectionLabel());
  tft_.setCursor(180, 28);
  tft_.print(game.statusString());
}

void UIManager::drawPanelTitles() {
  // Grosszuegig loeschen, damit auch abgeschnittene alte Buchstaben sicher verschwinden.
  tft_.fillRoundRect(8, 56, 146, 22, 8, COL_PANEL);
  tft_.fillRoundRect(166, 56, 146, 22, 8, COL_PANEL);
  tft_.drawRoundRect(6, 54, 150, 128, 12, COL_BORDER);
  tft_.drawRoundRect(164, 54, 150, 128, 12, COL_BORDER);

  tft_.setTextSize(2);
  tft_.setTextColor(COL_TEXT, COL_PANEL);
  tft_.setCursor(14, 60);
  tft_.print("Meine Flotte");
  tft_.setCursor(176, 60);
  tft_.print("Gegner");
}

void UIManager::drawMyBoard(const GameController& game) {
  for (uint8_t y = 0; y < Config::GRID_SIZE; ++y) {
    for (uint8_t x = 0; x < Config::GRID_SIZE; ++x) {
      int px = Config::MY_BOARD_X + x * Config::BOARD_CELL;
      int py = Config::MY_BOARD_Y + y * Config::BOARD_CELL;
      tft_.fillRect(px, py, Config::BOARD_CELL - 1, Config::BOARD_CELL - 1, ILI9341_BLACK);
      CellState cell = game.myCell(x, y);
      if (cell == CELL_SHIP) {
        tft_.fillRoundRect(px + 2, py + 2, Config::BOARD_CELL - 4, Config::BOARD_CELL - 4, 4, COL_SHIP);
      } else if (cell == CELL_HIT) {
        tft_.fillRoundRect(px + 2, py + 2, Config::BOARD_CELL - 4, Config::BOARD_CELL - 4, 4, COL_HIT);
      } else if (cell == CELL_MISS) {
        tft_.fillCircle(px + Config::BOARD_CELL / 2, py + Config::BOARD_CELL / 2, 3, COL_MISS);
      }
      tft_.drawRect(px, py, Config::BOARD_CELL, Config::BOARD_CELL, COL_GRID);
    }
  }
}

void UIManager::drawEnemyBoard(const GameController& game) {
  for (uint8_t y = 0; y < Config::GRID_SIZE; ++y) {
    for (uint8_t x = 0; x < Config::GRID_SIZE; ++x) {
      int px = Config::ENEMY_BOARD_X + x * Config::BOARD_CELL;
      int py = Config::ENEMY_BOARD_Y + y * Config::BOARD_CELL;
      tft_.fillRect(px, py, Config::BOARD_CELL - 1, Config::BOARD_CELL - 1, ILI9341_BLACK);
      EnemyCellState cell = game.enemyCell(x, y);
      if (cell == ENEMY_HIT) {
        tft_.fillRoundRect(px + 2, py + 2, Config::BOARD_CELL - 4, Config::BOARD_CELL - 4, 4, COL_HIT);
      } else if (cell == ENEMY_SUNK) {
        tft_.fillRoundRect(px + 2, py + 2, Config::BOARD_CELL - 4, Config::BOARD_CELL - 4, 4, COL_SUNK);
      } else if (cell == ENEMY_MISS) {
        tft_.fillCircle(px + Config::BOARD_CELL / 2, py + Config::BOARD_CELL / 2, 3, COL_MISS);
      }
      tft_.drawRect(px, py, Config::BOARD_CELL, Config::BOARD_CELL, COL_GRID);
    }
  }
}

void UIManager::drawBoards(const GameController& game) {
  drawMyBoard(game);
  drawEnemyBoard(game);
}

bool UIManager::myBoardChanged(const GameController& game) const {
  for (uint8_t y = 0; y < Config::GRID_SIZE; ++y) {
    for (uint8_t x = 0; x < Config::GRID_SIZE; ++x) {
      if (cachedMyBoard_[y][x] != game.myCell(x, y)) return true;
    }
  }
  return false;
}

bool UIManager::enemyBoardChanged(const GameController& game) const {
  for (uint8_t y = 0; y < Config::GRID_SIZE; ++y) {
    for (uint8_t x = 0; x < Config::GRID_SIZE; ++x) {
      if (cachedEnemyBoard_[y][x] != game.enemyCell(x, y)) return true;
    }
  }
  return false;
}

bool UIManager::headerChanged(const GameController& game, const BLEManager& ble) const {
  return cachedConnectionLabel_ != ble.connectionLabel() || cachedStatus_ != game.statusString();
}

bool UIManager::buttonsChanged(const GameController& game, const BLEManager& ble) const {
  return cachedPlacementHorizontal_ != game.placementHorizontal() ||
         cachedPlacedShipCount_ != game.placedShipCount() ||
         cachedLocalReady_ != game.isLocalReady() ||
         cachedFinished_ != game.isFinished() ||
         cachedBleReady_ != ble.isReadyForTraffic();
}

bool UIManager::footerChanged(const GameController& game) const {
  return cachedLocalReady_ != game.isLocalReady() ||
         cachedRemoteReady_ != game.isRemoteReady() ||
         cachedMyTurn_ != game.isMyTurn() ||
         cachedFinished_ != game.isFinished() ||
         cachedDidIWin_ != game.didIWin() ||
         cachedPlacedShipCount_ != game.placedShipCount();
}

void UIManager::updateCachedState(const GameController& game, const BLEManager& ble) {
  for (uint8_t y = 0; y < Config::GRID_SIZE; ++y) {
    for (uint8_t x = 0; x < Config::GRID_SIZE; ++x) {
      cachedMyBoard_[y][x] = game.myCell(x, y);
      cachedEnemyBoard_[y][x] = game.enemyCell(x, y);
    }
  }
  cachedConnectionLabel_ = ble.connectionLabel();
  cachedStatus_ = game.statusString();
  cachedPlacementHorizontal_ = game.placementHorizontal();
  cachedPlacedShipCount_ = game.placedShipCount();
  cachedLocalReady_ = game.isLocalReady();
  cachedRemoteReady_ = game.isRemoteReady();
  cachedMyTurn_ = game.isMyTurn();
  cachedFinished_ = game.isFinished();
  cachedDidIWin_ = game.didIWin();
  cachedBleReady_ = ble.isReadyForTraffic();
}

void UIManager::drawButton(int x, int y, int w, int h, uint16_t fill, const char* label, bool enabled) {
  uint16_t color = enabled ? fill : COL_DISABLED;
  tft_.fillRoundRect(x, y, w, h, 8, color);
  tft_.drawRoundRect(x, y, w, h, 8, COL_BORDER);
  tft_.setTextColor(COL_TEXT, color);
  tft_.setTextSize(1);
  int tx = x + (w / 2) - ((int)strlen(label) * 3);
  int ty = y + (h / 2) - 4;
  tft_.setCursor(tx, ty);
  tft_.print(label);
}

void UIManager::drawBadge(int x, int y, int w, int h, uint16_t fill, const String& text, uint16_t textColor) {
  tft_.fillRoundRect(x, y, w, h, 8, fill);
  tft_.drawRoundRect(x, y, w, h, 8, COL_BORDER);
  tft_.setTextColor(textColor, fill);
  tft_.setTextSize(1);
  tft_.setCursor(x + 6, y + 4);
  tft_.print(text);
}

void UIManager::drawButtons(const GameController& game, const BLEManager& ble) {
  bool placing = !game.isLocalReady() && !game.isFinished();
  drawButton(Config::BTN_ROTATE_X, Config::BTN_ROTATE_Y, Config::BTN_ROTATE_W, Config::BTN_ROTATE_H, COL_ACCENT, game.placementHorizontal() ? "ROT H" : "ROT V", placing);
  drawButton(Config::BTN_MIX_X, Config::BTN_MIX_Y, Config::BTN_MIX_W, Config::BTN_MIX_H, COL_ACCENT, "MIX", placing);
  drawButton(Config::BTN_UNDO_X, Config::BTN_UNDO_Y, Config::BTN_UNDO_W, Config::BTN_UNDO_H, COL_WARNING, "UNDO", placing && game.placedShipCount() > 0);
  drawButton(Config::BTN_READY_X, Config::BTN_READY_Y, Config::BTN_READY_W, Config::BTN_READY_H, COL_SUCCESS, "OK", ble.isReadyForTraffic() && game.allShipsPlaced() && !game.isLocalReady());
  drawButton(Config::BTN_RESET_X, Config::BTN_RESET_Y, Config::BTN_RESET_W, Config::BTN_RESET_H, COL_DANGER, "RST", true);
}

void UIManager::drawFooter(const GameController& game) {
  tft_.fillRect(10, 210, 298, 18, COL_PANEL);
  String next = game.allShipsPlaced() ? "bereit" : String("naechst: ") + game.currentPlacementLength();
  drawBadge(12, 212, 80, 14, game.isLocalReady() ? COL_SUCCESS : COL_PANEL_TOP, game.isLocalReady() ? "ich bereit" : next, COL_TEXT);
  drawBadge(98, 212, 92, 14, game.isRemoteReady() ? COL_SUCCESS : COL_PANEL_TOP, game.isRemoteReady() ? "gegner bereit" : "warte gegner", COL_TEXT);
  String turnText = game.isFinished() ? (game.didIWin() ? "sieg" : "niederlage") : (game.isMyTurn() ? "du dran" : "warte");
  drawBadge(196, 212, 110, 14, game.isMyTurn() ? COL_SUCCESS : COL_PANEL_TOP, turnText, COL_TEXT);
}

void UIManager::drawWaitingScreen(const GameController& game, const BLEManager& ble) {
  tft_.fillScreen(COL_BG);
  tft_.fillRoundRect(16, 16, 288, 208, 18, COL_PANEL);
  tft_.drawRoundRect(16, 16, 288, 208, 18, COL_BORDER);

  tft_.fillRoundRect(28, 28, 264, 42, 12, COL_PANEL_TOP);
  tft_.setTextColor(COL_TEXT, COL_PANEL_TOP);
  tft_.setTextSize(2);
  tft_.setCursor(44, 42);
  tft_.print("Battleship BLE");

  drawBadge(34, 86, 118, 24, COL_ACCENT, ble.connectionLabel(), COL_TEXT);
  drawBadge(168, 86, 118, 24, COL_PANEL_TOP, game.statusString(), COL_TEXT);

  tft_.setTextColor(COL_TEXT, COL_PANEL);
  tft_.setTextSize(2);
  tft_.setCursor(42, 128);
  tft_.print("Warte auf Gegner");

  tft_.setTextColor(COL_MUTED, COL_PANEL);
  tft_.setTextSize(1);
  tft_.setCursor(42, 154);
  tft_.print("Dieses Board ist im Peripheral-Modus.");
  tft_.setCursor(42, 168);
  tft_.print("Sobald das andere Geraet verbindet,");
  tft_.setCursor(42, 182);
  tft_.print("wechselt die Anzeige automatisch zum Spiel.");

  drawButton(114, 196, 92, 24, COL_DANGER, "RESET", true);
}

void UIManager::drawGameScreen(const GameController& game, const BLEManager& ble) {
  drawStaticLayout();
  drawHeader(game, ble);
  drawBoards(game);
  drawButtons(game, ble);
  drawFooter(game);
}

void UIManager::draw(const GameController& game, const BLEManager& ble) {
  UIScreenMode targetScreen = decideScreenMode(ble);
  if (firstDraw_ || targetScreen != currentScreen_) {
    currentScreen_ = targetScreen;
    resetCaches();
    if (currentScreen_ == UI_SCREEN_WAITING) {
      drawWaitingScreen(game, ble);
      updateCachedState(game, ble);
    } else {
      drawGameScreen(game, ble);
      updateCachedState(game, ble);
    }
    firstDraw_ = false;
    return;
  }

  if (currentScreen_ == UI_SCREEN_WAITING) {
    if (headerChanged(game, ble) || footerChanged(game) || buttonsChanged(game, ble)) {
      drawWaitingScreen(game, ble);
      updateCachedState(game, ble);
    }
    return;
  }

  if (headerChanged(game, ble)) {
    clearHeaderArea();
    drawHeader(game, ble);
  }
  if (myBoardChanged(game)) {
    drawMyBoard(game);
  }
  if (enemyBoardChanged(game)) {
    drawEnemyBoard(game);
  }
  if (buttonsChanged(game, ble)) {
    drawButtons(game, ble);
  }
  if (footerChanged(game)) {
    clearFooterArea();
    drawButtons(game, ble);
    drawFooter(game);
  }

  updateCachedState(game, ble);
}

bool UIManager::pointInRect(int x, int y, int rx, int ry, int rw, int rh) const {
  return x >= rx && x < (rx + rw) && y >= ry && y < (ry + rh);
}

int UIManager::mapTouchX(int rawX) const {
  return map(rawX, Config::TOUCH_MIN_X, Config::TOUCH_MAX_X, 0, Config::SCREEN_W);
}

int UIManager::mapTouchY(int rawY) const {
  return map(rawY, Config::TOUCH_MIN_Y, Config::TOUCH_MAX_Y, 0, Config::SCREEN_H);
}

TouchAction UIManager::readTouch() {
  TouchAction action = {false, false, false, false, false, false, false, 0, 0, 0, 0};
  if (!ts_.touched()) return action;
  if (millis() - lastTouchAt_ < Config::TOUCH_DEBOUNCE_MS) return action;

  TS_Point p = ts_.getPoint();
  int sx = mapTouchX(p.x);
  int sy = mapTouchY(p.y);
  lastTouchAt_ = millis();

  if (currentScreen_ == UI_SCREEN_WAITING) {
    if (pointInRect(sx, sy, 114, 196, 92, 24)) { action.resetPressed = true; }
    return action;
  }

  if (pointInRect(sx, sy, Config::BTN_ROTATE_X, Config::BTN_ROTATE_Y, Config::BTN_ROTATE_W, Config::BTN_ROTATE_H)) { action.rotatePressed = true; return action; }
  if (pointInRect(sx, sy, Config::BTN_MIX_X, Config::BTN_MIX_Y, Config::BTN_MIX_W, Config::BTN_MIX_H)) { action.mixPressed = true; return action; }
  if (pointInRect(sx, sy, Config::BTN_UNDO_X, Config::BTN_UNDO_Y, Config::BTN_UNDO_W, Config::BTN_UNDO_H)) { action.undoPressed = true; return action; }
  if (pointInRect(sx, sy, Config::BTN_READY_X, Config::BTN_READY_Y, Config::BTN_READY_W, Config::BTN_READY_H)) { action.readyPressed = true; return action; }
  if (pointInRect(sx, sy, Config::BTN_RESET_X, Config::BTN_RESET_Y, Config::BTN_RESET_W, Config::BTN_RESET_H)) { action.resetPressed = true; return action; }

  if (pointInRect(sx, sy, Config::MY_BOARD_X, Config::MY_BOARD_Y, Config::BOARD_W, Config::BOARD_H)) {
    action.myBoardPressed = true;
    action.myX = (sx - Config::MY_BOARD_X) / Config::BOARD_CELL;
    action.myY = (sy - Config::MY_BOARD_Y) / Config::BOARD_CELL;
    return action;
  }

  if (pointInRect(sx, sy, Config::ENEMY_BOARD_X, Config::ENEMY_BOARD_Y, Config::BOARD_W, Config::BOARD_H)) {
    action.enemyBoardPressed = true;
    action.enemyX = (sx - Config::ENEMY_BOARD_X) / Config::BOARD_CELL;
    action.enemyY = (sy - Config::ENEMY_BOARD_Y) / Config::BOARD_CELL;
  }

  return action;
}
