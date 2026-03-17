#include <ArduinoBLE.h>
#include <string.h>

#include "message.h"

/*  // Zur Verfügung stehende UUIDs
 419d3655-282c-4a0e-9b7d-fbed8e928581
 b7a73424-a7d9-4aff-958d-04f4612d461b
 b9e3b5f7-b6b8-42a8-ae4b-9ae9f7f6b618
 128b0f5a-f62a-477b-ab17-4caea64f9415
 */
int buttonPin = 2;
boolean ledSwitch;

BLEService LEDService("19B10000-E8F2-537E-4F6C-D104768A1214"); // BLE LED Service
// BLE LED Switch Characteristic - custom 128-bit UUID, read and writable by central
BLECharacteristic LEDCharacteristic("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLENotify | BLEWrite,
        sizeof(Message), true);
void setup() {
  Serial.begin(9600);
  Serial.println("Initialized");
  
  // begin initialization
  if (!BLE.begin()) {
    Serial.println("starting Bluetooth® Low Energy failed!");
  }
  // set advertised local name and service UUID:
  BLE.setLocalName("Button Device");
  BLE.setAdvertisedService(LEDService);
  // add the characteristic to the service
  LEDService.addCharacteristic(LEDCharacteristic);
  // add service
  BLE.addService(LEDService);
  // start advertising
  BLE.advertise();
  Serial.println("BLE LED Peripheral, waiting for connections....");
}

void loop() {
  // listen for BLE peripherals to connect:
  BLEDevice central = BLE.central();
  // if a central is connected to peripheral:
  if (central) {
    Serial.print("Connected to central: ");
    // print the central's MAC address:
    Serial.println(central.address());
    // while the central is still connected to peripheral:

    while (central.connected()) {

      delay(500);
      Message message = {
        BOMB_ATTACK,
        {},
        {},
        10,
        7
      };
      //memcpy(message.senderId, central.address().c_str(), central.address().length());
      //memcpy(message.receiverId, BLE.address().c_str(), BLE.address().length());
    
      Serial.print("Write value: ");
      //Serial.println(message);
      LEDCharacteristic.writeValue(&message, sizeof(Message)); 
     }

    // when the central disconnects, print it out:
    Serial.print(F("Disconnected from central: "));
    Serial.println(central.address());
  }
}