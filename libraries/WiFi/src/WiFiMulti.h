#ifndef WIFICLIENTMULTI_H_
#define WIFICLIENTMULTI_H_

#include "WiFi.h"
#include <vector>

typedef struct {
    char * ssid;
    char * passphrase;
} WifiAPlist_t;

class WiFiMulti
{
public:
    WiFiMulti();
    ~WiFiMulti();

    bool addAP(const char* ssid, const char *passphrase = NULL);
    uint8_t run(uint32_t connectTimeout=5000);

private:
    std::vector<WifiAPlist_t> APlist;
};

#endif /* WIFICLIENTMULTI_H_ */
