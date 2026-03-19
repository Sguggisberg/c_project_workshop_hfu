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

constexpr int TOUCH_OFFSET_X = 0;
constexpr int TOUCH_OFFSET_Y = 0;

constexpr bool TOUCH_SWAP_XY  = false;
constexpr bool TOUCH_INVERT_X = true;
constexpr bool TOUCH_INVERT_Y = false;

// ---------------- Spielfeld ----------------
constexpr uint8_t GRID_SIZE = 8;
constexpr int CELL = 16;

constexpr int OWN_X   = 16;
constexpr int OWN_Y   = 52;

constexpr int ENEMY_X = 176;
constexpr int ENEMY_Y = 52;

// ---------------- Lobby Buttons ----------------
constexpr int LOBBY_TITLE_Y = 16;

constexpr int BTN_SEARCH_X = 16;
constexpr int BTN_SEARCH_Y = 46;
constexpr int BTN_SEARCH_W = 88;
constexpr int BTN_SEARCH_H = 28;

constexpr int BTN_CONNECT_X = 112;
constexpr int BTN_CONNECT_Y = 46;
constexpr int BTN_CONNECT_W = 92;
constexpr int BTN_CONNECT_H = 28;

constexpr int BTN_TO_GAME_X = 212;
constexpr int BTN_TO_GAME_Y = 46;
constexpr int BTN_TO_GAME_W = 92;
constexpr int BTN_TO_GAME_H = 28;

constexpr int LOBBY_INFO_X = 16;
constexpr int LOBBY_INFO_Y = 86;
constexpr int LOBBY_INFO_W = 288;
constexpr int LOBBY_INFO_H = 108;

// ---------------- Top Buttons Game ----------------
constexpr int STATUS_BUTTON_X = 162;
constexpr int STATUS_BUTTON_Y = 4;
constexpr int STATUS_BUTTON_W = 48;
constexpr int STATUS_BUTTON_H = 18;

constexpr int RESTART_BUTTON_X = 214;
constexpr int RESTART_BUTTON_Y = 4;
constexpr int RESTART_BUTTON_W = 50;
constexpr int RESTART_BUTTON_H = 18;

constexpr int QUIT_BUTTON_X = 268;
constexpr int QUIT_BUTTON_Y = 4;
constexpr int QUIT_BUTTON_W = 48;
constexpr int QUIT_BUTTON_H = 18;

// ---------------- Statuspanel ----------------
constexpr int INFO_PANEL_X = 56;
constexpr int INFO_PANEL_Y = 22;
constexpr int INFO_PANEL_W = 208;
constexpr int INFO_PANEL_H = 118;

// ---------------- Untere Buttons ----------------
constexpr int BTN_DIR_X = 16;
constexpr int BTN_DIR_Y = 188;
constexpr int BTN_DIR_W = 86;
constexpr int BTN_DIR_H = 24;

constexpr int BTN_OK_X = 108;
constexpr int BTN_OK_Y = 188;
constexpr int BTN_OK_W = 46;
constexpr int BTN_OK_H = 24;

constexpr int ATTACK_BTN_X = 160;
constexpr int ATTACK_BTN_Y = 188;
constexpr int ATTACK_BTN_W = 76;
constexpr int ATTACK_BTN_H = 24;

// ---------------- Dialog Buttons ----------------
constexpr int DIALOG_X = 42;
constexpr int DIALOG_Y = 56;
constexpr int DIALOG_W = 236;
constexpr int DIALOG_H = 104;

constexpr int DIALOG_ACCEPT_X = 64;
constexpr int DIALOG_ACCEPT_Y = 122;
constexpr int DIALOG_ACCEPT_W = 84;
constexpr int DIALOG_ACCEPT_H = 24;

constexpr int DIALOG_DECLINE_X = 172;
constexpr int DIALOG_DECLINE_Y = 122;
constexpr int DIALOG_DECLINE_W = 84;
constexpr int DIALOG_DECLINE_H = 24;

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

constexpr uint16_t COLOR_DIALOG_FILL  = 0x2104;
constexpr uint16_t COLOR_DIALOG_FRAME = 0xFFFF;
constexpr uint16_t COLOR_ACCEPT_FILL  = 0x07E0;
constexpr uint16_t COLOR_DECLINE_FILL = 0xF800;

#endif