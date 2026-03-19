#include "DisplayManager.h"
#include "AppConfig.h"

DisplayManager::DisplayManager(Adafruit_ILI9341& display)
  : tft(display) {
}

void DisplayManager::begin() {
  tft.begin();
  tft.setRotation(1);
  tft.fillScreen(COLOR_BG);
}

void DisplayManager::drawFatal(const char* text) {
  tft.fillScreen(COLOR_BG);
  tft.setTextColor(COLOR_HIT);
  tft.setTextSize(2);
  tft.setCursor(12, 40);
  tft.print("Fehler");
  tft.setTextSize(1);
  tft.setCursor(12, 80);
  tft.print(text);
}

void DisplayManager::drawButton(int x, int y, int w, int h, const char* text,
                                uint16_t frame, uint16_t textColor, uint16_t fill) {
  tft.fillRoundRect(x, y, w, h, 4, fill);
  tft.drawRoundRect(x, y, w, h, 4, frame);
  tft.setTextColor(textColor);
  tft.setTextSize(1);
  tft.setCursor(x + 6, y + 8);
  tft.print(text);
}

void DisplayManager::drawLobby(const GameState& game, const BleLink& ble) {
  tft.fillScreen(COLOR_BG);

  tft.setTextColor(COLOR_TEXT);
  tft.setTextSize(2);
  tft.setCursor(12, LOBBY_TITLE_Y);
  tft.print("Schiffli BLE");

  tft.setTextSize(1);
  drawButton(BTN_SEARCH_X, BTN_SEARCH_Y, BTN_SEARCH_W, BTN_SEARCH_H,
             "Suchen", COLOR_BUTTON_FRAME, COLOR_BUTTON_TEXT, COLOR_BUTTON_FILL);

  uint16_t connectFill = ble.hasFoundDevice() ? COLOR_STATUS_ON : COLOR_BUTTON_FILL;
  drawButton(BTN_CONNECT_X, BTN_CONNECT_Y, BTN_CONNECT_W, BTN_CONNECT_H,
             "Verbinden", COLOR_BUTTON_FRAME, COLOR_BUTTON_TEXT, connectFill);

  uint16_t gameFill = (ble.isConnected() && game.connectionAccepted) ? COLOR_STATUS_ON : COLOR_BUTTON_FILL;
  drawButton(BTN_TO_GAME_X, BTN_TO_GAME_Y, BTN_TO_GAME_W, BTN_TO_GAME_H,
             "Zum Spiel", COLOR_BUTTON_FRAME, COLOR_BUTTON_TEXT, gameFill);

  tft.drawRoundRect(LOBBY_INFO_X, LOBBY_INFO_Y, LOBBY_INFO_W, LOBBY_INFO_H, 5, COLOR_PANEL_FRAME);

  tft.setTextColor(COLOR_TEXT);
  tft.setTextSize(1);

  tft.setCursor(LOBBY_INFO_X + 8, LOBBY_INFO_Y + 10);
  tft.print("Mein BLE Name:");
  tft.setCursor(LOBBY_INFO_X + 100, LOBBY_INFO_Y + 10);
  tft.print(GAME_DEVICE_NAME);

  tft.setCursor(LOBBY_INFO_X + 8, LOBBY_INFO_Y + 26);
  tft.print("Adresse:");
  tft.setCursor(LOBBY_INFO_X + 60, LOBBY_INFO_Y + 26);
  tft.print(BLE.address());

  tft.setCursor(LOBBY_INFO_X + 8, LOBBY_INFO_Y + 46);
  tft.print("Suche:");
  tft.setCursor(LOBBY_INFO_X + 60, LOBBY_INFO_Y + 46);
  tft.print(ble.isSearching() ? "aktiv" : "aus");

  tft.setCursor(LOBBY_INFO_X + 8, LOBBY_INFO_Y + 62);
  tft.print("Gefunden:");
  tft.setCursor(LOBBY_INFO_X + 60, LOBBY_INFO_Y + 62);
  tft.print(ble.hasFoundDevice() ? ble.foundName() : "-");

  tft.setCursor(LOBBY_INFO_X + 8, LOBBY_INFO_Y + 78);
  tft.print("Hinweis:");
  tft.setCursor(LOBBY_INFO_X + 60, LOBBY_INFO_Y + 78);
  tft.print("Suchen -> Verbinden");

  tft.setCursor(LOBBY_INFO_X + 8, LOBBY_INFO_Y + 94);
  tft.print("Status:");
  tft.setCursor(LOBBY_INFO_X + 60, LOBBY_INFO_Y + 94);
  tft.print(game.statusMessage);
}

