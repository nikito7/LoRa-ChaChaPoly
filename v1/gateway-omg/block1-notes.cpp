// ZgatewayLORA

// LORAConfig_s LORAConfig;
// --- cut ---

// ### CHA_CHA_POLY ###

#include "ChaChaPolyHelper.h"
#include "../../easyhan_secrets_cha.h"

byte iv[CHA_CHA_POLY_IV_SIZE];

int i;

void printHex(uint8_t num) {
  char hexCar[2];

  sprintf(hexCar, "%02X", num);
  Serial.print(hexCar);
}

// ### CHA_CHA_POLY EOF ###

// --- cut ---
// #  ifdef ZmqttDiscovery

// EOF
