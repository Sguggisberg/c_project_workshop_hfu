#include "message.h"
#include "peripheral.h"

BLEDevice central;

bool isCentral() {
  return !central;
}

void setupPeripheral()
{
    // Central does not exits, create it
    BLE.begin();
}