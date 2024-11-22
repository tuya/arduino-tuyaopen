#include "MQTTClient.h"


MQTTClient::MQTTClient()
{
}

MQTTClient::~MQTTClient()
{
}

MQTTClient& MQTTClient::SetCallback(MQTT_MSG_CB)
{
    this->callback = callback;
    return *this;
}

static void mqtt_connected_cb(void *client, void *userdata)
{
    PR_INFO("mqtt client connected!");
}

static void mqtt_disconnected_cb(void *client, void *userdata)
{
    PR_INFO("mqtt client disconnected!");
}



static void mqtt_subscribed_cb(void *client, uint16_t msgid, void *userdata)
{
    PR_DEBUG("Subscribe successed ID:%d", msgid);
}

static void mqtt_puback_cb(void *client, uint16_t msgid, void *userdata)
{

    PR_DEBUG("PUBACK ID:%d", msgid);
}


void * MQTTClient::init(mqtt_client_config_t *config)
{
    mqtt_client_status_t mqtt_status;
    void *cli= mqtt_client_new();
    if (cli == NULL) {
        PR_ERR("mqtt client new fault.");
        return NULL;
    }

    const mqtt_client_config_t mqtt_config = {.cacert = config->cacert,
                                              .cacert_len = config->cacert_len,
                                              .host = config->host,
                                              .port = config->port,
                                              .keepalive = config->keepalive,
                                              .timeout_ms = config->timeout_ms,
                                              .clientid = config->clientid,
                                              .username = config->username,
                                              .password = config->password,
                                              .on_connected = mqtt_connected_cb,
                                              .on_disconnected = mqtt_disconnected_cb,
                                              .on_message = this->callback,
                                              .on_published = mqtt_puback_cb,
                                              .on_subscribed = mqtt_subscribed_cb,
                                              };
    mqtt_status = mqtt_client_init(cli, &mqtt_config);
    if (mqtt_status != MQTT_STATUS_SUCCESS) {
        PR_ERR("MQTT init failed: Status = %d.", mqtt_status);
        return NULL;
    }
    PR_ERR("MQTT init OK: Status = %d.", mqtt_status);
    return cli;
}

int MQTTClient::connect(void *client)
{
    if(client==NULL){
          return OPRT_INVALID_PARM;
    }
    if(is_connected)
        return 0;
    PR_INFO("tuya_mqtt_start...");
    mqtt_client_status_t mqtt_status;
    mqtt_status = mqtt_client_connect(client);
    if(mqtt_status != MQTT_STATUS_SUCCESS)
    {
        PR_ERR("MQTT connect fail:%d", mqtt_status);
        return OPRT_COM_ERROR;
    }
    is_connected = true;
    PR_ERR("MQTT connect OK:%d", mqtt_status);
    return OPRT_OK;
}

int MQTTClient::connected()
{
    return is_connected;
}

int MQTTClient::disconnect(void *client)
{
    if(client == NULL)
        return OPRT_INVALID_PARM;
    if(is_connected)
    {
        mqtt_client_status_t mqtt_status;
        mqtt_status = mqtt_client_disconnect(client);
        PR_ERR("MQTT disconenct :%d", mqtt_status);
        is_connected = false;
        return OPRT_OK;
    }
    return OPRT_INVALID_PARM;
}

int MQTTClient::publish(void *client, const char *topic, const char *payload)
{
    int payload_len = strlen(payload);
    uint16_t msgid = mqtt_client_publish(client, topic, (uint8_t*)payload, payload_len, MQTT_QOS_0);
    if (msgid <= 0) {
        return OPRT_COM_ERROR;
    }
    return OPRT_OK;
}

int MQTTClient::subscribe(void *client,const char *topic)
{
    uint16_t msgid = mqtt_client_subscribe(client, topic, MQTT_QOS_1);
    if (msgid <= 0) {
        return OPRT_COM_ERROR;
    }
     return OPRT_OK;
}

int MQTTClient::unsubscribe(void *client,const char *topic)
{
    uint16_t msgid = mqtt_client_unsubscribe(client, topic, MQTT_QOS_1);
    if (msgid <= 0) {
        return OPRT_COM_ERROR;
    }
     return OPRT_OK;
}

int MQTTClient :: loop(void *client)
{
    if (client == NULL) {
        return OPRT_COM_ERROR;
    } 
    int rt = OPRT_OK;
    rt = mqtt_client_yield(client);
    return rt;
}
void MQTTClient::free(void *client)
{
    mqtt_client_free(client);
}
