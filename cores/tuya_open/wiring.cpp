#include <Arduino.h>

#include "tal_system.h"

unsigned long millis()
{
    unsigned long ms = tal_system_get_millisecond();
    return ms;
}

void delay(unsigned long ms)
{
    tal_system_sleep((uint32_t)ms);
}

void delayMicroseconds(unsigned int us)
{
    // Note: tuya open sdk not support delayMicroseconds
    unsigned int ms = ((us / 1000) == 0) ? 1 : (us / 1000);

    tal_system_sleep((uint32_t)ms);
}

void yield(void)
{
    tal_system_sleep(1);
}
