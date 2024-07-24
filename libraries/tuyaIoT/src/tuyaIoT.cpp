/******************************************************************************
 * INCLUDE
 ******************************************************************************/
#include "tuyaIoT.h"
#include "ArduinoTuyaIoTClient.h"

#include "tuya_iot_dp.h"

/******************************************************************************
 * GLOBAL VARIABLES
 ******************************************************************************/


/******************************************************************************
 * LOCAL MODULE FUNCTIONS
 ******************************************************************************/


/******************************************************************************
 * CTOR/DTOR
 ******************************************************************************/
TuyaIoTCloudClass::TuyaIoTCloudClass()
{
  ;
}

TuyaIoTCloudClass::~TuyaIoTCloudClass()
{
  ;
}

/******************************************************************************
 * PUBLIC MEMBER FUNCTIONS
 ******************************************************************************/
void TuyaIoTCloudClass::setEventCallback(void (*callback)(tuya_event_msg_t* event))
{
  app_iot_event_register_cb(callback);
}

void TuyaIoTCloudClass::setLicense(const char *uuid, const char *authKey)
{
  strncpy(this->_uuid, uuid, MAX_LENGTH_UUID);
  strncpy(this->_authKey, authKey, MAX_LENGTH_AUTHKEY);
  app_iot_license_set(this->_uuid, this->_authKey);
}

void TuyaIoTCloudClass::setLogLevel(TAL_LOG_LEVEL_E level)
{
  tal_log_set_level(level);
}

void TuyaIoTCloudClass::begin(const char *pid, const char *version)
{
  strncpy(this->_pid, pid, MAX_LENGTH_PRODUCT_ID);
  strncpy(this->_version, version, MAX_LENGTH_SW_VER);

  app_iot_task_start(this->_pid, this->_version);
}

void TuyaIoTCloudClass::begin(const char *pid, const char *version, const char *uuid, const char *authKey)
{
  strncpy(this->_pid, pid, MAX_LENGTH_PRODUCT_ID);
  strncpy(this->_version, version, MAX_LENGTH_SW_VER);
  strncpy(this->_uuid, uuid, MAX_LENGTH_UUID);
  strncpy(this->_authKey, authKey, MAX_LENGTH_AUTHKEY);

  app_iot_license_set(this->_uuid, this->_authKey);
  app_iot_task_start(this->_pid, this->_version);
}

int TuyaIoTCloudClass::write(uint8_t dpid, dp_prop_tp_t type, dp_value_t value, TIME_T timestamp)
{
  dp_obj_t dpObj;
  dpObj.id = dpid;
  dpObj.type = type;
  dpObj.value = value;
  dpObj.time_stamp = timestamp;

  return tuya_iot_dp_obj_report(&ArduinoIoTClient, ArduinoIoTClient.activate.devid, &dpObj, 1, 0);
}

int TuyaIoTCloudClass::write(uint8_t dpid, bool value)
{
  dp_prop_tp_t dpType = PROP_BOOL;
  dp_value_t dpValue;

  // find dp type
  dp_node_t *dpNode = dp_node_find_by_devid(ArduinoIoTClient.activate.devid, dpid);
  if (dpNode == NULL) {
    PR_ERR("dpid %d not found", dpid);
    return OPRT_COM_ERROR;
  }

  if (PROP_BOOL != dpNode->desc.prop_tp) {
    PR_ERR("dpid %d type is not bool", dpid);
    return OPRT_COM_ERROR;
  }

  dpValue.dp_bool = value;

  return write(dpid, dpType, dpValue, 0);
}

