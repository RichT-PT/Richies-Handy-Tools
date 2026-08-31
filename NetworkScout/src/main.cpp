#include <Arduino.h>
#include <WiFi.h>

void printEncryption(wifi_auth_mode_t type)
{
    switch (type)
    {
        case WIFI_AUTH_OPEN:          Serial.print("OPEN"); break;
        case WIFI_AUTH_WEP:           Serial.print("WEP"); break;
        case WIFI_AUTH_WPA_PSK:       Serial.print("WPA"); break;
        case WIFI_AUTH_WPA2_PSK:      Serial.print("WPA2"); break;
        case WIFI_AUTH_WPA_WPA2_PSK:  Serial.print("WPA/WPA2"); break;
        case WIFI_AUTH_WPA2_ENTERPRISE: Serial.print("WPA2-ENT"); break;
        case WIFI_AUTH_WPA3_PSK:      Serial.print("WPA3"); break;
        case WIFI_AUTH_WPA2_WPA3_PSK: Serial.print("WPA2/WPA3"); break;
        default:                       Serial.print("UNKNOWN"); break;
    }
}

void scanWiFi()
{
    Serial.println();
    Serial.println("========================================");
    Serial.println("       NETWORK SCOUT - WIFI SCAN");
    Serial.println("========================================");

    int count = WiFi.scanNetworks(false, true);

    if (count <= 0)
    {
        Serial.println("No networks found.");
        return;
    }

    Serial.printf("Found %d networks\n\n", count);

    for (int i = 0; i < count; i++)
    {
        Serial.printf("%2d. %s\n",
                      i + 1,
                      WiFi.SSID(i).length() ? WiFi.SSID(i).c_str() : "<hidden>");

        Serial.printf("    BSSID:    %s\n", WiFi.BSSIDstr(i).c_str());
        Serial.printf("    RSSI:     %d dBm\n", WiFi.RSSI(i));
        Serial.printf("    Channel:  %d\n", WiFi.channel(i));

        Serial.print("    Security: ");
        printEncryption(WiFi.encryptionType(i));
        Serial.println();
        Serial.println();
    }

    WiFi.scanDelete();
}

void setup()
{
    Serial.begin(115200);
    delay(1000);

    Serial.println();
    Serial.println("Network Scout booting...");

    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(250);

    Serial.print("ESP32 MAC: ");
    Serial.println(WiFi.macAddress());

    scanWiFi();
}

void loop()
{
    delay(10000);
    scanWiFi();
}