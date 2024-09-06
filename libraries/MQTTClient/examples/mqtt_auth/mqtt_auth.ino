/*
 Basic MQTT example with Authentication

  - connects to an MQTT server, providing username
    and password
  - publishes "hello world" to the topic "outTopic"
  - subscribes to the topic "inTopic"
*/


#include <WiFi.h>
#include <MQTTClient.h>

// Update these with values suitable for your network.
const char *ssid = "your_ssid";
const char *pass = "your_passwd";

//MQTT broker
const char *mqtt_broker = "broker.emqx.io";
const char *topic = "youroutTopic";
const char *mqtt_username = "emqx"; // No necessarily
const char *mqtt_password = "public"; // No necessarily
const int mqtt_port = 1883;

void *cli;
MQTTClient mqtt;

void callback(void *client, uint16_t msgid, const mqtt_client_message_t *msg, void *userdata){
  // handle message arrived
}

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

  mqtt_client_config_t config;
  config.cacert = NULL;
  config.cacert_len = 0;
  config.host = mqtt_broker;
  config.port = mqtt_port;
  config.keepalive = 15;
  config.timeout_ms = 100;
  config.username = mqtt_username;
  config.password = mqtt_password;
  config.clientid = "ty_test_id"; 

  const char *payload="asadadaadadasd";  
  mqtt.SetCallback(callback);
  cli = mqtt.init(&config);
  mqtt.connect(cli);
  mqtt.publish(cli,topic,payload);
  mqtt.subscribe(cli,topic);
}

void loop()
{
  mqtt.loop(cli);
}
