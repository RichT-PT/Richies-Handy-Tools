/*
   Created by DIYables

   This example code is in the public domain

   Product page: https://diyables.io
*/

// =============================================
// Single include brings in the base class plus all driver classes.
// =============================================
#include <DIYables_TFT_SPI.h>

#include <Fonts/FreeSansBold12pt7b.h>

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

#define MAGENTA   DIYables_TFT_SPI::colorRGB(255, 0, 255)
#define WHITE     DIYables_TFT_SPI::colorRGB(255, 255, 255)

void setup() {
  Serial.begin(9600);
  Serial.println(F("TFT SPI Display - Use external font"));

  TFT_display.begin();
  TFT_display.setFont(&FreeSansBold12pt7b);
  TFT_display.setRotation(1); // Landscape
  TFT_display.fillScreen(WHITE);

  TFT_display.setTextColor(MAGENTA);
  TFT_display.setTextSize(1);

  float temperature = 23.5;
  float humidity = 78.6;

  TFT_display.setCursor(20, 30);
  TFT_display.print("Temperature: ");
  TFT_display.print(temperature, 1);
  TFT_display.print(char(247));
  TFT_display.println("C");

  TFT_display.setCursor(20, 70);
  TFT_display.print("Humidity: ");
  TFT_display.print(humidity, 1);
  TFT_display.print("%");
}

void loop() {
}
