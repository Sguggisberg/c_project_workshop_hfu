#pragma once
#include <ArduinoBLE.h>

struct Message {
  uint8_t type;
  uint8_t senderId;
  int8_t x;
  int8_t y;
};

class BLEManager {
public:
  void begin();
  void update();

private:
  void runServer();
  void runClient();

  enum Role { UNKNOWN, SERVER, CLIENT };
  Role role = UNKNOWN;

  BLEService service = BLEService("180C");
  BLECharacteristic characteristic = BLECharacteristic("2A56", BLERead | BLEWrite | BLENotify, sizeof(Message));
};
