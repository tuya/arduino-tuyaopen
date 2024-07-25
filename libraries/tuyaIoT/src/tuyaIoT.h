#ifndef __TUYA_IOT_H__
#define __TUYA_IOT_H__

/******************************************************************************
 * INCLUDE
 ******************************************************************************/
#include <Arduino.h>
#include "tuya_iot.h"

/******************************************************************************
 * CONSTANTS
 ******************************************************************************/


/******************************************************************************
 * TYPEDEF
 ******************************************************************************/


/******************************************************************************
 * CLASS DECLARATION
 ******************************************************************************/
class TuyaIoTCloudClass {
public:
  TuyaIoTCloudClass();
  ~TuyaIoTCloudClass();

  void begin(const char *pid, const char *version);
  void begin(const char *pid, const char *version, const char *uuid, const char *authKey);

  void setLicense(const char *uuid, const char *authKey);
  void setLogLevel(TAL_LOG_LEVEL_E level);

  void setEventCallback(void (*callback)(tuya_event_msg_t* event));

  int write(uint8_t dpid, dp_prop_tp_t type, dp_value_t value, TIME_T timestamp);
  int write(uint8_t dpid, bool value);
  int write(uint8_t dpid, int value);
  int write(uint8_t dpid, uint32_t value);
  int write(uint8_t dpid, char* value);
  int write(uint8_t dpid, void* value, uint16_t len);

  // event
  tuya_event_id_t getEventId(tuya_event_msg_t* event);
  uint16_t getEventDpNum(tuya_event_msg_t* event);
  uint8_t getEventDpId(tuya_event_msg_t* event, uint16_t index);

  bool readBool(tuya_event_msg_t* event, uint8_t dpid);
  int readValue(tuya_event_msg_t* event, uint8_t dpid);
  uint32_t readEnum(tuya_event_msg_t* event, uint8_t dpid);
  char* readString(tuya_event_msg_t* event, uint8_t dpid);
  uint32_t readBitmap(tuya_event_msg_t* event, uint8_t dpid);
  uint8_t* readRaw(tuya_event_msg_t* event, uint8_t dpid, uint16_t* len);

  int remove(void);
private:
  char _pid[MAX_LENGTH_PRODUCT_ID+1]  = {0}; // +1 is '\0'
  char _version[MAX_LENGTH_SW_VER+1]  = {0};
  char _uuid[MAX_LENGTH_UUID+1]       = {0};
  char _authKey[MAX_LENGTH_AUTHKEY+1] = {0};
};


/******************************************************************************
 * EXTERN DECLARATION
 ******************************************************************************/
extern TuyaIoTCloudClass tuyaIoT;

#endif // __TUYA_IOT_H__
