#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <XPT2046_Touchscreen.h>

#include "AppConfig.h"
#include "GameState.h"
#include "DisplayManager.h"
#include "TouchManager.h"
#include "GameController.h"

#include "central.h"
#include "peripheral.h"

Adafruit_ILI9341 tft(TFT_CS, TFT_DC, TFT_RST);
XPT2046_Touchscreen ts(TOUCH_CS, TOUCH_IRQ);

GameState game;
DisplayManager displayManager(tft, TOUCH_CS);
TouchManager touchManager(ts, TFT_CS, TOUCH_CS);
GameController controller(game, displayManager, touchManager);

BLECharacteristic SysRequestCharacteristic;
BLECharacteristic SysResponseCharacteristic;

bool logging = false;
bool isCentral = true;

void setup() {
  SPI.begin();
  controller.begin();

  if (!BLE.begin()) {
    Serial.println("BLE start failed");
    return;
  }

  isCentral = foundPeripheral();
  if (isCentral) {
    Serial.println("BLE is central");
    setup_central();
  } else {
    Serial.println("BLE is peripheral");
    setupPeripheral();
  }
}

void loop() {
  BLE.poll();

  if (isCentral) {
    loop_central();
  } else {
    loopPeripheral();
  }

  controller.update();
}
