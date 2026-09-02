/*
   Created by DIYables

   This example code is in the public domain

   Product page: https://diyables.io
*/

// =============================================
// Single include brings in the base class plus all driver classes.
// =============================================
#include <DIYables_TFT_SPI.h>

#include "bitmap.h"

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

// =============================================
// Create display object (uncomment matching driver)
// =============================================
// DIYables_ILI9341_SPI TFT_display(TFT_WIDTH, TFT_HEIGHT, TFT_CS_PIN, TFT_DC_PIN, TFT_RST_PIN);
// DIYables_ILI9488_SPI TFT_display(TFT_WIDTH, TFT_HEIGHT, TFT_CS_PIN, TFT_DC_PIN, TFT_RST_PIN);
DIYables_ST7789_SPI TFT_display(TFT_WIDTH, TFT_HEIGHT, TFT_CS_PIN, TFT_DC_PIN, TFT_RST_PIN);

#define WHITE DIYables_TFT_SPI::colorRGB(255, 255, 255)

int img_width = 120;
int img_height = 53;

void setup() {
  Serial.begin(9600);
  Serial.println(F("TFT SPI Display - Draw Image"));

  TFT_display.begin();

  uint16_t SCREEN_WIDTH  = TFT_display.width();
  uint16_t SCREEN_HEIGHT = TFT_display.height();

  int x = (SCREEN_WIDTH  - img_width)  / 2;
  int y = (SCREEN_HEIGHT - img_height) / 2;

  TFT_display.fillScreen(WHITE);
  TFT_display.drawRGBBitmap(x, y, myBitmap, img_width, img_height);
}

void loop() {
  delay(2000);
  TFT_display.invertDisplay(true);

  delay(2000);
  TFT_display.invertDisplay(false);
}
