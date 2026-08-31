#include <Arduino.h>
#include <LovyanGFX.hpp>

class NetworkScoutDisplay : public lgfx::LGFX_Device
{
    lgfx::Panel_ILI9341 panel;   // was Panel_ST7789 - wrong driver for this board
    lgfx::Bus_SPI bus;
    lgfx::Light_PWM light;       // proper backlight control instead of raw digitalWrite

public:
    NetworkScoutDisplay()
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
            cfg.pin_miso = -1;   // reverted - this panel's MISO/SDO likely isn't wired, reading can hang init()
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

            cfg.dummy_read_pixel = 8;
            cfg.dummy_read_bits  = 1;
            cfg.readable  = false;  // reverted - avoid reading from a MISO line that likely isn't connected
            cfg.invert    = false;   // many of these ILI9341 clones need this for correct (non-inverted) colors
            cfg.rgb_order = false;   // many of these ILI9341 clones need this for correct (non-inverted) colors

            cfg.bus_shared = true;

            panel.config(cfg);
        }

        {
            auto cfg = light.config();
            cfg.pin_bl      = 27;   // confirmed by multimeter - this board's backlight is on 27, not 21
            cfg.invert      = false;
            cfg.freq        = 44100;
            cfg.pwm_channel = 7;

            light.config(cfg);
            panel.setLight(&light);
        }

        setPanel(&panel);
    }
};

NetworkScoutDisplay tft;

void setup()
{
    Serial.begin(115200);
    delay(500);

    Serial.println("LovyanGFX display test starting (ILI9341 config)...");

    tft.init();
    tft.setRotation(1);
    tft.setBrightness(255);

    Serial.println("RED");
    tft.fillScreen(TFT_RED);
    delay(1200);

    Serial.println("GREEN");
    tft.fillScreen(TFT_GREEN);
    delay(1200);

    Serial.println("BLUE");
    tft.fillScreen(TFT_BLUE);
    delay(1200);

    tft.fillScreen(TFT_BLACK);

    tft.setTextColor(TFT_CYAN);
    tft.setTextSize(3);
    tft.setCursor(35, 80);
    tft.print("NETWORK");

    tft.setCursor(70, 120);
    tft.print("SCOUT");

    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(2);
    tft.setCursor(75, 175);
    tft.print("DISPLAY ONLINE");

    Serial.println("Display test complete.");
}

void loop()
{
}
