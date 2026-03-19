#ifndef TOUCH_MANAGER_H
#define TOUCH_MANAGER_H

#include <Arduino.h>
#include <XPT2046_Touchscreen.h>

class TouchManager {
public:
  TouchManager(XPT2046_Touchscreen& ts, uint8_t tftCsPin, uint8_t touchCsPin);

  void begin();

  bool touchZuScreen(int& sx, int& sy);
  bool screenZuEigenemFeld(int sx, int sy, uint8_t& feldX, uint8_t& feldY);
  bool getEnemyGridCell(int sx, int sy, uint8_t& gx, uint8_t& gy);

  bool isStatusButton(int sx, int sy);
  bool isQuitButton(int sx, int sy);

private:
  XPT2046_Touchscreen& touch;
  uint8_t tftCs;
  uint8_t touchCs;

  bool isInside(int sx, int sy, int x, int y, int w, int h) const;
};

#endif