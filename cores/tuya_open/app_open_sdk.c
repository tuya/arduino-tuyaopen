#include "tuya_iot_config.h"

#include "cJSON.h"
#include "tal_api.h"

#if defined(ENABLE_WIFI) && (ENABLE_WIFI == 1)
#include "netconn_wifi.h"
#endif
#if defined(ENABLE_WIRED) && (ENABLE_WIRED == 1)
#include "netconn_wired.h"
#endif
#if defined(ENABLE_LIBLWIP) && (ENABLE_LIBLWIP == 1)
#include "lwip_init.h"
#endif

static void __wifi_callback_event(WF_EVENT_E event, void *arg)
{
  return;
}

void app_open_sdk_init(void)
{
  // cJSON init
  cJSON_Hooks hooks = {
    .malloc_fn = tal_malloc,
    .free_fn = tal_free
  };
  cJSON_InitHooks(&hooks);

  // file system init
  tal_kv_cfg_t kv_cfg = {
    .seed = "vmlkasdh93dlvlcy",
    .key = "dflfuap134ddlduq",
  };
  tal_kv_init(&kv_cfg);
  // software timer init
  tal_sw_timer_init();
  // work queue init
  tal_workq_init();

  // lwip init
#if defined(ENABLE_LIBLWIP) && (ENABLE_LIBLWIP == 1)
  TUYA_LwIP_Init();
#endif

  // wifi init
  // TODO: set country code
  // TODO: use netconn_wifi functions
  tal_wifi_init(__wifi_callback_event);
  tal_wifi_set_country_code("CN");
}

