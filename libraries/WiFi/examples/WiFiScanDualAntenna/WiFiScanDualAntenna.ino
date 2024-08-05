#include "WiFi.h"
#define GPIO_ANT1 2
#define GPIO_ANT2 25
void setup()
{
    bool err = false;
    Serial.begin(115200);

    WiFi.mode(WIFI_STA);
    err = WiFi.setDualAntennaConfig(GPIO_ANT1, GPIO_ANT2, WIFI_RX_ANT_AUTO, WIFI_TX_ANT_AUTO);
    
    if(err == false) {
        Serial.println("Dual Antenna configuration failed!");
    } else {
        Serial.println("Dual Antenna configuration successfuly done!");
    }

    WiFi.disconnect();
    delay(100);

    Serial.println("Setup done");
}

void loop()
{
    Serial.println("scan start");

    // WiFi.scanNetworks will return the number of networks found
    int n = WiFi.scanNetworks();
    Serial.println("scan done");
    if (n == 0) {
        Serial.println("no networks found");
    } else {
        Serial.print(n);
        Serial.println(" networks found");
        for (int i = 0; i < n; ++i) {
            // Print SSID and RSSI for each network found
            Serial.print(i + 1);
            Serial.print(": ");
            Serial.print(WiFi.SSID(i));
            Serial.print(" (");
            Serial.print(WiFi.RSSI(i));
            Serial.print(")");
            Serial.println((WiFi.encryptionType(i) == WAAM_OPEN)?" ":"*");
            delay(10);
        }
    }
    Serial.println("");

    // Wait a bit before scanning again
    delay(5000);
}
