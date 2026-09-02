/*
 * DIYables_TFT_SPI.cpp - Base class implementation for SPI TFT displays
 *
 * Copyright (c) DIYables.io
 * Licensed under the MIT License.
 */

#include "DIYables_TFT_SPI_Base.h"

// Hardware SPI constructor
DIYables_TFT_SPI::DIYables_TFT_SPI(uint16_t width, uint16_t height,
                                   int8_t cs_pin, int8_t dc_pin,
                                   int8_t rst_pin, SPIClass *spi_bus)
  : Adafruit_GFX(width, height),
    _cs(cs_pin), _dc(dc_pin), _rst(rst_pin),
    _freq(0), _spi(spi_bus) {}

DIYables_TFT_SPI::~DIYables_TFT_SPI() {
  delete _ts;
  delete _ts_spi;
}

void DIYables_TFT_SPI::begin(uint32_t freq) {
  _freq = freq ? freq : getDefaultFreq();
#ifdef DIYABLES_TFT_SPI_MAX_DEFAULT_FREQ
  if (!freq && _freq > DIYABLES_TFT_SPI_MAX_DEFAULT_FREQ) {
    _freq = DIYABLES_TFT_SPI_MAX_DEFAULT_FREQ;
  }
#endif

  // Drive CS / DC to a known state BEFORE touching the SPI bus.
  // After pinMode(OUTPUT) the pin's level is whatever was last in the
  // output register (often LOW). If CS stays LOW while _spi->begin()
  // toggles SCK, the display can latch garbage as commands and end up
  // in an undefined state (symptom: blank/white screen).
  if (_cs >= 0) {
    digitalWrite(_cs, HIGH); // deasserted
    pinMode(_cs, OUTPUT);
    digitalWrite(_cs, HIGH);
  }
  digitalWrite(_dc, HIGH);   // data mode by default
  pinMode(_dc, OUTPUT);
  digitalWrite(_dc, HIGH);

  // Hold the display in reset while the SPI peripheral initializes,
  // so it cannot latch any spurious clocks.
  if (_rst >= 0) {
    digitalWrite(_rst, LOW);
    pinMode(_rst, OUTPUT);
    digitalWrite(_rst, LOW);
  }

  _spi->begin();

  // Re-apply the pin modes AFTER the SPI peripheral is up.
  // Some cores mux the bus pins when SPI starts, and a control pin that the
  // board maps to an SPI alternate function is taken over at that moment.
  // On the Arduino UNO Q, for example, D10 is SPI2's hardware NSS: after
  // SPI.begin() it is no longer a GPIO, so digitalWrite() on the CS pin
  // stops reaching the pad and the display never sees its chip select.
  // Claiming the pins again here hands them back to the GPIO controller.
  // Harmless on cores that do not mux (AVR, ESP32, SAMD, ...).
  // The display is still held in reset, so any glitch here is not seen.
  if (_cs >= 0) {
    pinMode(_cs, OUTPUT);
    digitalWrite(_cs, HIGH);
  }
  pinMode(_dc, OUTPUT);
  digitalWrite(_dc, HIGH);
  if (_rst >= 0) {
    pinMode(_rst, OUTPUT);
    digitalWrite(_rst, LOW);
  }

  // Hardware reset pulse (datasheet: low >= 10us, then >= 120ms before commands)
  if (_rst >= 0) {
    digitalWrite(_rst, HIGH);
    delay(10);
    digitalWrite(_rst, LOW);
    delay(20);
    digitalWrite(_rst, HIGH);
    delay(150);
  }

  startWrite();
  initDriver();
  endWrite();
}

// ---- SPI primitives ----

void DIYables_TFT_SPI::spiWrite(uint8_t val) {
  _spi->transfer(val);
}

void DIYables_TFT_SPI::spiWriteBlock(uint8_t *buf, size_t len) {
#if defined(ARDUINO_ARCH_SAM)
  // On the Due, SPI.transfer(buf, len) drives its own hardware SS pin,
  // which would fight the CS pin this library manages. Keep the byte loop.
  for (size_t i = 0; i < len; i++) _spi->transfer(buf[i]);
#elif defined(ESP8266)
  _spi->writeBytes(buf, len); // write-only: leaves the buffer untouched
#else
  _spi->transfer(buf, len);
#endif
}

void DIYables_TFT_SPI::startWrite() {
  _spi->beginTransaction(SPISettings(_freq, MSBFIRST, SPI_MODE0));
  if (_cs >= 0) digitalWrite(_cs, LOW);
}

void DIYables_TFT_SPI::endWrite() {
  if (_cs >= 0) digitalWrite(_cs, HIGH);
  _spi->endTransaction();
}

