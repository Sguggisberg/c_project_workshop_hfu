#include "Arduino.h"
#include <ArduinoBLE.h>
#include <stdbool.h>
#include "message.h"

void setup_central();
void loop_central();
void controlLed(BLEDevice peripheral);