#ifndef ARDUINO_H
#define ARDUINO_H

#include "pins_arduino.h"
#include "api/ArduinoAPI.h"

#if defined(__cplusplus) && !defined(c_plusplus)

using namespace arduino;

#include "SerialUART.h"
#define Serial _SerialUART0_

#endif // __cplusplus

#endif // ARDUINO_H
