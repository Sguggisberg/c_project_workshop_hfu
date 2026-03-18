#include "message.h"
#include "central.h"
#include "peripheral.h"

bool logging = false;
bool isCentral = false;
extern const String BOARD_A="419d3655-282c-4a0e-9b7d-fbed8e928581";
/*
419d3655-282c-4a0e-9b7d-fbed8e928581
 b7a73424-a7d9-4aff-958d-04f4612d461b
 b9e3b5f7-b6b8-42a8-ae4b-9ae9f7f6b618
 128b0f5a-f62a-477b-ab17-4caea64f9415
 */

/*
bool isCentral() {
  return !BLE.central();
}
*/

void setup() {
  Serial.begin(9600);
  Serial.println("Begin");
  Serial.println("Setup");
  if (isCentral) {
    setup_central();
  } else {
    setupPeripheral();
  }
}
void loop() {
  if (isCentral) {
    loop_central();
  } else {
    loopPeripheral();
  }
}