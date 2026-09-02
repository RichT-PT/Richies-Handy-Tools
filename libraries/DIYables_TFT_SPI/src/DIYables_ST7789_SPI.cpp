/*
 * DIYables_ST7789_SPI.cpp - ST7789 SPI TFT driver
 *
 * Implementation notes:
 * - This driver targets the DIYables ST7789 SPI TFT module. It also
 *   works with generic ST7789 panels, since the commands used here
 *   (SWRESET, SLPOUT, COLMOD, MADCTL, CASET, RASET, DISPON) are part
 *   of the common MIPI DCS set supported by both controller families,
 *   and any extra commands are simply ignored by a real ST7789.
 * - The module is landscape-native (320x240 RAM). To keep the public
 *   API consistent with the ILI9341 driver -- where the user passes
 *   portrait dimensions (240x320) and rotation 0 is portrait -- this
 *   class internally adds a 90 degree offset in its MADCTL values so
 *   the application code does not have to know about it.
 *
 * Copyright (c) DIYables.io
 * Licensed under the MIT License.
 */

#include "DIYables_ST7789_SPI.h"

DIYables_ST7789_SPI::DIYables_ST7789_SPI(uint16_t width, uint16_t height,
                                         int8_t cs_pin, int8_t dc_pin,
                                         int8_t rst_pin, SPIClass *spi_bus)
  : DIYables_TFT_SPI(width, height, cs_pin, dc_pin, rst_pin, spi_bus) {
  _touch_invert_x = true;
  _touch_invert_y = false;
}

uint32_t DIYables_ST7789_SPI::getDefaultFreq() {
  return 40000000UL; // 40 MHz
}

void DIYables_ST7789_SPI::initDriver() {
  writeCommand(0xEF);
  writeData8(0x03); writeData8(0x80); writeData8(0x02);

  writeCommand(0xCF); // Power control B
  writeData8(0x00); writeData8(0xC1); writeData8(0x30);

  writeCommand(0xED); // Power on sequence control
  writeData8(0x64); writeData8(0x03); writeData8(0x12); writeData8(0x81);

  writeCommand(0xE8); // Driver timing control A
  writeData8(0x85); writeData8(0x00); writeData8(0x78);

  writeCommand(0xCB); // Power control A
  writeData8(0x39); writeData8(0x2C); writeData8(0x00);
  writeData8(0x34); writeData8(0x02);

  writeCommand(0xF7); // Pump ratio control
  writeData8(0x20);

  writeCommand(0xEA); // Driver timing control B
  writeData8(0x00); writeData8(0x00);

  writeCommand(0xC0); // Power control 1
  writeData8(0x23);

  writeCommand(0xC1); // Power control 2
  writeData8(0x10);

  writeCommand(0xC5); // VCOM control 1
  writeData8(0x3E); writeData8(0x28);

  writeCommand(0xC7); // VCOM control 2
  writeData8(0x86);

  writeCommand(0x36); // Memory Access Control
  writeData8(0x20);   // MV, RGB (rotation 0 = portrait on landscape-native panel)

  writeCommand(0x3A); // Pixel Format
  writeData8(0x55);   // 16-bit RGB565

  writeCommand(0xB1); // Frame Rate Control
  writeData8(0x00); writeData8(0x18);

  writeCommand(0xB6); // Display Function Control
  writeData8(0x08); writeData8(0x82); writeData8(0x27);

  // Gamma programming intentionally omitted: the ILI9341 reference
  // gamma values dim the red channel on this panel (pure magenta drifts
  // toward violet). Letting the chip use its NVM default gamma gives
  // more accurate primaries.

  writeCommand(0x11); // Sleep Out
  delay(120);

  writeCommand(0x29); // Display ON
  delay(50);
}

uint8_t DIYables_ST7789_SPI::getRotationData(uint8_t r) {
  // Table rotated +90 deg vs. the ILI9341 driver so the application
  // sees the same orientation behavior on both panels:
  //   rotation 0 = portrait, 1 = landscape, 2 = portrait flipped,
  //   3 = landscape flipped, regardless of whether the underlying panel
  //   is portrait-native or landscape-native.
  switch (r & 0x03) {
    case 0:  return 0x20; // MV,            RGB -> portrait
    case 1:  return 0x80; // MY,            RGB -> landscape
    case 2:  return 0xE0; // MX, MY, MV,    RGB -> portrait flipped
    default: return 0x40; // MX,            RGB -> landscape flipped
  }
}