int TuyaIoTCloudClass::write(uint8_t dpid, int value)
{
  dp_prop_tp_t dpType;
  dp_value_t dpValue;

  // find dp type
  dp_node_t *dpNode = dp_node_find_by_devid(ArduinoIoTClient.activate.devid, dpid);
  if (dpNode == NULL) {
    PR_ERR("dpid %d not found", dpid);
    return OPRT_COM_ERROR;
  }

  dpType = dpNode->desc.prop_tp;

  switch (dpType) {
    case PROP_BOOL:
      dpValue.dp_bool = value;
    break;
    case PROP_VALUE:
      dpValue.dp_value = value;
      break;
    case PROP_ENUM:
      dpValue.dp_enum = value;
      break;
    case PROP_BITMAP:
      dpValue.dp_bitmap = value;
      break;
    default:
      PR_ERR("The data type (int) is not supported, dpid %d type %d  ", dpid, dpType);
      return OPRT_COM_ERROR;
  }

  return write(dpid, dpType, dpValue, 0);
}

int TuyaIoTCloudClass::write(uint8_t dpid, uint32_t value)
{
  dp_prop_tp_t dpType;
  dp_value_t dpValue;

  // find dp type
  dp_node_t *dpNode = dp_node_find_by_devid(ArduinoIoTClient.activate.devid, dpid);
  if (dpNode == NULL) {
    PR_ERR("dpid %d not found", dpid);
    return OPRT_COM_ERROR;
  }

  dpType = dpNode->desc.prop_tp;

  switch (dpType) {
    case PROP_BOOL:
      dpValue.dp_bool = value;
    break;
    case PROP_VALUE:
      dpValue.dp_value = value;
      break;
    case PROP_ENUM:
      dpValue.dp_enum = value;
      break;
    case PROP_BITMAP:
      dpValue.dp_bitmap = value;
      break;
    default:
      PR_ERR("The data type (uint32_t) is not supported, dpid %d type %d  ", dpid, dpType);
      return OPRT_COM_ERROR;
  }

  return write(dpid, dpType, dpValue, 0);
}

int TuyaIoTCloudClass::write(uint8_t dpid, char* value)
{
  dp_prop_tp_t dpType;
  dp_value_t dpValue;

  // find dp type
  dp_node_t *dpNode = dp_node_find_by_devid(ArduinoIoTClient.activate.devid, dpid);
  if (dpNode == NULL) {
    PR_ERR("dpid %d not found", dpid);
    return OPRT_COM_ERROR;
  }

  if (PROP_STR != dpNode->desc.prop_tp) {
    PR_ERR("dpid %d type is not string", dpid);
    return OPRT_COM_ERROR;
  }

  dpType = dpNode->desc.prop_tp;
  dpValue.dp_str = value;

  return write(dpid, dpType, dpValue, 0);
}

int TuyaIoTCloudClass::write(uint8_t dpid, void* value, uint16_t len)
{
  int rt = OPRT_OK;

  dp_prop_tp_t dpType;
  dp_value_t dpValue;

  dp_raw_t *dpRaw = NULL;

  dp_node_t *dpNode = dp_node_find_by_devid(ArduinoIoTClient.activate.devid, dpid);
  if (dpNode == NULL) {
    PR_ERR("dpid %d not found", dpid);
    return OPRT_COM_ERROR;
  }

  if (T_OBJ == dpNode->desc.type) { // obj dp upload
    switch (dpNode->desc.prop_tp) {
      case PROP_STR:{
        dpType = PROP_STR;
        dpValue.dp_str = (char *)value;
      } break;
      case PROP_VALUE:{
        dpType = PROP_VALUE;
        dpValue.dp_value = *(int *)value;
      } break;
      case PROP_ENUM:{
        dpType = PROP_ENUM;
        dpValue.dp_enum = *(uint32_t *)value;
      } break;
      case PROP_BITMAP:{
        dpType = PROP_BITMAP;
        dpValue.dp_bitmap = *(uint32_t *)value;
      } break;
      case PROP_BOOL:{
        dpType = PROP_BOOL;
        dpValue.dp_bool = *(bool *)value;
      } break;
      default:{
        PR_ERR("The data type (void *) is not supported, dpid %d type %d  ", dpid, dpNode->desc.prop_tp);
        return OPRT_COM_ERROR;
      }
    }
    rt = write(dpid, dpType, dpValue, 0);
  } else if (T_RAW == dpNode->desc.type) { // raw dp upload
    dpRaw = (dp_raw_t *)tal_malloc(sizeof(dp_raw_t) + len);
    if (dpRaw == NULL) {
      PR_ERR("malloc failed");
      return OPRT_MALLOC_FAILED;
    }

    dpRaw->id = dpid;
    dpRaw->len = len;
    memcpy(dpRaw->data, value, len);
    rt = tuya_iot_dp_raw_report(&ArduinoIoTClient, ArduinoIoTClient.activate.devid, dpRaw, 3);

    tal_free(dpRaw);
    dpRaw = NULL;
  } else {
    PR_ERR("The data type (void *) is not supported, dpid %d type %d  ", dpid, dpNode->desc.type);
    return OPRT_NOT_SUPPORTED;
  }

  return rt;
}

