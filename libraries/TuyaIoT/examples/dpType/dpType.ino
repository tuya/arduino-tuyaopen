#include "OneButton.h"

#include "TuyaIoT.h"
#include "Log.h"

#define buttonPin BUTTON_BUILTIN
OneButton button(buttonPin);

#define DPID_SWITCH 20
#define DPID_MODE   21
#define DPID_BRIGHT 22
#define DPID_BITMAP 101
#define DPID_STRING 102
#define DPID_RAW    103

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  // button
  button.setPressMs(3*1000);
  button.attachLongPressStart(buttonLongPressStart);

  TuyaIoT.setEventCallback(tuyaIoTEventCallback);
  TuyaIoT.setLicense("uuidxxxxxxxxxxxxxxxx", "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");

  // The "PROJECT_VERSION" comes from the "PROJECT_VERSION" field in "appConfig.json"
  TuyaIoT.begin("2avicuxv6zgeiquf", PROJECT_VERSION);
}

void loop() {
  // put your main code here, to run repeatedly:
  button.tick();

  delay(10);
}

void tuyaIoTEventCallback(tuya_event_msg_t *event)
{
  tuya_event_id_t event_id = TuyaIoT.eventGetId(event);
  
  switch (event_id) {
    case TUYA_EVENT_TIMESTAMP_SYNC: {
      tal_time_set_posix(event->value.asInteger, 1);
    } break;
    case TUYA_EVENT_DP_RECEIVE_OBJ: {
      uint16_t dpNum = TuyaIoT.eventGetDpNum(event);
      for (uint16_t i = 0; i < dpNum; i++) {
        uint8_t dpid = TuyaIoT.eventGetDpId(event, i);
        switch (dpid) {
          case DPID_SWITCH: {
            bool switchStatus = 0;
            TuyaIoT.read(event, DPID_SWITCH, switchStatus);
            TuyaIoT.write(DPID_SWITCH, switchStatus);
            PR_DEBUG("switchStatus: %d", switchStatus);
          } break;
          case DPID_MODE: {
            uint32_t mode = 0;
            TuyaIoT.read(event, DPID_MODE, mode);
            TuyaIoT.write(DPID_MODE, mode);
            PR_DEBUG("mode: %d", mode);
          } break;
          case DPID_BRIGHT: {
            int brightValue = 0;
            TuyaIoT.read(event, DPID_BRIGHT, brightValue);
            TuyaIoT.write(DPID_BRIGHT, brightValue);
            PR_DEBUG("brightValue: %d", brightValue);
          } break;
          case DPID_STRING: {
            char *strValue = NULL;
            TuyaIoT.read(event, DPID_STRING, strValue);
            TuyaIoT.write(DPID_STRING, strValue);
          } break;
          default : break;
        }
      }
    } break;
    case TUYA_EVENT_DP_RECEIVE_RAW: {
      uint8_t *rawValue = NULL;
      uint16_t len = 0;
      TuyaIoT.read(event, DPID_RAW, rawValue, len);
      PR_DEBUG("---> len:%d", len);
      PR_HEXDUMP_DEBUG("raw", rawValue, len);
      TuyaIoT.write(DPID_RAW, rawValue, len);
    } break;
    default: break;
  }
}

void buttonLongPressStart()
{
  Serial.println("Button long press, remove IoT device.");
  TuyaIoT.remove();
}