void DisplayManager::drawInviteDialog(const char* title, const char* line1, const char* line2) {
  tft.fillRoundRect(DIALOG_X, DIALOG_Y, DIALOG_W, DIALOG_H, 6, COLOR_DIALOG_FILL);
  tft.drawRoundRect(DIALOG_X, DIALOG_Y, DIALOG_W, DIALOG_H, 6, COLOR_DIALOG_FRAME);

  tft.setTextColor(COLOR_TEXT);
  tft.setTextSize(1);
  tft.setCursor(DIALOG_X + 10, DIALOG_Y + 10);
  tft.print(title);

  tft.setCursor(DIALOG_X + 10, DIALOG_Y + 34);
  tft.print(line1);

  tft.setCursor(DIALOG_X + 10, DIALOG_Y + 48);
  tft.print(line2);

  drawButton(DIALOG_ACCEPT_X, DIALOG_ACCEPT_Y, DIALOG_ACCEPT_W, DIALOG_ACCEPT_H,
             "Annehmen", COLOR_BUTTON_FRAME, COLOR_BUTTON_TEXT, COLOR_ACCEPT_FILL);

  drawButton(DIALOG_DECLINE_X, DIALOG_DECLINE_Y, DIALOG_DECLINE_W, DIALOG_DECLINE_H,
             "Ablehnen", COLOR_BUTTON_FRAME, COLOR_QUIT_TEXT, COLOR_DECLINE_FILL);
}

void DisplayManager::drawGridFrame(int x, int y, const char* title) {
  tft.setTextColor(COLOR_TEXT);
  tft.setTextSize(1);
  tft.setCursor(x, y - 18);
  tft.print(title);

  for (uint8_t ix = 0; ix < GRID_SIZE; ix++) {
    tft.setCursor(x + 5 + ix * CELL, y - 10);
    tft.print(ix);
  }

  for (uint8_t iy = 0; iy < GRID_SIZE; iy++) {
    tft.setCursor(x - 10, y + 4 + iy * CELL);
    tft.print(iy);
  }

  for (uint8_t i = 0; i <= GRID_SIZE; i++) {
    tft.drawLine(x, y + i * CELL, x + GRID_SIZE * CELL, y + i * CELL, COLOR_GRID);
    tft.drawLine(x + i * CELL, y, x + i * CELL, y + GRID_SIZE * CELL, COLOR_GRID);
  }
}

uint16_t DisplayManager::ownColor(uint8_t state) const {
  switch (state) {
    case 1: return COLOR_SHIP;
    case 2: return COLOR_HIT;
    case 3: return COLOR_SUNK;
    case 4: return COLOR_WATER;
    default: return COLOR_BG;
  }
}

void DisplayManager::drawOwnCell(uint8_t gx, uint8_t gy, uint8_t state) {
  int px = OWN_X + gx * CELL + 1;
  int py = OWN_Y + gy * CELL + 1;
  tft.fillRect(px, py, CELL - 1, CELL - 1, ownColor(state));
}

void DisplayManager::drawEnemyCell(uint8_t gx, uint8_t gy, uint8_t state) {
  int px = ENEMY_X + gx * CELL + 1;
  int py = ENEMY_Y + gy * CELL + 1;

  uint16_t c = COLOR_BG;
  switch (state) {
    case ENEMY_WATER: c = COLOR_WATER; break;
    case ENEMY_HIT:   c = COLOR_HIT;   break;
    case ENEMY_SUNK:  c = COLOR_SUNK;  break;
    default:          c = COLOR_BG;    break;
  }

  tft.fillRect(px, py, CELL - 1, CELL - 1, c);
}

void DisplayManager::drawStatusButton(bool active) {
  uint16_t fill = active ? COLOR_STATUS_ON : COLOR_BUTTON_FILL;
  drawButton(STATUS_BUTTON_X, STATUS_BUTTON_Y, STATUS_BUTTON_W, STATUS_BUTTON_H,
             "Status", COLOR_BUTTON_FRAME, COLOR_BUTTON_TEXT, fill);
}

void DisplayManager::drawRestartButton() {
  drawButton(RESTART_BUTTON_X, RESTART_BUTTON_Y, RESTART_BUTTON_W, RESTART_BUTTON_H,
             "Neu", COLOR_BUTTON_FRAME, COLOR_BUTTON_TEXT, COLOR_BUTTON_FILL);
}

void DisplayManager::drawQuitButton() {
  drawButton(QUIT_BUTTON_X, QUIT_BUTTON_Y, QUIT_BUTTON_W, QUIT_BUTTON_H,
             "Quit", COLOR_BUTTON_FRAME, COLOR_QUIT_TEXT, COLOR_QUIT_FILL);
}

void DisplayManager::drawBottomText(const GameState& game) {
  tft.fillRect(0, 218, SCREEN_W, 22, COLOR_BG);
  tft.drawFastHLine(0, 218, SCREEN_W, COLOR_GRID);

  tft.setTextColor(COLOR_TEXT);
  tft.setTextSize(1);
  tft.setCursor(4, 226);
  tft.print(game.statusMessage);
}