void DIYables_TFT_SPI::writeCommand(uint8_t cmd) {
  digitalWrite(_dc, LOW);
  spiWrite(cmd);
  digitalWrite(_dc, HIGH);
}

void DIYables_TFT_SPI::writeData8(uint8_t data) {
  spiWrite(data);
}

void DIYables_TFT_SPI::writeData16(uint16_t data) {
  spiWrite(data >> 8);
  spiWrite(data & 0xFF);
}

// ---- Address window ----

void DIYables_TFT_SPI::setAddrWindow(int16_t x0, int16_t y0,
                                       int16_t x1, int16_t y1) {
  writeCommand(0x2A); // Column Address Set
  writeData16(x0);
  writeData16(x1);

  writeCommand(0x2B); // Row Address Set
  writeData16(y0);
  writeData16(y1);

  writeCommand(0x2C); // Memory Write
}

// ---- Adafruit_GFX overrides ----

void DIYables_TFT_SPI::drawPixel(int16_t x, int16_t y, uint16_t color) {
  if (x < 0 || x >= width() || y < 0 || y >= height()) return;
  startWrite();
  setAddrWindow(x, y, x, y);
  writeColor(color);
  endWrite();
}

void DIYables_TFT_SPI::writePixel(int16_t x, int16_t y, uint16_t color) {
  if (x < 0 || x >= width() || y < 0 || y >= height()) return;
  setAddrWindow(x, y, x, y);
  writeColor(color);
}

void DIYables_TFT_SPI::fillScreen(uint16_t color) {
  fillRect(0, 0, width(), height(), color);
}

void DIYables_TFT_SPI::fillRect(int16_t x, int16_t y,
                                  int16_t w, int16_t h, uint16_t color) {
  if (x >= width() || y >= height() || w <= 0 || h <= 0) return;
  if (x < 0) { w += x; x = 0; }
  if (y < 0) { h += y; y = 0; }
  if (x + w > width())  w = width()  - x;
  if (y + h > height()) h = height() - y;

  startWrite();
  setAddrWindow(x, y, x + w - 1, y + h - 1);
  fillColor(color, (uint32_t)w * h);
  endWrite();
}

void DIYables_TFT_SPI::writeFillRect(int16_t x, int16_t y,
                                        int16_t w, int16_t h, uint16_t color) {
  if (x >= width() || y >= height() || w <= 0 || h <= 0) return;
  if (x < 0) { w += x; x = 0; }
  if (y < 0) { h += y; y = 0; }
  if (x + w > width())  w = width()  - x;
  if (y + h > height()) h = height() - y;

  setAddrWindow(x, y, x + w - 1, y + h - 1);
  fillColor(color, (uint32_t)w * h);
}

void DIYables_TFT_SPI::drawFastHLine(int16_t x, int16_t y,
                                       int16_t w, uint16_t color) {
  fillRect(x, y, w, 1, color);
}

void DIYables_TFT_SPI::writeFastHLine(int16_t x, int16_t y,
                                        int16_t w, uint16_t color) {
  writeFillRect(x, y, w, 1, color);
}

void DIYables_TFT_SPI::drawFastVLine(int16_t x, int16_t y,
                                       int16_t h, uint16_t color) {
  fillRect(x, y, 1, h, color);
}

void DIYables_TFT_SPI::writeFastVLine(int16_t x, int16_t y,
                                        int16_t h, uint16_t color) {
  writeFillRect(x, y, 1, h, color);
}

void DIYables_TFT_SPI::setRotation(uint8_t r) {
  Adafruit_GFX::setRotation(r);
  startWrite();
  writeCommand(0x36);
  writeData8(getRotationData(r));
  endWrite();
}

void DIYables_TFT_SPI::invertDisplay(bool i) {
  startWrite();
  writeCommand(i ? 0x21 : 0x20);
  endWrite();
}

// ---- Pixel data push ----

void DIYables_TFT_SPI::pushColors(uint16_t *data, uint32_t len) {
  pushColorData(data, len);
}

void DIYables_TFT_SPI::drawRGBBitmap(int16_t x, int16_t y,
                                       const uint16_t bitmap[],
                                       int16_t w, int16_t h) {
  if (x >= width() || y >= height() || (x + w) <= 0 || (y + h) <= 0) return;

  if (x >= 0 && y >= 0 && (x + w) <= width() && (y + h) <= height()) {
    startWrite();
    setAddrWindow(x, y, x + w - 1, y + h - 1);
    pushColorDataPGM(bitmap, (uint32_t)w * h);
    endWrite();
  } else {
    for (int16_t j = 0; j < h; j++) {
      for (int16_t i = 0; i < w; i++) {
        int16_t px = x + i, py = y + j;
        if (px >= 0 && px < width() && py >= 0 && py < height()) {
          drawPixel(px, py, pgm_read_word(&bitmap[j * w + i]));
        }
      }
    }
  }
}

