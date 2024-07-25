#ifndef __ARDUINO_TUYA_IOT_CLIENT_H__
#define __ARDUINO_TUYA_IOT_CLIENT_H__

/******************************************************************************
 * INCLUDE
 ******************************************************************************/
#include <stdint.h>

#include "tuya_iot.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 * CONSTANTS
 ******************************************************************************/

/******************************************************************************
 * TYPEDEF
 ******************************************************************************/
void app_iot_event_register_cb(void (*callback)(tuya_event_msg_t* event));

void app_iot_license_set(const char *uuid, const char *authKey);

void app_iot_task_start(const char *pid, const char *version);

/******************************************************************************
 * EXTERN DECLARATION
 ******************************************************************************/
extern tuya_iot_client_t ArduinoIoTClient;

#ifdef __cplusplus
}
#endif

#endif // __ARDUINO_TUYA_IOT_CLIENT_H__
