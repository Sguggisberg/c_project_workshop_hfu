#include <WiFiNINA.h>

#ifndef PERIPHERAL_H
#define PERIPHERAL_H

#include <ArduinoBLE.h>

#include "message.h"

extern BLEDevice central;

void setupPeripheral();
void loopPeripheral();

#endif