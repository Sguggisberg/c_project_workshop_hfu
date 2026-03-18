#include <ArduinoBLE.h>
#include <stdbool.h>
#include "central.h"
#include "message.h"

extern bool logging;
void play_move(BLEDevice peripheral);
void log(BLEDevice peripheral);

void setup_central() {
  pinMode(LED_BUILTIN, OUTPUT);
  while (!Serial)
    ;
  // initialize the BLE hardware
  BLE.begin();
  Serial.println("BLE Central - LED control");
  // start scanning for Button Device BLE peripherals
  BLE.scanForUuid("19b10000-e8f2-537e-4f6c-d104768a1214");
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
    // discovered a peripheral, print out address, local name, and advertised service
    Serial.print("Found ");
    Serial.print(peripheral.address());
    Serial.print(" '");
    Serial.print(peripheral.localName());
    Serial.print("' ");
    Serial.print(peripheral.advertisedServiceUuid());
    Serial.println();
    if (peripheral.localName().indexOf("Button Device") < 0) {
      Serial.println("No 'Button Device' in name");
      return;  // If the name doesn't have "Button Device" in it then ignore it
    }
    // stop scanning
    BLE.stopScan();
    controlLed(peripheral);
    // peripheral disconnected, start scanning again
    BLE.scanForUuid("19b10000-e8f2-537e-4f6c-d104768a1214");
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
  // retrieve the LED characteristic
  BLECharacteristic GameCharacteristic = peripheral.characteristic("19b10001-e8f2-537e-4f6c-d104768a1214");
  if (!GameCharacteristic) {
    Serial.println("Peripheral does not have LED characteristic!");
    peripheral.disconnect();
    return;
  }
  while (peripheral.connected()) {
    // while the peripheral is connected
    if (GameCharacteristic.canRead()) {
      byte value = GameCharacteristic.read();
      GameCharacteristic.readValue(value);
      //Serial.println(LEDCharacteristic.readValue(value));
      if (value == 0x01) {
        Serial.println("ON");
        digitalWrite(LED_BUILTIN, HIGH);
      } else if (value == 0x00) {
        digitalWrite(LED_BUILTIN, LOW);
        Serial.println("OFF");
      }
    }
    delay(500);
  }
}
