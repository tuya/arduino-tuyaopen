/**
 * @file tuya_arduino_main.cpp
 * @author www.tuya.com
 * @brief tuya_arduino_main module is used to
 * @version 0.1
 * @date 2024-02-02
 *
 * @copyright Copyright (c) tuya.inc 2024
 *
 */

#include "tuya_iot_config.h"

#include "tal_api.h"
#include "tkl_output.h"
#include "tuya_iot.h"

#include <Arduino.h>

/***********************************************************
************************macro define************************
***********************************************************/

/***********************************************************
***********************typedef define***********************
***********************************************************/

/***********************************************************
********************function declaration********************
***********************************************************/

/***********************************************************
***********************variable define**********************
***********************************************************/
static THREAD_HANDLE arduino_thrd_hdl = NULL;

/***********************************************************
***********************function define**********************
***********************************************************/

#if (defined(__cplusplus) || defined(c_plusplus))
extern "C"
{
#endif

extern void app_open_sdk_init(void);

static void log_output_cb(const char *str)
{
  Serial.print(str);
}

static void arduino_thread(void *arg)
{
#if defined(ARDUINO_TUYA_T2)
  // wait rf cali
  extern char get_rx2_flag(void);
  while (get_rx2_flag() == 0)
  {
    tal_system_sleep(1);
  }
  // deinit t2 uart
  tkl_uart_deinit(TUYA_UART_NUM_0);
  tkl_uart_deinit(TUYA_UART_NUM_1);
#endif // defined(ARDUINO_TUYA_T2)

  // tuya log init
  tal_log_init(TAL_LOG_LEVEL_DEBUG, 1024, (TAL_LOG_OUTPUT_CB)log_output_cb);

  app_open_sdk_init();

  setup();

  for (;;)
  {
    loop();
    tal_system_sleep(1);
  }

  return;
}

void tuya_app_main(void)
{
#if defined(ARDUINO_TUYA_T2)
  __asm("BL __libc_init_array");
#endif // defined(ARDUINO_TUYA_T2)

  THREAD_CFG_T thrd_param = {1024 * 4, THREAD_PRIO_3, (CHAR_T *)"arduino_thread"};
  tal_thread_create_and_start(&arduino_thrd_hdl, NULL, NULL, arduino_thread, NULL, &thrd_param);
}

/* stub for __libc_init_array */
void _fini(void) {}
void _init(void) { ; }

// This hack is needed because Beken SDK is not linking against libstdc++ correctly.
void *__dso_handle = 0;

#if (defined(__cplusplus) || defined(c_plusplus))
}
#endif
