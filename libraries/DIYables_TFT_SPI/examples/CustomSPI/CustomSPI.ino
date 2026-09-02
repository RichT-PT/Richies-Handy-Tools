/*
   Created by DIYables

   This example code is in the public domain

   Product page: https://diyables.io

   This example demonstrates how to use a custom (non-default) SPI bus
   with the DIYables TFT SPI library. This is useful on boards that have
   multiple SPI interfaces, such as:
     - ESP32:  HSPI / VSPI
     - Arduino Giga / Portenta: SPI1
     - Raspberry Pi Pico: SPI1
*/

// =============================================
// Single include brings in the base class plus all driver classes.
// =============================================
#include <DIYables_TFT_SPI.h>

// =============================================
// Wiring (Arduino Uno / Nano - default SPI bus)
// ---------------------------------------------
// NOTE: Uno / Nano have only ONE hardware SPI bus, so this example is
// most useful on boards with multiple buses (ESP32, Giga, RP2040...).
// On Uno / Nano, MY_SPI must remain &SPI and the wiring is the standard
// hardware SPI mapping below.
//
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
// Select alternate SPI bus (uncomment for your board)
// =============================================

// --- ESP32: use HSPI ---
// SPIClass hspi(HSPI);
// #define MY_SPI &hspi

// --- Arduino Giga / Portenta / RP2040: use SPI1 ---
// #define MY_SPI &SPI1

// --- Default SPI (fallback) ---
#define MY_SPI &SPI

// =============================================
// Create display object with custom SPI bus
// (uncomment matching driver)
// =============================================
// DIYables_ILI9341_SPI TFT_display(TFT_WIDTH, TFT_HEIGHT, TFT_CS_PIN, TFT_DC_PIN, TFT_RST_PIN, MY_SPI);
// DIYables_ILI9488_SPI TFT_display(TFT_WIDTH, TFT_HEIGHT, TFT_CS_PIN, TFT_DC_PIN, TFT_RST_PIN, MY_SPI);
DIYables_ST7789_SPI TFT_display(TFT_WIDTH, TFT_HEIGHT, TFT_CS_PIN, TFT_DC_PIN, TFT_RST_PIN, MY_SPI);

#define BLACK   DIYables_TFT_SPI::colorRGB(0, 0, 0)
#define WHITE   DIYables_TFT_SPI::colorRGB(255, 255, 255)
#define RED     DIYables_TFT_SPI::colorRGB(255, 0, 0)
#define GREEN   DIYables_TFT_SPI::colorRGB(0, 255, 0)
#define BLUE    DIYables_TFT_SPI::colorRGB(0, 0, 255)

void setup() {
  Serial.begin(9600);

  // For ESP32 HSPI: optionally remap pins before begin()
  // hspi.begin(14, 12, 13, -1);  // SCK, MISO, MOSI, SS

  TFT_display.begin();
  TFT_display.setRotation(1); // Landscape

  TFT_display.fillScreen(BLACK);

  uint16_t w = TFT_display.width();
  uint16_t h = TFT_display.height();

  // Draw a simple test pattern
  TFT_display.fillRect(0, 0, w / 3, h, RED);
  TFT_display.fillRect(w / 3, 0, w / 3, h, GREEN);
  TFT_display.fillRect(w * 2 / 3, 0, w / 3, h, BLUE);

  TFT_display.setTextColor(WHITE);
  TFT_display.setTextSize(2);
  TFT_display.setCursor(10, h / 2 - 10);
  TFT_display.print("Custom SPI bus OK");
}

void loop() {
  // Nothing to do
}