tuya_event_id_t TuyaIoTCloudClass::getEventId(tuya_event_msg_t* event)
{
  return event->id;
}
uint16_t TuyaIoTCloudClass::getEventDpNum(tuya_event_msg_t* event)
{
  if (NULL == event) {
    PR_ERR("event is NULL");
    return 0;
  }

  if (TUYA_EVENT_DP_RECEIVE_OBJ != event->id) {
    PR_ERR("event id is not TUYA_EVENT_DP_RECEIVE_OBJ");
    return 0;
  }

  return event->value.dpobj->dpscnt;
}

uint8_t TuyaIoTCloudClass::getEventDpId(tuya_event_msg_t* event, uint16_t index)
{
  if (NULL == event) {
    PR_ERR("event is NULL");
    return 0;
  }

  if (TUYA_EVENT_DP_RECEIVE_OBJ != event->id) {
    PR_ERR("event id is not TUYA_EVENT_DP_RECEIVE_OBJ");
    return 0;
  }

  return event->value.dpobj->dps[index].id;
}

bool TuyaIoTCloudClass::readBool(tuya_event_msg_t* event, uint8_t dpid)
{
  if (NULL == event) {
    PR_ERR("event is NULL");
    return false;
  }

  if (TUYA_EVENT_DP_RECEIVE_OBJ != event->id) {
    PR_ERR("event id is not TUYA_EVENT_DP_RECEIVE_OBJ");
    return false;
  }

  dp_obj_recv_t *dpObj = event->value.dpobj;

  for (int i = 0; i < dpObj->dpscnt; i++) {
    if (dpid == dpObj->dps[i].id) {
      if (PROP_BOOL != dpObj->dps[i].type) {
        PR_ERR("dpid %d type is not bool", dpid);
        return false;
      } else {
        return dpObj->dps[i].value.dp_bool;
      }
    }
  }

  PR_ERR("dpid %d not found", dpid);
  return false;
}

int TuyaIoTCloudClass::readValue(tuya_event_msg_t* event, uint8_t dpid)
{
  if (NULL == event) {
    PR_ERR("event is NULL");
    return 0;
  }

  if (TUYA_EVENT_DP_RECEIVE_OBJ != event->id) {
    PR_ERR("event id is not TUYA_EVENT_DP_RECEIVE_OBJ");
    return 0;
  }

  dp_obj_recv_t *dpObj = event->value.dpobj;

  for (int i = 0; i < dpObj->dpscnt; i++) {
    if (dpid == dpObj->dps[i].id) {
      if (PROP_VALUE != dpObj->dps[i].type) {
        PR_ERR("dpid %d type is not value", dpid);
        return 0;
      } else {
        return dpObj->dps[i].value.dp_value;
      }
    }
  }

  PR_ERR("dpid %d not found", dpid);
  return 0;
}

