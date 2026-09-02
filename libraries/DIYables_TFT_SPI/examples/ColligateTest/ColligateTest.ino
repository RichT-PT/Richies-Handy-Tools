/*
   Created by DIYables

   This example code is in the public domain

   Comprehensive graphics benchmark - tests most drawing functions
   and reports timing for each operation, both on the screen and
   on the Serial Monitor.

   Product page: https://diyables.io/tft-spi

   Credit: This example is based on the colligate_test example from the LCDWIKI library
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

// The tests run in landscape, so the screen size is filled in by setup()
uint16_t SCREEN_WIDTH;
uint16_t SCREEN_HEIGHT;

// Drawing area: everything between the header and the footer bar
#define TOP_Y     16
#define BOT_Y     (SCREEN_HEIGHT - 17)

// Color definitions (RGB565)
#define BLACK     DIYables_TFT_SPI::colorRGB(0, 0, 0)
#define WHITE     DIYables_TFT_SPI::colorRGB(255, 255, 255)
#define RED       DIYables_TFT_SPI::colorRGB(255, 0, 0)
#define GREEN     DIYables_TFT_SPI::colorRGB(0, 255, 0)
#define BLUE      DIYables_TFT_SPI::colorRGB(0, 0, 255)
#define CYAN      DIYables_TFT_SPI::colorRGB(0, 255, 255)
#define MAGENTA   DIYables_TFT_SPI::colorRGB(255, 0, 255)
#define YELLOW    DIYables_TFT_SPI::colorRGB(255, 255, 0)
#define ORANGE    DIYables_TFT_SPI::colorRGB(255, 165, 0)

// Print text horizontally centered on the screen
void printCentered(const char *text, int16_t y, uint16_t color) {
  int16_t x1, y1;
  uint16_t w, h;
  TFT_display.setTextColor(color);
  TFT_display.setTextSize(1);
  TFT_display.getTextBounds(text, 0, y, &x1, &y1, &w, &h);
  int16_t x = (SCREEN_WIDTH - (int16_t)w) / 2;
  if (x < 0) x = 0;
  TFT_display.setCursor(x, y);
  TFT_display.print(text);
}

// Plot one point of a curve, clipping values that run far off screen
// (tan and cot grow without bound near their asymptotes)
void plotCurve(int16_t x, float value, uint16_t color) {
  if (value > 30000.0 || value < -30000.0) return;
  int16_t y = SCREEN_HEIGHT / 2 - 1 + (int16_t)value;
  TFT_display.drawPixel(x, y, color);
}

// Display main surface with header/footer
unsigned long show_text(void) {
  unsigned long time_start = micros();

  TFT_display.fillRect(0, 0, SCREEN_WIDTH, 15, DIYables_TFT_SPI::colorRGB(32, 0, 255));
  printCentered("* DIYables SPI TFT LCD Display *", 3, GREEN);

  TFT_display.fillRect(0, SCREEN_HEIGHT - 15, SCREEN_WIDTH, 15,
                       DIYables_TFT_SPI::colorRGB(128, 128, 128));
  printCentered("<https://diyables.io/tft-spi>", SCREEN_HEIGHT - 11, WHITE);

  TFT_display.drawRect(0, 15, SCREEN_WIDTH, SCREEN_HEIGHT - 31, RED);
  return micros() - time_start;
}

// Display triangle functions (sin, cos, tan, cot)
unsigned long show_triangle_function(void) {
  uint16_t i;
  unsigned long time_start = micros();
  float amp = (BOT_Y - TOP_Y) / 2.0 - 2.0;      // fill the drawing area
  float step = 360.0 / (SCREEN_WIDTH - 2);      // one full period across the screen

  // Draw crosshairs
  TFT_display.drawFastVLine(SCREEN_WIDTH / 2 - 1, TOP_Y, SCREEN_HEIGHT - 32, BLUE);
  TFT_display.drawFastHLine(1, SCREEN_HEIGHT / 2 - 1, SCREEN_WIDTH - 2, BLUE);

  for (i = 1; i <= (SCREEN_HEIGHT - 32) / 2 / 10; i++) {
    TFT_display.drawFastHLine(SCREEN_WIDTH / 2 - 3, SCREEN_HEIGHT / 2 - 1 - i * 10, 5, BLUE);
    TFT_display.drawFastHLine(SCREEN_WIDTH / 2 - 3, SCREEN_HEIGHT / 2 - 1 + i * 10, 5, BLUE);
  }
  for (i = 1; i <= (SCREEN_WIDTH - 2) / 2 / 10; i++) {
    TFT_display.drawFastVLine(SCREEN_WIDTH / 2 - 1 - i * 10, SCREEN_HEIGHT / 2 - 3, 5, BLUE);
    TFT_display.drawFastVLine(SCREEN_WIDTH / 2 - 1 + i * 10, SCREEN_HEIGHT / 2 - 3, 5, BLUE);
  }

  TFT_display.setTextSize(1);
  TFT_display.setTextColor(CYAN);
  TFT_display.setCursor(5, TOP_Y + 1);
  TFT_display.print(F("sin"));
  TFT_display.setTextColor(GREEN);
  TFT_display.setCursor(5, TOP_Y + 9);
  TFT_display.print(F("cos"));
  TFT_display.setTextColor(YELLOW);
  TFT_display.setCursor(5, TOP_Y + 17);
  TFT_display.print(F("tan"));
  TFT_display.setTextColor(RED);
  TFT_display.setCursor(5, TOP_Y + 25);
  TFT_display.print(F("cot"));

  for (i = 1; i < SCREEN_WIDTH - 1; i++) {
    float rad = radians(i * step);
    plotCurve(i, sin(rad) * amp, CYAN);
    plotCurve(i, cos(rad) * amp, GREEN);
    plotCurve(i, tan(rad) * 10.0, YELLOW);
    float t = tan(rad);
    if (t != 0.0) plotCurve(i, (1.0 / t) * 10.0, RED);
  }
  return micros() - time_start;
}

// Draw a moving sinewave. The previous trace is erased by recomputing the
// value one screen-width earlier, so no frame buffer is needed - that keeps
// this example runnable on boards with very little RAM, such as the Uno R3.
unsigned long show_sinewave(void) {
  unsigned long time_start = micros();
  uint16_t span = SCREEN_WIDTH - 2;
  uint32_t total = (uint32_t)span * 12;
  float amp = (BOT_Y - TOP_Y) / 2.0 - 2.0;
  float step = 720.0 / span;                    // two periods across the screen

  TFT_display.drawFastVLine(SCREEN_WIDTH / 2 - 1, TOP_Y, SCREEN_HEIGHT - 32, BLUE);
  TFT_display.drawFastHLine(1, SCREEN_HEIGHT / 2 - 1, SCREEN_WIDTH - 2, BLUE);

  for (uint32_t i = 0; i < total; i++) {
    uint16_t x = 1 + (i % span);

    // Erase the point drawn at this x one screen ago
    if (i >= span) {
      uint32_t prev = i - span;
      float decay = 1.0 - (float)prev / (float)total * 0.5;
      int16_t oldY = SCREEN_HEIGHT / 2 - 1 + (int16_t)(sin(radians(prev * step)) * amp * decay);
      // Keep the axes intact while erasing
      if (x == SCREEN_WIDTH / 2 - 1 || oldY == SCREEN_HEIGHT / 2 - 1) {
        TFT_display.drawPixel(x, oldY, BLUE);
      } else {
        TFT_display.drawPixel(x, oldY, BLACK);
      }
    }

    float decay = 1.0 - (float)i / (float)total * 0.5;
    int16_t y = SCREEN_HEIGHT / 2 - 1 + (int16_t)(sin(radians(i * step)) * amp * decay);
    TFT_display.drawPixel(x, y, DIYables_TFT_SPI::colorRGB(255, 64, 255));
  }
  return micros() - time_start;
}

// Draw some filled rectangles
unsigned long show_fill_rectangle(void) {
  uint16_t i;
  unsigned long time_start = micros();
  uint16_t side_len = (SCREEN_HEIGHT - 40) / 5;
  uint16_t x_spec = (SCREEN_WIDTH - 5 * side_len) / 2;
  uint16_t y_spec = (SCREEN_HEIGHT - 5 * side_len) / 2;
  uint16_t colors[] = { MAGENTA, RED, GREEN, BLUE, YELLOW };
  for (i = 0; i < 5; i++) {
    TFT_display.fillRect(x_spec + i * side_len, y_spec + i * side_len,
                         side_len, side_len, colors[i]);
    TFT_display.fillRect(x_spec + i * side_len, y_spec + (4 - i) * side_len,
                         side_len, side_len, colors[i]);
  }
  return micros() - time_start;
}

// Draw some filled round rectangles
unsigned long show_fill_round_rectangle(void) {
  uint16_t i;
  unsigned long time_start = micros();
  uint16_t side_len = (SCREEN_HEIGHT - 40) / 5;
  uint16_t x_spec = (SCREEN_WIDTH - 5 * side_len) / 2;
  uint16_t y_spec = (SCREEN_HEIGHT - 5 * side_len) / 2;
  uint16_t colors[] = { MAGENTA, RED, GREEN, BLUE, YELLOW };
  for (i = 0; i < 5; i++) {
    TFT_display.fillRoundRect(x_spec + i * side_len, y_spec + i * side_len,
                              side_len, side_len, 10, colors[i]);
    TFT_display.fillRoundRect(x_spec + i * side_len, y_spec + (4 - i) * side_len,
                              side_len, side_len, 10, colors[i]);
  }
  return micros() - time_start;
}

// Draw some filled circles
unsigned long show_fill_circle(void) {
  uint16_t i;
  unsigned long time_start = micros();
  uint16_t r_len = (SCREEN_HEIGHT - 40) / 5 / 2;
  uint16_t x_spec = (SCREEN_WIDTH - 5 * r_len * 2) / 2;
  uint16_t y_spec = (SCREEN_HEIGHT - 5 * r_len * 2) / 2;
  uint16_t colors[] = { MAGENTA, RED, GREEN, BLUE, YELLOW };
  for (i = 0; i < 5; i++) {
    TFT_display.fillCircle(x_spec + r_len + i * r_len * 2,
                           y_spec + r_len + i * r_len * 2, r_len, colors[i]);
    TFT_display.fillCircle(x_spec + r_len + i * r_len * 2,
                           y_spec + (5 - i) * r_len * 2 - r_len, r_len, colors[i]);
  }
  return micros() - time_start;
}

// Draw some filled triangles
unsigned long show_fill_triangle(void) {
  uint16_t i;
  unsigned long time_start = micros();
  uint16_t h_len = (SCREEN_HEIGHT - 40) / 5;
  uint16_t side_len = (h_len * 115) / 100;
  uint16_t x_spec = (SCREEN_WIDTH - 5 * side_len) / 2;
  uint16_t y_spec = (SCREEN_HEIGHT - 5 * h_len) / 2;
  uint16_t colors[] = { MAGENTA, RED, GREEN, BLUE, YELLOW };
  for (i = 0; i < 5; i++) {
    TFT_display.fillTriangle(
      x_spec + i * side_len, y_spec + (i + 1) * h_len,
      x_spec + side_len / 2 + i * side_len, y_spec + i * h_len,
      x_spec + (i + 1) * side_len, y_spec + (i + 1) * h_len, colors[i]);
    TFT_display.fillTriangle(
      x_spec + i * side_len, y_spec + (5 - i) * h_len,
      x_spec + side_len / 2 + i * side_len, y_spec + (4 - i) * h_len,
      x_spec + (i + 1) * side_len, y_spec + (5 - i) * h_len, colors[i]);
  }
  return micros() - time_start;
}

// Draw grid lines pattern
unsigned long show_grid_lines(void) {
  uint16_t i;
  unsigned long time_start = micros();
  // Sweep the far end of each line across the full width as i walks down
  float k = (float)(SCREEN_WIDTH - 3) / (float)(BOT_Y - TOP_Y);

  for (i = TOP_Y; i < BOT_Y; i += 5)
    TFT_display.drawLine(1, i, 1 + (i - TOP_Y) * k, BOT_Y, RED);
  for (i = TOP_Y; i < BOT_Y; i += 5)
    TFT_display.drawLine(SCREEN_WIDTH - 2, i, SCREEN_WIDTH - 2 - (i - TOP_Y) * k, TOP_Y, RED);
  for (i = TOP_Y; i < BOT_Y; i += 5)
    TFT_display.drawLine(1, i, 1 + (i - TOP_Y) * k, TOP_Y, CYAN);
  for (i = TOP_Y; i < BOT_Y; i += 5)
    TFT_display.drawLine(SCREEN_WIDTH - 2, i, SCREEN_WIDTH - 2 - (i - TOP_Y) * k, BOT_Y, CYAN);
  return micros() - time_start;
}

// Draw some random pixels
unsigned long show_random_pixels(void) {
  uint16_t i;
  unsigned long time_start = micros();
  for (i = 0; i < 10000; i++) {
    TFT_display.drawPixel(
      2 + random(SCREEN_WIDTH - 4),
      17 + random(SCREEN_HEIGHT - 34),
      DIYables_TFT_SPI::colorRGB(random(255), random(255), random(255)));
  }
  return micros() - time_start;
}

// Draw some random lines
unsigned long show_random_lines(void) {
  uint16_t i;
  unsigned long time_start = micros();
  for (i = 0; i < 300; i++) {
    TFT_display.drawLine(
      2 + random(SCREEN_WIDTH - 4), 17 + random(SCREEN_HEIGHT - 34),
      2 + random(SCREEN_WIDTH - 4), 17 + random(SCREEN_HEIGHT - 34),
      DIYables_TFT_SPI::colorRGB(random(255), random(255), random(255)));
  }
  return micros() - time_start;
}

// Draw some random rectangles
unsigned long show_random_rectangles(void) {
  uint16_t i;
  unsigned long time_start = micros();
  for (i = 0; i < 150; i++) {
    int16_t x1 = 2 + random(SCREEN_WIDTH - 4);
    int16_t y1 = 17 + random(SCREEN_HEIGHT - 34);
    int16_t x2 = 2 + random(SCREEN_WIDTH - 4);
    int16_t y2 = 17 + random(SCREEN_HEIGHT - 34);
    TFT_display.drawRect(min(x1, x2), min(y1, y2), abs(x2 - x1), abs(y2 - y1),
                         DIYables_TFT_SPI::colorRGB(random(255), random(255), random(255)));
  }
  return micros() - time_start;
}

// Draw some random round rectangles
unsigned long show_random_round_rectangles(void) {
  uint16_t i;
  unsigned long time_start = micros();
  for (i = 0; i < 150; i++) {
    int16_t x1 = 2 + random(SCREEN_WIDTH - 4);
    int16_t y1 = 17 + random(SCREEN_HEIGHT - 34);
    int16_t x2 = 2 + random(SCREEN_WIDTH - 4);
    int16_t y2 = 17 + random(SCREEN_HEIGHT - 34);
    int16_t w = abs(x2 - x1);
    int16_t h = abs(y2 - y1);
    // A round rectangle needs room for the corner radius
    if (w < 12 || h < 12) continue;
    TFT_display.drawRoundRect(min(x1, x2), min(y1, y2), w, h, 5,
                              DIYables_TFT_SPI::colorRGB(random(255), random(255), random(255)));
  }
  return micros() - time_start;
}

// Draw some random circles
unsigned long show_random_circles(void) {
  uint16_t i;
  unsigned long time_start = micros();
  uint16_t r_max = (SCREEN_HEIGHT - 34) / 6;
  for (i = 0; i < 150; i++) {
    TFT_display.drawCircle(
      2 + r_max + random(SCREEN_WIDTH - 4 - r_max * 2),
      17 + r_max + random(SCREEN_HEIGHT - 34 - r_max * 2),
      random(r_max),
      DIYables_TFT_SPI::colorRGB(random(255), random(255), random(255)));
  }
  return micros() - time_start;
}

// Draw some random triangles
unsigned long show_random_triangles(void) {
  uint16_t i;
  unsigned long time_start = micros();
  for (i = 0; i < 150; i++) {
    TFT_display.drawTriangle(
      2 + random(SCREEN_WIDTH - 4), 17 + random(SCREEN_HEIGHT - 34),
      2 + random(SCREEN_WIDTH - 4), 17 + random(SCREEN_HEIGHT - 34),
      2 + random(SCREEN_WIDTH - 4), 17 + random(SCREEN_HEIGHT - 34),
      DIYables_TFT_SPI::colorRGB(random(255), random(255), random(255)));
  }
  return micros() - time_start;
}

// Clear the drawing area
void clear_screen(void) {
  delay(2000);
  TFT_display.fillRect(1, 16, SCREEN_WIDTH - 2, SCREEN_HEIGHT - 32, BLACK);
}

// Function pointer array and labels
typedef unsigned long (*ShowFunc)(void);
ShowFunc show_function[] = {
  show_text,
  show_triangle_function,
  show_sinewave,
  show_fill_rectangle,
  show_fill_round_rectangle,
  show_fill_circle,
  show_fill_triangle,
  show_grid_lines,
  show_random_pixels,
  show_random_lines,
  show_random_rectangles,
  show_random_round_rectangles,
  show_random_circles,
  show_random_triangles,
};

#define NUM_TESTS 14
#define LABEL_LEN 32   // 30 visible chars + terminator, rounded up

// Kept in flash so the sketch still fits the Uno R3's 2 KB of RAM
const char show_str[NUM_TESTS][LABEL_LEN] PROGMEM = {
  "show text                    :",
  "show triangle function       :",
  "show sinewave                :",
  "show fill rectangle          :",
  "show fill round rectangle    :",
  "show fill circle             :",
  "show fill triangle           :",
  "show grid lines              :",
  "show random pixels           :",
  "show random lines            :",
  "show random rectangles       :",
  "show random round rectangles :",
  "show random circles          :",
  "show random triangles        :"
};

// Run all tests and display timing results
unsigned long show_total_time(void) {
  uint16_t i;
  unsigned long buf[NUM_TESTS];
  unsigned long time_start = micros();
  char strbuf[LABEL_LEN];

  for (i = 0; i < NUM_TESTS; i++) {
    buf[i] = show_function[i]();
    clear_screen();
  }

  int16_t x_off = (SCREEN_WIDTH - 260) / 2;
  if (x_off < 2) x_off = 2;
  int16_t y_off = (SCREEN_HEIGHT - NUM_TESTS * 10) / 2;
  if (y_off < TOP_Y + 2) y_off = TOP_Y + 2;

  for (i = 0; i < NUM_TESTS; i++) {
    strcpy_P(strbuf, show_str[i]);
    TFT_display.setTextColor(ORANGE);
    TFT_display.setTextSize(1);
    TFT_display.setCursor(x_off, y_off + i * 10);
    TFT_display.print(strbuf);
    TFT_display.setTextColor(GREEN);
    TFT_display.setCursor(x_off + 185, y_off + i * 10);
    TFT_display.print(buf[i]);

    // Also print to Serial for easy comparison
    Serial.print(strbuf);
    Serial.print(F(" "));
    Serial.print(buf[i]);
    Serial.println(F(" us"));
  }

  // Print total
  unsigned long total = 0;
  for (i = 0; i < NUM_TESTS; i++) total += buf[i];
  Serial.print(F("Total (tests only):            "));
  Serial.print(total);
  Serial.println(F(" us"));
  Serial.println();

  delay(5000);
  return micros() - time_start;
}

// Display ending screen with total time
void show_end(unsigned long run_time) {
  TFT_display.fillScreen(CYAN);
  int16_t bw = 240, bh = 120;
  if (bw > SCREEN_WIDTH - 10) bw = SCREEN_WIDTH - 10;
  TFT_display.fillRoundRect((SCREEN_WIDTH - bw) / 2, (SCREEN_HEIGHT - bh) / 2,
                            bw, bh, 5, RED);

  int16_t cy = (SCREEN_HEIGHT - bh) / 2 + 20;
  printCentered("Running over!", cy, CYAN);
  printCentered("That's ok!", cy + 10, CYAN);
  printCentered("After a few seconds,", cy + 20, CYAN);
  printCentered("it will restart.", cy + 30, CYAN);
  printCentered("Please wait ...", cy + 40, CYAN);

  char line[40];
  sprintf(line, "Total runtime(us): %lu", run_time);
  printCentered(line, cy + 60, YELLOW);
  delay(10000);
}

void setup() {
  Serial.begin(9600);

  TFT_display.begin();
  TFT_display.setRotation(1); // landscape
  TFT_display.fillScreen(BLACK);

  SCREEN_WIDTH = TFT_display.width();
  SCREEN_HEIGHT = TFT_display.height();

  randomSeed(analogRead(A0));
}

void loop() {
  unsigned long total_time;
  TFT_display.fillScreen(BLACK);
  total_time = show_total_time();
  show_end(total_time);
}
