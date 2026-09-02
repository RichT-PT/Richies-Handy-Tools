/*
  Touch Draw Lines Example
  -------------------------
  Draws lines on the screen following the pen.

  - Touch and drag on the screen to draw.
  - Lift the pen to stop drawing.
  - Touch again to start a new line from the last point.

  NOTE:
  Run the TouchCalibration example and update setTouchCalibration() below
  if the touch coordinates are inaccurate.

  Created by DIYables

  This example code is in the public domain

  Product page: https://diyables.io
*/

// =============================================
// Single include brings in the base class plus all driver classes.
// =============================================
#include <DIYables_TFT_SPI.h>

// =============================================
// Wiring (Arduino Uno / Nano)
// =============================================
// TFT pins (always required)
//   TFT module     Arduino Uno / Nano
//   ------------   ---------------------------------
//   VCC        ->  5V
//   GND        ->  GND
//   CS         ->  D10  (TFT_CS_PIN)
//   RESET      ->  D9   (TFT_RST_PIN)
//   DC / RS    ->  D8   (TFT_DC_PIN)
//   SDI / MOSI ->  D11  (hardware SPI MOSI)
//   SCK        ->  D13  (hardware SPI SCK)
//   SDO / MISO ->  D12  (only needed when reading from display)
//   LED        ->  3.3V (or any GPIO via initBacklight)
//
// XPT2046 / HR2046 / ADS7843 SPI touch controller
// (modules with pins: T_CS, T_CLK, T_DIN, T_DO, T_IRQ)
//   Touch pin      Arduino Uno / Nano
//   ------------   ---------------------------------
//   T_CS       ->  D2   (TOUCH_CS_PIN)
//   T_IRQ      ->  D2   (TOUCH_IRQ_PIN, optional - use -1 to skip)
//   T_CLK      ->  D13  (shared with display SCK)
//   T_DIN      ->  D11  (shared with display MOSI)
//   T_DO       ->  D12  (shared with display MISO)
// =============================================

// =============================================
// SPI pin definitions (adjust for your board)
// =============================================
#define TFT_CS_PIN   10
#define TFT_DC_PIN    8
#define TFT_RST_PIN   9

// Panel resolution in native (portrait) orientation - change to match your module
#define TFT_WIDTH   240
#define TFT_HEIGHT  320

// =============================================
// Touch pin definitions (XPT2046 / HR2046 SPI touch controller)
// =============================================
#define TOUCH_CS_PIN    2   // T_CS  (any GPIO)
#define TOUCH_IRQ_PIN   -1  // T_IRQ (any GPIO, or -1 if not connected)
// =============================================

// =============================================
// Calibration values.
// Run the TouchCalibration example and update these if touch is inaccurate.
// Typical raw ranges:
//   - XPT2046 / HR2046 : ~200..3900  (default below)
//   - 4-wire resistive : ~100..900
// =============================================
#define TOUCH_LEFT_X   300
#define TOUCH_RIGHT_X  3700
#define TOUCH_TOP_Y    300
#define TOUCH_BOT_Y    3700

// =============================================
// Create display object (uncomment matching driver)
// =============================================
// DIYables_ILI9341_SPI TFT_display(TFT_WIDTH, TFT_HEIGHT, TFT_CS_PIN, TFT_DC_PIN, TFT_RST_PIN);
// DIYables_ILI9488_SPI TFT_display(TFT_WIDTH, TFT_HEIGHT, TFT_CS_PIN, TFT_DC_PIN, TFT_RST_PIN);
DIYables_ST7789_SPI  TFT_display(TFT_WIDTH, TFT_HEIGHT, TFT_CS_PIN, TFT_DC_PIN, TFT_RST_PIN);

#define RED   DIYables_TFT_SPI::colorRGB(255,   0,   0)
#define WHITE DIYables_TFT_SPI::colorRGB(255, 255, 255)

#define PEN_RADIUS 3

void setup() {
  TFT_display.begin();
  TFT_display.setRotation(0);
  TFT_display.fillScreen(WHITE);

  TFT_display.initTouchSPI(TOUCH_CS_PIN, TOUCH_IRQ_PIN);

  // If touch X is mirrored on your board, uncomment:
  //TFT_display.setTouchInvertX(true);
  // If touch Y is mirrored on your board, uncomment:
  //TFT_display.setTouchInvertY(false);

  // Some panels report the touch point on the opposite side of the screen:
  // press on the left and it lands on the right, or press at the top and it
  // lands at the bottom. These flip the point in SCREEN coordinates, so X is
  // always the screen's X no matter what setRotation() is set to.
  // Uncomment the one that matches what you see:
  //TFT_display.setTouchInvertScreenX(true); // left <-> right
  //TFT_display.setTouchInvertScreenY(true); // top  <-> bottom

  TFT_display.setTouchCalibration(TOUCH_LEFT_X, TOUCH_RIGHT_X,
                                   TOUCH_TOP_Y,  TOUCH_BOT_Y);
}

void loop() {
  int x, y;
  if (TFT_display.getTouch(x, y)) {
    TFT_display.fillCircle(x, y, PEN_RADIUS, RED);
  }
}
