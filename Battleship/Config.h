#pragma once

#include <Arduino.h>

namespace Config {
  constexpr uint8_t GRID_SIZE = 6;
  constexpr uint8_t SHIP_COUNT = 4;
  constexpr uint8_t SHIP_LENGTHS[SHIP_COUNT] = {3, 2, 2, 1};

  constexpr unsigned long TOUCH_DEBOUNCE_MS = 160;
  constexpr unsigned long UI_REFRESH_MS = 120;
  constexpr unsigned long ROLE_RETRY_MS = 1800;
  constexpr unsigned long CONNECT_RETRY_MS = 1200;

  constexpr int SCREEN_W = 320;
  constexpr int SCREEN_H = 240;

  constexpr int TFT_CS  = 10;
  constexpr int TFT_DC  = 9;
  constexpr int TFT_RST = 8;
  constexpr int TOUCH_CS = 7;
  constexpr int TOUCH_IRQ = 6;

  constexpr int TOUCH_MIN_X = 200;
  constexpr int TOUCH_MAX_X = 3850;
  constexpr int TOUCH_MIN_Y = 200;
  constexpr int TOUCH_MAX_Y = 3850;

  constexpr int BOARD_CELL = 20;
  constexpr int BOARD_W = GRID_SIZE * BOARD_CELL;
  constexpr int BOARD_H = GRID_SIZE * BOARD_CELL;

  constexpr int MY_BOARD_X = 12;
  constexpr int MY_BOARD_Y = 62;
  constexpr int ENEMY_BOARD_X = 188;
  constexpr int ENEMY_BOARD_Y = 62;

  constexpr int BTN_ROTATE_X = 12;
  constexpr int BTN_ROTATE_Y = 192;
  constexpr int BTN_ROTATE_W = 70;
  constexpr int BTN_ROTATE_H = 32;

  constexpr int BTN_MIX_X = 88;
  constexpr int BTN_MIX_Y = 192;
  constexpr int BTN_MIX_W = 60;
  constexpr int BTN_MIX_H = 32;

  constexpr int BTN_UNDO_X = 154;
  constexpr int BTN_UNDO_Y = 192;
  constexpr int BTN_UNDO_W = 60;
  constexpr int BTN_UNDO_H = 32;

  constexpr int BTN_READY_X = 220;
  constexpr int BTN_READY_Y = 192;
  constexpr int BTN_READY_W = 42;
  constexpr int BTN_READY_H = 32;

  constexpr int BTN_RESET_X = 268;
  constexpr int BTN_RESET_Y = 192;
  constexpr int BTN_RESET_W = 40;
  constexpr int BTN_RESET_H = 32;

  constexpr char DEVICE_NAME[] = "NanoBattleship";
  constexpr char SERVICE_UUID[] = "19b10010-e8f2-537e-4f6c-d104768a1214";
  constexpr char REQUEST_UUID[] = "19b10011-e8f2-537e-4f6c-d104768a1214";
  constexpr char RESPONSE_UUID[] = "19b10012-e8f2-537e-4f6c-d104768a1214";
}
