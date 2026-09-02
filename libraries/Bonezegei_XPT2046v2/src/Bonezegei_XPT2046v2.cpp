/*
  This Library is written for XPT2046
  Author: Bonezegei (Jofel Batutay)
  Date: July 2023 
*/
#include "Bonezegei_XPT2046v2.h"

Bonezegei_XPT2046v2::Bonezegei_XPT2046v2() {
}

Bonezegei_XPT2046v2::Bonezegei_XPT2046v2(uint8_t cs, uint8_t irq) {
  _cs = cs;
  _irq = irq;
}

Bonezegei_XPT2046v2::Bonezegei_XPT2046v2(uint8_t cs) {
  _cs = cs;
}

void Bonezegei_XPT2046v2::begin() {
  vspi = new SPIClass(VSPI);
  vspi->begin();
  vspi->setFrequency(XPT2046_SPISPEED);
  vspi->setDataMode(SPI_MODE0);
  vspi->setBitOrder(MSBFIRST);
  pinMode(_cs, OUTPUT);
}


/* uint8_t Bonezegei_XPT2046v2::getInput() {


}
 */