uint32_t TuyaIoTCloudClass::readEnum(tuya_event_msg_t* event, uint8_t dpid)
{
  if (NULL == event) {
    PR_ERR("event is NULL");
    return 0;
  }

  if (TUYA_EVENT_DP_RECEIVE_OBJ != event->id) {
    PR_ERR("event id is not TUYA_EVENT_DP_RECEIVE_OBJ");
    return 0;
  }

  dp_obj_recv_t *dpObj = event->value.dpobj;

  for (int i = 0; i < dpObj->dpscnt; i++) {
    if (dpid == dpObj->dps[i].id) {
      if (PROP_ENUM != dpObj->dps[i].type) {
        PR_ERR("dpid %d type is not enum", dpid);
        return 0;
      } else {
        return dpObj->dps[i].value.dp_enum;
      }
    }
  }

  PR_ERR("dpid %d not found", dpid);
  return 0;
}

char* TuyaIoTCloudClass::readString(tuya_event_msg_t* event, uint8_t dpid)
{
  if (NULL == event) {
    PR_ERR("event is NULL");
    return NULL;
  }

  if (TUYA_EVENT_DP_RECEIVE_OBJ != event->id) {
    PR_ERR("event id is not TUYA_EVENT_DP_RECEIVE_OBJ");
    return NULL;
  }

  dp_obj_recv_t *dpObj = event->value.dpobj;

  for (int i = 0; i < dpObj->dpscnt; i++) {
    if (dpid == dpObj->dps[i].id) {
      if (PROP_STR != dpObj->dps[i].type) {
        PR_ERR("dpid %d type is not string", dpid);
        return NULL;
      } else {
        return dpObj->dps[i].value.dp_str;
      }
    }
  }

  PR_ERR("dpid %d not found", dpid);
  return NULL;
}

uint32_t TuyaIoTCloudClass::readBitmap(tuya_event_msg_t* event, uint8_t dpid)
{
  if (NULL == event) {
    PR_ERR("event is NULL");
    return 0;
  }

  if (TUYA_EVENT_DP_RECEIVE_OBJ != event->id) {
    PR_ERR("event id is not TUYA_EVENT_DP_RECEIVE_OBJ");
    return 0;
  }

  dp_obj_recv_t *dpObj = event->value.dpobj;

  for (int i = 0; i < dpObj->dpscnt; i++) {
    if (dpid == dpObj->dps[i].id) {
      if (PROP_BITMAP != dpObj->dps[i].type) {
        PR_ERR("dpid %d type is not bitmap", dpid);
        return 0;
      } else {
        return dpObj->dps[i].value.dp_bitmap;
      }
    }
  }

  PR_ERR("dpid %d not found", dpid);
  return 0;
}

uint8_t* TuyaIoTCloudClass::readRaw(tuya_event_msg_t* event, uint8_t dpid, uint16_t* len)
{
  if (NULL == event) {
    PR_ERR("event is NULL");
    return NULL;
  }

  if (NULL == len) {
    PR_ERR("len is NULL");
    return NULL;
  }

  if (TUYA_EVENT_DP_RECEIVE_RAW != event->id) {
    PR_ERR("event id is not TUYA_EVENT_DP_RECEIVE_RAW");
    return NULL;
  }

  dp_raw_recv_t *dpRaw = event->value.dpraw;

  if (dpid != dpRaw->dp.id) {
    PR_ERR("dpid %d not found", dpid);
    return NULL;
  }

  *len = dpRaw->dp.len;

  return dpRaw->dp.data;
}

int TuyaIoTCloudClass::remove(void)
{
  tuya_iot_reset(&ArduinoIoTClient);
  return OPRT_OK;
}

/******************************************************************************
 * PRIVATE MEMBER FUNCTIONS
 ******************************************************************************/


/******************************************************************************
 * EXTERN DEFINITION
 ******************************************************************************/
TuyaIoTCloudClass tuyaIoT;
