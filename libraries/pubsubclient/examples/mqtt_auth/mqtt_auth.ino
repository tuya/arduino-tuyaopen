/*
 Basic MQTT example with Authentication

  - connects to an MQTT server, providing username
    and password
  - publishes "hello world" to the topic "outTopic"
  - subscribes to the topic "inTopic"
*/


#include <WiFi.h>
#include <PubSubClient.h>

// Update these with values suitable for your network.
const char *ssid = "your_ssid";
const char *pass = "your_passwd";

//MQTT broker
const char *mqtt_broker = "broker.emqx.io";
const char *topic = "outTopic";
const char *mqtt_username = "emqx"; // No necessarily
const char *mqtt_password = "public"; // No necessarily
const int mqtt_port = 1883;

void callback(char* topic, byte* payload, unsigned int length) {
  // handle message arrived
}

WiFiClient TESTClient;
PubSubClient client(TESTClient);

void setup()
{
  Serial.begin(115200);
  WiFi.begin(ssid,pass);
  while(WiFi.status() != WSS_GOT_IP)
  {
    delay(500);
    Serial.println("...");
  }
  Serial.println("");
  Serial.println("");
  Serial.println("WiFi connected!");
  Serial.println("IP Address:");
  Serial.println(WiFi.localIP());

  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);

  if (client.connect("arduinoClient", mqtt_username, mqtt_password)) {
    client.publish(topic,"hello world");
    client.subscribe(topic);
  }
}

void loop()
{
  client.loop();
}
