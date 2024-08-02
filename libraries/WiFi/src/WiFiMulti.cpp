#include <Arduino.h>
#include "WiFiMulti.h"

extern "C" {
#include <limits.h>
#include <string.h>
}
//#include <bk_hal.h>
#include "tkl_system.h"
#include "tkl_output.h"

WiFiMulti::WiFiMulti()
{
}

WiFiMulti::~WiFiMulti()
{
    for(uint32_t i = 0; i < APlist.size(); i++) {
        WifiAPlist_t entry = APlist[i];
        if(entry.ssid) {
            free(entry.ssid);
        }
        if(entry.passphrase) {
            free(entry.passphrase);
        }
    }
    APlist.clear();
}

bool WiFiMulti::addAP(const char* ssid, const char *passphrase)
{
#if 1
    WifiAPlist_t newAP;

    if(!ssid || *ssid == 0x00 || strlen(ssid) > 31) {
        // fail SSID too long or missing!
        tkl_log_output("[WIFI][APlistAdd] no ssid or ssid too long\r\n");
        return false;
    }

    if(passphrase && strlen(passphrase) > 64) {
        // fail passphrase too long!
        tkl_log_output("[WIFI][APlistAdd] passphrase too long\r\n");
        return false;
    }
    newAP.ssid =(char*) malloc(sizeof(char)*strlen(ssid));
    strcpy(newAP.ssid,ssid);
    //newAP.ssid = strdup(ssid);

    if(!newAP.ssid) {
        tkl_log_output("[WIFI][APlistAdd] fail newAP.ssid == 0\r\n");
        return false;
    }

    if(passphrase && *passphrase != 0x00) {
        newAP.passphrase =(char*) malloc(sizeof(char)*strlen(passphrase));
        strcpy(newAP.passphrase,passphrase);
        //newAP.passphrase = strdup(passphrase);
        if(!newAP.passphrase) {
            tkl_log_output("[WIFI][APlistAdd] fail newAP.passphrase == 0\r\n");
            free(newAP.ssid);
            return false;
        }
    } else {
        newAP.passphrase = NULL;
    }

    APlist.push_back(newAP);
    tkl_log_output("[WIFI][APlistAdd] add SSID: %s\r\n", newAP.ssid);
#endif
    return true;
}

