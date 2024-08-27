#include "OneButton.h"
#include "SimpleLed.h"

#include "TuyaIoT.h"

#define ledPin  LED_BUILTIN
// Turn on LED when output low level
SimpleLed led(ledPin, LOW);

#define buttonPin BUTTON_BUILTIN
OneButton button(buttonPin);


#define DPID_SWITCH 1

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  // led
  led.off();

  // button
  button.attachClick(buttonClick);
  button.setPressMs(3*1000);
  button.attachLongPressStart(buttonLongPressStart);

  TuyaIoT.setEventCallback(tuyaIoTEventCallback);
  TuyaIoT.setLicense("uuidxxxxxxxxxxxxxxxx", "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");

  // The "PROJECT_VERSION" comes from the "PROJECT_VERSION" field in "appConfig.json"
  TuyaIoT.begin("qhivvyqawogv04e4", PROJECT_VERSION);
}

void loop() {
  // put your main code here, to run repeatedly:
  button.tick();
  led.update();

  delay(10);
}

void tuyaIoTEventCallback(tuya_event_msg_t *event)
{
  int ledState = 0;

  tuya_event_id_t event_id = TuyaIoT.getEventId(event);

  switch (event_id) {
    case TUYA_EVENT_BIND_START: {
      led.blink(500);
    } break;
    case TUYA_EVENT_ACTIVATE_SUCCESSED: {
      led.off();
    } break;
    case TUYA_EVENT_MQTT_CONNECTED: {
      // Update all DP
      Serial.println("---> TUYA_EVENT_MQTT_CONNECTED");
      uint8_t ledState = led.getState();
      TuyaIoT.write(DPID_SWITCH, ledState);
    } break;
    case TUYA_EVENT_TIMESTAMP_SYNC: {
      tal_time_set_posix(event->value.asInteger, 1);
    } break;
    case TUYA_EVENT_DP_RECEIVE_OBJ: {
      uint16_t dpNum = TuyaIoT.getEventDpNum(event);
      for (uint16_t i = 0; i < dpNum; i++) {
        uint8_t dpid = TuyaIoT.getEventDpId(event, i);
        switch (dpid) {
          case DPID_SWITCH: {
            ledState = TuyaIoT.readBool(event, DPID_SWITCH);
            led.setState(ledState);
            TuyaIoT.write(DPID_SWITCH, ledState);
          } break;
          default : break;
        }
      }
    } break;
    default: break;
  }
}

void buttonClick()
{
  Serial.println("Button clicked");
  uint8_t ledState = led.getState();
  Serial.print("Upload DPID_SWITCH: "); Serial.println(ledState);
  TuyaIoT.write(DPID_SWITCH, ledState);
}

void buttonLongPressStart()
{
  Serial.println("Button long press, remove IoT device.");
  TuyaIoT.remove();
}