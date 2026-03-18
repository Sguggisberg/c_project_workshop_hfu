#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <XPT2046_Touchscreen.h>

#include "AppConfig.h"
#include "GameState.h"
#include "DisplayManager.h"
#include "TouchManager.h"
#include "GameController.h"

Adafruit_ILI9341 tft(TFT_CS, TFT_DC, TFT_RST);
XPT2046_Touchscreen ts(TOUCH_CS, TOUCH_IRQ);

GameState game;
DisplayManager displayManager(tft, TOUCH_CS);
TouchManager touchManager(ts, TFT_CS, TOUCH_CS);
GameController controller(game, displayManager, touchManager);

void setup() {
  SPI.begin();   //  MUSS drin bleiben!
  controller.begin();
}

void loop() {
  controller.update();
}