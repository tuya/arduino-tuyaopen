/*
 Basic MQTT example

 This sketch demonstrates the basic capabilities of the library.
 It connects to an MQTT server then:
  - publishes "hello world" to the topic "outTopic"
  - subscribes to the topic "inTopic", printing out any messages
    it receives. NB - it assumes the received payloads are strings not binary

 It will reconnect to the server if the connection is lost using a blocking
 reconnect function. See the 'mqtt_reconnect_nonblocking' example for how to
 achieve the same result without blocking the main loop.
 
*/
/*
 Basic MQTT example

 This sketch demonstrates the basic capabilities of the library.
 It connects to an MQTT server then:
  - publishes "hello world" to the topic "outTopic"
  - subscribes to the topic "inTopic", printing out any messages
    it receives. NB - it assumes the received payloads are strings not binary

 It will reconnect to the server if the connection is lost using a blocking
 reconnect function. See the 'mqtt_reconnect_nonblocking' example for how to
 achieve the same result without blocking the main loop.
 
*/

#include <WiFi.h>
#include <PubSubClient.h>

// Update these with values suitable for your network.
const char * ssid ="your_ssid";
const char * pass = "your_paswd";

//MQTT broker
const char *mqtt_broker = "broker.emqx.io";
const char *topic = "youroutTopic";
const char *mqtt_username = "emqx";
const char *mqtt_password ="public";
const int mqtt_port = 1883;

void *cli;
mqtt_client_config_t config;
PubSubClient mqtt;
void setup()
{
  Serial.begin(115200);
  WiFi.begin(ssid,pass);

  while (WiFi.status() != WSS_GOT_IP) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("wifi connected ");

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
  int payload_length = strlen(payload);
  
  cli = mqtt.init(&config);
  mqtt.connect(cli);
  mqtt.publish(cli,topic,(const uint8_t*)payload,payload_length);
  mqtt.subscribe(cli,topic);
}

void loop(){

  mqtt.loop(cli);
}