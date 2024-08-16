// (c) Copyright Arduino. 2016
// Released under Apache License, version 2.0

#ifndef WebSocketClient_h
#define WebSocketClient_h

#include <Arduino.h>

#include "HttpClient.h"

#ifndef WS_TX_BUFFER_SIZE
  #define WS_TX_BUFFER_SIZE 128
#endif

static const int TYPE_CONTINUATION     = 0x0;
static const int TYPE_TEXT             = 0x1;
static const int TYPE_BINARY           = 0x2;
static const int TYPE_CONNECTION_CLOSE = 0x8;
static const int TYPE_PING             = 0x9;
static const int TYPE_PONG             = 0xa;

class WebSocketClient : public HttpClient
{
public:
    WebSocketClient(Client& aClient, const char* aServerName, uint16_t aServerPort = HttpClient::kHttpPort);
    WebSocketClient(Client& aClient, const String& aServerName, uint16_t aServerPort = HttpClient::kHttpPort);
    WebSocketClient(Client& aClient, const IPAddress& aServerAddress, uint16_t aServerPort = HttpClient::kHttpPort);

    /** Start the Web Socket connection to the specified path
      @param aURLPath     Path to use in request (optional, "/" is used by default)
      @return 0 if successful, else error
     */
    int begin(const char* aPath = "/");
    int begin(const String& aPath);

    /** Begin to send a message of type (TYPE_TEXT or TYPE_BINARY)
        Use the write or Stream API's to set message content, followed by endMessage
        to complete the message.
      @param aURLPath     Path to use in request
      @return 0 if successful, else error
    */
    int beginMessage(int aType);

    /** Completes sending of a message started by beginMessage
      @return 0 if successful, else error
    */
    int endMessage();

    /** Try to parse an incoming messages
      @return 0 if no message available, else size of parsed message
    */
    int parseMessage();

    /** Returns type of current parsed message
      @return type of current parsedMessage (TYPE_TEXT or TYPE_BINARY)
    */
    int messageType();

    /** Returns if the current message is the final chunk of a split
        message
      @return true for final message, false otherwise
    */
    bool isFinal();

    /** Read the current messages as a string
      @return current message as a string
    */
    String readString();

    /** Send a ping
      @return 0 if successful, else error
    */
    int ping();

    // Inherited from Print
    virtual size_t write(uint8_t aByte);
    virtual size_t write(const uint8_t *aBuffer, size_t aSize);
    // Inherited from Stream
    virtual int available();
    /** Read the next byte from the server.
      @return Byte read or -1 if there are no bytes available.
    */
    virtual int read();
    virtual int read(uint8_t *buf, size_t size);
    virtual int peek();

private:
    void flushRx();

private:
    bool iTxStarted;
    uint8_t iTxMessageType;
    uint8_t iTxBuffer[WS_TX_BUFFER_SIZE];
    uint64_t iTxSize;

    uint8_t iRxOpCode;
    uint64_t iRxSize;
    bool iRxMasked;
    int iRxMaskIndex;
    uint8_t iRxMaskKey[4];
};

#endif
