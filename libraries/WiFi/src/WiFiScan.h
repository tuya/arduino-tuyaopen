#ifndef WIFISCAN_H_
#define WIFISCAN_H_

#include "WiFiType.h"
#include "WiFiGeneric.h"

class WiFiScanClass
{
public:
    int16_t scanNetworks(bool async = false, bool show_hidden = false, bool passive = false, uint32_t max_ms_per_chan = 300, uint8_t channel = 0, const char * ssid=nullptr, const uint8_t * bssid=nullptr);
    void scanDelete();
    bool getNetworkInfo(uint8_t networkItem, String &ssid, uint8_t &encryptionType, int32_t &RSSI, uint8_t* &BSSID, int32_t &channel);

    String SSID(uint8_t networkItem);
    WF_AP_AUTH_MODE_E encryptionType(uint8_t networkItem);
    int32_t RSSI(uint8_t networkItem);
    uint8_t * BSSID(uint8_t networkItem);
    String BSSIDstr(uint8_t networkItem);
    int32_t channel(uint8_t networkItem);
    void * getScanInfoByIndex(int i) { return _getScanInfoByIndex(i); }; 

protected:
    void * _getScanInfoByIndex(int i);

private:
    AP_IF_S * ap_info;
    uint32_t ap_num;
};


#endif /* WIFISCAN_H_ */
