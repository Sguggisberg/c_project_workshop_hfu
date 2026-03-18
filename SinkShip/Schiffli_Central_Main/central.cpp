#include <ArduinoBLE.h>
#include <stdbool.h>
#include "central.h"
#include "message.h"

extern bool logging;
void play_move(BLEDevice peripheral);
void log(BLEDevice peripheral);
void send_response(BLEDevice peripheral);

void setup_central() {

  Serial.println("BLE Central - LED control");
  // start scanning for Button Device BLE peripherals
  BLE.scanForUuid(GAME_SERVICE);
}

String getMacAddress() {
  String address = BLE.address();
  if (logging) {
    Serial.print("Own Mac Address: ");
    Serial.println(address);
  }
  return address;
}

void loop_central() {
  // check if a peripheral has been discovered
  BLEDevice peripheral = BLE.available();
  if (peripheral) {
    Serial.print("Found ");
    Serial.print(peripheral.address());
    Serial.print(" '");
    Serial.print(peripheral.localName());
    Serial.print("' ");
    Serial.print(peripheral.advertisedServiceUuid());
    Serial.println();
    if (peripheral.localName().indexOf("Game Device") < 0) {
      Serial.println("No 'Game Device' in name");
      return;
    }
    // stop scanning
    BLE.stopScan();
    controlLed(peripheral);
    // peripheral disconnected, start scanning again
    BLE.scanForUuid(GAME_SERVICE);
  }
}

void controlLed(BLEDevice peripheral) {
  // connect to the peripheral
  Serial.println("Connecting ...");
  if (peripheral.connect()) {
    Serial.println("Connected");
  } else {
    Serial.println("Failed to connect!");
    return;
  }
  // discover peripheral attributes
  Serial.println("Discovering attributes ...");
  if (peripheral.discoverAttributes()) {
    Serial.println("Attributes discovered");
  } else {
    Serial.println("Attribute discovery failed!");
    peripheral.disconnect();
    return;
  }

  play_move(peripheral);
  Serial.println("Peripheral disconnected");
}

void play_move(BLEDevice peripheral) {
  BLECharacteristic GameRequestCharacteristic = peripheral.characteristic(GAME_REQUEST_CHARACTERSITIC_UUID);

  if (!GameRequestCharacteristic) {
    Serial.println("Peripheral does not have MOVE characteristic!");
    peripheral.disconnect();
    return;
  }
  while (peripheral.connected()) {
    // while the peripheral is connected
    if (GameRequestCharacteristic.written()) {
      Message value;
      GameRequestCharacteristic.readValue(&value, sizeof(Message));
      Serial.println("Wert: ");
      Serial.println(value.type);
      Serial.println(value.x);
      Serial.println(value.y);
      Serial.println("Ende");
    }
    delay(500);
    send_response(peripheral);
  }
}


void send_response(BLEDevice peripheral) {
  BLECharacteristic GameResponseCharacteristic = peripheral.characteristic(GAME_RESPONSE_CHARACTERSITIC_UUID);
  Message response;
  response = (Message){ HIT, " ", " ", 9, 3 };
  if (GameResponseCharacteristic.canWrite()) {
    int res = GameResponseCharacteristic.writeValue(&response, sizeof(response));
    Serial.println("IsConnected: ");
    Serial.println(peripheral.connected());
    Serial.println("Response: ");
    Serial.println(res);
  }
}
