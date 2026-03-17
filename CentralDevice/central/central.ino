#include <ArduinoBLE.h>
#include <stdbool.h>
#include "message.h"
#include "central.h"
#include "peripheral.h"

bool logging = true;
bool isCentral = false;

void setup() {
  if (isCentral) {
    setup_central();
  } else {
  }
}
void loop() {
  if (isCentral) {
    loop_central();
  } else {
  }
}


String getMacAddress() {
  String address = BLE.address();
  if (logging) {
    Serial.print("Own Mac Address: ");
    Serial.print(address);
  }
  return address;
}