// (c) Copyright Arduino. 2016
// Released under Apache License, version 2.0

#include "b64.h"

#include "WebSocketClient.h"

WebSocketClient::WebSocketClient(Client& aClient, const char* aServerName, uint16_t aServerPort)
 : HttpClient(aClient, aServerName, aServerPort),
   iTxStarted(false),
   iRxSize(0)
{
}

WebSocketClient::WebSocketClient(Client& aClient, const String& aServerName, uint16_t aServerPort) 
 : HttpClient(aClient, aServerName, aServerPort),
   iTxStarted(false),
   iRxSize(0)
{
}

WebSocketClient::WebSocketClient(Client& aClient, const IPAddress& aServerAddress, uint16_t aServerPort)
 : HttpClient(aClient, aServerAddress, aServerPort),
   iTxStarted(false),
   iRxSize(0)
{
}

int WebSocketClient::begin(const char* aPath)
{
    // start the GET request
    beginRequest();
    connectionKeepAlive();
    int status = get(aPath);

    if (status == 0)
    {
        uint8_t randomKey[16];
        char base64RandomKey[25];

        // create a random key for the connection upgrade
        for (int i = 0; i < (int)sizeof(randomKey); i++)
        {
            randomKey[i] = random(0x01, 0xff);
        }
        memset(base64RandomKey, 0x00, sizeof(base64RandomKey));
        b64_encode(randomKey, sizeof(randomKey), (unsigned char*)base64RandomKey, sizeof(base64RandomKey));

        // start the connection upgrade sequence
        sendHeader("Upgrade", "websocket");
        sendHeader("Connection", "Upgrade");
        sendHeader("Sec-WebSocket-Key", base64RandomKey);
        sendHeader("Sec-WebSocket-Version", "13");
        endRequest();

        status = responseStatusCode();

        if (status > 0)
        {
            skipResponseHeaders();
        }
    }

    iRxSize = 0;

    // status code of 101 means success
    return (status == 101) ? 0 : status;
}

int WebSocketClient::begin(const String& aPath)
{
    return begin(aPath.c_str());
}

int WebSocketClient::beginMessage(int aType)
{
    if (iTxStarted)
    {
        // fail TX already started
        return 1;
    }

    iTxStarted = true;
    iTxMessageType = (aType & 0xf);
    iTxSize = 0;

    return 0;
}

int WebSocketClient::endMessage()
{
    if (!iTxStarted)
    {
        // fail TX not started
        return 1;
    }

    // send FIN + the message type (opcode)
    HttpClient::write(0x80 | iTxMessageType);

    // the message is masked (0x80)
    // send the length
    if (iTxSize < 126)
    {
        HttpClient::write(0x80 | (uint8_t)iTxSize);
    }
    else if (iTxSize < 0xffff)
    {
        HttpClient::write(0x80 | 126);
        HttpClient::write((iTxSize >> 8) & 0xff);
        HttpClient::write((iTxSize >> 0) & 0xff);
    }
    else
    {
        HttpClient::write(0x80 | 127);
        HttpClient::write((iTxSize >> 56) & 0xff);
        HttpClient::write((iTxSize >> 48) & 0xff);
        HttpClient::write((iTxSize >> 40) & 0xff);
        HttpClient::write((iTxSize >> 32) & 0xff);
        HttpClient::write((iTxSize >> 24) & 0xff);
        HttpClient::write((iTxSize >> 16) & 0xff);
        HttpClient::write((iTxSize >> 8) & 0xff);
        HttpClient::write((iTxSize >> 0) & 0xff);
    }

    uint8_t maskKey[4];

    // create a random mask for the data and send
    for (int i = 0; i < (int)sizeof(maskKey); i++)
    {
        maskKey[i] = random(0xff);
    }
    HttpClient::write(maskKey, sizeof(maskKey));

    // mask the data and send
    for (int i = 0; i < (int)iTxSize; i++) {
        iTxBuffer[i] ^= maskKey[i % sizeof(maskKey)];
    }

    size_t txSize = iTxSize;

    iTxStarted = false;
    iTxSize = 0;

    return (HttpClient::write(iTxBuffer, txSize) == txSize) ? 0 : 1;
}

size_t WebSocketClient::write(uint8_t aByte)
{
    return write(&aByte, sizeof(aByte));
}

