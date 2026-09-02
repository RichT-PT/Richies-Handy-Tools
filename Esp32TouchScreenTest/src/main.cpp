#include <Arduino.h>

#define LGFX_USE_V1
#include <LovyanGFX.hpp>

// ============================================================
// TOUCHSCREEN TOOL
// ILI9341 LCD + XPT2046 TOUCH
// ============================================================

class TouchscreenDisplay : public lgfx::LGFX_Device
{
    lgfx::Panel_ILI9341 panel;
    lgfx::Bus_SPI bus;
    lgfx::Light_PWM light;
    lgfx::Touch_XPT2046 touch;

public:
    TouchscreenDisplay()
    {
        // ====================================================
        // LCD SPI BUS
        // ====================================================
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

        // ====================================================
        // LCD PANEL
        // ====================================================
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

        // ====================================================
        // BACKLIGHT
        // ====================================================
        {
            auto cfg = light.config();

            cfg.pin_bl = 27;
            cfg.invert = false;
            cfg.freq = 44100;
            cfg.pwm_channel = 7;

            light.config(cfg);
            panel.setLight(&light);
        }

        // ====================================================
        // XPT2046 TOUCH CONTROLLER
        // ====================================================
        {
            auto cfg = touch.config();

            // Initial calibration values
            cfg.x_min = 240;
            cfg.x_max = 3800;
            cfg.y_min = 3700;
            cfg.y_max = 200;

            // Touch IRQ
            cfg.pin_int = 36;

            cfg.bus_shared = false;
            cfg.offset_rotation = 0;

            // IMPORTANT:
            // -1 uses LovyanGFX software SPI on these pins
            cfg.spi_host = -1;

            cfg.freq = 1000000;

            cfg.pin_sclk = 25;
            cfg.pin_mosi = 32;
            cfg.pin_miso = 39;
            cfg.pin_cs   = 33;

            touch.config(cfg);

            panel.setTouch(&touch);
        }

        setPanel(&panel);
    }
};

TouchscreenDisplay tft;


// ============================================================
// SETUP
// ============================================================

void setup()
{
    Serial.begin(115200);
    delay(500);

    Serial.println();
    Serial.println("Touchscreen Tool starting...");

    tft.init();

    // Landscape: 320 x 240
    tft.setRotation(1);

    tft.setBrightness(200);

    tft.fillScreen(TFT_BLACK);

    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(2);

    tft.setCursor(20, 20);
    tft.println("TOUCH TEST");

    tft.setTextSize(1);

    tft.setCursor(20, 55);
    tft.println("LovyanGFX XPT2046");

    tft.setCursor(20, 70);
    tft.println("Touch the screen");

    tft.setTextColor(TFT_GREEN, TFT_BLACK);

    tft.setCursor(20, 90);
    tft.println("Waiting for touch...");
}


// ============================================================
// LOOP
// ============================================================

void loop()
{
    uint16_t x;
    uint16_t y;

    if (tft.getTouch(&x, &y))
    {
        // ----------------------------------------------------
        // Serial output
        // ----------------------------------------------------

        Serial.print("X: ");
        Serial.print(x);

        Serial.print("   Y: ");
        Serial.println(y);


        // ----------------------------------------------------
        // Coordinate display
        // ----------------------------------------------------

        tft.fillRect(
            20,
            105,
            290,
            70,
            TFT_BLACK
        );

        tft.setTextColor(TFT_YELLOW, TFT_BLACK);
        tft.setTextSize(2);

        tft.setCursor(20, 110);
        tft.printf("X: %u", x);

        tft.setCursor(20, 140);
        tft.printf("Y: %u", y);


        // ----------------------------------------------------
        // Draw dot at detected touch location
        // ----------------------------------------------------

        tft.fillCircle(
            x,
            y,
            4,
            TFT_WHITE
        );

        delay(50);
    }
}