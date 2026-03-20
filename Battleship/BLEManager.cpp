#include "BLEManager.h"

BLEManager::BLEManager()
  : service_(Config::SERVICE_UUID),
    localRequest_(Config::REQUEST_UUID, BLERead | BLEWrite | BLENotify, sizeof(Packet)),
    localResponse_(Config::RESPONSE_UUID, BLERead | BLEWrite | BLENotify, sizeof(Packet)),
    role_(ROLE_UNDECIDED), connected_(false),
    lastRoleTryAt_(0), lastConnectTryAt_(0),
    hasRequest_(false), hasResponse_(false) {
  pendingRequest_ = {MSG_NONE, 0, 0, 0};
  pendingResponse_ = {MSG_NONE, 0, 0, 0};
}

bool BLEManager::begin() {
  if (!BLE.begin()) {
    return false;
  }

  BLE.setDeviceName(Config::DEVICE_NAME);
  BLE.setLocalName(Config::DEVICE_NAME);
  BLE.setAdvertisedService(service_);
  service_.addCharacteristic(localRequest_);
  service_.addCharacteristic(localResponse_);
  BLE.addService(service_);

  Packet empty = {MSG_NONE, 0, 0, 0};
  localRequest_.writeValue(reinterpret_cast<const uint8_t*>(&empty), sizeof(Packet));
  localResponse_.writeValue(reinterpret_cast<const uint8_t*>(&empty), sizeof(Packet));

  tryBecomeCentral();
  if (role_ == ROLE_UNDECIDED) {
    startPeripheralMode();
  }
  return true;
}

void BLEManager::startPeripheralMode() {
  BLE.stopScan();
  BLE.advertise();
  role_ = ROLE_PERIPHERAL;
  connected_ = false;
}

void BLEManager::tryBecomeCentral() {
  BLE.stopAdvertise();
  BLE.scanForUuid(Config::SERVICE_UUID);
  unsigned long startAt = millis();
  while (millis() - startAt < 700) {
    BLEDevice p = BLE.available();
    if (p && bindRemoteCharacteristics(p)) {
      role_ = ROLE_CENTRAL;
      connected_ = true;
      BLE.stopScan();
      return;
    }
    delay(10);
  }
  BLE.stopScan();
}

bool BLEManager::bindRemoteCharacteristics(BLEDevice peripheral) {
  if (!peripheral) return false;
  if (!peripheral.connect()) return false;
  if (!peripheral.discoverAttributes()) {
    peripheral.disconnect();
    return false;
  }

  BLECharacteristic req = peripheral.characteristic(Config::REQUEST_UUID);
  BLECharacteristic res = peripheral.characteristic(Config::RESPONSE_UUID);
  if (!req || !res) {
    peripheral.disconnect();
    return false;
  }

  remotePeripheral_ = peripheral;
  remoteRequest_ = req;
  remoteResponse_ = res;
  remoteRequest_.subscribe();
  remoteResponse_.subscribe();
  return true;
}

void BLEManager::queueRequest(const Packet& packet) {
  pendingRequest_ = packet;
  hasRequest_ = true;
}

void BLEManager::queueResponse(const Packet& packet) {
  pendingResponse_ = packet;
  hasResponse_ = true;
}

bool BLEManager::readPacketFromCharacteristic(BLECharacteristic& characteristic, Packet& outPacket) {
  if (!characteristic) return false;
  if (characteristic.valueLength() < static_cast<int>(sizeof(Packet))) return false;
  int readLen = characteristic.readValue(reinterpret_cast<uint8_t*>(&outPacket), sizeof(Packet));
  return readLen == static_cast<int>(sizeof(Packet));
}

