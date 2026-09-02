/*
 * DIYables_ILI9341_SPI.h - ILI9341 SPI TFT driver (240x320, 16-bit RGB565)
 *
 * Copyright (c) DIYables.io
 * Licensed under the MIT License.
 */

#ifndef DIYABLES_ILI9341_SPI_H
#define DIYABLES_ILI9341_SPI_H

#include "DIYables_TFT_SPI_Base.h"

class DIYables_ILI9341_SPI : public DIYables_TFT_SPI {
public:
  // width/height: panel resolution in its native orientation.
  //   Common ILI9341 modules: 240x320 (portrait) or 320x240 (landscape).
  // Pass &SPI1, &HSPI, etc. for an alternate SPI bus.
  DIYables_ILI9341_SPI(uint16_t width, uint16_t height,
                       int8_t cs_pin, int8_t dc_pin,
                       int8_t rst_pin = -1, SPIClass *spi_bus = &SPI);

protected:
  void     initDriver() override;
  uint8_t  getRotationData(uint8_t r) override;
  uint32_t getDefaultFreq() override;
};

#endif // DIYABLES_ILI9341_SPI_H
