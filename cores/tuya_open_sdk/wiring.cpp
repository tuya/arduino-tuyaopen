#include <Arduino.h>

#include "tal_system.h"

#if defined(ARDUINO_TUYA_T2)
#include "FreeRTOS.h"
#include "portmacro.h"
#endif // defined(ARDUINO_TUYA_T2)

unsigned long millis()
{
    unsigned long ms = tal_system_get_millisecond();
    return ms;
}

void delay(unsigned long ms)
{
    return tal_system_sleep((UINT_T)ms);
}

void yield(void)
{
#if defined(ARDUINO_TUYA_T2)
    portYIELD();
#endif // defined(ARDUINO_TUYA_T2)
    return;
}
