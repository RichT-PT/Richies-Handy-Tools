/*
 * DIYables_XPT2046.h - Driver for the XPT2046 / ADS7843 SPI touch controller.
 *
 * The XPT2046 is the touch IC found on TFT modules whose touch breakout pins
 * are labeled T_CS, T_CLK, T_DIN, T_DO, and (optionally) T_IRQ. It speaks
 * SPI and is normally wired to share the same SPI bus as the display, but
 * with its own dedicated chip-select line.
 *
 * This is an INTERNAL header. Sketches should include <DIYables_TFT_SPI.h>
 * and use the high-level helpers (initTouchSPI / getTouch / readTouchRaw)
 * exposed by the base DIYables_TFT_SPI class.
 *
 * Copyright (c) DIYables.io
 * Licensed under the MIT License.
 */

#ifndef DIYABLES_XPT2046_H
#define DIYABLES_XPT2046_H

#include <Arduino.h>
#include <SPI.h>
#include "DIYables_TouchScreen.h" // for TSPoint

class DIYables_XPT2046 {
public:
  // cs  : chip-select pin connected to T_CS
  // irq : interrupt pin connected to T_IRQ (optional, -1 to disable)
  // spi : SPI bus to use (defaults to &SPI; pass &SPI1 / &HSPI as needed)
  DIYables_XPT2046(uint8_t cs, int8_t irq = -1, SPIClass *spi = &SPI);

  void begin();

  bool     isTouching();
  uint16_t pressure();
  TSPoint  getPoint();

  // Pressure threshold for isTouching(). XPT2046 returns small Z values when
  // not touched; default 400 works for most resistive panels.
  int16_t pressureThreshhold;

private:
  uint8_t   _cs;
  int8_t    _irq;
  SPIClass *_spi;

  uint16_t readChannel(uint8_t cmd);
  void     readSample(int16_t &x, int16_t &y, int16_t &z);
};

#endif // DIYABLES_XPT2046_H