void DIYables_TFT_SPI::drawRGBBitmap(int16_t x, int16_t y,
                                       uint16_t *bitmap,
                                       int16_t w, int16_t h) {
  if (x >= width() || y >= height() || (x + w) <= 0 || (y + h) <= 0) return;

  if (x >= 0 && y >= 0 && (x + w) <= width() && (y + h) <= height()) {
    startWrite();
    setAddrWindow(x, y, x + w - 1, y + h - 1);
    pushColorData(bitmap, (uint32_t)w * h);
    endWrite();
  } else {
    for (int16_t j = 0; j < h; j++) {
      for (int16_t i = 0; i < w; i++) {
        int16_t px = x + i, py = y + j;
        if (px >= 0 && px < width() && py >= 0 && py < height()) {
          drawPixel(px, py, bitmap[j * w + i]);
        }
      }
    }
  }
}

// ---- Default RGB565 pixel methods (16-bit, 2 bytes/pixel) ----

void DIYables_TFT_SPI::writeColor(uint16_t color) {
  spiWrite(color >> 8);
  spiWrite(color & 0xFF);
}

void DIYables_TFT_SPI::fillColor(uint16_t color, uint32_t count) {
  uint8_t hi = color >> 8;
  uint8_t lo = color & 0xFF;
  uint8_t buf[DIYABLES_TFT_SPI_CHUNK];
  const uint16_t max_pixels = DIYABLES_TFT_SPI_CHUNK / 2;

  while (count) {
    uint16_t n = (count > max_pixels) ? max_pixels : (uint16_t)count;
    // Refilled every pass: the block transfer overwrites the buffer.
    for (uint16_t i = 0; i < n; i++) {
      buf[i * 2]     = hi;
      buf[i * 2 + 1] = lo;
    }
    spiWriteBlock(buf, (size_t)n * 2);
    count -= n;
  }
}

void DIYables_TFT_SPI::pushColorData(uint16_t *data, uint32_t len) {
  uint8_t buf[DIYABLES_TFT_SPI_CHUNK];
  const uint16_t max_pixels = DIYABLES_TFT_SPI_CHUNK / 2;

  while (len) {
    uint16_t n = (len > max_pixels) ? max_pixels : (uint16_t)len;
    for (uint16_t i = 0; i < n; i++) {
      uint16_t c = *data++;
      buf[i * 2]     = c >> 8;
      buf[i * 2 + 1] = c & 0xFF;
    }
    spiWriteBlock(buf, (size_t)n * 2);
    len -= n;
  }
}

void DIYables_TFT_SPI::pushColorDataPGM(const uint16_t *data, uint32_t len) {
  uint8_t buf[DIYABLES_TFT_SPI_CHUNK];
  const uint16_t max_pixels = DIYABLES_TFT_SPI_CHUNK / 2;

  while (len) {
    uint16_t n = (len > max_pixels) ? max_pixels : (uint16_t)len;
    for (uint16_t i = 0; i < n; i++) {
      uint16_t c = pgm_read_word(data++);
      buf[i * 2]     = c >> 8;
      buf[i * 2 + 1] = c & 0xFF;
    }
    spiWriteBlock(buf, (size_t)n * 2);
    len -= n;
  }
}

// ---- Touch screen support ----

void DIYables_TFT_SPI::initTouch(uint8_t xp, uint8_t yp, uint8_t xm, uint8_t ym,
                                   uint16_t rx) {
  delete _ts;
  delete _ts_spi;
  _ts_spi = nullptr;
  _ts = new DIYables_TouchScreen(xp, yp, xm, ym, rx);
}

void DIYables_TFT_SPI::initTouchSPI(uint8_t cs_pin, int8_t irq_pin,
                                      SPIClass *spi_bus) {
  delete _ts;
  delete _ts_spi;
  _ts = nullptr;
  // Default to the same SPI bus the display is using.
  if (spi_bus == nullptr) spi_bus = _spi;
  _ts_spi = new DIYables_XPT2046(cs_pin, irq_pin, spi_bus);
  _ts_spi->begin();
}

void DIYables_TFT_SPI::setTouchCalibration(int min_x, int max_x,
                                             int min_y, int max_y) {
  _touch_min_x = min_x;
  _touch_max_x = max_x;
  _touch_min_y = min_y;
  _touch_max_y = max_y;
}

