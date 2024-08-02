#ifndef _WIFICLIENT_H_
#define _WIFICLIENT_H_

#include <Arduino.h>
#include "api/Client.h"
#include "cbuf.h"
#include <memory>

class WiFiClientSocketHandle;
#define TCP_TX_PACKET_MAX_SIZE 1436
#define TCP_RX_PACKET_MAX_SIZE 1436
class LwIPClient : public Client
{
public:
        virtual int connect(IPAddress ip, uint16_t port, int32_t timeout) = 0;
        virtual int connect(const char *host, uint16_t port, int32_t timeout) = 0;
        virtual int setTimeout(uint32_t seconds) = 0;
};

class WiFiClient : public LwIPClient
{
protected:
    std::shared_ptr<WiFiClientSocketHandle> clientSocketHandle;
    bool _connected ;
    int _timeout;
    cbuf *_rxBuff;
public:
    WiFiClient *next;
    WiFiClient();
    WiFiClient(int fd);
    ~WiFiClient();
    int connect(IPAddress ip, uint16_t port);
    int connect(IPAddress ip, uint16_t port, int32_t timeout_ms);
    int connect(const char *host, uint16_t port);
    int connect(const char *host, uint16_t port, int32_t timeout_ms);
    size_t write(uint8_t data);
    size_t write(const uint8_t *buf, size_t size);
    size_t write_P(PGM_P buf, size_t size);
    size_t write(Stream &stream);
    int available();
    int read();
    int read(uint8_t *buf, size_t size);
    int peek();
    void flush();
    void stop();
    uint8_t connected();

    operator bool()
    {
        return connected();
    }
    WiFiClient & operator=(const WiFiClient &other);
    bool operator==(const bool value)
    {
        return bool() == value;
    }
    bool operator!=(const bool value)
    {
        return bool() != value;
    }
    bool operator==(const WiFiClient&);
    bool operator!=(const WiFiClient& rhs)
    {
        return !this->operator==(rhs);
    };

    virtual int fd() const;

    int setSocketOption(int option, char* value, size_t len);
    int setSocketOption(int level, int option, const void* value, size_t len);
    int setOption(int option, int *value);
    int getOption(int option, int *value);
    int setTimeout(uint32_t seconds);
    int setNoDelay(bool nodelay);
    bool getNoDelay();

    IPAddress remoteIP() const;
    IPAddress remoteIP(int fd) const;
    uint16_t remotePort() const;
    uint16_t remotePort(int fd) const;
    IPAddress localIP() const;
    IPAddress localIP(int fd) const;
    uint16_t localPort() const;
    uint16_t localPort(int fd) const;

    //friend class WiFiServer;
    using Print::write;
};

#endif /* _WIFICLIENT_H_ */
