#ifndef WIFIAP_H_
#define WIFIAP_H_

#include "WiFiType.h"
#include "WiFiGeneric.h"

#define   WLAN_DEFAULT_IP         "192.168.176.1"
#define   WLAN_DEFAULT_GW         "192.168.176.1"
#define   WLAN_DEFAULT_MASK       "255.255.255.0"

class WiFiAPClass
{
public:
    bool softAP(const char* ssid, const char* passphrase = NULL, int channel = 1, int ssid_hidden = 0, int max_connection = 4, bool ftm_responder = false);
    bool softAPConfig(IPAddress local_ip, IPAddress gateway, IPAddress subnet, IPAddress dhcp_lease_start = (uint32_t) 0);
    bool softAPdisconnect(bool wifioff = false);

    uint8_t softAPgetStationNum();
    IPAddress softAPIP();

    IPAddress softAPBroadcastIP();
    IPAddress softAPNetworkID();
    IPAddress softAPSubnetMask();
    uint8_t softAPSubnetCIDR();

    const char * softAPgetHostname();
    bool softAPsetHostname(const char * hostname);

    uint8_t* softAPmacAddress(uint8_t* mac);
    String softAPmacAddress(void);
    String softAPSSID(void) const;
    
private:
    WF_AP_CFG_IF_S ap_cfg_info ;
    WF_STA_INFO_S * sta_info ;
    uint32_t sta_num;
};

#endif /* WIFIAP_H_*/
