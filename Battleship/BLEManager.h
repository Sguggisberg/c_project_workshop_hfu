#pragma once

#include <ArduinoBLE.h>
#include "Config.h"
#include "GameTypes.h"

class BLEManager {
public:
  BLEManager();

  bool begin();
  void update();
  bool sendRequest(const Packet& packet);
  bool sendResponse(const Packet& packet);
  bool hasIncomingRequest() const;
  bool hasIncomingResponse() const;
  Packet popIncomingRequest();
  Packet popIncomingResponse();

  bool isConnected() const;
  Role role() const;
  bool isReadyForTraffic() const;
  String connectionLabel() const;
  void resetSession();

private:
  void startPeripheralMode();
  void tryBecomeCentral();
  void serviceCentral();
  void servicePeripheral();
  bool bindRemoteCharacteristics(BLEDevice peripheral);
  bool readPacketFromCharacteristic(BLECharacteristic& characteristic, Packet& outPacket);
  void queueRequest(const Packet& packet);
  void queueResponse(const Packet& packet);

  BLEService service_;
  BLECharacteristic localRequest_;
  BLECharacteristic localResponse_;
  BLEDevice remotePeripheral_;
  BLECharacteristic remoteRequest_;
  BLECharacteristic remoteResponse_;

  Role role_;
  bool connected_;
  unsigned long lastRoleTryAt_;
  unsigned long lastConnectTryAt_;

  bool hasRequest_;
  bool hasResponse_;
  Packet pendingRequest_;
  Packet pendingResponse_;
};
