#include <ArduinoBLE.h>
#include <stdbool.h>
#include "message.h"
#include "central.h"
#include "peripheral.h"

bool logging = true;
bool isCentral = true;

/*
bool isCentral() {
  return !BLE.central();
}
*/

void setup() {
  Serial.begin(9600);
  if (isCentral) {
    setup_central();
  } else {
    setupPeripheral();
  }
}
void loop() {
  if (isCentral) {
    loop_central();
  } else {
    loopPeripheral();
  }
}