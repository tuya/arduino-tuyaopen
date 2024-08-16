/*
 Reconnecting MQTT example - non-blocking

 This sketch demonstrates how to keep the client connected
 using a non-blocking reconnect function. If the client loses
 its connection, it attempts to reconnect every 5 seconds
 without blocking the main loop.

*/

#include <WiFi.h>
#include <PubSubClient.h>

// Update these with values suitable for your hardware/network.
const char *ssid = "your_ssid";
const char *pass = "your_passwd";
const char *mqtt_server = "broker.emqx.io";

void callback(char* topic, byte* payload, unsigned int length) {
  // handle message arrived
}

WiFiClient TESTClient;
PubSubClient client(TESTClient);

long lastReconnectAttempt = 0;

boolean reconnect() {
  if (client.connect("arduinoClient")) {
    Serial.println("connected");
    // Once connected, publish an announcement...
    client.publish("outTopic","hello world");
    // ... and resubscribe
    client.subscribe("inTopic");
  }
  return client.state();
}

void setup()
{
  WiFi.begin(ssid,pass);
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  
  delay(1500);
  lastReconnectAttempt = 0;
}


void loop()
{
  if (!client.connected()) {
    long now = millis();
    if (now - lastReconnectAttempt > 5000) {
      lastReconnectAttempt = now;
      // Attempt to reconnect
      if (reconnect()) {
        lastReconnectAttempt = 0;
      }
    }
  } else {
    // Client connected

    client.loop();
  }

}
