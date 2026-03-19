#ifndef APP_CONFIG_H
#define APP_CONFIG_H

#include <Arduino.h>

// ---------------- Pins ----------------
constexpr uint8_t TFT_CS    = 10;
constexpr uint8_t TFT_DC    = 9;
constexpr uint8_t TFT_RST   = 8;

constexpr uint8_t TOUCH_CS  = 7;
constexpr uint8_t TOUCH_IRQ = 6;

// ---------------- Display ----------------
constexpr int SCREEN_W = 320;
constexpr int SCREEN_H = 240;

// ---------------- Touch Kalibrierung ----------------
constexpr int RAW_MIN_X = 430;
constexpr int RAW_MAX_X = 3940;
constexpr int RAW_MIN_Y = 220;
constexpr int RAW_MAX_Y = 3780;

constexpr int TOUCH_MIN_Z = 180;

// 1 Feld nach rechts korrigieren
constexpr int TOUCH_OFFSET_X = 0;
constexpr int TOUCH_OFFSET_Y = 0;

constexpr bool TOUCH_SWAP_XY  = false;
constexpr bool TOUCH_INVERT_X = true;
constexpr bool TOUCH_INVERT_Y = false;

// ---------------- Spielfeld ----------------
constexpr uint8_t GRID_SIZE = 8;
constexpr int CELL = 16;

// Eigenes Feld links
constexpr int OWN_X   = 16;
constexpr int OWN_Y   = 52;

// Gegnerfeld rechts
constexpr int ENEMY_X = 176;
constexpr int ENEMY_Y = 52;

// ---------------- Top Buttons ----------------
constexpr int STATUS_BUTTON_X = 216;
constexpr int STATUS_BUTTON_Y = 4;
constexpr int STATUS_BUTTON_W = 46;
constexpr int STATUS_BUTTON_H = 18;

constexpr int QUIT_BUTTON_X = 266;
constexpr int QUIT_BUTTON_Y = 4;
constexpr int QUIT_BUTTON_W = 50;
constexpr int QUIT_BUTTON_H = 18;

// ---------------- Statuspanel ----------------
// Als zentriertes Popup, damit alles hineinpasst
constexpr int INFO_PANEL_X = 58;
constexpr int INFO_PANEL_Y = 24;
constexpr int INFO_PANEL_W = 154;
constexpr int INFO_PANEL_H = 108;

// ---------------- Untere Buttons ----------------
constexpr int BTN_DIR_X = 16;
constexpr int BTN_DIR_Y = 188;
constexpr int BTN_DIR_W = 88;
constexpr int BTN_DIR_H = 24;

constexpr int BTN_OK_X = 110;
constexpr int BTN_OK_Y = 188;
constexpr int BTN_OK_W = 46;
constexpr int BTN_OK_H = 24;

constexpr int ATTACK_BTN_X = 164;
constexpr int ATTACK_BTN_Y = 188;
constexpr int ATTACK_BTN_W = 72;
constexpr int ATTACK_BTN_H = 24;

// ---------------- Farben ----------------
constexpr uint16_t COLOR_BG           = 0x0000;
constexpr uint16_t COLOR_TEXT         = 0xFFFF;
constexpr uint16_t COLOR_GRID         = 0x7BEF;
constexpr uint16_t COLOR_WATER        = 0x001F;
constexpr uint16_t COLOR_SHIP         = 0x07E0;
constexpr uint16_t COLOR_HIT          = 0xF800;
constexpr uint16_t COLOR_SUNK         = 0xFD20;

constexpr uint16_t COLOR_BUTTON_FILL  = 0x07FF;
constexpr uint16_t COLOR_BUTTON_TEXT  = 0x0000;
constexpr uint16_t COLOR_BUTTON_FRAME = 0xFFFF;
constexpr uint16_t COLOR_STATUS_ON    = 0x07E0;

constexpr uint16_t COLOR_QUIT_FILL    = 0xF800;
constexpr uint16_t COLOR_QUIT_TEXT    = 0xFFFF;

constexpr uint16_t COLOR_PANEL_FILL   = 0x18C3;
constexpr uint16_t COLOR_PANEL_FRAME  = 0xFFFF;

#endif