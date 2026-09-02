/*
  Touch Screen Calibration Example
  ---------------------------------
  This example measures the raw touch coordinates at all four screen corners
  and prints ready-to-use calibration values to the Serial Monitor.

  It uses readTouchRaw() directly — it does NOT rely on getTouch() or any
  existing calibration values, so it works even when touch is completely broken.

  INSTRUCTIONS:

  1. Upload this sketch to your board.

  2. Open the Serial Monitor (Ctrl+Shift+M) and set baud rate to 9600.

  3. The screen shows a blinking red dot in each corner, numbered 1–4:
       1 = Top-left
       2 = Top-right
       3 = Bottom-right
       4 = Bottom-left

  4. Press and HOLD firmly on the blinking dot.
     Keep holding until the Serial Monitor prints "Captured!" for that corner.

  5. Release, then wait for the next dot to appear and repeat.

  6. After all 4 corners, the Serial Monitor prints the calibration values and
     a ready-to-use setTouchCalibration() call. Copy it into your sketch.

  NOTE: While waiting, the Serial Monitor continuously prints the live raw
  Z/X/Y readings so you can confirm that touch is being detected.

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
// Create display object (uncomment matching driver)
// =============================================
// DIYables_ILI9341_SPI TFT_display(TFT_WIDTH, TFT_HEIGHT, TFT_CS_PIN, TFT_DC_PIN, TFT_RST_PIN);
// DIYables_ILI9488_SPI TFT_display(TFT_WIDTH, TFT_HEIGHT, TFT_CS_PIN, TFT_DC_PIN, TFT_RST_PIN);
DIYables_ST7789_SPI  TFT_display(TFT_WIDTH, TFT_HEIGHT, TFT_CS_PIN, TFT_DC_PIN, TFT_RST_PIN);

// Minimum pressure to count as a valid touch.
#define TOUCH_Z_MIN    10

// How many consecutive valid samples required before a corner is accepted.
#define SAMPLES_NEEDED 10

// Delay between samples (ms).
#define SAMPLE_DELAY_MS 30

#define DOT_RADIUS 12

#define BLACK DIYables_TFT_SPI::colorRGB(  0,   0,   0)
#define WHITE DIYables_TFT_SPI::colorRGB(255, 255, 255)
#define RED   DIYables_TFT_SPI::colorRGB(255,   0,   0)

// Corner pixel positions — filled in setup() once display size is known.
// Order: 0=top-left, 1=top-right, 2=bottom-right, 3=bottom-left
int cx[4], cy[4];

// Captured averaged raw values per corner.
int cap_x[4], cap_y[4];

// -----------------------------------------------------------------------

void drawDot(int corner, bool on) {
  uint16_t color = on ? RED : WHITE;
  TFT_display.fillCircle(cx[corner], cy[corner], DOT_RADIUS, color);
  TFT_display.setTextSize(2);
  TFT_display.setTextColor(BLACK, color);
  TFT_display.setCursor(cx[corner] - 6, cy[corner] - 8);
  TFT_display.print(corner + 1);
}

void captureCorner(int corner) {
  const char* names[] = { "Top-left", "Top-right", "Bottom-right", "Bottom-left" };

  Serial.println();
  Serial.print("Corner "); Serial.print(corner + 1);
  Serial.print(" ("); Serial.print(names[corner]); Serial.println(")");
  Serial.println("  Press and HOLD firmly on the blinking dot.");
  Serial.println("  Keep holding until you see 'Captured!'");

  unsigned long lastBlink = 0;
  unsigned long lastPrint = 0;
  bool dotOn = false;

  int goodSamples = 0;
  long sumX = 0, sumY = 0;

  while (true) {
    // Blink the dot
    if (millis() - lastBlink > 400) {
      lastBlink = millis();
      dotOn = !dotOn;
      drawDot(corner, dotOn);
    }

    int raw_x, raw_y, z;
    TFT_display.readTouchRaw(raw_x, raw_y, z);

    // Print live readings every 500 ms
    if (millis() - lastPrint > 500) {
      lastPrint = millis();
      Serial.print("  Z="); Serial.print(z);
      Serial.print("  X="); Serial.print(raw_x);
      Serial.print("  Y="); Serial.println(raw_y);
    }

    if (z >= TOUCH_Z_MIN) {
      sumX += raw_x;
      sumY += raw_y;
      goodSamples++;

      if (goodSamples >= SAMPLES_NEEDED) {
        cap_x[corner] = sumX / goodSamples;
        cap_y[corner] = sumY / goodSamples;
        Serial.print("  Captured!  raw_x="); Serial.print(cap_x[corner]);
        Serial.print("  raw_y="); Serial.println(cap_y[corner]);
        drawDot(corner, false);
        delay(500);
        return;
      }
    } else {
      goodSamples = 0;
      sumX = 0;
      sumY = 0;
    }

    delay(SAMPLE_DELAY_MS);
  }
}

// -----------------------------------------------------------------------

void setup() {
  Serial.begin(9600);

  TFT_display.begin();
  TFT_display.setRotation(0); // Always calibrate in rotation 0
  TFT_display.fillScreen(WHITE);

  TFT_display.initTouchSPI(TOUCH_CS_PIN, TOUCH_IRQ_PIN);

  // If touch X is mirrored on your board, uncomment the line below
  // BEFORE calibrating (so the printed values match your panel):
  //TFT_display.setTouchInvertX(true);
  // If touch Y is mirrored on your board, uncomment:
  //TFT_display.setTouchInvertY(false);

  int w = TFT_display.width();
  int h = TFT_display.height();
  int m = DOT_RADIUS + 4;

  cx[0] = m;     cy[0] = m;
  cx[1] = w - m; cy[1] = m;
  cx[2] = w - m; cy[2] = h - m;
  cx[3] = m;     cy[3] = h - m;

  Serial.println("=== Touch Calibration ===");

  for (int i = 0; i < 4; i++) {
    captureCorner(i);
  }

  // Derive calibration values from the four corners
  int min_x = (cap_x[0] + cap_x[3]) / 2;  // left edge
  int max_x = (cap_x[1] + cap_x[2]) / 2;  // right edge
  int min_y = (cap_y[0] + cap_y[1]) / 2;  // top edge
  int max_y = (cap_y[2] + cap_y[3]) / 2;  // bottom edge

  Serial.println();
  Serial.println("=== Calibration Results ===");
  Serial.print("  Left  X (min_x): "); Serial.println(min_x);
  Serial.print("  Right X (max_x): "); Serial.println(max_x);
  Serial.print("  Top   Y (min_y): "); Serial.println(min_y);
  Serial.print("  Bot   Y (max_y): "); Serial.println(max_y);
  Serial.println();
  Serial.println("Copy this line into your sketch:");
  Serial.print("  TFT_display.setTouchCalibration(");
  Serial.print(min_x); Serial.print(", ");
  Serial.print(max_x); Serial.print(", ");
  Serial.print(min_y); Serial.print(", ");
  Serial.print(max_y); Serial.println(");");

  TFT_display.fillScreen(WHITE);
  TFT_display.setTextColor(BLACK);
  TFT_display.setTextSize(2);
  TFT_display.setCursor(10, 10);
  TFT_display.println("Done! Check");
  TFT_display.setCursor(10, 35);
  TFT_display.println("Serial Monitor");
}

void loop() {}
