/*
  Client.h - Client class for Raspberry Pi
  Copyright (c) 2016 Hristo Gochkov  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "WiFiClient.h"
#include "WiFi.h"
#include "lwip/sockets.h"
#include "lwip/netdb.h"
#include <errno.h>
#include "tal_network.h"
#include "tal_log.h"

#define WIFI_CLIENT_DEF_CONN_TIMEOUT_MS  (3000)
#define WIFI_CLIENT_MAX_WRITE_RETRY      (10)
#define WIFI_CLIENT_SELECT_TIMEOUT_US    (1000000)
#define WIFI_CLIENT_FLUSH_BUFFER_SIZE    (1024)

#undef connect
#undef write
#undef read

class WiFiClientRxBuffer {
private:
        size_t _size;
        uint8_t *_buffer;
        size_t _pos;
        size_t _fill;
        int _fd;
        bool _failed;

        size_t r_available()
        {
            if(_fd < 0){
                return 0;
            }
            int count;

            int res = lwip_ioctl(_fd, FIONREAD, &count);
            if(res < 0) {
                _failed = true;
                return 0;
            }
            return count;
        }

        size_t fillBuffer()
        {
            if(!_buffer){
                _buffer = (uint8_t *)malloc(_size);
                if(!_buffer) {
                    PR_ERR("Not enough memory to allocate buffer");
                    _failed = true;
                    return 0;
                }
            }
            if(_fill && _pos == _fill){
                _fill = 0;
                _pos = 0;
            }
            if(!_buffer || _size <= _fill || !r_available()) {
                return 0;
            }
            int res = recv(_fd, _buffer + _fill, _size - _fill, MSG_DONTWAIT);
            if(res < 0) {
                if(errno != EWOULDBLOCK) {
                    _failed = true;
                }
                return 0;
            }
            _fill += res;
            return res;
        }

public:
    WiFiClientRxBuffer(int fd, size_t size=1436)
        :_size(size)
        ,_buffer(NULL)
        ,_pos(0)
        ,_fill(0)
        ,_fd(fd)
        ,_failed(false)
    {
        //_buffer = (uint8_t *)malloc(_size);
    }

    ~WiFiClientRxBuffer()
    {
        free(_buffer);
    }

    bool failed(){
        return _failed;
    }

    int read(uint8_t * dst, size_t len){
        if(!dst || !len || (_pos == _fill && !fillBuffer())){
            return _failed ? -1 : 0;
        }
        size_t a = _fill - _pos;
        if(len <= a || ((len - a) <= (_size - _fill) && fillBuffer() >= (len - a))){
            if(len == 1){
                *dst = _buffer[_pos];
            } else {
                memcpy(dst, _buffer + _pos, len);
            }
            _pos += len;
            return len;
        }
        size_t left = len;
        size_t toRead = a;
        uint8_t * buf = dst;
        memcpy(buf, _buffer + _pos, toRead);
        _pos += toRead;
        left -= toRead;
        buf += toRead;
        while(left){
            if(!fillBuffer()){
                return len - left;
            }
            a = _fill - _pos;
            toRead = (a > left)?left:a;
            memcpy(buf, _buffer + _pos, toRead);
            _pos += toRead;
            left -= toRead;
            buf += toRead;
        }
        return len;
    }

    int peek(){
        if(_pos == _fill && !fillBuffer()){
            return -1;
        }
        return _buffer[_pos];
    }

    size_t available(){
        return _fill - _pos + r_available();
    }

    void flush(){
        if(r_available()){
            fillBuffer();
        }
        _pos = _fill;
    }
};

class WiFiClientSocketHandle {
private:
    int sockfd;

public:
    WiFiClientSocketHandle(int fd):sockfd(fd)
    {
    }

    ~WiFiClientSocketHandle()
    {
        tal_net_close(sockfd);
    }

    int fd()
    {
        return sockfd;
    }
};

WiFiClient::WiFiClient():_rxBuffer(nullptr),_connected(false),_timeout(WIFI_CLIENT_DEF_CONN_TIMEOUT_MS),next(NULL)
{
}

WiFiClient::WiFiClient(int fd):_connected(true),_timeout(WIFI_CLIENT_DEF_CONN_TIMEOUT_MS),next(NULL)
{
    clientSocketHandle.reset(new WiFiClientSocketHandle(fd));
    _rxBuffer.reset(new WiFiClientRxBuffer(fd));
}

WiFiClient::~WiFiClient()
{
    stop();
}

WiFiClient & WiFiClient::operator=(const WiFiClient &other)
{
    stop();
    clientSocketHandle = other.clientSocketHandle;
    _rxBuffer = other._rxBuffer;
    _connected = other._connected;
    return *this;
}

void WiFiClient::stop()
{
    clientSocketHandle = NULL;
    _rxBuffer = NULL;
    _connected = false;
}

int WiFiClient::connect(IPAddress ip, uint16_t port)
{
    return connect(ip,port,_timeout);
}
int WiFiClient::connect(IPAddress ip, uint16_t port, int32_t timeout_ms)
{
    PR_INFO("connect: %s,port:%d\r\n", ip.toString().c_str(),port);
    _timeout = timeout_ms;

    int sockfd = tal_net_socket_create(PROTOCOL_TCP);
    if (sockfd < 0) {
        PR_ERR("socket: %d\r\n", errno);
        return 0;
    }
    tal_net_set_block(sockfd,0);
   
    uint32_t tmpIP = static_cast<uint32_t>(ip);
    TUYA_IP_ADDR_T serverIP = (TUYA_IP_ADDR_T)UNI_HTONL(tmpIP);

    TUYA_FD_SET_T fdset;
    TAL_FD_ZERO(&fdset);
    TAL_FD_SET(sockfd, &fdset);


    struct timeval tv;
    tv.tv_sec = _timeout / 1000;
    tv.tv_usec = (_timeout  % 1000) * 1000;

    int res = tal_net_connect(sockfd,serverIP, port);

    if (res < 0 && errno != EINPROGRESS) {
        PR_ERR("connect on fd %d, errno: %d, \"%s\"", sockfd, errno, strerror(errno));
        tal_net_close(sockfd);
        return 0;
    }

    res = tal_net_select(sockfd + 1, nullptr, &fdset, nullptr, _timeout<0 ? 50 :_timeout);
    if (res < 0) {
        PR_ERR("select on fd %d, errno: %d, \"%s\"", sockfd, errno, strerror(errno));
        tal_net_close(sockfd);
        return 0;
    } else if (res == 0) {
        PR_ERR("select returned due to timeout %d ms for fd %d", _timeout, sockfd);
        tal_net_close(sockfd);
        return 0;
    } else {
        int sockerr;
        int len = (int  )sizeof(int);
        res =  tal_net_getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &sockerr, &len);

        if (res < 0) {
            PR_ERR("getsockopt on fd %d, errno: %d, \"%s\"", sockfd, errno, strerror(errno));
            tal_net_close(sockfd);
            return 0;
        }

        if (sockerr != 0) {
            PR_ERR("socket error on fd %d, errno: %d, \"%s\"", sockfd, sockerr, strerror(sockerr));
            tal_net_close(sockfd);
            return 0;
        }
    }

#define ROE_WIFICLIENT(x,msg) { if (((x)<0)) { PR_ERR("Setsockopt '" msg "'' on fd %d failed. errno: %d, \"%s\"", sockfd, errno, strerror(errno)); return 0; }}
    ROE_WIFICLIENT(tal_net_setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv)),"SO_SNDTIMEO");
    ROE_WIFICLIENT(tal_net_setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)),"SO_RCVTIMEO");

    // These are also set in WiFiClientSecure, should be set here too?
    //ROE_WIFICLIENT(tal_net_setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, &enable, sizeof(enable)),"TCP_NODELAY"); 
    //ROE_WIFICLIENT (tal_net_setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, &enable, sizeof(enable)),"SO_KEEPALIVE");

    tal_net_set_block(sockfd,1);
    clientSocketHandle.reset(new WiFiClientSocketHandle(sockfd));
    _rxBuffer.reset(new WiFiClientRxBuffer(sockfd));

    _connected = true;
    return 1;
}

int WiFiClient::connect(const char *host, uint16_t port)
{
    return connect(host,port,_timeout);
}

int WiFiClient::connect(const char *host, uint16_t port, int32_t timeout_ms)
{
    IPAddress srv((uint32_t)0);
    if(!WiFiGenericClass::hostByName(host, srv)){
        return 0;
    }
    return connect(srv, port, timeout_ms);
}

int WiFiClient::setSocketOption(int option, char* value, size_t len)
{
    return setSocketOption(SOL_SOCKET, option, (const void*)value, len);
}

int WiFiClient::setSocketOption(int level, int option, const void* value, size_t len)
{
    int res = tal_net_setsockopt(fd(), level, option, value, len);
    if(res < 0) {
        PR_ERR("fail on %d, errno: %d, \"%s\"", fd(), errno, strerror(errno));
    }
    return res;
}

int WiFiClient::setTimeout(uint32_t seconds)
{
    Client::setTimeout(seconds * 1000); // This should be here?
    _timeout = seconds * 1000;
    if(fd() >= 0) {
        struct timeval tv;
        tv.tv_sec = seconds;
        tv.tv_usec = 0;
        if(setSocketOption(SO_RCVTIMEO, (char *)&tv, sizeof(struct timeval)) < 0) {
            return -1;
        }
        return setSocketOption(SO_SNDTIMEO, (char *)&tv, sizeof(struct timeval));
    }
    else {
        return 0;
    }
}

int WiFiClient::setOption(int option, int *value)
{
    return setSocketOption(IPPROTO_TCP, option, (const void*)value, sizeof(int));
}

int WiFiClient::getOption(int option, int *value)
{
	int size = sizeof(int);
    int res = tal_net_getsockopt(fd(), IPPROTO_TCP, option, (char *)value, &size);
    if(res < 0) {
        PR_ERR("getOption fail on fd %d, errno: %d, \"%s\"", fd(), errno, strerror(errno));
    }
    return res;
}

int WiFiClient::setNoDelay(bool nodelay)
{
    int flag = nodelay;
    return setOption(TCP_NODELAY, &flag);
}

bool WiFiClient::getNoDelay()
{
    int flag = 0;
    getOption(TCP_NODELAY, &flag);
    return flag;
}

size_t WiFiClient::write(uint8_t data)
{
    return write(&data, 1);
}

int WiFiClient::read()
{
    uint8_t data = 0;
    int res = read(&data, 1);
    if(res < 0) {
        return res;
    }
    if (res == 0) {  //  No data available.
        return -1;
    }

    return data;
}

size_t WiFiClient::write(const uint8_t *buf, size_t size)
{
    int res =0;
    int retry = WIFI_CLIENT_MAX_WRITE_RETRY;
    int socketFileDescriptor = fd();
    size_t totalBytesSent = 0;
    size_t bytesRemaining = size;

    if(!_connected || (socketFileDescriptor < 0)) {
        return 0;
    }

    while(retry) {
        //use select to make sure the socket is ready for writing
        TUYA_FD_SET_T set;
        TAL_FD_ZERO(&set);        // empties the set
        TAL_FD_SET(socketFileDescriptor, &set); // adds FD to the set

        uint32_t timeoutMs = WIFI_CLIENT_SELECT_TIMEOUT_US / 1000;
        retry--;

        if(tal_net_select(socketFileDescriptor + 1, NULL, &set, NULL, timeoutMs) < 0) {
            return 0;
        }

        if(TAL_FD_ISSET(socketFileDescriptor, &set)) {
            res = send(socketFileDescriptor, (void*) buf, bytesRemaining, MSG_DONTWAIT);
            if(res > 0) {
                totalBytesSent += res;
                if (totalBytesSent >= size) {
                    //completed successfully
                    retry = 0;
                } else {
                    buf += res;
                    bytesRemaining -= res;
                    retry = WIFI_CLIENT_MAX_WRITE_RETRY;
                }
            }
            else if(res < 0) {
                PR_ERR("write fail on fd %d, errno: %d, \"%s\"", fd(), errno, strerror(errno));
                if(errno != EAGAIN) {
                    //if resource was busy, can try again, otherwise give up
                    stop();
                    res = 0;
                    retry = 0;
                }
            }
            else {
                // Try again
            }
        }
    }
    return totalBytesSent;
}

size_t WiFiClient::write_P(PGM_P buf, size_t size)
{
    return write(buf, size);
}

size_t WiFiClient::write(Stream &stream)
{
    uint8_t * buf = (uint8_t *)malloc(1360);
    if(!buf){
        return 0;
    }
    size_t toRead = 0, toWrite = 0, written = 0;
    size_t available = stream.available();
    while(available){
        toRead = (available > 1360)?1360:available;
        toWrite = stream.readBytes(buf, toRead);
        written += write(buf, toWrite);
        available = stream.available();
    }
    free(buf);
    return written;
}

int WiFiClient::read(uint8_t *buf, size_t size)
{
    int res = -1;
    if (_rxBuffer) {
        res = _rxBuffer->read(buf, size);
        if(_rxBuffer->failed()) {
            PR_ERR("read fail on fd %d, errno: %d, \"%s\"", fd(), errno, strerror(errno));
            stop();
        }
    }
    return res;
}

int WiFiClient::peek()
{
    int res = -1;
    if (_rxBuffer) {
        res = _rxBuffer->peek();
        if(_rxBuffer->failed()) {
            PR_ERR("peek fail on fd %d, errno: %d, \"%s\"", fd(), errno, strerror(errno));
            stop();
        }
    }
    return res;
}

int WiFiClient::available()
{
    if(!_rxBuffer)
    {
        return 0;
    }
    int res = _rxBuffer->available();
    if(_rxBuffer->failed()) {
        PR_ERR("available fail on fd %d, errno: %d, \"%s\"", fd(), errno, strerror(errno));
        stop();
    }
    return res;
}

// Though flushing means to send all pending data,
// seems that in Arduino it also means to clear RX
void WiFiClient::flush() {
    if (_rxBuffer != nullptr) {
        _rxBuffer->flush();
    }
}

uint8_t WiFiClient::connected()
{
    if (_connected) {
        uint8_t dummy;
        int res = recv(fd(), &dummy, 0, MSG_DONTWAIT);
        // avoid unused var warning by gcc
        (void)res;
        // recv only sets errno if res is <= 0
        if (res <= 0){
          switch (errno) {
              case EWOULDBLOCK:
              case ENOENT: //caused by vfs
                  _connected = true;
                  break;
              case ENOTCONN:
              case EPIPE:
              case ECONNRESET:
              case ECONNREFUSED:
              case ECONNABORTED:
                  _connected = false;
                  PR_ERR("Disconnected: RES: %d, ERR: %d", res, errno);
                  break;
              default:
                  PR_ERR("Unexpected: RES: %d, ERR: %d", res, errno);
                  _connected = true;
                  break;
          }
        } else {
          _connected = true;
        }
    }
    return _connected;
}

IPAddress WiFiClient::remoteIP(int fd) const
{
    struct sockaddr_storage addr;
    socklen_t len = sizeof addr;
    getpeername(fd, (struct sockaddr*)&addr, &len);
    struct sockaddr_in *s = (struct sockaddr_in *)&addr;
    return IPAddress((uint32_t)(s->sin_addr.s_addr));
}

uint16_t WiFiClient::remotePort(int fd) const
{
    struct sockaddr_storage addr;
    socklen_t len = sizeof addr;
    getpeername(fd, (struct sockaddr*)&addr, &len);
    struct sockaddr_in *s = (struct sockaddr_in *)&addr;
    return ntohs(s->sin_port);
}

IPAddress WiFiClient::remoteIP() const
{
    return remoteIP(fd());
}

uint16_t WiFiClient::remotePort() const
{
    return remotePort(fd());
}

IPAddress WiFiClient::localIP(int fd) const
{
    struct sockaddr_storage addr;
    socklen_t len = sizeof addr;
    getsockname(fd, (struct sockaddr*)&addr, &len);
    struct sockaddr_in *s = (struct sockaddr_in *)&addr;
    return IPAddress((uint32_t)(s->sin_addr.s_addr));
}

uint16_t WiFiClient::localPort(int fd) const
{
    struct sockaddr_storage addr;
    socklen_t len = sizeof addr;
    getsockname(fd, (struct sockaddr*)&addr, &len);
    struct sockaddr_in *s = (struct sockaddr_in *)&addr;
    return ntohs(s->sin_port);
}

IPAddress WiFiClient::localIP() const
{
    return localIP(fd());
}

uint16_t WiFiClient::localPort() const
{
    return localPort(fd());
}

bool WiFiClient::operator==(const WiFiClient& rhs)
{
    return clientSocketHandle == rhs.clientSocketHandle && remotePort() == rhs.remotePort() && remoteIP() == rhs.remoteIP();
}

int WiFiClient::fd() const
{
    if (clientSocketHandle == NULL) {
        return -1;
    } else {
        return clientSocketHandle->fd();
    }
}

