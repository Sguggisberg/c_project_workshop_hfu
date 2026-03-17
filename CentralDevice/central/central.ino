#include <ArduinoBLE.h>
#include <stdbool.h>
#include "message.h"
#include "central.h"
#include "peripheral.h"

bool logging = true;
bool isCentral = false;

/*
bool isCentral() {
  return !BLE.central();
}
*/

void setup() {
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