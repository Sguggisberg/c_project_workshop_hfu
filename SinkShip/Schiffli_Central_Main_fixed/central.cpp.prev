#include <ArduinoBLE.h>
#include <stdbool.h>
#include "central.h"
#include "message.h"

extern bool logging;
void play_move(BLEDevice peripheral);
void log(BLEDevice peripheral);
void send_response(BLEDevice peripheral);

void setup_central() {
  Serial.println("BLE Central - Game control");
  BLE.scanForUuid(GAME_SERVICE_UUID);
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
  BLEDevice peripheral = BLE.available();
  if (!peripheral) {
    return;
  }

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

  BLE.stopScan();
  controlLed(peripheral);
  BLE.scanForUuid(GAME_SERVICE_UUID);
}

void controlLed(BLEDevice peripheral) {
  Serial.println("Connecting ...");
  if (peripheral.connect()) {
    Serial.println("Connected");
  } else {
    Serial.println("Failed to connect!");
    return;
  }

  Serial.println("Discovering attributes ...");
  if (peripheral.discoverAttributes()) {
    Serial.println("Attributes discovered");
  } else {
    Serial.println("Attribute discovery failed!");
    peripheral.disconnect();
    return;
  }

  play_move(peripheral);
  peripheral.disconnect();
  Serial.println("Peripheral disconnected");
}

void play_move(BLEDevice peripheral) {
  BLECharacteristic gameRequestCharacteristic = peripheral.characteristic(GAME_REQUEST_CHARACTERSITIC_UUID);
  BLECharacteristic gameResponseCharacteristic = peripheral.characteristic(GAME_RESPONSE_CHARACTERSITIC_UUID);

  if (!gameRequestCharacteristic || !gameResponseCharacteristic) {
    Serial.println("Peripheral does not have required characteristics!");
    peripheral.disconnect();
    return;
  }

  if (gameRequestCharacteristic.canSubscribe()) {
    if (gameRequestCharacteristic.subscribe()) {
      Serial.println("Subscribed to request characteristic");
    } else {
      Serial.println("Subscribe failed for request characteristic");
    }
  }

  bool responseSent = false;

  while (peripheral.connected()) {
    BLE.poll();

    if (gameRequestCharacteristic.valueUpdated()) {
      Message value{};
      const int bytesRead = gameRequestCharacteristic.readValue(&value, sizeof(value));
      if (bytesRead == sizeof(value)) {
        Serial.println("Received request:");
        Serial.println(value.type);
        Serial.println(value.x);
        Serial.println(value.y);

        send_response(peripheral);
        responseSent = true;
      } else {
        Serial.print("Request read failed, bytes: ");
        Serial.println(bytesRead);
      }
    }

    delay(20);
  }
}

void send_response(BLEDevice peripheral) {
  BLECharacteristic gameResponseCharacteristic = peripheral.characteristic(GAME_RESPONSE_CHARACTERSITIC_UUID);
  if (!gameResponseCharacteristic) {
    Serial.println("Response characteristic missing");
    return;
  }

  Message response{};
  response.type = HIT;
  response.senderId[0] = '\0';
  response.recevierId[0] = '\0';
  response.x = 9;
  response.y = 3;

  if (!gameResponseCharacteristic.canWrite()) {
    Serial.println("Response characteristic is not writable");
    return;
  }

  int res = gameResponseCharacteristic.writeValue(&response, sizeof(response));
  Serial.println("IsConnected: ");
  Serial.println(peripheral.connected());
  Serial.println("Response write result: ");
  Serial.println(res);
}
