/*
 * DIYables_TFT_SPI.h - Umbrella header for the DIYables TFT SPI library.
 *
 * Sketches only need this single include to get the base class and all
 * supported display drivers (ILI9341, ILI9488, ST7789) plus touch support.
 *
 *     #include <DIYables_TFT_SPI.h>
 *
 * Then instantiate one of (width and height must match your panel):
 *     DIYables_ILI9341_SPI tft(240, 320, cs, dc, rst);
 *     DIYables_ILI9488_SPI tft(320, 480, cs, dc, rst);
 *     DIYables_ST7789_SPI  tft(240, 320, cs, dc, rst);
 *
 * Copyright (c) DIYables.io
 * Licensed under the MIT License.
 */

#ifndef DIYABLES_TFT_SPI_UMBRELLA_H
#define DIYABLES_TFT_SPI_UMBRELLA_H

#include "DIYables_TFT_SPI_Base.h"
#include "DIYables_ILI9341_SPI.h"
#include "DIYables_ILI9488_SPI.h"
#include "DIYables_ST7789_SPI.h"

#endif // DIYABLES_TFT_SPI_UMBRELLA_H