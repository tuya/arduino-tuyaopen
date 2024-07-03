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
#define p6  (6u)
#define p7  (7u)
#define p8  (8u)
#define p9  (9u)
#define p14 (14u)
#define p15 (15u)
#define p16 (16u)
#define p17 (17u)
#define p20 (20u)
#define p22 (22u)
#define p23 (23u)
#define p24 (24u)
#define p26 (26u)
#define p28 (28u)

static const uint8_t A0 = (23u);
static const uint8_t A1 = (22u);
static const uint8_t A2 = (24u);
static const uint8_t A3 = (26u);
static const uint8_t A4 = (28u);

static const uint8_t D0 = (6u);
static const uint8_t D1 = (7u);
static const uint8_t D2 = (8u);
static const uint8_t D3 = (24u);
static const uint8_t D4 = (26u);

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

#if defined(__cplusplus) && !defined(c_plusplus)
}
#endif // __cplusplus

#endif /* __PINS_ARDUINO_H__ */
