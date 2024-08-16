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
const char * pass = "your_passwd";

  //MQTT broker
const char *mqtt_broker = "broker.emqx.io";
const char *topic = "outTopic";
const char *mqtt_username = "emqx";
const char *mqtt_password ="public";
const int mqtt_port = 1883;

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived in topic");
  Serial.print(topic);
  Serial.print("Message : ");
  for (int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

WiFiClient TESTClient;
PubSubClient client(TESTClient);

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("arduinoClient")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish(topic,"hello world");
      // ... and resubscribe
      client.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

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
  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);

  while (!client.connected()) {
     String client_id = "ty-client-";
     client_id += String(WiFi.macAddress());
     Serial.print("The client ");
     Serial.print(client_id.c_str());
     Serial.println("connects to the public mqtt broker");
     if (client.connect(client_id.c_str(), mqtt_username, mqtt_password))
     {
         Serial.println("Public emqx mqtt broker connected");
     } 
     else {
         Serial.print("failed with state ");
         Serial.print(client.state());
         delay(2000);
     }
 }
  // publish and subscribe
  client.publish(topic, "Hi EMQX I'm Tyta ^^");
  client.subscribe(topic);
  
}


void loop()
{
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
