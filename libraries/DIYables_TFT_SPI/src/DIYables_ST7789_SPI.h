/*
 * DIYables_ST7789_SPI.h - ST7789 SPI TFT driver (240x320, 16-bit RGB565)
 *
 * Copyright (c) DIYables.io
 * Licensed under the MIT License.
 */

#ifndef DIYABLES_ST7789_SPI_H
#define DIYABLES_ST7789_SPI_H

#include "DIYables_TFT_SPI_Base.h"

class DIYables_ST7789_SPI : public DIYables_TFT_SPI {
public:
  // width/height: panel resolution in its native orientation.
  //   Common ST7789 modules: 240x320, 240x240, 135x240, 170x320, 172x320.
  // Pass &SPI1, &HSPI, etc. for an alternate SPI bus.
  DIYables_ST7789_SPI(uint16_t width, uint16_t height,
                      int8_t cs_pin, int8_t dc_pin,
                      int8_t rst_pin = -1, SPIClass *spi_bus = &SPI);

protected:
  void     initDriver() override;
  uint8_t  getRotationData(uint8_t r) override;
  uint32_t getDefaultFreq() override;
};

#endif // DIYABLES_ST7789_SPI_H
