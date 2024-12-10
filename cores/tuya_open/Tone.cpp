#include "Arduino.h"
#include "tkl_pwm.h"

void tone(uint8_t _pin, unsigned int frequency, unsigned long duration)
{
  OPERATE_RET rt = OPRT_OK;
  static uint8_t is_init = 0;

  TUYA_PWM_NUM_E pwmNum = pwmPinToNum(_pin);
  if (TUYA_PWM_NUM_MAX == pwmNum) {
      return;
  }

  TUYA_PWM_BASE_CFG_T pwmCfg = pwmCfgGet(_pin);
  pwmCfg.frequency = frequency;

  pwmCfg.duty = 5000; // duty 0-10000, 5000 is 50%

  if (0 == is_init) {
      rt = tkl_pwm_init(pwmNum, &pwmCfg);
      if (OPRT_OK != rt) {
          return;
      }
      is_init = 1;
  }

  tkl_pwm_start(pwmNum);

  return;
}

void noTone(uint8_t _pin)
{
  tkl_pwm_stop(pwmPinToNum(_pin));

  return;
}

