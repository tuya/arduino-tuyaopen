#ifndef __PUBSUBCLIENT_H__
#define __PUBSUBCLIENT_H__

#ifdef __cplusplus
extern "C" {
#endif
#include "mqtt_client_interface.h"
#include "tal_log.h"

class PubSubClient
{
public:
    PubSubClient();
    ~PubSubClient();
    void* init(mqtt_client_config_t *config);
    int connect(void *cli);
    int disconnect(void *cli);
    int publish(void *client, const char *topic, const uint8_t *payload,  size_t payload_length);

    int subscribe(void *client,const char *topic);
    int unsubscribe(void *client,const char *topic);
    int loop(void *client);

};



#ifdef __cplusplus
}
#endif

#endif
