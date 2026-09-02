/*
 * DIYables_ILI9341_SPI.cpp - ILI9341 SPI TFT driver (240x320, 16-bit RGB565)
 *
 * Copyright (c) DIYables.io
 * Licensed under the MIT License.
 */

#include "DIYables_ILI9341_SPI.h"

DIYables_ILI9341_SPI::DIYables_ILI9341_SPI(uint16_t width, uint16_t height,
                                           int8_t cs_pin, int8_t dc_pin,
                                           int8_t rst_pin, SPIClass *spi_bus)
  : DIYables_TFT_SPI(width, height, cs_pin, dc_pin, rst_pin, spi_bus) {}

uint32_t DIYables_ILI9341_SPI::getDefaultFreq() {
  return 40000000UL; // 40 MHz
}

void DIYables_ILI9341_SPI::initDriver() {
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
  writeData8(0x48);   // MX, BGR

  writeCommand(0x3A); // Pixel Format
  writeData8(0x55);   // 16-bit RGB565

  writeCommand(0xB1); // Frame Rate Control
  writeData8(0x00); writeData8(0x18);

  writeCommand(0xB6); // Display Function Control
  writeData8(0x08); writeData8(0x82); writeData8(0x27);

  writeCommand(0xF2); // 3Gamma Function Disable
  writeData8(0x00);

  writeCommand(0x26); // Gamma curve selected
  writeData8(0x01);

  writeCommand(0xE0); // Positive Gamma Correction
  writeData8(0x0F); writeData8(0x31); writeData8(0x2B); writeData8(0x0C);
  writeData8(0x0E); writeData8(0x08); writeData8(0x4E); writeData8(0xF1);
  writeData8(0x37); writeData8(0x07); writeData8(0x10); writeData8(0x03);
  writeData8(0x0E); writeData8(0x09); writeData8(0x00);

  writeCommand(0xE1); // Negative Gamma Correction
  writeData8(0x00); writeData8(0x0E); writeData8(0x14); writeData8(0x03);
  writeData8(0x11); writeData8(0x07); writeData8(0x31); writeData8(0xC1);
  writeData8(0x48); writeData8(0x08); writeData8(0x0F); writeData8(0x0C);
  writeData8(0x31); writeData8(0x36); writeData8(0x0F);

  writeCommand(0x11); // Sleep Out
  delay(120);

  writeCommand(0x29); // Display ON
  delay(50);
}

uint8_t DIYables_ILI9341_SPI::getRotationData(uint8_t r) {
  switch (r & 0x03) {
    case 0:  return 0x48; // MX, BGR
    case 1:  return 0x28; // MV, BGR
    case 2:  return 0x88; // MY, BGR
    default: return 0xE8; // MX, MY, MV, BGR
  }
}
