/*
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
// ---------------------------------------------
//   TFT module     Arduino Uno / Nano
//   ------------   ---------------------------------
//   VCC        ->  5V
//   GND        ->  GND
//   CS         ->  D10  (TFT_CS_PIN)
//   RESET      ->  D9   (TFT_RST_PIN)
//   DC / RS    ->  D8   (TFT_DC_PIN)
//   SDI / MOSI ->  D11  (hardware SPI MOSI)
//   SCK        ->  D13  (hardware SPI SCK)
//   LED        ->  3.3V (or any GPIO via initBacklight)
//   SDO / MISO ->  D12  (only needed when reading from display)
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
// MOSI and SCK use default hardware SPI pins

// =============================================
// Create display object (uncomment matching driver)
// =============================================
// DIYables_ILI9341_SPI TFT_display(TFT_WIDTH, TFT_HEIGHT, TFT_CS_PIN, TFT_DC_PIN, TFT_RST_PIN);
// DIYables_ILI9488_SPI TFT_display(TFT_WIDTH, TFT_HEIGHT, TFT_CS_PIN, TFT_DC_PIN, TFT_RST_PIN);
DIYables_ST7789_SPI TFT_display(TFT_WIDTH, TFT_HEIGHT, TFT_CS_PIN, TFT_DC_PIN, TFT_RST_PIN);

#define BLACK     DIYables_TFT_SPI::colorRGB(0, 0, 0)
#define BLUE      DIYables_TFT_SPI::colorRGB(0, 0, 255)
#define RED       DIYables_TFT_SPI::colorRGB(255, 0, 0)
#define GREEN     DIYables_TFT_SPI::colorRGB(0, 255, 0)
#define ORANGE    DIYables_TFT_SPI::colorRGB(255, 165, 0)
#define PINK      DIYables_TFT_SPI::colorRGB(255, 192, 203)
#define VIOLET    DIYables_TFT_SPI::colorRGB(148, 0, 211)
#define TURQUOISE DIYables_TFT_SPI::colorRGB(64, 224, 208)
#define WHITE     DIYables_TFT_SPI::colorRGB(255, 255, 255)

// Helper to draw a filled diamond
void fillDiamond(int cx, int cy, int h, int v, uint16_t color) {
  int x0 = cx, y0 = cy - v;
  int x1 = cx + h, y1 = cy;
  int x2 = cx, y2 = cy + v;
  int x3 = cx - h, y3 = cy;
  TFT_display.fillTriangle(x0, y0, x1, y1, x2, y2, color);
  TFT_display.fillTriangle(x0, y0, x2, y2, x3, y3, color);
}

void setup() {
  TFT_display.begin();
  TFT_display.setRotation(1); // Landscape
}

void loop() {
  TFT_display.fillScreen(WHITE);

  uint16_t w = TFT_display.width();
  uint16_t h = TFT_display.height();

  // Scale positions relative to screen size with better spacing
  int col1 = w / 8;
  int col2 = w * 3 / 8;
  int col3 = w * 5 / 8;
  int col4 = w * 7 / 8;
  int row1 = h / 4;
  int row2 = h / 2;
  int row3 = h * 3 / 4;

  // Outlined circle
  TFT_display.drawCircle(col1, row1, 30, RED);

  // Filled circle
  TFT_display.fillCircle(col2, row1, 30, RED);

  // Outlined triangle
  TFT_display.drawTriangle(col3 - 30, row1 + 25, col3 + 30, row1 + 25, col3, row1 - 25, BLUE);

  // Filled triangle
  TFT_display.fillTriangle(col4 - 30, row1 + 25, col4 + 30, row1 + 25, col4, row1 - 25, GREEN);

  // Outlined rectangle
  TFT_display.drawRect(col1 - 35, row2 - 20, 70, 40, ORANGE);

  // Filled rectangle
  TFT_display.fillRect(col2 - 35, row2 - 20, 70, 40, TURQUOISE);

  // Outlined round rectangle
  TFT_display.drawRoundRect(col3 - 35, row2 - 20, 70, 40, 10, VIOLET);

  // Filled round rectangle
  TFT_display.fillRoundRect(col4 - 35, row2 - 20, 70, 40, 10, PINK);

  // Outlined diamond (centered between col1 and col2)
  int diamond1_x = (col1 + col2) / 2;
  TFT_display.drawLine(diamond1_x, row3 - 30, diamond1_x + 25, row3, GREEN);
  TFT_display.drawLine(diamond1_x + 25, row3, diamond1_x, row3 + 30, GREEN);
  TFT_display.drawLine(diamond1_x, row3 + 30, diamond1_x - 25, row3, GREEN);
  TFT_display.drawLine(diamond1_x - 25, row3, diamond1_x, row3 - 30, GREEN);

  // Filled diamond (centered between col3 and col4)
  int diamond2_x = (col3 + col4) / 2;
  fillDiamond(diamond2_x, row3, 25, 30, BLUE);

  delay(10000);
}
