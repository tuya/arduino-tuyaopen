#include "PubSubClient.h"


PubSubClient::PubSubClient()
{
}

PubSubClient::~PubSubClient()
{
}

static void mqtt_connected_cb(void *client, void *userdata)
{
  PR_INFO("mqtt client connected!");
}

static void mqtt_disconnected_cb(void *client, void *userdata)
{
    PR_INFO("mqtt client disconnected!");
}

static void mqtt_message_cb(void *client, uint16_t msgid, const mqtt_client_message_t *msg, void *userdata)
{
  PR_INFO("recv message TopicName:%s, payload len:%d", msg->topic, msg->length);
  char message [256] = {0};
  memcpy(message ,msg->payload ,msg->length); 
  PR_INFO("message:%s \r\n",message);
}

static void mqtt_subscribed_cb(void *client, uint16_t msgid, void *userdata)
{
    PR_DEBUG("Subscribe successed ID:%d", msgid);
}

static void mqtt_puback_cb(void *client, uint16_t msgid, void *userdata)
{

    PR_DEBUG("PUBACK ID:%d", msgid);
}


void * PubSubClient::init(mqtt_client_config_t *config)
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
                                              .on_message = mqtt_message_cb,
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

int PubSubClient::connect(void *cli)
{
    if(cli==NULL){
          return OPRT_INVALID_PARM;
    }

    PR_INFO("tuya_mqtt_start...");
    mqtt_client_status_t mqtt_status;
    mqtt_status = mqtt_client_connect(cli);
    if(mqtt_status != MQTT_STATUS_SUCCESS)
    {
        PR_ERR("MQTT connect fail:%d", mqtt_status);
        return OPRT_COM_ERROR;
    }
    PR_ERR("MQTT connect OK:%d", mqtt_status);
    return OPRT_OK;
}

int PubSubClient::disconnect(void *cli)
{
    if(cli == NULL)
        return OPRT_INVALID_PARM;
    mqtt_client_status_t mqtt_status;
    mqtt_status = mqtt_client_disconnect(cli);
    PR_ERR("MQTT disconenct :%d", mqtt_status);
    return OPRT_OK;
}

int PubSubClient::publish(void *client, const char *topic, const uint8_t *payload,  size_t payload_length)
{
    uint16_t msgid = mqtt_client_publish(client, topic, payload, payload_length, MQTT_QOS_0);
    if (msgid <= 0) {
        return OPRT_COM_ERROR;
    }
    return OPRT_OK;
}
int PubSubClient::subscribe(void *client,const char *topic)
{
    uint16_t msgid = mqtt_client_subscribe(client, topic, MQTT_QOS_1);
    if (msgid <= 0) {
        return OPRT_COM_ERROR;
    }
     return OPRT_OK;
}

int PubSubClient::unsubscribe(void *client,const char *topic)
{
    uint16_t msgid = mqtt_client_unsubscribe(client, topic, MQTT_QOS_1);
    if (msgid <= 0) {
        return OPRT_COM_ERROR;
    }
     return OPRT_OK;
}

int PubSubClient :: loop(void *client)
{
    if (client == NULL) {
        return OPRT_COM_ERROR;
    } 
    int rt = OPRT_OK;
    rt = mqtt_client_yield(client);
    return rt;
}