#include "WiFi.h"

extern "C" {
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>

}

void WiFiClass::printDiag(Print& p)
{
    const char* modes[] = { "NULL", "SNIFFER", "STA", "AP", "STA+AP" };

    WF_WK_MD_E mode;
    tkl_wifi_get_work_mode(&mode);

    uint8_t primaryChan;
    tkl_wifi_get_cur_channel(&primaryChan);

    p.print("Mode: ");
    p.println(modes[mode]);

    p.print("Channel: ");
    p.println(primaryChan);

}

void WiFiClass::enableProv(bool status)
{
    prov_enable = status;
}

bool WiFiClass::isProvEnabled()
{
    return prov_enable;
}

WiFiClass WiFi;