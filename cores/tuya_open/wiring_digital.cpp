#include "Arduino.h"

#include "tkl_gpio.h"

void pinMode(pin_size_t pinNumber, PinMode pinMode)
{
    TUYA_GPIO_BASE_CFG_T gpioCfg;

    TUYA_GPIO_NUM_E tuyaPin = (TUYA_GPIO_NUM_E)pinNumber;

    switch (pinMode) {
        case INPUT: {
            gpioCfg.direct = TUYA_GPIO_INPUT;
            gpioCfg.mode = TUYA_GPIO_FLOATING;
        } break;
        case OUTPUT: {
            gpioCfg.direct = TUYA_GPIO_OUTPUT;
            gpioCfg.mode = TUYA_GPIO_PUSH_PULL;
            gpioCfg.level = TUYA_GPIO_LEVEL_LOW;
        } break;
        case INPUT_PULLUP: {
            gpioCfg.direct = TUYA_GPIO_INPUT;
            gpioCfg.mode = TUYA_GPIO_PULLUP;
        } break;
        case INPUT_PULLDOWN: {
            gpioCfg.direct = TUYA_GPIO_INPUT;
            gpioCfg.mode = TUYA_GPIO_PULLDOWN;
        } break;
        case OUTPUT_OPENDRAIN:{
            gpioCfg.direct = TUYA_GPIO_OUTPUT;
            gpioCfg.mode = TUYA_GPIO_OPENDRAIN;
            gpioCfg.level = TUYA_GPIO_LEVEL_LOW;
        } break;
        default : return;
    }

    tkl_gpio_init(tuyaPin, &gpioCfg);

    return;
}

void digitalWrite(pin_size_t pinNumber, PinStatus status)
{
    TUYA_GPIO_NUM_E tuyaPin = (TUYA_GPIO_NUM_E)pinNumber;
    TUYA_GPIO_LEVEL_E tuyaLevel = (status == HIGH) ? (TUYA_GPIO_LEVEL_HIGH) : (TUYA_GPIO_LEVEL_LOW);

    tkl_gpio_write(tuyaPin, tuyaLevel);

    return;
}

PinStatus digitalRead(pin_size_t pinNumber)
{
    TUYA_GPIO_LEVEL_E level = TUYA_GPIO_LEVEL_LOW;
    TUYA_GPIO_NUM_E tuyaPin = (TUYA_GPIO_NUM_E)pinNumber;

    tkl_gpio_read(tuyaPin, &level);

    return ((level==TUYA_GPIO_LEVEL_HIGH) ? (HIGH) : (LOW));
}
