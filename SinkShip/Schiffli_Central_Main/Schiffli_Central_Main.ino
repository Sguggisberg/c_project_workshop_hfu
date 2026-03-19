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
  SPI.begin();  //  MUSS drin bleiben!
  controller.begin();
  // initialize the BLE hardware
  BLE.begin();
  isCentral = foundPeripheral();
  if (isCentral) {
    Serial.println("BLE is central");
  } else {
    Serial.println("BLE is peripherie");
  }
  if (isCentral) {
    setup_central();
    
  } else {
    setupPeripheral();
  }

  // Mac address 1
  // Mac address 2

}

void loop() {
  controller.update();
}