#include  <Arduino.h>
#include "HTTPClient.h"
#include "tal_log.h"
HTTPClient::HTTPClient()
{
}

HTTPClient::~HTTPClient()
{
}

bool HTTPClient::begin(String url)
{
    return beginInternal(url);
}

bool HTTPClient::beginInternal(String url)
{
    PR_INFO("url: %s", url.c_str());
    // check for : (http: or https:
    int index = url.indexOf(':');
    if(index < 0) {
        PR_ERR("failed to parse protocol");
        return false;
    }

    _protocol = url.substring(0, index);


    url.remove(0, (index + 3)); // remove http:// or https://

    index = url.indexOf('/');
    if (index == -1) {
        index = url.length();
        url += '/';
    }
    String host = url.substring(0, index);
    url.remove(0, index); // remove host part

    // get Authorization
    index = host.indexOf('@');
    if(index >= 0) {
        // auth info
        String auth = host.substring(0, index);
        host.remove(0, index + 1); // remove auth part including @
        //_base64Authorization = base64::encode(auth);
    }

    // get port
    index = host.indexOf(':');
    String the_host;
    if(index >= 0) {
        the_host = host.substring(0, index); // hostname
        host.remove(0, (index + 1)); // remove hostname + :
        _port = host.toInt(); // get port
    } else {
        the_host = host;
    }
    
    _host = the_host;
    _path = url;
    PR_INFO("protocol: %s, host: %s port: %d url: %s", _protocol.c_str(), _host.c_str(), _port, _path.c_str());
    return true;
}

http_client_status_t HTTPClient::GET(http_client_header_t *headers,uint8_t  headers_length, const uint8_t *ca , size_t ca_len ,http_client_response_t *response)
{

    /* HTTP headers */
    PR_DEBUG("http request send!");
    const http_client_request_t http_request = {
            .host = _host.c_str(),
            .path = _path.c_str(),
            .cacert = ca,
            .cacert_len = ca_len,
            .method = "GET",
            .headers = headers,
            .headers_count = headers_length,
            .body = NULL,
            .body_length = 0,
            .timeout_ms = 10
        };
    http_client_status_t http_status = http_client_request(&http_request, response);
    if (HTTP_CLIENT_SUCCESS != http_status) {
        PR_ERR("http_request_send error:%d", http_status);
    }
    return http_status;
}

http_client_status_t HTTPClient::POST(http_client_header_t *headers,uint8_t headers_length,  const  uint8_t *body,  http_client_response_t *response)
{
    PR_DEBUG("http request send!");
    const http_client_request_t http_request = {
            .host = _host.c_str(),
            .path = _path.c_str(),
            .method = "POST",
            .headers = headers,
            .headers_count = headers_length,
            .body = body,
            .body_length = body?strlen((const char*)body):0,
            .timeout_ms = 10
        };
    http_client_status_t http_status = http_client_request(&http_request, response);
    if (HTTP_CLIENT_SUCCESS != http_status) {
        PR_ERR("http_request_send error:%d", http_status);
    }
    return http_status;
}

http_client_status_t HTTPClient::PUT(http_client_header_t *headers,uint8_t headers_length,  const  uint8_t *body,  http_client_response_t *response)
{
    PR_DEBUG("http request send!");
    const http_client_request_t http_request = {
            .host = _host.c_str(),
            .path = _path.c_str(),
            .method = "PUT",
            .headers = headers,
            .headers_count = headers_length,
            .body = body,
            .body_length = body?strlen((const char*)body):0,
            .timeout_ms = 10
        };
    http_client_status_t http_status = http_client_request(&http_request, response);
    if (HTTP_CLIENT_SUCCESS != http_status) {
        PR_ERR("http_request_send error:%d", http_status);
    }
    return http_status;
}

http_client_status_t HTTPClient::PATCH(http_client_header_t *headers,uint8_t headers_length,  const  uint8_t *body,  http_client_response_t *response)
{
    PR_DEBUG("http request send!");
    const http_client_request_t http_request = {
            .host = _host.c_str(),
            .path = _path.c_str(),
            .method = "PATCH",
            .headers = headers,
            .headers_count = headers_length,
            .body = body,
            .body_length = body?strlen((const char*)body):0,
            .timeout_ms = 10
        };
    http_client_status_t http_status = http_client_request(&http_request, response);
    if (HTTP_CLIENT_SUCCESS != http_status) {
        PR_ERR("http_request_send error:%d", http_status);
    }
    return http_status;
}
bool HTTPClient::end(http_client_response_t *response)
{
    return http_client_free(response);
}