void DisplayManager::drawStatusPanelInternal(const GameState& game) {
  tft.fillRoundRect(INFO_PANEL_X, INFO_PANEL_Y, INFO_PANEL_W, INFO_PANEL_H, 5, COLOR_PANEL_FILL);
  tft.drawRoundRect(INFO_PANEL_X, INFO_PANEL_Y, INFO_PANEL_W, INFO_PANEL_H, 5, COLOR_PANEL_FRAME);

  int x = INFO_PANEL_X + 8;
  int y = INFO_PANEL_Y + 8;

  tft.setTextColor(COLOR_TEXT);
  tft.setTextSize(1);

  tft.setCursor(x, y);
  tft.print("STATUS");
  y += 14;

  tft.drawFastHLine(INFO_PANEL_X + 4, y, INFO_PANEL_W - 8, COLOR_PANEL_FRAME);
  y += 8;

  tft.setCursor(x, y);
  tft.print(game.turnText());
  y += 14;

  tft.setCursor(x, y);
  tft.print("Ich 2:");
  tft.print(game.ownShips[0].anzahlLebend);
  tft.print(" 3:");
  tft.print(game.ownShips[1].anzahlLebend);
  y += 12;

  tft.setCursor(x, y);
  tft.print("Ich 4:");
  tft.print(game.ownShips[2].anzahlLebend);
  tft.print(" 5:");
  tft.print(game.ownShips[3].anzahlLebend);
  y += 14;

  tft.setCursor(x, y);
  tft.print("Ge  2:");
  tft.print(game.enemyShips[0].anzahlLebend);
  tft.print(" 3:");
  tft.print(game.enemyShips[1].anzahlLebend);
  y += 12;

  tft.setCursor(x, y);
  tft.print("Ge  4:");
  tft.print(game.enemyShips[2].anzahlLebend);
  tft.print(" 5:");
  tft.print(game.enemyShips[3].anzahlLebend);
  y += 14;

  tft.setCursor(x, y);
  tft.print("Ende: ");
  tft.print(game.finishedText());
}

void DisplayManager::redrawBottomBar(const GameState& game) {
  drawBottomText(game);
}

void DisplayManager::redrawStatusPanel(const GameState& game) {
  drawGame(game);
}

void DisplayManager::redrawAttackButton(bool active) {
  uint16_t fill = active ? COLOR_STATUS_ON : COLOR_BUTTON_FILL;
  drawButton(ATTACK_BTN_X, ATTACK_BTN_Y, ATTACK_BTN_W, ATTACK_BTN_H,
             "Attacke", COLOR_BUTTON_FRAME, COLOR_BUTTON_TEXT, fill);
}

void DisplayManager::redrawPlacementButtons(const GameState& game) {
  drawButton(BTN_DIR_X, BTN_DIR_Y, BTN_DIR_W, BTN_DIR_H,
             game.directionText(), COLOR_BUTTON_FRAME, COLOR_BUTTON_TEXT, COLOR_BUTTON_FILL);

  const char* okText = game.myReady ? "Ready" : "OK";
  drawButton(BTN_OK_X, BTN_OK_Y, BTN_OK_W, BTN_OK_H,
             okText, COLOR_BUTTON_FRAME, COLOR_BUTTON_TEXT, COLOR_BUTTON_FILL);
}

void DisplayManager::drawGame(const GameState& game) {
  tft.fillScreen(COLOR_BG);

  drawGridFrame(OWN_X, OWN_Y, "Mein Feld");
  drawGridFrame(ENEMY_X, ENEMY_Y, "Gegner");

  for (uint8_t y = 0; y < GRID_SIZE; y++) {
    for (uint8_t x = 0; x < GRID_SIZE; x++) {
      drawOwnCell(x, y, game.ownGrid[y][x]);
      drawEnemyCell(x, y, game.enemyGrid[y][x]);
    }
  }

  drawStatusButton(game.statusVisible);
  drawRestartButton();
  drawQuitButton();

  drawButton(BTN_DIR_X, BTN_DIR_Y, BTN_DIR_W, BTN_DIR_H,
             game.directionText(), COLOR_BUTTON_FRAME, COLOR_BUTTON_TEXT, COLOR_BUTTON_FILL);

  const char* okText = "OK";
  if (game.myReady && game.phase != GamePhase::RUNNING) okText = "Ready";
  if (game.phase == GamePhase::RUNNING) okText = "--";

  drawButton(BTN_OK_X, BTN_OK_Y, BTN_OK_W, BTN_OK_H,
             okText, COLOR_BUTTON_FRAME, COLOR_BUTTON_TEXT, COLOR_BUTTON_FILL);

  redrawAttackButton(game.attackArmed);
  drawBottomText(game);

  if (game.statusVisible) {
    drawStatusPanelInternal(game);
  }
}