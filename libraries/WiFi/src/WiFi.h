#ifndef WiFi_h
#define WiFi_h

#include <stdint.h>
#include "api/Print.h"

#include "WiFiType.h"
#include "WiFiSTA.h"
#include "WiFiAP.h"
#include "WiFiScan.h"
#include "WiFiGeneric.h"

#include "WiFiClient.h"
#include "WiFiServer.h"
#include "WiFiUdp.h"

class WiFiClass : public WiFiGenericClass, public WiFiSTAClass, public WiFiScanClass, public WiFiAPClass
{
private:
    bool prov_enable;
public:
    WiFiClass()
    {
        prov_enable = false;
    }

    using WiFiGenericClass::channel;

    using WiFiSTAClass::SSID;
    using WiFiSTAClass::RSSI;
    using WiFiSTAClass::BSSID;
    using WiFiSTAClass::BSSIDstr;

    using WiFiScanClass::SSID;
   // using WiFiScanClass::encryptionType;
    using WiFiScanClass::RSSI;
    using WiFiScanClass::BSSID;
    using WiFiScanClass::BSSIDstr;
    using WiFiScanClass::channel;
public:  
    void printDiag(Print& dest);
    friend class WiFiClient;
    friend class WiFiServer;
    friend class WiFiUDP;
    void enableProv(bool status);
    bool isProvEnabled();
};

extern WiFiClass WiFi;

#endif