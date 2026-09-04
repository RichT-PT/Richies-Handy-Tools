// ============================================================
// TCI = Touchscreen Integration
// Donor: Esp32TouchScreenTest
// All touchscreen integration additions are marked TCI.
// ============================================================




#include <Arduino.h>
#include <WiFi.h>
#include <esp_sleep.h>
#include <LovyanGFX.hpp>
#include <SPI.h>
#include <XPT2046_Touchscreen.h>   // TCI


// ============================================================
// DISPLAY
// Confirmed working configuration for this board
// ============================================================

class NetworkScoutDisplay : public lgfx::LGFX_Device
{
    lgfx::Panel_ILI9341 panel;
    lgfx::Bus_SPI bus;
    lgfx::Light_PWM light;

public:
    NetworkScoutDisplay()
    {
        // ---------- SPI bus ----------
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

        // ---------- LCD panel ----------
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

        // ---------- Backlight ----------
        {
            auto cfg = light.config();

            cfg.pin_bl = 27;
            cfg.invert = false;
            cfg.freq = 44100;
            cfg.pwm_channel = 7;

            light.config(cfg);
            panel.setLight(&light);
        }

        setPanel(&panel);
    }
};

NetworkScoutDisplay tft;


// ============================================================
// STATUS LED
// GPIO 4  = red
// GPIO 17 = green
// GPIO 16 = blue
// Active LOW
// ============================================================
// ============================================================
// TOUCHSCREEN
// XPT2046 resistive touch controller
// Dedicated SPI bus
// ============================================================

#define LED_RED_PIN   4
#define LED_GREEN_PIN 17
#define LED_BLUE_PIN  16
#define LED_CH_RED    0
#define LED_CH_GREEN  1
#define LED_CH_BLUE   2
#define LED_PWM_FREQ  5000
#define LED_PWM_RES   8
// ============================================================
// TCI - Touchscreen hardware
// ============================================================
// VVVVVVV section added for TCI VVVVVVV

#define TOUCH_CLK   14
#define TOUCH_MOSI  13
#define TOUCH_MISO  12
#define TOUCH_CS    33
#define TOUCH_IRQ   36

// TCI - Touchscreen objects/state
SPIClass touchSPI(HSPI);
XPT2046_Touchscreen touch(TOUCH_CS, TOUCH_IRQ);

bool touchOnline = false;
unsigned long lastTouchReport = 0;
// ^^^^^^^ section added for TCI ^^^^^^^ 


uint8_t ledBrightness = 40;

void setupTouch();   // TCI
void checkTouch();   // TCI

void ledSetup()

{
    ledcSetup(LED_CH_RED, LED_PWM_FREQ, LED_PWM_RES);
    ledcSetup(LED_CH_GREEN, LED_PWM_FREQ, LED_PWM_RES);
    ledcSetup(LED_CH_BLUE, LED_PWM_FREQ, LED_PWM_RES);

    ledcAttachPin(LED_RED_PIN, LED_CH_RED);
    ledcAttachPin(LED_GREEN_PIN, LED_CH_GREEN);
    ledcAttachPin(LED_BLUE_PIN, LED_CH_BLUE);
}

void statusLed(bool red, bool green, bool blue)
{
    ledcWrite(
        LED_CH_RED,
        red ? (255 - ledBrightness) : 255
    );

    ledcWrite(
        LED_CH_GREEN,
        green ? (255 - ledBrightness) : 255
    );

    ledcWrite(
        LED_CH_BLUE,
        blue ? (255 - ledBrightness) : 255
    );
}


// ============================================================
// NETWORK DATA
// ============================================================

struct NetworkInfo
{
    String ssid;
    String bssid;
    int32_t rssi;
    int channel;
    wifi_auth_mode_t security;
};

const int MAX_NETWORKS = 40;

NetworkInfo cachedNetworks[MAX_NETWORKS];
int cachedCount = 0;


// ============================================================
// LCD LAYOUT
// Landscape = 320 x 240
// ============================================================

const int SCREEN_W = 320;
const int SCREEN_H = 240;

