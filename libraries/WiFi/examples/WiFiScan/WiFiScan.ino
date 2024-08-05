/*
 *  This sketch demonstrates how to scan WiFi networks.
 *  The API is based on the Arduino WiFi Shield library, but has significant changes as newer WiFi functions are supported.
 *  E.g. the return value of `encryptionType()` different because more modern encryption is supported.
 */
#include "WiFi.h"
void setup()
{
    Serial.begin(115200);

    // Set WiFi to station mode and disconnect from an AP if it was previously connected.
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);

    Serial.println("Setup done");

}

void loop()
{
    Serial.println("Scan start");

    // WiFi.scanNetworks will return the number of networks found.
    int n = WiFi.scanNetworks();

   Serial.println("Scan done");
    if (n == 0) {
        Serial.println("no networks found");
    } else {
        Serial.print(n);
        Serial.println(" networks found");
        Serial.println("Nr | SSID                             | RSSI | CH | Encryption");
        for (int i = 0; i < n; ++i) {
            // Print SSID and RSSI for each network found
            Serial.print(i + 1);
            Serial.print(" | \t\t");
            Serial.print(WiFi.SSID(i));
            Serial.print("\t\t | \t");
            Serial.print( WiFi.RSSI(i));
            Serial.print("\t\t | \t");
            Serial.print( WiFi.channel(i));
            Serial.print("\t\t | \t");
            switch (WiFi.encryptionType(i))
            {
            case WAAM_OPEN:
                Serial.print("open");
                break;
            case WAAM_WEP:
                Serial.print("WEP");
                break;
            case WAAM_WPA_PSK:
                Serial.print("WPA PSK");
                break;
            case WAAM_WPA2_PSK:
                Serial.print("WPA2 PSK");
                break;
            case WAAM_WPA_WPA2_PSK:
                Serial.print("WPA + WPA2");
                break;
            case WAAM_WPA_WPA3_SAE:
                Serial.print("WPA3");
                break;
            default:
                Serial.print("unknown");
            }
            Serial.println();
            delay(10);
        }
    }
    Serial.println("");
    // Delete the scan result to free memory for code below.
    WiFi.scanDelete();

    // Wait a bit before scanning again.
    delay(5000);
}
