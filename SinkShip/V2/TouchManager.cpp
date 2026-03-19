#include "TouchManager.h"
#include "AppConfig.h"

TouchManager::TouchManager(XPT2046_Touchscreen& ts, uint8_t tftCsPin, uint8_t touchCsPin)
  : touch(ts), tftCs(tftCsPin), touchCs(touchCsPin) {
}

void TouchManager::begin() {
  pinMode(tftCs, OUTPUT);
  pinMode(touchCs, OUTPUT);
  pinMode(TOUCH_IRQ, INPUT_PULLUP);

  digitalWrite(tftCs, HIGH);
  digitalWrite(touchCs, HIGH);

  touch.begin();
}

bool TouchManager::touchZuScreen(int& sx, int& sy) {
  digitalWrite(tftCs, HIGH);

  if (!touch.touched()) {
    return false;
  }

  const uint8_t sampleCount = 3;
  long sumX = 0;
  long sumY = 0;
  uint8_t validCount = 0;

  for (uint8_t i = 0; i < sampleCount; i++) {
    if (!touch.touched()) continue;

    TS_Point p = touch.getPoint();
    if (p.z >= TOUCH_MIN_Z) {
      sumX += p.x;
      sumY += p.y;
      validCount++;
    }
    delay(2);
  }

  if (validCount == 0) return false;

  int rawX = sumX / validCount;
  int rawY = sumY / validCount;

  if (TOUCH_SWAP_XY) {
    int tmp = rawX;
    rawX = rawY;
    rawY = tmp;
  }

  int mappedX = map(rawX, RAW_MIN_X, RAW_MAX_X, 0, SCREEN_W - 1);
  int mappedY = map(rawY, RAW_MIN_Y, RAW_MAX_Y, 0, SCREEN_H - 1);

  if (TOUCH_INVERT_X) mappedX = SCREEN_W - 1 - mappedX;
  if (TOUCH_INVERT_Y) mappedY = SCREEN_H - 1 - mappedY;

  mappedX += TOUCH_OFFSET_X;
  mappedY += TOUCH_OFFSET_Y;

  if (mappedX < 0) mappedX = 0;
  if (mappedX >= SCREEN_W) mappedX = SCREEN_W - 1;
  if (mappedY < 0) mappedY = 0;
  if (mappedY >= SCREEN_H) mappedY = SCREEN_H - 1;

  sx = mappedX;
  sy = mappedY;

  return true;
}

bool TouchManager::screenZuEigenemFeld(int sx, int sy, uint8_t& feldX, uint8_t& feldY) {
  if (sx < OWN_X || sy < OWN_Y) return false;
  if (sx >= OWN_X + GRID_SIZE * CELL || sy >= OWN_Y + GRID_SIZE * CELL) return false;

  feldX = (sx - OWN_X) / CELL;
  feldY = (sy - OWN_Y) / CELL;
  return true;
}

bool TouchManager::getEnemyGridCell(int sx, int sy, uint8_t& gx, uint8_t& gy) {
  if (sx < ENEMY_X || sy < ENEMY_Y) return false;
  if (sx >= ENEMY_X + GRID_SIZE * CELL || sy >= ENEMY_Y + GRID_SIZE * CELL) return false;

  gx = (sx - ENEMY_X) / CELL;
  gy = (sy - ENEMY_Y) / CELL;
  return true;
}

bool TouchManager::isStatusButton(int sx, int sy) {
  return isInside(sx, sy, STATUS_BUTTON_X, STATUS_BUTTON_Y, STATUS_BUTTON_W, STATUS_BUTTON_H);
}

bool TouchManager::isQuitButton(int sx, int sy) {
  return isInside(sx, sy, QUIT_BUTTON_X, QUIT_BUTTON_Y, QUIT_BUTTON_W, QUIT_BUTTON_H);
}

bool TouchManager::isInside(int sx, int sy, int x, int y, int w, int h) const {
  return (sx >= x && sx < (x + w) && sy >= y && sy < (y + h));
}