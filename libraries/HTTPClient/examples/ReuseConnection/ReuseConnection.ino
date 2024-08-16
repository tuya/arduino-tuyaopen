/**
 * reuseConnection.ino
 *
 *  Created on: 22.11.2015
 *
 */


#include <Arduino.h>

#include <WiFi.h>

#include <HTTPClient.h>

#define USE_SERIAL Serial


HTTPClient http;

void setup() {

    USE_SERIAL.begin(115200);

    USE_SERIAL.println();
    USE_SERIAL.println();
    USE_SERIAL.println();

    for(uint8_t t = 4; t > 0; t--) {
        USE_SERIAL.print("[SETUP] WAIT ..");
        USE_SERIAL.print( t);
        USE_SERIAL.flush();
        delay(1000);
    }

    WiFi.begin("SSID", "PASSWORD");

    // allow reuse (if server supports it)
    http.setReuse(true);
}

void loop() {
    // wait for WiFi connection
    if((WiFi.status() == WSS_GOT_IP)) {

        //http.begin("http://192.168.1.12/test.html");
        //http.begin("192.168.1.12", 80, "/test.html");
        http.begin("https://www.cnblogs.com/topgoking/p/11505376.html");
        int httpCode = http.GET();
        if(httpCode > 0) {
            USE_SERIAL.print("[HTTP] GET... code: " );
            USE_SERIAL.println(httpCode);
            // file found at server
            if(httpCode == HTTP_CODE_OK) {
                http.writeToStream(&USE_SERIAL);
            }
        } else {
            USE_SERIAL.print("[HTTP] GET... failed, error: " );
            USE_SERIAL.print(http.errorToString(httpCode).c_str());
        }

        http.end();
    }

    delay(1000);
}



