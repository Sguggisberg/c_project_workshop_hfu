#include "BleLink.h"
#include <cstring>

BleLink::BleLink()
  : localService(GAME_SERVICE_UUID),
    localRx(GAME_RX_UUID, BLEWrite | BLEWriteWithoutResponse, sizeof(Packet)),
    localTx(GAME_TX_UUID, BLERead | BLENotify, sizeof(Packet)),
    searching(false),
    connected(false),
    centralRole(false),
    initiator(false),
    hasDevice(false) {

  foundNameBuf[0] = '\0';
  foundAddrBuf[0] = '\0';
}

bool BleLink::begin() {
  if (!BLE.begin()) {
    return false;
  }

  BLE.setDeviceName(GAME_DEVICE_NAME);
  BLE.setLocalName(GAME_DEVICE_NAME);
  BLE.setAdvertisedService(localService);

  localService.addCharacteristic(localRx);
  localService.addCharacteristic(localTx);
  BLE.addService(localService);

  Packet empty{};
  localRx.writeValue((const uint8_t*)&empty, sizeof(empty));
  localTx.writeValue((const uint8_t*)&empty, sizeof(empty));

  startAdvertising();
  return true;
}

void BleLink::startAdvertising() {
  BLE.stopScan();
  searching = false;
  BLE.advertise();
}

void BleLink::clearFound() {
  hasDevice = false;
  foundNameBuf[0] = '\0';
  foundAddrBuf[0] = '\0';
}

void BleLink::setFound(const BLEDevice& dev) {
  clearFound();

  String name = dev.localName();
  String addr = dev.address();

  name.toCharArray(foundNameBuf, sizeof(foundNameBuf));
  addr.toCharArray(foundAddrBuf, sizeof(foundAddrBuf));
  hasDevice = true;
}

bool BleLink::startScan() {
  if (connected) return false;

  clearFound();
  BLE.stopAdvertise();
  BLE.scanForUuid(GAME_SERVICE_UUID);
  searching = true;
  return true;
}

void BleLink::stopScan() {
  BLE.stopScan();
  searching = false;
  startAdvertising();
}

bool BleLink::hasFoundDevice() const {
  return hasDevice;
}

const char* BleLink::foundName() const {
  return foundNameBuf;
}

const char* BleLink::foundAddress() const {
  return foundAddrBuf;
}

bool BleLink::connectToFound() {
  if (!hasDevice || connected) return false;

  BLE.stopScan();
  searching = false;

  BLEDevice dev = BLE.available();
  if (!dev) {
    BLE.scanForUuid(GAME_SERVICE_UUID);
    delay(250);
    dev = BLE.available();
    BLE.stopScan();
  }

  if (!dev) {
    startAdvertising();
    return false;
  }

  if (!dev.connect()) {
    startAdvertising();
    return false;
  }

  if (!dev.discoverAttributes()) {
    dev.disconnect();
    startAdvertising();
    return false;
  }

  BLECharacteristic rx = dev.characteristic(GAME_RX_UUID);
  BLECharacteristic tx = dev.characteristic(GAME_TX_UUID);

  if (!rx || !tx) {
    dev.disconnect();
    startAdvertising();
    return false;
  }

  tx.subscribe();

  remoteDevice = dev;
  remoteRx = rx;
  remoteTx = tx;

  connected = true;
  centralRole = true;
  initiator = true;
  return true;
}

void BleLink::disconnect() {
  if (centralRole && remoteDevice) {
    remoteDevice.disconnect();
  }

  connected = false;
  centralRole = false;
  initiator = false;
  connectedCentral = BLEDevice();

  startAdvertising();
}

bool BleLink::isConnected() const {
  return connected;
}

bool BleLink::isSearching() const {
  return searching;
}

bool BleLink::isCentralRole() const {
  return centralRole;
}

bool BleLink::isInitiator() const {
  return initiator;
}

void BleLink::update() {
  if (!connected) {
    if (searching) {
      BLEDevice dev = BLE.available();
      if (dev) {
        String addr = dev.address();
        if (addr != BLE.address()) {
          setFound(dev);
          BLE.stopScan();
          searching = false;
          startAdvertising();
        }
      }
    }

    BLEDevice c = BLE.central();
    if (c) {
      connectedCentral = c;
      connected = true;
      centralRole = false;
      initiator = false;
    }
    return;
  }

  if (centralRole) {
    if (!remoteDevice || !remoteDevice.connected()) {
      disconnect();
    }
  } else {
    if (!connectedCentral || !connectedCentral.connected()) {
      connected = false;
      centralRole = false;
      initiator = false;
      connectedCentral = BLEDevice();
      startAdvertising();
    }
  }
}

bool BleLink::send(const Packet& packet) {
  if (!connected) return false;

  if (centralRole) {
    if (!remoteRx) return false;
    return remoteRx.writeValue((const uint8_t*)&packet, sizeof(packet));
  } else {
    return localTx.writeValue((const uint8_t*)&packet, sizeof(packet));
  }
}

bool BleLink::receive(Packet& packet) {
  if (!connected) return false;

  if (centralRole) {
    if (remoteTx && remoteTx.valueUpdated()) {
      int len = remoteTx.readValue((uint8_t*)&packet, sizeof(packet));
      return (len == (int)sizeof(packet));
    }
  } else {
    if (localRx.written()) {
      int len = localRx.readValue((uint8_t*)&packet, sizeof(packet));
      return (len == (int)sizeof(packet));
    }
  }

  return false;
}