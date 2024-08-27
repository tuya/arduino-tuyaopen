#ifndef __PINS_ARDUINO_H__
#define __PINS_ARDUINO_H__

#include <stdint.h>

#include "tuya_cloud_types.h"

#if defined(__cplusplus) && !defined(c_plusplus)
extern "C" {
#endif // __cplusplus

// Number of pins defined in PinDescription array
#define p0  (0u)
#define p1  (1u)
#define p10 (10u)
#define p11 (11u)
#define p12 (12u)
#define p13 (13u)
#define p14 (14u)
#define p15 (15u)
#define p16 (16u)
#define p17 (17u)
#define p18 (18u)
#define p19 (19u)
#define p24 (24u)
#define p25 (25u)
#define p28 (28u)
#define p32 (32u)
#define p34 (34u)
#define p36 (36u)
#define p41 (41u)
#define p40 (40u)
#define p48 (48u)

static const uint8_t A0 = (25u);
static const uint8_t A1 = (24u);
static const uint8_t A2 = (28u);
static const uint8_t A3 = (12u);
static const uint8_t A4 = (13u);

static const uint8_t D0 = (18u);
static const uint8_t D1 = (24u);
static const uint8_t D2 = (32u);
static const uint8_t D3 = (34u);
static const uint8_t D4 = (36u);

// uart
static const uint8_t defaultSerial = 0;

// adc
#define TUYA_ADC_INVALID_VALUE (0xFFu)

TUYA_ADC_NUM_E adcPinToNum(uint8_t pin);
uint8_t adcPinToChannel(uint8_t pin);
TUYA_ADC_BASE_CFG_T adcCfgGet(uint8_t pin);

// pwm
TUYA_PWM_NUM_E pwmPinToNum(uint8_t pin);
TUYA_PWM_BASE_CFG_T pwmCfgGet(uint8_t pin);

// SPI
#define SPI_DEFAULT_CLOCK  (8000000u)

#define PIN_SPI0_MISO       (p17)
#define PIN_SPI0_MOSI       (p16)
#define PIN_SPI0_SCK        (p14)
#define PIN_SPI0_CS         (p15)

#if defined(__cplusplus) && !defined(c_plusplus)
}
#endif // __cplusplus

#endif /* __PINS_ARDUINO_H__ */
