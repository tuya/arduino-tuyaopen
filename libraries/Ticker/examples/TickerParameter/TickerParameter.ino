#include <Ticker.h>

int arg_1s = 1;
Ticker ticker_1s;

int arg_5s = 5;
Ticker ticker_5s;

void ticker_callback(int *arg)
{
  Serial.print("arg: ");
  Serial.println(*arg);
}

void setup() {
  Serial.begin(115200);

  // every 1 s, call ticker_callback()
  ticker_1s.attach(1, ticker_callback, &arg_1s);

  // every 5 s, call ticker_callback()
  ticker_5s.attach(5, ticker_callback, &arg_5s);
}

void loop() {}
