#ifndef HTTPClient_H_
#define HTTPClient_H_

#ifdef __cplusplus
extern "C"{
#endif

#include "http_client_interface.h"

class HTTPClient
{
public:
    HTTPClient();
    ~HTTPClient();

    bool begin(String url);

    http_client_status_t GET(http_client_header_t *headers,uint8_t headers_length,const uint8_t *ca, size_t ca_len, http_client_response_t *response);
    http_client_status_t POST(http_client_header_t *headers,uint8_t  headers_length, const  uint8_t *body, http_client_response_t *response);
    http_client_status_t PATCH(http_client_header_t *headers,uint8_t  headers_length, const  uint8_t *body, http_client_response_t *response);
    http_client_status_t PUT(http_client_header_t *headers,uint8_t  headers_length, const  uint8_t *body, http_client_response_t *response);
    bool end(http_client_response_t *response);

protected:
    String _host;
    uint16_t _port = 0;
    String _path;
    String _protocol;
    bool beginInternal(String url);
};



#ifdef __cplusplus
}
#endif


#endif 
