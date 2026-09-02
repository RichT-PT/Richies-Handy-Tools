/*
  This Library is written for XPT2046
  Author: Bonezegei (Jofel Batutay)
  Date: July 2023 
*/
#ifndef _BONEZEGEI_XPT2046V2_H_
#define _BONEZEGEI_XPT2046V2_H_
#include <SPI.h>
#include <Arduino.h>

#include <Bonezegei_Utility.h>

static const int XPT2046_SPISPEED = 1000000;  // 1 MHz

class Bonezegei_XPT2046v2 : public Bonezegei_Input {
public:
  Bonezegei_XPT2046v2();
  Bonezegei_XPT2046v2(uint8_t cs, uint8_t irq);
  Bonezegei_XPT2046v2(uint8_t cs);
  ~Bonezegei_XPT2046v2() {}

  void begin();
  
  uint8_t getInput() {
    vspi->setFrequency(XPT2046_SPISPEED);
    digitalWrite(_cs, LOW);
    vspi->transfer(0xB1);
    int16_t z1 = vspi->transfer16(0xC1) >> 3;
    int16_t z2 = vspi->transfer16(0x91) >> 3;
    x = vspi->transfer16(0xD0) >> 3;
    y = vspi->transfer16(0) >> 3;
    digitalWrite(_cs, HIGH);
    z = z1 + 4095 - z2;

    if (z > 500) {
      return 1;
    } else {
      return 0;
    }
  }

  uint8_t _cs;
  uint8_t _irq;


  SPIClass *vspi = NULL;
};


#endif
