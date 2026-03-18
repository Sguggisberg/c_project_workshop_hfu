#include <WiFiNINA.h>

#ifndef PERIPHERAL_H
#define PERIPHERAL_H

#include <ArduinoBLE.h>

#include "message.h"

void setupPeripheral();
void loopPeripheral();

extern BLECharacteristic RequestCharacteristic;
extern BLECharacteristic ResponseCharacteristic;

#endif