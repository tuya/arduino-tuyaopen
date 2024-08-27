#include <Ticker.h>

Ticker blinker;
Ticker changer;

float blinkerPace = 0.1;  //seconds

int ledStatus = 0;

void blinkCallback() {
  ledStatus = !ledStatus;
  digitalWrite(LED_BUILTIN, ledStatus);
}

void changeCallback() {
  blinkerPace = 0.5;

  blinker.detach();
  blinker.attach(blinkerPace, blinkCallback);
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  ledStatus = LOW;

  blinker.attach(blinkerPace, blinkCallback);
  changer.once(10, changeCallback);
}

void loop() {}

