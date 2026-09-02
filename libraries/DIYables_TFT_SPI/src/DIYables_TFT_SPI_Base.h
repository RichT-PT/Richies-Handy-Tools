/*
 * DIYables_TFT_SPI_Base.h - Base class for SPI TFT displays
 *
 * Internal header. Do NOT include this directly from sketches.
 * Sketches should include <DIYables_TFT_SPI.h> instead, which exposes
 * this base class and all driver classes (ILI9341, ILI9488, ST7789).
 *
 * Copyright (c) DIYables.io
 * Licensed under the MIT License.
 */

#ifndef DIYABLES_TFT_SPI_BASE_H
#define DIYABLES_TFT_SPI_BASE_H

#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include "DIYables_TouchScreen.h"
#include "DIYables_XPT2046.h"

// Highest SPI clock the library will pick on its own.
// The Arduino UNO Q (Zephyr core) drives the panel through Zephyr's STM32 SPI
// driver, and the usual module wiring does not survive the 24-40 MHz the
// drivers ask for: the init sequence arrives shifted and the panel stays dark.
// 16 MHz is the fastest clock that reads back clean on that board, so the
// per-driver default is capped there. An explicit begin(freq) is never capped.
#if defined(ARDUINO_ARCH_ZEPHYR)
  #define DIYABLES_TFT_SPI_MAX_DEFAULT_FREQ 16000000UL
#endif

// Size of the scratch buffer used to batch pixel bytes into a single SPI
// block transfer. Sending one byte at a time is fine on AVR, but on cores
// where SPI goes through a driver (the Zephyr core on the UNO Q turns every
// byte into a complete SPI transaction) it is orders of magnitude slower.
// Kept a multiple of 6 so it divides evenly for both RGB565 (2 bytes/pixel)
// and RGB666 (3 bytes/pixel).
#if defined(__AVR__)
  #define DIYABLES_TFT_SPI_CHUNK 30
#else
  #define DIYABLES_TFT_SPI_CHUNK 96
#endif

class DIYables_TFT_SPI : public Adafruit_GFX {
public:
  // Hardware SPI constructor (pass &SPI1, &HSPI, etc. for alternate SPI bus)
  DIYables_TFT_SPI(uint16_t width, uint16_t height,
                   int8_t cs_pin, int8_t dc_pin,
                   int8_t rst_pin = -1, SPIClass *spi_bus = &SPI);

  virtual ~DIYables_TFT_SPI();

  void begin(uint32_t freq = 0);

