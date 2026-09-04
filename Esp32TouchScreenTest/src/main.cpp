// Touch test — adapted from Scout's proven display setup.
// Purpose: isolate and fix touch, without touching Scout itself.
//
// The one real change vs. Scout's checkTouch(): the `tirqTouched()` gate is
// removed. That function only returns true if a hardware interrupt fired on
// the touch chip's IRQ pin (GPIO36), and LVGL_CYD's own README notes that
// exact interrupt is unreliable on these boards ("even touches that meet the
// set pressure threshold sometimes do not signal the interrupt, somehow").
// LVGL_CYD works around it by never checking the IRQ pin at all — just
// polling touched()/getPoint() directly, every loop. That's what this does.

#include <Arduino.h>
#include <LovyanGFX.hpp>
#include <SPI.h>
#include <XPT2046_Touchscreen.h> // added for TCI 

// ============================================================
// DISPLAY — identical to Scout's NetworkScoutDisplay
// ============================================================

class TestDisplay : public lgfx::LGFX_Device
{
    lgfx::Panel_ILI9341 panel;
    lgfx::Bus_SPI bus;
    lgfx::Light_PWM light;

public:
    TestDisplay()
    {
        {
            auto cfg = bus.config();
            cfg.spi_host = VSPI_HOST;
            cfg.spi_mode = 0;
            cfg.freq_write = 40000000;
            cfg.freq_read  = 16000000;
            cfg.spi_3wire = true;
            cfg.use_lock = true;
            cfg.dma_channel = 1;
            cfg.pin_sclk = 14;
            cfg.pin_mosi = 13;
            cfg.pin_miso = -1;
            cfg.pin_dc   = 2;
            bus.config(cfg);
            panel.setBus(&bus);
        }
        {
            auto cfg = panel.config();
            cfg.pin_cs   = 15;
            cfg.pin_rst  = -1;
            cfg.pin_busy = -1;
            cfg.panel_width  = 240;
            cfg.panel_height = 320;
            cfg.offset_x = 0;
            cfg.offset_y = 0;
            cfg.offset_rotation = 0;
            cfg.readable = false;
            cfg.invert = false;
            cfg.rgb_order = false;
            cfg.bus_shared = true;
            panel.config(cfg);
        }
        {
            auto cfg = light.config();
            cfg.pin_bl = 27;   // confirmed correct pin for this board
            cfg.invert = false;
            cfg.freq = 44100;
            cfg.pwm_channel = 7;
            light.config(cfg);
            panel.setLight(&light);
        }
        setPanel(&panel);
    }
};

TestDisplay tft;

// ============================================================
// TOUCHSCREEN — same pins as Scout
// ============================================================

#define TOUCH_CLK   14 /// USED THIS WHOLE SECTION FOR TOUCHSCREEN INTEGRATION (TCI)    
#define TOUCH_MOSI  13
#define TOUCH_MISO  12
#define TOUCH_CS    33
#define TOUCH_IRQ   36


SPIClass touchSPI(HSPI);
XPT2046_Touchscreen touch(TOUCH_CS, TOUCH_IRQ);

bool touchOnline = false;
unsigned long lastTouchReport = 0;

const int SCREEN_W = 320;
const int SCREEN_H = 240; ////  ^^^^^^^^^^^^^^

// Raw hardware sanity check, done BEFORE the SPI library touches anything.
// The XPT2046's IRQ pin is open-drain and idles HIGH via a pull-up — either
// inside the chip's own support circuitry on the board, or (more commonly on
// these clone boards) an external pull-up resistor near the chip. GPIO36 is
// an ESP32 input-only pin with NO internal pull resistor available, so if
// nothing is out there holding it up, this will read LOW or float rather
// than read a clean HIGH. That makes this a quick, SPI-independent way to
// tell "is *something* electrically present and wired to this pin" apart
// from "is the SPI protocol talking to it correctly" — the two possible
// failure points we've been trying to separate.
void checkIrqLineRaw()
{
    pinMode(TOUCH_IRQ, INPUT);
    delay(2);
    int readings[5];
    for (int i = 0; i < 5; i++) {
        readings[i] = digitalRead(TOUCH_IRQ);
        delay(2);
    }
    bool allHigh = true, allLow = true;
    for (int i = 0; i < 5; i++) {
        if (readings[i] == LOW)  allHigh = false;
        if (readings[i] == HIGH) allLow = false;
    }
    Serial.print("Raw IRQ pin (GPIO36) idle reads: ");
    for (int i = 0; i < 5; i++) Serial.print(readings[i]);
    Serial.println();
    if (allHigh) {
        Serial.println("  -> steady HIGH: something is pulling this line up. Consistent with a touch chip's IRQ output idling (or an external pull-up resistor) actually being there.");
    } else if (allLow) {
        Serial.println("  -> steady LOW: nothing is pulling this line up. Either no chip/pull-up present, the IRQ trace is broken, or it's shorted to ground.");
    } else {
        Serial.println("  -> unstable/floating: pin isn't being driven either way. Likely nothing connected to it at all.");
    }
}

