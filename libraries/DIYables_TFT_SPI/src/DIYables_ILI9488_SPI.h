/*
 * DIYables_ILI9488_SPI.h - ILI9488 SPI TFT driver (320x480, 18-bit RGB666)
 *
 * Note: ILI9488 in SPI mode only supports 18-bit color (3 bytes per pixel).
 * RGB565 colors are automatically converted to RGB666 during transfer.
 *
 * Copyright (c) DIYables.io
 * Licensed under the MIT License.
 */

#ifndef DIYABLES_ILI9488_SPI_H
#define DIYABLES_ILI9488_SPI_H

#include "DIYables_TFT_SPI_Base.h"

class DIYables_ILI9488_SPI : public DIYables_TFT_SPI {
public:
  // width/height: panel resolution in its native orientation.
  //   Common ILI9488 modules: 320x480 (portrait) or 480x320 (landscape).
  // Pass &SPI1, &HSPI, etc. for an alternate SPI bus.
  DIYables_ILI9488_SPI(uint16_t width, uint16_t height,
                       int8_t cs_pin, int8_t dc_pin,
                       int8_t rst_pin = -1, SPIClass *spi_bus = &SPI);

protected:
  void     initDriver() override;
  uint8_t  getRotationData(uint8_t r) override;
  uint32_t getDefaultFreq() override;

  // Override RGB565 defaults: ILI9488 SPI mode requires 18-bit RGB666 (3 bytes/pixel)
  void     writeColor(uint16_t color) override;
  void     fillColor(uint16_t color, uint32_t count) override;
  void     pushColorData(uint16_t *data, uint32_t len) override;
  void     pushColorDataPGM(const uint16_t *data, uint32_t len) override;
};

#endif // DIYABLES_ILI9488_SPI_H