const int HEADER_H = 20;
const int ROW_H = 20;

const int MAX_VISIBLE_ROWS =
    (SCREEN_H - HEADER_H) / ROW_H;


// ============================================================
// SCANNING
// ============================================================

const unsigned long SCAN_INTERVAL_MS = 6000;
unsigned long lastScanFinished = 0;


uint16_t rssiColor(int32_t rssi)
{
    if (rssi >= -60)
        return TFT_GREEN;

    if (rssi >= -75)
        return TFT_YELLOW;

    return TFT_RED;
}


const char* securityName(wifi_auth_mode_t auth)
{
    switch (auth)
    {
        case WIFI_AUTH_OPEN:
            return "OPEN";

        case WIFI_AUTH_WEP:
            return "WEP";

        case WIFI_AUTH_WPA_PSK:
            return "WPA";

        case WIFI_AUTH_WPA2_PSK:
            return "WPA2";

        case WIFI_AUTH_WPA_WPA2_PSK:
            return "WPA12";

        case WIFI_AUTH_WPA2_ENTERPRISE:
            return "ENT";

        case WIFI_AUTH_WPA3_PSK:
            return "WPA3";

        case WIFI_AUTH_WPA2_WPA3_PSK:
            return "WPA23";

        default:
            return "SEC";
    }
}


void drawHeader(bool scanning)
{
    tft.fillRect(
        0,
        0,
        SCREEN_W,
        HEADER_H,
        TFT_NAVY
    );

    tft.setTextColor(
        TFT_WHITE,
        TFT_NAVY
    );

    tft.setTextSize(1);
    tft.setCursor(4, 6);

    if (scanning)
    {
        tft.print(
            "NETWORK SCOUT   scanning..."
        );
    }
    else
    {
        tft.printf(
            "NETWORK SCOUT   %d networks",
            cachedCount
        );
    }
}


void drawNetworkList()
{
    drawHeader(false);

    tft.fillRect(
        0,
        HEADER_H,
        SCREEN_W,
        SCREEN_H - HEADER_H,
        TFT_BLACK
    );

    tft.setTextSize(1);

    int rowsToShow =
        min(cachedCount, MAX_VISIBLE_ROWS);

    for (int row = 0;
         row < rowsToShow;
         row++)
    {
        NetworkInfo &net =
            cachedNetworks[row];

        int y =
            HEADER_H +
            row * ROW_H +
            3;

        String ssid = net.ssid;

        if (ssid.length() > 22)
        {
            ssid =
                ssid.substring(0, 21)
                + "...";
        }

        // SSID
        tft.setTextColor(
            TFT_WHITE,
            TFT_BLACK
        );

        tft.setCursor(4, y);
        tft.print(ssid);

        // Security
        tft.setTextColor(
            net.security == WIFI_AUTH_OPEN
                ? TFT_DARKGREY
                : TFT_ORANGE,
            TFT_BLACK
        );

        tft.setCursor(180, y);

        if (net.security == WIFI_AUTH_OPEN)
            tft.print("OPEN");
        else
            tft.print("SEC");

        // Channel
        tft.setTextColor(
            TFT_CYAN,
            TFT_BLACK
        );

        tft.setCursor(225, y);

        tft.printf(
            "Ch%2d",
            net.channel
        );

        // RSSI
        tft.setTextColor(
            rssiColor(net.rssi),
            TFT_BLACK
        );

        tft.setCursor(265, y);

        tft.printf(
            "%4ddBm",
            net.rssi
        );
    }

    if (cachedCount >
        MAX_VISIBLE_ROWS)
    {
        tft.setTextColor(
            TFT_DARKGREY,
            TFT_BLACK
        );

        tft.setCursor(
            4,
            SCREEN_H - 11
        );

        tft.printf(
            "+ %d more",
            cachedCount -
                MAX_VISIBLE_ROWS
        );
    }
}