uint8_t WiFiMulti::run(uint32_t connectTimeout)
{

    int8_t scanResult ;
    uint8_t status = WiFi.status();

    if(status == WSS_GOT_IP) {
        for(uint32_t x = 0; x < APlist.size(); x++) {
            if(WiFi.SSID()==APlist[x].ssid) {
                return status;
            }
        }
        tkl_log_output("WiFi.disconnect\r\n");
        WiFi.disconnect(false,false);
    
        tkl_system_delay(10);
        status = WiFi.status();
    }

    scanResult = WiFi.scanNetworks();
    if(scanResult == WIFI_SCAN_RUNNING) {
        // scan is running
        return WSS_NO_AP_FOUND;
    }
    else if(scanResult >= 0) {
        // scan done analyze
        WifiAPlist_t bestNetwork { NULL, NULL };
        int bestNetworkDb = INT_MIN;
        uint8_t bestBSSID[6];
        int32_t bestChannel = 0;

        tkl_log_output("[WIFI] scan done\r\n");

        if(scanResult == 0) {
            tkl_log_output("[WIFI] no networks found\r\n");
        } else {
            tkl_log_output("[WIFI] %d networks found\r\n", scanResult);
            for(int8_t i = 0; i < scanResult; ++i) {

                String ssid_scan;
                int32_t rssi_scan;
                uint8_t sec_scan;
                uint8_t* BSSID_scan;
                int32_t chan_scan;

                WiFi.getNetworkInfo(i, ssid_scan, sec_scan, rssi_scan, BSSID_scan, chan_scan);

                bool known = false;
                for(uint32_t x = APlist.size() ; x > 0; x--) {
                    WifiAPlist_t entry = APlist[x-1];

                    if(ssid_scan == entry.ssid) { // SSID match
                        known = true;
                        if(rssi_scan > bestNetworkDb) { // best network
                            if(sec_scan == WAAM_OPEN || entry.passphrase) { // check for passphrase if not open wlan
                                bestNetworkDb = rssi_scan;
                                bestChannel = chan_scan;
                                memcpy((void*) &bestNetwork, (void*) &entry, sizeof(bestNetwork));
                                memcpy((void*) &bestBSSID, (void*) BSSID_scan, sizeof(bestBSSID));
                            }
                        }
                        break;
                    }
                }

                if(known) {
                    tkl_log_output(" --->   %d: [%d][%02X:%02X:%02X:%02X:%02X:%02X] %s (%d) %c\r\n", i, chan_scan, BSSID_scan[0], BSSID_scan[1], BSSID_scan[2], BSSID_scan[3], BSSID_scan[4], BSSID_scan[5], ssid_scan, rssi_scan, (sec_scan == WAAM_OPEN) ? ' ' : '*');
                } else {
                    tkl_log_output("       %d: [%d][%02X:%02X:%02X:%02X:%02X:%02X] %s (%d) %c\r\n", i, chan_scan, BSSID_scan[0], BSSID_scan[1], BSSID_scan[2], BSSID_scan[3], BSSID_scan[4], BSSID_scan[5], ssid_scan, rssi_scan, (sec_scan == WAAM_OPEN) ? ' ' : '*');
                }
            }
        }

        // clean up ram
        WiFi.scanDelete();

        if(bestNetwork.ssid) {
            tkl_log_output("[WIFI] Connecting BSSID: %02X:%02X:%02X:%02X:%02X:%02X SSID: %s Channel: %d (%d)\r\n", bestBSSID[0], bestBSSID[1], bestBSSID[2], bestBSSID[3], bestBSSID[4], bestBSSID[5], bestNetwork.ssid, bestChannel, bestNetworkDb);

            WiFi.begin(bestNetwork.ssid, bestNetwork.passphrase, bestChannel, bestBSSID);
            status = WiFi.status();

            auto startTime = millis();
            // wait for connection, fail, or timeout
            while(status != WSS_GOT_IP && status != WSS_NO_AP_FOUND && status != WSS_CONN_FAIL && (millis() - startTime) <= connectTimeout) {
                //rtos_delay_milliseconds(10);
                tkl_system_delay(10);
                status = WiFi.status();
            }

            switch(status) {
            case WSS_GOT_IP:
                tkl_log_output("[WIFI] Connecting done.\r\n");
                tkl_log_output("[WIFI] SSID: %s\r\n", WiFi.SSID().c_str());
                tkl_log_output("[WIFI] IP: %s\r\n", WiFi.localIP().toString().c_str());
                tkl_log_output("[WIFI] MAC: %s\r\n", WiFi.BSSIDstr().c_str());
                tkl_log_output("[WIFI] Channel: %d\r\n", WiFi.channel());
                break;
            case WSS_NO_AP_FOUND:
                tkl_log_output("[WIFI] Connecting Failed AP not found.\r\n");
                break;
            case WSS_CONN_FAIL:
                tkl_log_output("[WIFI] Connecting Failed.\r\n");
                break;
            default:
                tkl_log_output("[WIFI] Connecting Failed (%d).\r\n", status);
                break;
            }
        } 
        else {
            tkl_log_output("[WIFI] no matching wifi found!\r\n");
        }
    } 
    else {
        // start scan
        tkl_log_output("[WIFI] delete old wifi config...\r\n");
        WiFi.disconnect();

        tkl_log_output("[WIFI] start scan\r\n");
        // scan wifi async mode
        WiFi.scanNetworks(true);
    }
    return status;
}