// Manual, library-bypassing SPI probe. XPT2046_Touchscreen::begin() always
// returns true whether or not a real chip is attached, so it proves nothing.
// This instead bit-bangs the actual XPT2046 command bytes (0xD0 = read X,
// 0x90 = read Y, per the datasheet) directly over the SPI bus and prints the
// raw 12-bit result. A live, responding chip should show numbers that jitter
// around under a steady touch and drift with noise even when idle. Readings
// that are dead-flat at 0x000 or 0xFFF every single call, touched or not,
// mean the bus isn't getting real data back from anything — consistent with
// no chip / wrong pins / a dead part, not a library or math issue.
uint16_t xptRawRead(uint8_t command)
{
    digitalWrite(TOUCH_CS, LOW);
    touchSPI.transfer(command);
    uint16_t hi = touchSPI.transfer(0x00);
    uint16_t lo = touchSPI.transfer(0x00);
    digitalWrite(TOUCH_CS, HIGH);
    return ((hi << 8) | lo) >> 3;   // 12-bit result, MSB-first, 3 trailing pad bits
}

void rawSpiProbe()
{
    Serial.println("Raw SPI probe (bypassing XPT2046 library):");
    for (int i = 0; i < 6; i++) {
        uint16_t x = xptRawRead(0xD0);
        uint16_t y = xptRawRead(0x90);
        Serial.printf("  raw-probe X:%u Y:%u\n", x, y);
        delay(150);
    }
}

void setupTouch()
{
    Serial.println("Starting touchscreen...");
    checkIrqLineRaw();
    touchSPI.begin(TOUCH_CLK, TOUCH_MISO, TOUCH_MOSI, TOUCH_CS);
    pinMode(TOUCH_CS, OUTPUT);
    digitalWrite(TOUCH_CS, HIGH);
    rawSpiProbe();
    touchOnline = touch.begin(touchSPI);
    touch.setRotation(1);
    Serial.println(touchOnline ? "Touch controller initialized." : "Touch controller initialization failed.");
}

// last crosshair position, so we can erase it before drawing the new one
int lastX = -1, lastY = -1;

void checkTouch()
{
    if (!touchOnline) return;

    // NOTE: no tirqTouched() check here on purpose — see header comment.
    if (!touch.touched()) return;

    if (millis() - lastTouchReport < 50) return;
    lastTouchReport = millis();

    TS_Point p = touch.getPoint();

    // Raw XPT2046 ADC values need mapping to actual screen pixels.
    // These starting values come from LVGL_CYD's known-working calibration
    // for this same touch chip — treat them as a starting point, not gospel;
    // if the crosshair doesn't land under your finger, we'll adjust these.
    int x = map(p.x, 200, 3750, 0, SCREEN_W - 1);
    int y = map(p.y, 200, 3700, SCREEN_H - 1, 0);
    x = constrain(x, 0, SCREEN_W - 1);
    y = constrain(y, 0, SCREEN_H - 1);

    Serial.printf("raw x=%d y=%d z=%d  ->  mapped x=%d y=%d\n", p.x, p.y, p.z, x, y);

    // erase old crosshair
    if (lastX >= 0) {
        tft.drawFastHLine(0, lastY, SCREEN_W, TFT_BLACK);
        tft.drawFastVLine(lastX, 0, SCREEN_H, TFT_BLACK);
    }

    // draw new crosshair
    tft.drawFastHLine(0, y, SCREEN_W, TFT_RED);
    tft.drawFastVLine(x, 0, SCREEN_H, TFT_RED);

    tft.fillRect(0, SCREEN_H - 18, SCREEN_W, 18, TFT_BLACK);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(1);
    tft.setCursor(4, SCREEN_H - 13);
    tft.printf("raw X:%d Y:%d Z:%d  px X:%d Y:%d", p.x, p.y, p.z, x, y);

    lastX = x;
    lastY = y;
}

void setup()
{
    Serial.begin(115200);
    delay(300);
    Serial.println("Touch test booting...");

    tft.init();
    tft.setRotation(1);
    tft.setBrightness(255);
    tft.fillScreen(TFT_BLACK);

    tft.setTextColor(TFT_CYAN, TFT_BLACK);
    tft.setTextSize(2);
    tft.setCursor(40, 100);
    tft.println("TOUCH TEST");

    setupTouch();
}

void loop()
{
    checkTouch();
    delay(10);
}
