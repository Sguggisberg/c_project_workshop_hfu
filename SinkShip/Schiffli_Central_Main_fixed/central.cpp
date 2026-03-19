#include <ArduinoBLE.h>
#include <stdbool.h>
#include "central.h"
#include "message.h"

extern bool logging;

static bool centralConnected = false;

static void clearRemoteHandles() {
  SysPeripheral = BLEDevice();
  SysRequestCharacteristic = BLECharacteristic();
  SysResponseCharacteristic = BLECharacteristic();
  centralConnected = false;
}

static bool connectPeripheral(BLEDevice peripheral) {
  Serial.println("Connecting ...");
  if (!peripheral.connect()) {
    Serial.println("Failed to connect!");
    return false;
  }

  Serial.println("Discovering attributes ...");
  if (!peripheral.discoverAttributes()) {
    Serial.println("Attribute discovery failed!");
    peripheral.disconnect();
    return false;
  }

  BLECharacteristic requestCharacteristic = peripheral.characteristic(GAME_REQUEST_CHARACTERSITIC_UUID);
  BLECharacteristic responseCharacteristic = peripheral.characteristic(GAME_RESPONSE_CHARACTERSITIC_UUID);

  if (!requestCharacteristic || !responseCharacteristic) {
    Serial.println("Peripheral does not have required characteristics!");
    peripheral.disconnect();
    return false;
  }

  if (requestCharacteristic.canSubscribe()) {
    Serial.print("Subscribe request: ");
    Serial.println(requestCharacteristic.subscribe() ? 1 : 0);
  }

  if (responseCharacteristic.canSubscribe()) {
    Serial.print("Subscribe response: ");
    Serial.println(responseCharacteristic.subscribe() ? 1 : 0);
  }

  SysPeripheral = peripheral;
  SysRequestCharacteristic = requestCharacteristic;
  SysResponseCharacteristic = responseCharacteristic;
  centralConnected = true;

  Serial.println("Central connected and ready");
  return true;
}

void setup_central() {
  Serial.println("BLE Central - Game control");
  clearRemoteHandles();
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
  BLE.poll();

  if (centralConnected) {
    if (!SysPeripheral || !SysPeripheral.connected()) {
      Serial.println("Peripheral disconnected");
      clearRemoteHandles();
      BLE.scanForUuid(GAME_SERVICE_UUID);
    }
    return;
  }

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

  if (peripheral.localName().indexOf(GAME_DEVICE_NAME) < 0) {
    Serial.println("No 'Game Device' in name");
    return;
  }

  BLE.stopScan();
  if (!connectPeripheral(peripheral)) {
    BLE.scanForUuid(GAME_SERVICE_UUID);
  }
}

void controlLed(BLEDevice peripheral) {
  connectPeripheral(peripheral);
}