void cacheScanResults(int count)
{
    if (count < 0)
    {
        cachedCount = 0;
        return;
    }

    count =
        min(count, MAX_NETWORKS);

    int idx[MAX_NETWORKS];

    for (int i = 0;
         i < count;
         i++)
    {
        idx[i] = i;
    }

    // Sort strongest RSSI first
    for (int i = 0;
         i < count - 1;
         i++)
    {
        for (int j = i + 1;
             j < count;
             j++)
        {
            if (
                WiFi.RSSI(idx[j]) >
                WiFi.RSSI(idx[i])
            )
            {
                int temp = idx[i];
                idx[i] = idx[j];
                idx[j] = temp;
            }
        }
    }

    cachedCount = count;

    for (int row = 0;
         row < count;
         row++)
    {
        int i = idx[row];

        String ssid =
            WiFi.SSID(i);

        if (ssid.length() == 0)
            ssid = "[hidden]";

        cachedNetworks[row].ssid =
            ssid;

        cachedNetworks[row].bssid =
            WiFi.BSSIDstr(i);

        cachedNetworks[row].rssi =
            WiFi.RSSI(i);

        cachedNetworks[row].channel =
            WiFi.channel(i);

        cachedNetworks[row].security =
            WiFi.encryptionType(i);
    }
}


void performScan()
{
    Serial.println();
    Serial.println(
        "========================================"
    );

    Serial.println(
        "       NETWORK SCOUT - WIFI SCAN"
    );

    Serial.println(
        "========================================"
    );

    statusLed(
        false,
        false,
        true
    );

    drawHeader(true);

    WiFi.scanDelete();

    Serial.println(
        "Starting scan..."
    );

    // Synchronous STA-only scan
    // false = not asynchronous
    // true  = show hidden networks
    int count =
        WiFi.scanNetworks(
            false,
            true
        );

    Serial.printf(
        "Scan returned %d\n",
        count
    );

    if (count < 0)
    {
        Serial.println(
            "SCAN FAILED"
        );

        statusLed(
            true,
            false,
            false
        );

        tft.fillScreen(
            TFT_BLACK
        );

        tft.setTextColor(
            TFT_RED,
            TFT_BLACK
        );

        tft.setTextSize(2);
        tft.setCursor(70, 100);

        tft.println(
            "SCAN FAILED"
        );

        WiFi.scanDelete();

        lastScanFinished =
            millis();

        return;
    }

    cacheScanResults(count);

    Serial.printf(
        "Found %d networks\n\n",
        cachedCount
    );

    for (int i = 0;
         i < cachedCount;
         i++)
    {
        NetworkInfo &net =
            cachedNetworks[i];

        Serial.printf(
            "%2d. %s\n",
            i + 1,
            net.ssid.c_str()
        );

        Serial.printf(
            "    BSSID:    %s\n",
            net.bssid.c_str()
        );

        Serial.printf(
            "    RSSI:     %d dBm\n",
            net.rssi
        );

        Serial.printf(
            "    Channel:  %d\n",
            net.channel
        );

        Serial.printf(
            "    Security: %s\n\n",
            securityName(
                net.security
            )
        );
    }

    drawNetworkList();

    statusLed(
        false,
        true,
        false
    );

    WiFi.scanDelete();

    lastScanFinished =
        millis();
}


// ============================================================
// POWER BUTTON
//
// GPIO35 has NO internal pull-up.
// Keep the external 10K resistor:
//
// 3V3 --- 10K --- GPIO35
//                  |
//               SWITCH
//                  |
//                 GND
//
// ============================================================

#define PWR_BUTTON_PIN 35

bool pwrButtonWasPressed = false;

unsigned long
    pwrButtonPressedAt = 0;

const unsigned long
    PWR_DEBOUNCE_MS = 300;


