#ifndef TOUCH_MANAGER_H
#define TOUCH_MANAGER_H

#include <Arduino.h>
#include <XPT2046_Touchscreen.h>

class TouchManager {
public:
  TouchManager(XPT2046_Touchscreen& ts, uint8_t tftCsPin, uint8_t touchCsPin);

  void begin();
  bool getTouchPressedEdge(int& sx, int& sy);

  bool isInside(int sx, int sy, int x, int y, int w, int h) const;
  bool getOwnGridCell(int sx, int sy, uint8_t& gx, uint8_t& gy) const;
  bool getEnemyGridCell(int sx, int sy, uint8_t& gx, uint8_t& gy) const;

private:
  XPT2046_Touchscreen& touch;
  uint8_t tftCs;
  uint8_t touchCs;
  bool lastTouchDown;

  bool readTouch(int& sx, int& sy);
};

#endif