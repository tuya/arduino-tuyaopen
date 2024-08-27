// SPI Pin      T2
//   SCK -----> p14
//  MOSI -----> p16
//  MISO -----> p17
//   CS  -----> p15

#include "SPI.h"

void setup() {
  SPI.begin();
}

char str[] = "Hello Arduino.";

void loop() {
  // T2 clock setting suggest 1M ~ 8M
  // T2 only support MSBFIRST
  SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
  SPI.transfer(str, 15);
  SPI.endTransaction();

  delay(3*1000);
}