void enterDeepSleep()
{
    Serial.println(
        "Power button pressed - shutting down..."
    );

    tft.fillScreen(
        TFT_BLACK
    );

    tft.setTextColor(
        TFT_WHITE,
        TFT_BLACK
    );

    tft.setTextSize(2);

    tft.setCursor(
        40,
        100
    );

    tft.println(
        "Powering off..."
    );

    delay(800);

    tft.fillScreen(
        TFT_BLACK
    );

    tft.setBrightness(0);

    // Stop PWM control of the RGB LED
ledcDetachPin(LED_RED_PIN);
ledcDetachPin(LED_GREEN_PIN);
ledcDetachPin(LED_BLUE_PIN);

// Active-low LEDs: HIGH = OFF
pinMode(LED_RED_PIN, OUTPUT);
pinMode(LED_GREEN_PIN, OUTPUT);
pinMode(LED_BLUE_PIN, OUTPUT);

digitalWrite(LED_RED_PIN, HIGH);
digitalWrite(LED_GREEN_PIN, HIGH);
digitalWrite(LED_BLUE_PIN, HIGH);

WiFi.mode(WIFI_OFF);

    WiFi.mode(
        WIFI_OFF
    );

    // Wait for button release
    while (
        digitalRead(
            PWR_BUTTON_PIN
        ) == LOW
    )
    {
        delay(10);
    }

    delay(50);

    // Wake when GPIO35 is
    // pulled LOW again
    esp_sleep_enable_ext0_wakeup(
        (gpio_num_t)
            PWR_BUTTON_PIN,
        0
    );

    esp_deep_sleep_start();
}


void checkPowerButton()
{
    bool pressed =
        digitalRead(
            PWR_BUTTON_PIN
        ) == LOW;

    if (
        pressed &&
        !pwrButtonWasPressed
    )
    {
        pwrButtonWasPressed =
            true;

        pwrButtonPressedAt =
            millis();
    }

    else if (
        pressed &&
        pwrButtonWasPressed &&
        millis() -
            pwrButtonPressedAt >
            PWR_DEBOUNCE_MS
    )
    {
        enterDeepSleep();
    }

    else if (!pressed)
    {
        pwrButtonWasPressed =
            false;
    }
}


// ============================================================
// SETUP
// ============================================================

void setup()
{
    Serial.begin(115200);
    delay(300);

    // GPIO35 needs the external 10K
    // pull-up to 3V3
    pinMode(
        PWR_BUTTON_PIN,
        INPUT
    );

    // If waking from the button,
    // wait until it is released.
    unsigned long releaseWaitStart =
        millis();

    while (
        digitalRead(
            PWR_BUTTON_PIN
        ) == LOW &&
        millis() -
            releaseWaitStart <
            3000
    )
    {
        delay(10);
    }

    // ---------- LED ----------
    ledSetup();

    statusLed(
        false,
        false,
        false
    );

    // ---------- Display ----------
    tft.init();

    tft.setRotation(1);

    tft.setBrightness(255);

    tft.fillScreen(
        TFT_BLACK
    );
    //  Initializing the touchscreen here
    

    // ---------- Wi-Fi ----------
    //
    // STA ONLY.
    // No hotspot.
    // No web server.
    //
    WiFi.mode(
        WIFI_STA
    );

    WiFi.setAutoReconnect(
        false
    );

    WiFi.disconnect(
        false,
        false
    );

    delay(250);

    Serial.println();
    Serial.println(
        "Network Scout booting..."
    );

    Serial.print(
        "ESP32 MAC: "
    );

    Serial.println(
        WiFi.macAddress()
    );

    Serial.println(
        "Wi-Fi mode: STA only"
    );

    // ---------- Splash ----------
    tft.setTextColor(
        TFT_CYAN,
        TFT_BLACK
    );

    tft.setTextSize(2);

    tft.setCursor(
        72,
        75
    );

    tft.println(
        "NETWORK SCOUT"
    );

    tft.setTextSize(1);

    tft.setTextColor(
        TFT_WHITE,
        TFT_BLACK
    );

    tft.setCursor(
        92,
        110
    );

    tft.println(
        "STA Wi-Fi Scanner"
    );

    tft.setCursor(
        93,
        130
    );

    tft.println(
        "Starting scan..."
    );

    delay(1500);

    tft.fillScreen(
        TFT_BLACK
    );

    performScan();
}
// ============================================================
// LOOP
// ============================================================

void loop()
{
    checkPowerButton();

    if (
        millis() -
            lastScanFinished >=
        SCAN_INTERVAL_MS
    )
    {
        performScan();
    }

    delay(10);
}
