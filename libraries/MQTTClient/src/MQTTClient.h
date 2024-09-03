#ifndef __MQTTCLIENT_H__
#define __MQTTCLIENT_H__

#ifdef __cplusplus
extern "C" {
#endif
#include "mqtt_client_interface.h"
#include "tal_log.h"
#define MQTT_MSG_CB void (*callback)(void *client, uint16_t msgid, const mqtt_client_message_t *msg, void *userdata)
class MQTTClient
{
private:
    MQTT_MSG_CB;
    bool is_connected =false;
public:
    MQTTClient();
    ~MQTTClient();

    MQTTClient& SetCallback(MQTT_MSG_CB);
    void* init(mqtt_client_config_t *config);
    int connect(void *client);
    int connected();
    int disconnect(void *client);
    int publish(void *client, const char *topic, const char *payload);

    int subscribe(void *client,const char *topic);
    int unsubscribe(void *client,const char *topic);
    int loop(void *client);
    void free(void *client);
};



#ifdef __cplusplus
}
#endif

#endif
