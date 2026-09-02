/*
 * DIYables_XPT2046.cpp - Implementation of the XPT2046 SPI touch driver.
 *
 * Copyright (c) DIYables.io
 * Licensed under the MIT License.
 */

#include "DIYables_XPT2046.h"

// XPT2046 control byte format: S A2 A1 A0 MODE SER/DFR PD1 PD0
//   S        = 1 (start bit)
//   A2..A0   = channel select
//   MODE     = 0 -> 12-bit, 1 -> 8-bit
//   SER/DFR  = 0 -> differential, 1 -> single-ended
//   PD1 PD0  = power-down between conversions (00 = lowest power)
// Standard differential 12-bit reads:
#define XPT2046_CMD_READ_X   0xD0
#define XPT2046_CMD_READ_Y   0x90
#define XPT2046_CMD_READ_Z1  0xB0
#define XPT2046_CMD_READ_Z2  0xC0

// XPT2046 max SPI clock: ~2 MHz. Stay safe on slow MCUs and shared buses.
static const uint32_t XPT2046_SPI_FREQ = 2000000UL;

DIYables_XPT2046::DIYables_XPT2046(uint8_t cs, int8_t irq, SPIClass *spi)
  : pressureThreshhold(400), _cs(cs), _irq(irq), _spi(spi) {}

void DIYables_XPT2046::begin() {
  pinMode(_cs, OUTPUT);
  digitalWrite(_cs, HIGH);
  if (_irq >= 0) pinMode(_irq, INPUT_PULLUP);
}

uint16_t DIYables_XPT2046::readChannel(uint8_t cmd) {
  _spi->transfer(cmd);
  uint8_t hi = _spi->transfer(0x00);
  uint8_t lo = _spi->transfer(0x00);
  // 12-bit result is left-aligned in the 16 read bits.
  return ((uint16_t)hi << 5) | (lo >> 3);
}

// Pick the two of three samples that are closest to each other and return
// their average. Discards the outlier, which is almost always a noise spike.
// This is the same filter used by Paul Stoffregen's XPT2046_Touchscreen lib.
static int16_t bestTwoAvg(int16_t a, int16_t b, int16_t c) {
  int16_t dab = (a > b) ? (a - b) : (b - a);
  int16_t dac = (a > c) ? (a - c) : (c - a);
  int16_t dbc = (b > c) ? (b - c) : (c - b);
  if (dab <= dac && dab <= dbc) return (int16_t)((a + b) >> 1);
  if (dac <= dab && dac <= dbc) return (int16_t)((a + c) >> 1);
  return (int16_t)((b + c) >> 1);
}

void DIYables_XPT2046::readSample(int16_t &x, int16_t &y, int16_t &z) {
  _spi->beginTransaction(SPISettings(XPT2046_SPI_FREQ, MSBFIRST, SPI_MODE0));
  digitalWrite(_cs, LOW);

  // Read Z first (pressure).
  uint16_t z1 = readChannel(XPT2046_CMD_READ_Z1);
  uint16_t z2 = readChannel(XPT2046_CMD_READ_Z2);
  // Approximate pressure indicator (smaller value -> harder press).
  // Most sketches just need a relative number, so use a simple form:
  //   z = z1 + 4095 - z2
  // which grows with press strength and is 0-ish when not touched.
  int32_t z_calc = (int32_t)z1 + 4095 - (int32_t)z2;
  if (z_calc < 0)    z_calc = 0;
  if (z_calc > 4095) z_calc = 4095;
  z = (int16_t)z_calc;

  // If clearly not touched, skip the X/Y oversampling and return early with
  // a power-down read. Saves SPI time and avoids reporting junk coordinates.
  if (z_calc < pressureThreshhold) {
    x = 0;
    y = 0;
    _spi->transfer(0x80); // power down between conversions
    _spi->transfer(0x00);
    _spi->transfer(0x00);
    digitalWrite(_cs, HIGH);
    _spi->endTransaction();
    return;
  }

  // The first sample after switching channels is always noisy on the
  // XPT2046 — throw it away before taking the three real samples.
  (void)readChannel(XPT2046_CMD_READ_X);

  int16_t x0 = (int16_t)readChannel(XPT2046_CMD_READ_X);
  int16_t y0 = (int16_t)readChannel(XPT2046_CMD_READ_Y);
  int16_t x1 = (int16_t)readChannel(XPT2046_CMD_READ_X);
  int16_t y1 = (int16_t)readChannel(XPT2046_CMD_READ_Y);
  int16_t x2 = (int16_t)readChannel(XPT2046_CMD_READ_X);
  int16_t y2 = (int16_t)readChannel(XPT2046_CMD_READ_Y);

  // Best-two-of-three average rejects the worst outlier in each axis.
  x = bestTwoAvg(x0, x1, x2);
  y = bestTwoAvg(y0, y1, y2);

  // Final read with PD1=PD0=0 to power down between conversions.
  _spi->transfer(0x80); // dummy power-down command
  _spi->transfer(0x00);
  _spi->transfer(0x00);

  digitalWrite(_cs, HIGH);
  _spi->endTransaction();
}

TSPoint DIYables_XPT2046::getPoint() {
  int16_t x, y, z;
  readSample(x, y, z);
  return TSPoint(x, y, z);
}

uint16_t DIYables_XPT2046::pressure() {
  int16_t x, y, z;
  readSample(x, y, z);
  return (uint16_t)z;
}

bool DIYables_XPT2046::isTouching() {
  // Fast path: if IRQ pin is wired, it goes LOW while screen is touched.
  if (_irq >= 0) {
    if (digitalRead(_irq) == HIGH) return false;
  }
  int16_t x, y, z;
  readSample(x, y, z);
  return z > pressureThreshhold;
}
