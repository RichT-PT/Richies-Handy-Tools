/*
   Created by DIYables

   This example code is in the public domain

   Product page: https://diyables.io
*/

// =============================================
// Single include brings in the base class plus all driver classes.
// =============================================
#include <DIYables_TFT_SPI.h>

#include <SD.h>

// =============================================
// Wiring (Arduino Uno / Nano)
// ---------------------------------------------
//   TFT + SD module           Arduino Uno / Nano
//   -----------------------   ---------------------------------
//   VCC                  ->   5V
//   GND                  ->   GND
//   TFT  CS              ->   D10  (TFT_CS_PIN)
//   TFT  RESET           ->   D9   (TFT_RST_PIN)
//   TFT  DC / RS         ->   D8   (TFT_DC_PIN)
//   SD   CS              ->   D4   (SD_CS)
//   SDI  / MOSI (shared) ->   D11  (hardware SPI MOSI)
//   SDO  / MISO (shared) ->   D12  (hardware SPI MISO)
//   SCK         (shared) ->   D13  (hardware SPI SCK)
//   LED                  ->   3.3V (or any GPIO via initBacklight)
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
#define SD_CS      4  // SD card chip select (must differ from TFT_CS_PIN)

// =============================================
// Create display object (uncomment matching driver)
// =============================================
// DIYables_ILI9341_SPI TFT_display(TFT_WIDTH, TFT_HEIGHT, TFT_CS_PIN, TFT_DC_PIN, TFT_RST_PIN);
// DIYables_ILI9488_SPI TFT_display(TFT_WIDTH, TFT_HEIGHT, TFT_CS_PIN, TFT_DC_PIN, TFT_RST_PIN);
DIYables_ST7789_SPI TFT_display(TFT_WIDTH, TFT_HEIGHT, TFT_CS_PIN, TFT_DC_PIN, TFT_RST_PIN);

#define WHITE     DIYables_TFT_SPI::colorRGB(255, 255, 255)
#define BUFFPIXEL 20

File bmpFile;
uint16_t SCREEN_WIDTH;
uint16_t SCREEN_HEIGHT;

// Helper functions to read BMP file header
uint16_t read16(File &f) {
  uint16_t result;
  result = f.read();
  result |= (f.read() << 8);
  return result;
}

uint32_t read32(File &f) {
  uint32_t result;
  result = f.read();
  result |= ((uint32_t)f.read() << 8);
  result |= ((uint32_t)f.read() << 16);
  result |= ((uint32_t)f.read() << 24);
  return result;
}

int32_t readS32(File &f) {
  int32_t result;
  result = f.read();
  result |= ((uint32_t)f.read() << 8);
  result |= ((uint32_t)f.read() << 16);
  result |= ((uint32_t)f.read() << 24);
  return result;
}

bool getBMPDimensions(const char *filename, uint32_t &w, uint32_t &h) {
  File f = SD.open(filename);
  if (!f) return false;
  if (read16(f) != 0x4D42) { f.close(); return false; }
  read32(f); // file size
  read32(f); // reserved
  read32(f); // image offset
  read32(f); // DIB header size
  w = read32(f);
  int32_t sh = readS32(f);
  h = (sh < 0) ? -sh : sh;
  f.close();
  return true;
}

void drawBMP(const char *filename, int x, int y) {
  bmpFile = SD.open(filename);
  if (!bmpFile) {
    Serial.println("File not found");
    return;
  }

  if (read16(bmpFile) != 0x4D42) {
    Serial.println("Not a BMP file");
    bmpFile.close();
    return;
  }

  uint32_t fileSize = read32(bmpFile);
  read32(bmpFile);                         // Reserved
  uint32_t imageOffset = read32(bmpFile);
  uint32_t dibHeaderSize = read32(bmpFile);
  uint32_t bmpWidth = read32(bmpFile);
  int32_t bmpHeight = readS32(bmpFile);

  bool topDown = false;
  if (bmpHeight < 0) {
    bmpHeight = -bmpHeight;
    topDown = true;
  }

  if (read16(bmpFile) != 1) {
    Serial.println("Invalid BMP file");
    bmpFile.close();
    return;
  }

  uint16_t depth = read16(bmpFile);
  if (depth != 24) {
    Serial.println("Only 24-bit BMP is supported");
    bmpFile.close();
    return;
  }

  if (read32(bmpFile) != 0) {
    Serial.println("Unsupported BMP compression");
    bmpFile.close();
    return;
  }

  bmpFile.seek(imageOffset);

  uint8_t sdbuffer[3 * BUFFPIXEL];
  uint16_t color;
  uint32_t rowSize = (bmpWidth * 3 + 3) & ~3;

  if (x >= SCREEN_WIDTH || y >= SCREEN_HEIGHT) return;

  uint32_t maxRow = min((uint32_t)bmpHeight, (uint32_t)(SCREEN_HEIGHT - y));
  uint32_t maxCol = min(bmpWidth, (uint32_t)(SCREEN_WIDTH - x));

  for (uint32_t row = 0; row < maxRow; row++) {
    int32_t rowPos = topDown ? row : bmpHeight - 1 - row;
    uint32_t filePosition = imageOffset + rowPos * rowSize;
    bmpFile.seek(filePosition);

    for (uint32_t col = 0; col < maxCol; col += BUFFPIXEL) {
      uint32_t pixelsToRead = min((uint32_t)BUFFPIXEL, maxCol - col);
      bmpFile.read(sdbuffer, 3 * pixelsToRead);

      for (uint32_t i = 0; i < pixelsToRead; i++) {
        uint8_t b = sdbuffer[i * 3];
        uint8_t g = sdbuffer[i * 3 + 1];
        uint8_t r = sdbuffer[i * 3 + 2];
        color = DIYables_TFT_SPI::colorRGB(r, g, b);

        if ((x + col + i) < SCREEN_WIDTH && (y + row) < SCREEN_HEIGHT) {
          TFT_display.drawPixel(x + col + i, y + row, color);
        }
      }
    }
  }

  bmpFile.close();
  Serial.println("BMP drawn");
}

void setup() {
  Serial.begin(9600);

  if (!SD.begin(SD_CS)) {
    Serial.println("SD card initialization failed!");
    return;
  }
  Serial.println("SD card initialized.");

  TFT_display.begin();
  TFT_display.setRotation(1); // Landscape

  SCREEN_WIDTH  = TFT_display.width();
  SCREEN_HEIGHT = TFT_display.height();

  TFT_display.fillScreen(WHITE);

  uint32_t imgWidth, imgHeight;
  if (getBMPDimensions("diyables.bmp", imgWidth, imgHeight)) {
    int x = (SCREEN_WIDTH  - imgWidth)  / 2;
    int y = (SCREEN_HEIGHT - imgHeight) / 2;
    drawBMP("diyables.bmp", x, y);
  } else {
    Serial.println("Failed to get BMP dimensions");
  }
}

void loop() {
}
