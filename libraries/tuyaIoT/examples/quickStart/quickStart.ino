#include "OneButton.h"
#include "tuyaIoT.h"

#define ledPin    p16
int ledState = LOW;

#define buttonPin p14
OneButton button(buttonPin);

#define DPID_SWITCH 1

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  // led
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, ledState);

  // button
  button.attachClick(buttonClick);
  button.setPressMs(3*1000);
  button.attachLongPressStart(buttonLongPressStart);

  tuyaIoT.setEventCallback(tuyaIoTEventCallback);
  tuyaIoT.setLicense("uuidxxxxxxxxxxxxxxxx", "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
  tuyaIoT.begin("qhivvyqawogv04e4", "1.0.0");
}

void loop() {
  // put your main code here, to run repeatedly:
  button.tick();

  delay(10);
}

void tuyaIoTEventCallback(tuya_event_msg_t *e)
{
  tuya_event_id_t event_id = e->id;

  switch (event_id) {
    case TUYA_EVENT_DP_RECEIVE_OBJ: {
      uint16_t dpNum = tuyaIoT.getEventDpNum(e);
      for (uint16_t i = 0; i < dpNum; i++) {
        uint8_t dpid = tuyaIoT.getEventDpId(e, i);
        switch (dpid) {
          case DPID_SWITCH: {
            ledState = tuyaIoT.readBool(e, DPID_SWITCH);
            digitalWrite(ledPin, ledState);
            tuyaIoT.write(DPID_SWITCH, ledState);
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
  ledState = (ledState == LOW) ? (HIGH) : (LOW);
  digitalWrite(ledPin, ledState);
  tuyaIoT.write(DPID_SWITCH, ledState);
}

void buttonLongPressStart()
{
  Serial.println("Button long press");
  tuyaIoT.remove();
}