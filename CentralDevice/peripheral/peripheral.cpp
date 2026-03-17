#include "message.h"
#include "peripheral.h"

BLEDevice central;

bool isCentral() {
  return !central;
}

void setupPeripheral()
{
  central = BLE.central();
  if (!isCentral())
  {
    // Central does not exits, create it
    //BLE.init()
  }
}