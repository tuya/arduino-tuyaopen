/*
 Publishing in the callback

  - connects to an MQTT server
  - subscribes to the topic "inTopic"
  - when a message is received, republishes it to "outTopic"

  This example shows how to publish messages within the
  callback function. The callback function header needs to
  be declared before the PubSubClient constructor and the
  actual callback defined afterwards.
  This ensures the client reference in the callback function
  is valid.

*/

#include <WiFi.h>
#include <MQTTClient.h>
#include <Log.h>
// Update these with values suitable for your network.

const char* ssid = "your_ssid";
const char* password = "your_passwd";
const char* mqtt_server = "broker.emqx.io";
const char *mqtt_username = "emqx";
const char *mqtt_password ="public";

void *cli;
MQTTClient mqtt;

// Callback function
void mqtt_message_cb(void *client, uint16_t msgid, const mqtt_client_message_t *msg, void *userdata)
{
  char message[256] = {0};
  memcpy(message ,msg->payload ,msg->length); 
  Serial.print("message:");
  Serial.println(message);
  mqtt.publish(cli,"youroutTopic",message);
}

void  setup()
{
  Log.setLevel(LogClass::DEBUG);
  Serial.begin(115200);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WSS_GOT_IP) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  mqtt_client_config_t config = {0};
  config.cacert = NULL;
  config.cacert_len = 0;
  config.host = mqtt_server;
  config.port = 1883;
  config.keepalive = 120;
  config.timeout_ms = 8000;
  config.username = mqtt_username;
  config.password = mqtt_password;
  config.clientid = "ty_test_id"; 

  mqtt.SetCallback(mqtt_message_cb);
  cli = mqtt.init(&config);
}

void loop()
{
  if (!mqtt.connected()) {
    mqtt.connect(cli);
    mqtt.publish(cli,"youroutTopic","hello world");
    mqtt.subscribe(cli,"inTopic");
  }
  mqtt.loop(cli);
}
