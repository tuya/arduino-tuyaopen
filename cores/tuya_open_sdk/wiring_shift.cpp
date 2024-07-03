#include "Arduino.h"

uint8_t shiftIn(pin_size_t dataPin, uint8_t clockPin, BitOrder bitOrder)
{
    uint8_t value = 0;
    uint8_t i;

    for (i = 0; i < 8; ++i) {
        digitalWrite(clockPin, HIGH);
        if (bitOrder == LSBFIRST)
            value |= digitalRead(dataPin) << i;
        else
            value |= digitalRead(dataPin) << (7 - i);
        digitalWrite(clockPin, LOW);
    }
    return value;
}

void shiftOut(pin_size_t dataPin, uint8_t clockPin, BitOrder bitOrder, uint8_t val)
{
    uint8_t i;

    for (i = 0; i < 8; i++)  {
        if (bitOrder == LSBFIRST)
            digitalWrite(dataPin, !!(val & (1 << i)) ? HIGH : LOW);
        else	
            digitalWrite(dataPin, !!(val & (1 << (7 - i))) ? HIGH : LOW);
            
        digitalWrite(clockPin, HIGH);
        digitalWrite(clockPin, LOW);
    }
}
