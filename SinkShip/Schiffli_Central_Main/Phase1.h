#ifndef PHASE1_H
#define PHASE1_H

#include <Arduino.h>

enum StartSpieler {
  MAC1_BEGINNT,
  MAC2_BEGINNT
};

StartSpieler MacVergleich(const uint8_t mac1[6], const uint8_t mac2[6]);

#endif