size_t WebSocketClient::write(const uint8_t *aBuffer, size_t aSize)
{
    if (iState < eReadingBody)
    {
        // have not upgraded the connection yet
        return HttpClient::write(aBuffer, aSize);
    }

    if (!iTxStarted)
    {
        // fail TX not   started
        return 0;
    }

    // check if the write size, fits in the buffer
    if ((iTxSize + aSize) > sizeof(iTxBuffer))
    {
        aSize = sizeof(iTxSize) - iTxSize;
    }

    // copy data into the buffer
    memcpy(iTxBuffer + iTxSize, aBuffer, aSize);

    iTxSize += aSize;
    
    return aSize;
}

int WebSocketClient::parseMessage()
{
    flushRx();

    // make sure 2 bytes (opcode + length)
    // are available
    if (HttpClient::available() < 2)
    {
        return 0;
    }

    // read open code and length
    uint8_t opcode = HttpClient::read();
    int length = HttpClient::read();

    if ((opcode & 0x0f) == 0)
    {
        // continuation, use previous opcode and update flags
        iRxOpCode |= opcode;
    }
    else
    {
        iRxOpCode = opcode;
    }

    iRxMasked = (length & 0x80);
    length &= 0x7f;

    // read the RX size
    if (length < 126)
    {
        iRxSize = length;
    }
    else if (length == 126)
    {
        iRxSize = (HttpClient::read() << 8) | HttpClient::read();
    }
    else
    {
        iRxSize = ((uint64_t)HttpClient::read() << 56) | 
                    ((uint64_t)HttpClient::read() << 48) | 
                    ((uint64_t)HttpClient::read() << 40) | 
                    ((uint64_t)HttpClient::read() << 32) | 
                    ((uint64_t)HttpClient::read() << 24) | 
                    ((uint64_t)HttpClient::read() << 16) | 
                    ((uint64_t)HttpClient::read() << 8)  |
                    (uint64_t)HttpClient::read(); 
    }

    // read in the mask, if present
    if (iRxMasked)
    {
        for (int i = 0; i < (int)sizeof(iRxMaskKey); i++)
        {
            iRxMaskKey[i] = HttpClient::read();
        }
    }

    iRxMaskIndex = 0;

    if (TYPE_CONNECTION_CLOSE == messageType())
    {
        flushRx();
        stop();
        iRxSize = 0;
    }
    else if (TYPE_PING == messageType())
    {
        beginMessage(TYPE_PONG);
        while(available())
        {
            write(read());
        }
        endMessage();

        iRxSize = 0;
    }
    else if (TYPE_PONG == messageType())
    {
        flushRx();
        iRxSize = 0;
    }

    return iRxSize;
}

int WebSocketClient::messageType()
{
    return (iRxOpCode & 0x0f);
}

bool WebSocketClient::isFinal()
{
    return ((iRxOpCode & 0x80) != 0);
}

String WebSocketClient::readString()
{
    int avail = available();
    String s;

    if (avail > 0)
    {
        s.reserve(avail);

        for (int i = 0; i < avail; i++)
        {
            s += (char)read();
        }
    }

    return s;
}

int WebSocketClient::ping()
{
    uint8_t pingData[16];

    // create random data for the ping
    for (int i = 0; i < (int)sizeof(pingData); i++)
    {
        pingData[i] = random(0xff);
    }

    beginMessage(TYPE_PING);
    write(pingData, sizeof(pingData));
    return endMessage();
}

int WebSocketClient::available()
{
    if (iState < eReadingBody)
    {
        return HttpClient::available();
    }

    return iRxSize;
}

int WebSocketClient::read()
{
    byte b;

    if (read(&b, sizeof(b)))
    {
        return b;
    }

    return -1;
}

int WebSocketClient::read(uint8_t *aBuffer, size_t aSize)
{
    int readCount = HttpClient::read(aBuffer, aSize);

    if (readCount > 0)
    {
        iRxSize -= readCount;

        // unmask the RX data if needed
        if (iRxMasked)
        {
            for (int i = 0; i < (int)aSize; i++, iRxMaskIndex++)
            {
                aBuffer[i] ^= iRxMaskKey[iRxMaskIndex % sizeof(iRxMaskKey)];
            }
        }
    }

    return readCount;
}

int WebSocketClient::peek()
{
    int p = HttpClient::peek();

    if (p != -1 && iRxMasked)
    {
        // unmask the RX data if needed
        p = (uint8_t)p ^ iRxMaskKey[iRxMaskIndex % sizeof(iRxMaskKey)];
    }

    return p;
}

void WebSocketClient::flushRx()
{
    while(available())
    {
        read();
    }
}
