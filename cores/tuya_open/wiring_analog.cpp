#include "Arduino.h"

#include "tkl_adc.h"
#include "tkl_pwm.h"

int analogRead(pin_size_t pinNumber)
{
    OPERATE_RET rt = OPRT_OK;
    int32_t readValue = 0;

    uint8_t adcChannel = adcPinToChannel(pinNumber);
    if (TUYA_ADC_INVALID_VALUE == adcChannel) {
        return 0;
    }

    TUYA_ADC_NUM_E adcNum = adcPinToNum(pinNumber);

    TUYA_ADC_BASE_CFG_T adcCfg = adcCfgGet(pinNumber);

    rt = tkl_adc_init(adcNum, &adcCfg);
    if (OPRT_OK != rt) {
        return 0;
    }

    tkl_adc_read_single_channel(adcNum, adcChannel, &readValue);

    tkl_adc_deinit(adcNum);

    return readValue;
}

void analogReference(uint8_t mode)
{
    return;
}

// value: 0-100
void analogWrite(pin_size_t pinNumber, int value)
{
    OPERATE_RET rt = OPRT_OK;

    TUYA_PWM_NUM_E pwmNum = pwmPinToNum(pinNumber);
    if (TUYA_PWM_NUM_MAX == pwmNum) {
        return;
    }

    TUYA_PWM_BASE_CFG_T pwmCfg = pwmCfgGet(pinNumber);

    pwmCfg.duty = value * 100; // duty 0-10000, 5000 is 50%

    rt = tkl_pwm_init(pwmNum, &pwmCfg);
    if (OPRT_OK != rt) {
        return;
    }
    tkl_pwm_start(pwmNum);

    return;
}
