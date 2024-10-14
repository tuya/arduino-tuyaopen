#include <Arduino.h>
#include <Wire.h>

void setup() {
  Serial.begin(115200); // start serial for output
  Wire.begin(); // join i2c bus (address optional for master)
}

byte x = 0;

void loop() {
  Serial.println("Sending: " + String(x));
  Wire.beginTransmission(8); // transmit to device #8
  Wire.write(x);              // sends one byte
  Wire.endTransmission();    // stop transmitting

  x++;
  delay(500);
}