void DIYables_TFT_SPI::setTouchInvert(bool invert_x, bool invert_y) {
  _touch_invert_x = invert_x;
  _touch_invert_y = invert_y;
}

void DIYables_TFT_SPI::setTouchInvertX(bool invert) {
  _touch_invert_x = invert;
}

void DIYables_TFT_SPI::setTouchInvertY(bool invert) {
  _touch_invert_y = invert;
}

void DIYables_TFT_SPI::setTouchInvertScreenX(bool invert) {
  _touch_screen_invert_x = invert;
}

void DIYables_TFT_SPI::setTouchInvertScreenY(bool invert) {
  _touch_screen_invert_y = invert;
}

void DIYables_TFT_SPI::setADCResolution(uint8_t bits) {
  if (_ts) _ts->setADCResolution(bits);
}

void DIYables_TFT_SPI::readTouchRaw(int &x, int &y, int &z) {
  if (_ts) {
    // The 4-wire resistive touch panel uses its own dedicated GPIOs that do
    // not overlap with the SPI bus, so no SPI transaction handling is needed.
    TSPoint tp = _ts->getPoint();
    x = tp.x;
    y = tp.y;
    z = tp.z;
    return;
  }
  if (_ts_spi) {
    // XPT2046 manages its own SPI transaction internally.
    TSPoint tp = _ts_spi->getPoint();
    x = tp.x;
    y = tp.y;
    z = tp.z;
    return;
  }
  x = y = z = 0;
}

bool DIYables_TFT_SPI::getTouch(int &screenX, int &screenY) {
  int raw_x, raw_y, z;
  int16_t threshold;
  if (_ts) {
    threshold = _ts->pressureThreshhold;
  } else if (_ts_spi) {
    threshold = _ts_spi->pressureThreshhold;
  } else {
    return false;
  }
  readTouchRaw(raw_x, raw_y, z);

  if (z > threshold) {
    // Apply per-driver axis inversion before mapping. This lets two
    // modules with the touch sheet mounted in opposite polarities share
    // the same calibration values.
    if (_touch_invert_x) raw_x = _touch_min_x + _touch_max_x - raw_x;
    if (_touch_invert_y) raw_y = _touch_min_y + _touch_max_y - raw_y;

    // Calibration values are for portrait orientation (rotation 0).
    // raw_y is always the long dimension of the touch panel.
    // Map to current pixel orientation, matching MCUFRIEND_kbv convention.
    switch (getRotation()) {
      case 0:
        screenX = map(raw_x, _touch_max_x, _touch_min_x, 0, width()  - 1);
        screenY = map(raw_y, _touch_max_y, _touch_min_y, 0, height() - 1);
        break;
      case 1:
        screenX = map(raw_y, _touch_min_y, _touch_max_y, 0, width()  - 1);
        screenY = map(raw_x, _touch_min_x, _touch_max_x, 0, height() - 1);
        break;
      case 2:
        screenX = map(raw_x, _touch_min_x, _touch_max_x, 0, width()  - 1);
        screenY = map(raw_y, _touch_min_y, _touch_max_y, 0, height() - 1);
        break;
      default: // 3
        screenX = map(raw_y, _touch_max_y, _touch_min_y, 0, width()  - 1);
        screenY = map(raw_x, _touch_max_x, _touch_min_x, 0, height() - 1);
        break;
    }
    screenX = constrain(screenX, 0, width()  - 1);
    screenY = constrain(screenY, 0, height() - 1);

    // Mirror in screen space, i.e. after the rotation mapping above, so
    // these always flip the axis the user actually sees.
    if (_touch_screen_invert_x) screenX = width()  - 1 - screenX;
    if (_touch_screen_invert_y) screenY = height() - 1 - screenY;

    return true;
  }
  return false;
}

// ---- Backlight (LED pin) control ----

void DIYables_TFT_SPI::initBacklight(int8_t led_pin) {
  _led_pin = led_pin;
  if (_led_pin >= 0) {
    pinMode(_led_pin, OUTPUT);
    digitalWrite(_led_pin, HIGH); // default: backlight on
  }
}

void DIYables_TFT_SPI::backlightOn() {
  if (_led_pin >= 0) digitalWrite(_led_pin, HIGH);
}

void DIYables_TFT_SPI::backlightOff() {
  if (_led_pin >= 0) digitalWrite(_led_pin, LOW);
}

void DIYables_TFT_SPI::setBacklight(uint8_t brightness) {
  if (_led_pin < 0) return;
  if (brightness == 0) {
    digitalWrite(_led_pin, LOW);
  } else if (brightness == 255) {
    digitalWrite(_led_pin, HIGH);
  } else {
    analogWrite(_led_pin, brightness);
  }
}
