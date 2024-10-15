#include <WiFi.h>
#include <WiFiUdp.h>

// WiFi network name and password:
const char * networkName = "********";
const char * networkPswd = "********";

const int udpPort = 4556;

#define UDP_TX_MAX_SIZE 1024

// buffers for receiving and sending data
char packetBuffer[UDP_TX_MAX_SIZE + 1];  // buffer to hold incoming packet,
char ReplyBuffer[] = "acknowledged\r\n";  
//Are we currently connected?
boolean connected = false;

//The udp library class
WiFiUDP udp;

void setup(){
  // Initialize hardware serial:
  Serial.begin(115200);
  
  //Connect to the WiFi network
    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(networkName);

    WiFi.begin(networkName,networkPswd);
    while (WiFi.status() != WSS_GOT_IP) {
        delay(500);
        Serial.print(".");
    }
    
    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    connected = true;
    udp.begin(WiFi.localIP(),udpPort);

}

void loop(){
  //only send data when connected
  int packetSize = udp.parsePacket();
  if (packetSize) {
    Serial.print("packet size: ");
    Serial.println(packetSize);
    Serial.print("remoteIP: ");
    Serial.println(udp.remoteIP().toString());
    Serial.print("remotePort: ");
    Serial.println(udp.remotePort());

    // read the packet into packetBufffer
    int n = udp.read(packetBuffer, UDP_TX_MAX_SIZE);
    packetBuffer[n] = 0;
    Serial.println("Contents:");
    Serial.println(packetBuffer);

    // send a reply, to the IP address and port that sent us the packet we received
    udp.beginPacket(udp.remoteIP(), udp.remotePort());
    udp.write((const uint8_t*)ReplyBuffer, 14);
    udp.endPacket();
  }
  //Wait for 1 second
  delay(1000);
}

