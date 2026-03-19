#include <SPI.h>
#include <ArduinoBLE.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <XPT2046_Touchscreen.h>

#include "AppConfig.h"
#include "Protocol.h"
#include "BleLink.h"
#include "GameState.h"
#include "DisplayManager.h"
#include "TouchManager.h"
#include "GameController.h"

Adafruit_ILI9341 tft(TFT_CS, TFT_DC, TFT_RST);
XPT2046_Touchscreen ts(TOUCH_CS, TOUCH_IRQ);

BleLink ble;
GameState game;
DisplayManager display(tft);
TouchManager touch(ts, TFT_CS, TOUCH_CS);
GameController controller(game, display, touch, ble);

void setup() {
  Serial.begin(115200);
  delay(200);

  SPI.begin();

  display.begin();
  touch.begin();

  if (!ble.begin()) {
    display.drawFatal("BLE konnte nicht gestartet werden");
    while (true) {
      delay(1000);
    }
  }

  controller.begin();
}

void loop() {
  BLE.poll();
  ble.update();
  controller.update();
}