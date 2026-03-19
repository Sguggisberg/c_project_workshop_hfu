#ifndef BLE_LINK_H
#define BLE_LINK_H

#include <ArduinoBLE.h>
#include "Protocol.h"

class BleLink {
public:
  BleLink();

  bool begin();
  void update();

  bool startScan();
  void stopScan();

  bool hasFoundDevice() const;
  const char* foundName() const;
  const char* foundAddress() const;

  bool connectToFound();
  void disconnect();

  bool isConnected() const;
  bool isSearching() const;
  bool isCentralRole() const;
  bool isInitiator() const;

  bool send(const Packet& packet);
  bool receive(Packet& packet);

private:
  BLEService localService;
  BLECharacteristic localRx;
  BLECharacteristic localTx;

  BLEDevice remoteDevice;
  BLECharacteristic remoteRx;
  BLECharacteristic remoteTx;

  BLEDevice connectedCentral;

  bool searching;
  bool connected;
  bool centralRole;
  bool initiator;
  bool hasDevice;

  char foundNameBuf[32];
  char foundAddrBuf[24];

  void clearFound();
  void setFound(const BLEDevice& dev);
  void startAdvertising();
};

#endif