  // Adafruit_GFX overrides
  void drawPixel(int16_t x, int16_t y, uint16_t color) override;
  void writePixel(int16_t x, int16_t y, uint16_t color) override;
  void fillScreen(uint16_t color) override;
  void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) override;
  void writeFillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) override;
  void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) override;
  void writeFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) override;
  void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) override;
  void writeFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) override;
  void invertDisplay(bool i) override;
  void setRotation(uint8_t r) override;

  // Low-level methods (call within startWrite/endWrite block)
  void setAddrWindow(int16_t x0, int16_t y0, int16_t x1, int16_t y1);
  void pushColors(uint16_t *data, uint32_t len);

  // Optimized bitmap drawing
  void drawRGBBitmap(int16_t x, int16_t y, const uint16_t bitmap[], int16_t w, int16_t h);
  void drawRGBBitmap(int16_t x, int16_t y, uint16_t *bitmap, int16_t w, int16_t h);

  // SPI transaction control (public for advanced use)
  void startWrite();
  void endWrite();

  // Touch screen support.
  // Two backends are supported; pick the one that matches your module:
  //  1) initTouch(...)    - bare 4-wire resistive panel (XP/YP/XM/YM pins).
  //                         Uses analogRead(); no extra controller IC.
  //  2) initTouchSPI(...) - XPT2046 / ADS7843 SPI touch controller
  //                         (modules with T_CS / T_CLK / T_DIN / T_DO / T_IRQ).
  // After init, use setTouchCalibration(), getTouch() and readTouchRaw()
  // identically regardless of which backend is active.
  void initTouch(uint8_t xp, uint8_t yp, uint8_t xm, uint8_t ym, uint16_t rx = 300);
  void initTouchSPI(uint8_t cs_pin, int8_t irq_pin = -1, SPIClass *spi_bus = nullptr);
  void setTouchCalibration(int min_x, int max_x, int min_y, int max_y);
  // Flip the touch axes without touching the calibration numbers.
  // Useful when two TFT modules share the same calibration but the
  // touch sheet is mounted with opposite polarity on one of them.
  void setTouchInvert(bool invert_x, bool invert_y);
  void setTouchInvertX(bool invert);
  void setTouchInvertY(bool invert);
  // Mirror the touch point in SCREEN space, i.e. after the rotation
  // mapping. Use these when a press on the left is reported on the right
  // (X), or a press at the top is reported at the bottom (Y).
  // Difference from setTouchInvertX()/setTouchInvertY() above: those flip
  // the raw panel axes BEFORE rotation is applied, so on a rotated screen
  // "X" there may end up mirroring what the user sees as Y. These two
  // always match the screen axes, whatever setRotation() is set to.
  void setTouchInvertScreenX(bool invert);
  void setTouchInvertScreenY(bool invert);
  void setADCResolution(uint8_t bits);
  void readTouchRaw(int &x, int &y, int &z);
  bool getTouch(int &screenX, int &screenY);

  // Backlight (LED pin) control.
  // Call initBacklight() once with the GPIO connected to the module's LED/BL
  // pin. Use a PWM-capable pin if you want analog dimming via setBacklight().
  void initBacklight(int8_t led_pin);
  void backlightOn();
  void backlightOff();
  void setBacklight(uint8_t brightness); // 0 = off, 255 = full (PWM)

  static uint16_t colorRGB(uint8_t r, uint8_t g, uint8_t b) {
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
  }

protected:
  int8_t  _cs, _dc, _rst;
  uint32_t _freq;
  SPIClass *_spi;

  void spiWrite(uint8_t val);
  // Push len bytes in one bus transfer. The buffer is clobbered with the
  // received data on most cores, so callers must refill it between calls.
  void spiWriteBlock(uint8_t *buf, size_t len);
  void writeCommand(uint8_t cmd);
  void writeData8(uint8_t data);
  void writeData16(uint16_t data);

  // Pure virtual — implemented by each driver
  virtual void    initDriver() = 0;
  virtual uint8_t getRotationData(uint8_t r) = 0;
  virtual uint32_t getDefaultFreq() = 0;

  // Default touch-axis inversion flags. Driver subclasses can override
  // these in their constructor so that the same calibration values map
  // correctly on panels whose touch sheet is mounted with opposite
  // polarity (e.g. the landscape-native "ST7789" clones).
  bool _touch_invert_x = false;
  bool _touch_invert_y = false;

  // Default RGB565 (16-bit, 2 bytes/pixel) — override for other formats (e.g. ILI9488 RGB666)
  virtual void    writeColor(uint16_t color);
  virtual void    fillColor(uint16_t color, uint32_t count);
  virtual void    pushColorData(uint16_t *data, uint32_t len);
  virtual void    pushColorDataPGM(const uint16_t *data, uint32_t len);

private:
  DIYables_TouchScreen *_ts = nullptr;     // 4-wire resistive backend
  DIYables_XPT2046     *_ts_spi = nullptr; // XPT2046 SPI backend
  int _touch_min_x = 150, _touch_max_x = 880;
  int _touch_min_y = 120, _touch_max_y = 900;
  bool _touch_screen_invert_x = false;
  bool _touch_screen_invert_y = false;
  int8_t _led_pin = -1;
};

#endif // DIYABLES_TFT_SPI_BASE_H
