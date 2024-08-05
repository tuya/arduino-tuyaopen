#ifndef WIFITYPE_H_
#define WIFITYPE_H_

#include "tkl_wifi.h"

#define WIFI_SCAN_RUNNING   (-1)
#define WIFI_SCAN_FAILED    (-2)

#define WiFiMode_t   WF_WK_MD_E
#define WIFI_OFF     WWM_POWERDOWN
#define WIFI_STA     WWM_STATION
#define WIFI_AP      WWM_SOFTAP
#define WIFI_AP_STA  WWM_STATIONAP

#define WiFiEvent_t  arduino_event_id_t
#define WiFiEventInfo_t arduino_event_info_t
#define WiFiEventId_t wifi_event_id_t

#endif 