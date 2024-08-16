/**
 * Authorization.ino
 *
 *  Created on: 09.12.2015
 *
 */

#include <Arduino.h>

#include <WiFi.h>

#include <HTTPClient.h>

#define USE_SERIAL Serial

void setup() {

    USE_SERIAL.begin(115200);

    USE_SERIAL.println();
    USE_SERIAL.println();
    USE_SERIAL.println();

    for(uint8_t t = 4; t > 0; t--) {
        USE_SERIAL.print("[SETUP] WAIT ...\n");
        USE_SERIAL.println(t);
        USE_SERIAL.flush();
        delay(1000);
    }

    WiFi.begin("SSID", "PASSWORD");

}

void loop() {
    // wait for WiFi connection
    if((WiFi.status() == WSS_GOT_IP)) {

        HTTPClient http;

        USE_SERIAL.print("[HTTP] begin...\n");
        // configure traged server and url

        http.begin("http://user:password@192.168.1.12/test.html");
   
        /*
          // or
          http.begin("http://192.168.1.12/test.html");
          http.setAuthorization("user", "password");

          // or
          http.begin("http://192.168.1.12/test.html");
          http.setAuthorization("dXNlcjpwYXN3b3Jk");
         */


        USE_SERIAL.print("[HTTP] GET...\n");
        // start connection and send HTTP header
        int httpCode = http.GET();

        // httpCode will be negative on error
        if(httpCode > 0) {
            // HTTP header has been send and Server response header has been handled
            USE_SERIAL.print("[HTTP] GET... code: ");
            USE_SERIAL.println(httpCode);

            // file found at server
            if(httpCode == HTTP_CODE_OK) {
                String payload = http.getString();
                USE_SERIAL.println(payload);
            }
        } else {
            USE_SERIAL.print("[HTTP] GET... failed, error: ");
            USE_SERIAL.println(http.errorToString(httpCode).c_str());
        }

        http.end();
    }

    delay(10000);
}

