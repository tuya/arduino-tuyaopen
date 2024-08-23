/**
 * BasicHTTPClient.ino
 *
 *  Created on: 24.05.2015
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
        USE_SERIAL.print("[SETUP] WAIT ...");
        USE_SERIAL.println(t);     
        USE_SERIAL.flush();
        delay(1000);
    }

    WiFi.begin("YOUR_SSID", "PASSWD");
 
}

void loop() {
    // wait for WiFi connection
    if((WiFi.status() == WSS_GOT_IP)) {

        http.begin("http://www.moe.gov.cn/jyb_sjzl/");

        http_client_response_t http_response ={0};

        http_client_header_t headers[] = {{.key = "Content-Type", .value = "application/json"}};

        int headers_length = sizeof(headers)/sizeof(http_client_header_t);

        http_client_status_t http_status = http.GET(headers,(uint8_t)headers_length,NULL,0,&http_response);
        
       if(http_status != HTTP_CLIENT_SUCCESS)
        {
            USE_SERIAL.print("http_request_send error:");
            USE_SERIAL.println(http_status);
        }else{
            char * payload = (char *)http_response.body;
            USE_SERIAL.println(payload);
        }
        http.end(&http_response);
    }
   delay(5000);
}
