#include "DisplayManager.h"
#include "AppConfig.h"

DisplayManager::DisplayManager(Adafruit_ILI9341& display, uint8_t touchCsPin)
  : tft(display), touchCs(touchCsPin) {
}

void DisplayManager::selectDisplay() {
  digitalWrite(touchCs, HIGH);
}

void DisplayManager::begin() {
  pinMode(touchCs, OUTPUT);
  digitalWrite(touchCs, HIGH);

  tft.begin();
  tft.setRotation(1);
  tft.fillScreen(COLOR_BG);
}

void DisplayManager::drawBaseScreen(const GameState& game) {
  selectDisplay();
  tft.fillScreen(COLOR_BG);

  drawGridFrame(OWN_X, OWN_Y, "Mein Feld", GRID_SIZE);
  drawGridFrame(ENEMY_X, ENEMY_Y, "Gegner", GRID_SIZE);

  for (uint8_t y = 0; y < GRID_SIZE; y++) {
    for (uint8_t x = 0; x < GRID_SIZE; x++) {
      drawOwnCell(x, y, game.meinFeld[y][x]);

      switch (game.gegnerFeld[y][x]) {
        case ENEMY_WATER: drawEnemyWater(x, y); break;
        case ENEMY_HIT:   drawEnemyHit(x, y); break;
        case ENEMY_SUNK:  drawEnemySunkCell(x, y); break;
        default: break;
      }
    }
  }

  redrawPlacementButtons(game);
  redrawAttackButton(false);
  drawStatusButton(game.statusSichtbar);
  drawQuitButton();
  drawBottomText(game);
}

void DisplayManager::drawInitialScreen(const GameState& game) {
  drawBaseScreen(game);
  if (game.statusSichtbar) {
    drawStatusPanel(game);
  }
}

void DisplayManager::drawGridFrame(int startX, int startY, const char* titel, uint8_t gridSize) {
  tft.setTextColor(COLOR_TEXT);
  tft.setTextSize(1);

  tft.setCursor(startX, startY - 18);
  tft.print(titel);

  for (uint8_t x = 0; x < gridSize; x++) {
    tft.setCursor(startX + 5 + x * CELL, startY - 10);
    tft.print(x);
  }

  for (uint8_t y = 0; y < gridSize; y++) {
    tft.setCursor(startX - 10, startY + 4 + y * CELL);
    tft.print(y);
  }

  for (uint8_t i = 0; i <= gridSize; i++) {
    tft.drawLine(startX, startY + i * CELL, startX + gridSize * CELL, startY + i * CELL, COLOR_GRID);
    tft.drawLine(startX + i * CELL, startY, startX + i * CELL, startY + gridSize * CELL, COLOR_GRID);
  }
}

void DisplayManager::redrawBottomBar(const GameState& game) {
  selectDisplay();
  drawBottomText(game);
}

void DisplayManager::redrawStatusPanel(const GameState& game) {
  // Bei Status-Toggle alles neu zeichnen, damit nichts stehen bleibt
  drawInitialScreen(game);
}

void DisplayManager::redrawPlacementButtons(const GameState& game) {
  selectDisplay();

  drawButton(BTN_DIR_X, BTN_DIR_Y, BTN_DIR_W, BTN_DIR_H,
             game.getDirectionText(),
             COLOR_BUTTON_FRAME, COLOR_BUTTON_TEXT, COLOR_BUTTON_FILL);

  drawButton(BTN_OK_X, BTN_OK_Y, BTN_OK_W, BTN_OK_H,
             "OK",
             COLOR_BUTTON_FRAME, COLOR_BUTTON_TEXT, COLOR_BUTTON_FILL);
}

void DisplayManager::redrawAttackButton(bool active) {
  selectDisplay();
  uint16_t fill = active ? COLOR_STATUS_ON : COLOR_BUTTON_FILL;

  drawButton(ATTACK_BTN_X, ATTACK_BTN_Y, ATTACK_BTN_W, ATTACK_BTN_H,
             "Attacke",
             COLOR_BUTTON_FRAME, COLOR_BUTTON_TEXT, fill);
}

void DisplayManager::drawOwnCell(uint8_t gx, uint8_t gy, uint8_t state) {
  selectDisplay();
  int px = OWN_X + gx * CELL + 1;
  int py = OWN_Y + gy * CELL + 1;
  tft.fillRect(px, py, CELL - 1, CELL - 1, farbeFuerMeinFeld(state));
}

void DisplayManager::drawEnemyWater(uint8_t gx, uint8_t gy) {
  selectDisplay();
  int px = ENEMY_X + gx * CELL + 1;
  int py = ENEMY_Y + gy * CELL + 1;
  tft.fillRect(px, py, CELL - 1, CELL - 1, COLOR_WATER);
}