void BLEManager::servicePeripheral() {
  BLEDevice central = BLE.central();
  connected_ = central && central.connected();

  if (connected_) {
    if (localRequest_.written()) {
      Packet packet;
      if (readPacketFromCharacteristic(localRequest_, packet)) {
        queueRequest(packet);
      }
    }
    if (localResponse_.written()) {
      Packet packet;
      if (readPacketFromCharacteristic(localResponse_, packet)) {
        queueResponse(packet);
      }
    }
  }
}

void BLEManager::serviceCentral() {
  if (!remotePeripheral_ || !remotePeripheral_.connected()) {
    connected_ = false;
    if (millis() - lastConnectTryAt_ > Config::CONNECT_RETRY_MS) {
      lastConnectTryAt_ = millis();
      tryBecomeCentral();
      if (role_ != ROLE_CENTRAL || !connected_) {
        startPeripheralMode();
      }
    }
    return;
  }

  connected_ = true;

  if (remoteRequest_.valueUpdated()) {
    Packet packet;
    if (readPacketFromCharacteristic(remoteRequest_, packet)) {
      queueRequest(packet);
    }
  }
  if (remoteResponse_.valueUpdated()) {
    Packet packet;
    if (readPacketFromCharacteristic(remoteResponse_, packet)) {
      queueResponse(packet);
    }
  }
}

void BLEManager::update() {
  BLE.poll();
  switch (role_) {
    case ROLE_CENTRAL: serviceCentral(); break;
    case ROLE_PERIPHERAL: servicePeripheral(); break;
    default:
      if (millis() - lastRoleTryAt_ > Config::ROLE_RETRY_MS) {
        lastRoleTryAt_ = millis();
        tryBecomeCentral();
        if (role_ == ROLE_UNDECIDED) {
          startPeripheralMode();
        }
      }
      break;
  }
}

bool BLEManager::sendRequest(const Packet& packet) {
  if (!isReadyForTraffic()) return false;
  if (role_ == ROLE_CENTRAL) {
    return remoteRequest_.writeValue(reinterpret_cast<const uint8_t*>(&packet), sizeof(Packet));
  }
  return localRequest_.writeValue(reinterpret_cast<const uint8_t*>(&packet), sizeof(Packet));
}

bool BLEManager::sendResponse(const Packet& packet) {
  if (!isReadyForTraffic()) return false;
  if (role_ == ROLE_CENTRAL) {
    return remoteResponse_.writeValue(reinterpret_cast<const uint8_t*>(&packet), sizeof(Packet));
  }
  return localResponse_.writeValue(reinterpret_cast<const uint8_t*>(&packet), sizeof(Packet));
}

bool BLEManager::hasIncomingRequest() const { return hasRequest_; }
bool BLEManager::hasIncomingResponse() const { return hasResponse_; }

Packet BLEManager::popIncomingRequest() {
  hasRequest_ = false;
  Packet copy = pendingRequest_;
  pendingRequest_ = {MSG_NONE, 0, 0, 0};
  return copy;
}

Packet BLEManager::popIncomingResponse() {
  hasResponse_ = false;
  Packet copy = pendingResponse_;
  pendingResponse_ = {MSG_NONE, 0, 0, 0};
  return copy;
}

bool BLEManager::isConnected() const { return connected_; }
Role BLEManager::role() const { return role_; }

bool BLEManager::isReadyForTraffic() const {
  if (!connected_) return false;
  if (role_ == ROLE_CENTRAL) {
    return remoteRequest_ && remoteResponse_;
  }
  return true;
}

String BLEManager::connectionLabel() const {
  switch (role_) {
    case ROLE_CENTRAL: return connected_ ? "Central verbunden" : "Central sucht";
    case ROLE_PERIPHERAL: return connected_ ? "Peripheral verbunden" : "Peripheral wartet";
    default: return "BLE startet";
  }
}

void BLEManager::resetSession() {
  hasRequest_ = false;
  hasResponse_ = false;
  pendingRequest_ = {MSG_NONE, 0, 0, 0};
  pendingResponse_ = {MSG_NONE, 0, 0, 0};
}
