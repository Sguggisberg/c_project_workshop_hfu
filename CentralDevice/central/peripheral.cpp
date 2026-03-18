#include "peripheral.h"

BLECharacteristic messageCharacteristic;
int buttonPin = 2;
boolean ledSwitch;

BLEService GameService("19B10000-E8F2-537E-4F6C-D104768A1214");  // BLE LED Service
// BLE LED Switch Characteristic - custom 128-bit UUID, read and writable by central
BLECharacteristic RequestCharacteristic("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite,
     sizeof(Message), true);
BLECharacteristic ResponseCharacteristic("19b10002-e8f2-537e-4f6c-d104768a1214", BLERead | BLEWrite,
     sizeof(Message), true);

/*
void setupPeripheral() {
  //if (!isCentral())
  Serial.println("setupPeripheral");
  // Central does not exits, create it
  if (!BLE.begin()) {
    Serial.println("starting Bluetooth® Low Energy failed!");
  }
  Serial.println("BLE goot central");
  BLE.setLocalName("Player Device");
  // Create game Service
  BLEService gameService("419d3655-282c-4a0e-9b7d-fbed8e928581");  // BLE LED Service
  // BLE LED Switch Characteristic - custom 128-bit UUID, read and writable by central
  messageCharacteristic = BLECharacteristic("b7a73424-a7d9-4aff-958d-04f4612d461b", BLEWrite, sizeof(Message), true);
  BLE.setAdvertisedService(gameService);
  // add the characteristic to the service
  gameService.addCharacteristic(messageCharacteristic);
  // add service
  BLE.addService(gameService);
  // start advertising
  BLE.advertise();
}
*/

void setupPeripheral() {
  pinMode(buttonPin, INPUT_PULLUP);
  // begin initialization
  if (!BLE.begin()) {
    Serial.println("starting Bluetooth® Low Energy failed!");
  }
  // set advertised local name and service UUID:
  BLE.setLocalName("Game Device");
  BLE.setAdvertisedService(GameService);
  // add the characteristic to the service
  GameService.addCharacteristic(RequestCharacteristic);
  GameService.addCharacteristic(ResponseCharacteristic);
  // add service
  BLE.addService(GameService);
  // start advertising
  BLE.advertise();
  Serial.println("BLE Game Peripheral, waiting for connections....");
}

int count = 1;

/*
void loopPeripheral() {
  Serial.println("loopPeripheral");

  Message message = {
    BOMB_ATTACK,
    {},
    {},
    16,
    7,
  };

  String senderId = getMacAddress();
  strcpy(message.senderId, senderId.c_str());
  BLEDevice central = BLE.central();
  if (central) {
    Serial.println("Connected to central");
  }
  else {
    Serial.println("No central");
  }
  String receiverId = central.address();
  Serial.println(receiverId);
  Serial.println(receiverId.length());
  strcpy(message.recevierId, receiverId.c_str());

  Serial.print("Connected: ");
  Serial.println(central.connected());

  Serial.println(message.senderId);
  Serial.println(messageCharacteristic.uuid());
   
  while (central.connected()) {
    Serial.print("Write: ");
    Serial.println(count);
    Serial.println("WriteValue: ");
    Serial.print("Write: ");
    BLEByteCharacteristic testCharacteristic("b9e3b5f7-b6b8-42a8-ae4b-9ae9f7f6b618", BLEWrite);
    testCharacteristic.writeValue((byte) 'A'); 
    // messageCharacteristic.writeValue(&message, sizeof(Message), false);
    Serial.print("Written: ");
    Serial.println(message.senderId);
    count++;
    delay(500);
    Serial.print("Connected: ");
    Serial.println(central.connected());
  }

  */

void loopPeripheral() {
  // listen for BLE peripherals to connect:
  BLEDevice central = BLE.central();
  // if a central is connected to peripheral:
  if (central) {
    Serial.print("Connected to central: ");
    // print the central's MAC address:
    Serial.println(central.address());

    // while the central is still connected to peripheral:
    while (central.connected()) {

      Serial.println("Write message");
      sendMessage(RequestCharacteristic, BOMB_ATTACK, 3, 7);

      if (ResponseCharacteristic.canRead())
      {
        Serial.println("Can read from response characteristic");
      }
      else
      {
        Serial.println("Cannot read from response characterstic");
      }
      Message received;
      //receiveMessage(ResponseCharacteristic, received);
      int result = ResponseCharacteristic.readValue(&received, sizeof(Message));
      Serial.println(result);
      
      Serial.print("Received: ");
      Serial.println(received.type);
      Serial.println(received.x);
      delay(500);
    }
    // when the central disconnects, print it out:
    Serial.print(F("Disconnected from central: "));
    Serial.println(central.address());
  }
}