void DisplayManager::drawEnemyHit(uint8_t gx, uint8_t gy) {
  selectDisplay();
  int px = ENEMY_X + gx * CELL + 1;
  int py = ENEMY_Y + gy * CELL + 1;
  tft.fillRect(px, py, CELL - 1, CELL - 1, COLOR_HIT);
}

void DisplayManager::drawEnemySunkCell(uint8_t gx, uint8_t gy) {
  selectDisplay();
  int px = ENEMY_X + gx * CELL + 1;
  int py = ENEMY_Y + gy * CELL + 1;
  tft.fillRect(px, py, CELL - 1, CELL - 1, COLOR_SUNK);
}

void DisplayManager::drawStatusButton(bool active) {
  uint16_t fill = active ? COLOR_STATUS_ON : COLOR_BUTTON_FILL;
  drawButton(STATUS_BUTTON_X, STATUS_BUTTON_Y, STATUS_BUTTON_W, STATUS_BUTTON_H,
             "Status?",
             COLOR_BUTTON_FRAME, COLOR_BUTTON_TEXT, fill);
}

void DisplayManager::drawQuitButton() {
  drawButton(QUIT_BUTTON_X, QUIT_BUTTON_Y, QUIT_BUTTON_W, QUIT_BUTTON_H,
             "Quit",
             COLOR_BUTTON_FRAME, COLOR_QUIT_TEXT, COLOR_QUIT_FILL);
}

void DisplayManager::drawStatusPanel(const GameState& game) {
  tft.fillRoundRect(INFO_PANEL_X, INFO_PANEL_Y, INFO_PANEL_W, INFO_PANEL_H, 5, COLOR_PANEL_FILL);
  tft.drawRoundRect(INFO_PANEL_X, INFO_PANEL_Y, INFO_PANEL_W, INFO_PANEL_H, 5, COLOR_PANEL_FRAME);

  tft.setTextColor(COLOR_TEXT);
  tft.setTextSize(1);

  int x = INFO_PANEL_X + 5;
  int y = INFO_PANEL_Y + 7;

  tft.setCursor(x, y);
  tft.print("STATUS");
  y += 12;

  tft.drawFastHLine(INFO_PANEL_X + 4, y, INFO_PANEL_W - 8, COLOR_PANEL_FRAME);
  y += 7;

  tft.setCursor(x, y);
  tft.print(game.getTurnText());
  y += 12;

  tft.setCursor(x, y);
  tft.print("Ich 2:");
  tft.print(game.eigeneSchiffe[0].anzahlLebend);
  tft.print(" 3:");
  tft.print(game.eigeneSchiffe[1].anzahlLebend);
  y += 11;

  tft.setCursor(x, y);
  tft.print("Ich 4:");
  tft.print(game.eigeneSchiffe[2].anzahlLebend);
  tft.print(" 5:");
  tft.print(game.eigeneSchiffe[3].anzahlLebend);
  y += 13;

  tft.setCursor(x, y);
  tft.print("Ge  2:");
  tft.print(game.gegnerSchiffe[0].anzahlLebend);
  tft.print(" 3:");
  tft.print(game.gegnerSchiffe[1].anzahlLebend);
  y += 11;

  tft.setCursor(x, y);
  tft.print("Ge  4:");
  tft.print(game.gegnerSchiffe[2].anzahlLebend);
  tft.print(" 5:");
  tft.print(game.gegnerSchiffe[3].anzahlLebend);
  y += 13;

  tft.setCursor(x, y);
  tft.print("Ende: ");
  tft.print(game.getSpielBeendetText());
}

void DisplayManager::drawBottomText(const GameState& game) {
  tft.fillRect(0, 218, SCREEN_W, 22, COLOR_BG);
  tft.drawFastHLine(0, 218, SCREEN_W, COLOR_GRID);

  tft.setTextColor(COLOR_TEXT);
  tft.setTextSize(1);
  tft.setCursor(4, 226);
  tft.print(game.statusMeldung);
}

void DisplayManager::drawButton(int x, int y, int w, int h, const char* text,
                                uint16_t frame, uint16_t textColor, uint16_t fill) {
  tft.fillRoundRect(x, y, w, h, 4, fill);
  tft.drawRoundRect(x, y, w, h, 4, frame);
  tft.setTextColor(textColor);
  tft.setTextSize(1);
  tft.setCursor(x + 5, y + 6);
  tft.print(text);
}

uint16_t DisplayManager::farbeFuerMeinFeld(uint8_t state) const {
  switch (state) {
    case 1: return COLOR_SHIP;
    case 2: return COLOR_HIT;
    case 3: return COLOR_SUNK;
    case 4: return COLOR_WATER;
    default: return COLOR_BG;
  }
}