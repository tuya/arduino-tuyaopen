// Class to simplify HTTP fetching on Arduino
// (c) Copyright 2010-2011 MCQN Ltd
// Released under Apache License, version 2.0

#include "HttpClient.h"
#include "b64.h"

// Initialize constants
const char* HttpClient::kUserAgent = "Arduino/2.2.0";
const char* HttpClient::kContentLengthPrefix = HTTP_HEADER_CONTENT_LENGTH ": ";
const char* HttpClient::kTransferEncodingChunked = HTTP_HEADER_TRANSFER_ENCODING ": " HTTP_HEADER_VALUE_CHUNKED;

HttpClient::HttpClient(Client& aClient, const char* aServerName, uint16_t aServerPort)
 : iClient(&aClient), iServerName(aServerName), iServerAddress(), iServerPort(aServerPort),
   iConnectionClose(true), iSendDefaultRequestHeaders(true)
{
  resetState();
}

HttpClient::HttpClient(Client& aClient, const String& aServerName, uint16_t aServerPort)
 : HttpClient(aClient, aServerName.c_str(), aServerPort)
{
}

HttpClient::HttpClient(Client& aClient, const IPAddress& aServerAddress, uint16_t aServerPort)
 : iClient(&aClient), iServerName(NULL), iServerAddress(aServerAddress), iServerPort(aServerPort),
   iConnectionClose(true), iSendDefaultRequestHeaders(true)
{
  resetState();
}

void HttpClient::resetState()
{
  iState = eIdle;
  iStatusCode = 0;
  iContentLength = kNoContentLengthHeader;
  iBodyLengthConsumed = 0;
  iContentLengthPtr = kContentLengthPrefix;
  iTransferEncodingChunkedPtr = kTransferEncodingChunked;
  iIsChunked = false;
  iChunkLength = 0;
  iHttpResponseTimeout = kHttpResponseTimeout;
  iHttpWaitForDataDelay = kHttpWaitForDataDelay;
}

void HttpClient::stop()
{
  iClient->stop();
  resetState();
}

void HttpClient::connectionKeepAlive()
{
  iConnectionClose = false;
}

void HttpClient::noDefaultRequestHeaders()
{
  iSendDefaultRequestHeaders = false;
}

void HttpClient::beginRequest()
{
  iState = eRequestStarted;
}

int HttpClient::startRequest(const char* aURLPath, const char* aHttpMethod, 
                                const char* aContentType, int aContentLength, const byte aBody[])
{
    if (iState == eReadingBody || iState == eReadingChunkLength || iState == eReadingBodyChunk)
    {
        flushClientRx();

        resetState();
    }

    tHttpState initialState = iState;

    if ((eIdle != iState) && (eRequestStarted != iState))
    {
        return HTTP_ERROR_API;
    }

    if (iConnectionClose || !iClient->connected())
    {
        if (iServerName)
        {
            if (!(iClient->connect(iServerName, iServerPort) > 0))
            {
#ifdef LOGGING
                Serial.println("Connection failed");
#endif
                return HTTP_ERROR_CONNECTION_FAILED;
            }
        }
        else
        {
            if (!(iClient->connect(iServerAddress, iServerPort) > 0))
            {
#ifdef LOGGING
                Serial.println("Connection failed");
#endif
                return HTTP_ERROR_CONNECTION_FAILED;
            }    
        }
    }
    else
    {
#ifdef LOGGING
        Serial.println("Connection already open");
#endif
    }

    // Now we're connected, send the first part of the request
    int ret = sendInitialHeaders(aURLPath, aHttpMethod);

    if (HTTP_SUCCESS == ret)
    {
        if (aContentType)
        {
            sendHeader(HTTP_HEADER_CONTENT_TYPE, aContentType);
        }

        if (aContentLength > 0)
        {
            sendHeader(HTTP_HEADER_CONTENT_LENGTH, aContentLength);
        }

        bool hasBody = (aBody && aContentLength > 0);

        if (initialState == eIdle || hasBody)
        {
            // This was a simple version of the API, so terminate the headers now
            finishHeaders();
        }
        // else we'll call it in endRequest or in the first call to print, etc.

        if (hasBody)
        {
                write(aBody, aContentLength);
        }
    }

    return ret;
}

int HttpClient::sendInitialHeaders(const char* aURLPath, const char* aHttpMethod)
{
#ifdef LOGGING
    Serial.println("Connected");
#endif
    // Send the HTTP command, i.e. "GET /somepath/ HTTP/1.0"
    iClient->print(aHttpMethod);
    iClient->print(" ");

    iClient->print(aURLPath);
    iClient->println(" HTTP/1.1");
    if (iSendDefaultRequestHeaders)
    {
        // The host header, if required
        if (iServerName)
        {
            iClient->print("Host: ");
            iClient->print(iServerName);
            if (iServerPort != kHttpPort && iServerPort != kHttpsPort)
            {
              iClient->print(":");
              iClient->print(iServerPort);
            }
            iClient->println();
        }
        // And user-agent string
        sendHeader(HTTP_HEADER_USER_AGENT, kUserAgent);
    }

    if (iConnectionClose)
    {
        // Tell the server to
        // close this connection after we're done
        sendHeader(HTTP_HEADER_CONNECTION, "close");
    }

    // Everything has gone well
    iState = eRequestStarted;
    return HTTP_SUCCESS;
}

void HttpClient::sendHeader(const char* aHeader)
{
    iClient->println(aHeader);
}

void HttpClient::sendHeader(const char* aHeaderName, const char* aHeaderValue)
{
    iClient->print(aHeaderName);
    iClient->print(": ");
    iClient->println(aHeaderValue);
}

void HttpClient::sendHeader(const char* aHeaderName, const int aHeaderValue)
{
    iClient->print(aHeaderName);
    iClient->print(": ");
    iClient->println(aHeaderValue);
}

void HttpClient::sendBasicAuth(const char* aUser, const char* aPassword)
{
    // Send the initial part of this header line
    iClient->print("Authorization: Basic ");
    // Now Base64 encode "aUser:aPassword" and send that
    // This seems trickier than it should be but it's mostly to avoid either
    // (a) some arbitrarily sized buffer which hopes to be big enough, or
    // (b) allocating and freeing memory
    // ...so we'll loop through 3 bytes at a time, outputting the results as we
    // go.
    // In Base64, each 3 bytes of unencoded data become 4 bytes of encoded data
    unsigned char input[3];
    unsigned char output[5]; // Leave space for a '\0' terminator so we can easily print
    int userLen = strlen(aUser);
    int passwordLen = strlen(aPassword);
    int inputOffset = 0;
    for (int i = 0; i < (userLen+1+passwordLen); i++)
    {
        // Copy the relevant input byte into the input
        if (i < userLen)
        {
            input[inputOffset++] = aUser[i];
        }
        else if (i == userLen)
        {
            input[inputOffset++] = ':';
        }
        else
        {
            input[inputOffset++] = aPassword[i-(userLen+1)];
        }
        // See if we've got a chunk to encode
        if ( (inputOffset == 3) || (i == userLen+passwordLen) )
        {
            // We've either got to a 3-byte boundary, or we've reached then end
            b64_encode(input, inputOffset, output, 4);
            // NUL-terminate the output string
            output[4] = '\0';
            // And write it out
            iClient->print((char*)output);
// FIXME We might want to fill output with '=' characters if b64_encode doesn't
// FIXME do it for us when we're encoding the final chunk
            inputOffset = 0;
        }
    }
    // And end the header we've sent
    iClient->println();
}

void HttpClient::finishHeaders()
{
    iClient->println();
    iState = eRequestSent;
}

void HttpClient::flushClientRx()
{
    while (iClient->available())
    {
        iClient->read();
    }
}

void HttpClient::endRequest()
{
    beginBody();
}

void HttpClient::beginBody()
{
    if (iState < eRequestSent)
    {
        // We still need to finish off the headers
        finishHeaders();
    }
    // else the end of headers has already been sent, so nothing to do here
}

int HttpClient::get(const char* aURLPath)
{
    return startRequest(aURLPath, HTTP_METHOD_GET);
}

int HttpClient::get(const String& aURLPath)
{
    return get(aURLPath.c_str());
}

int HttpClient::post(const char* aURLPath)
{
    return startRequest(aURLPath, HTTP_METHOD_POST);
}

int HttpClient::post(const String& aURLPath)
{
    return post(aURLPath.c_str());
}

int HttpClient::post(const char* aURLPath, const char* aContentType, const char* aBody)
{
    return post(aURLPath, aContentType, strlen(aBody), (const byte*)aBody);
}

int HttpClient::post(const String& aURLPath, const String& aContentType, const String& aBody)
{
    return post(aURLPath.c_str(), aContentType.c_str(), aBody.length(), (const byte*)aBody.c_str());
}

int HttpClient::post(const char* aURLPath, const char* aContentType, int aContentLength, const byte aBody[])
{
    return startRequest(aURLPath, HTTP_METHOD_POST, aContentType, aContentLength, aBody);
}

int HttpClient::put(const char* aURLPath)
{
    return startRequest(aURLPath, HTTP_METHOD_PUT);
}

int HttpClient::put(const String& aURLPath)
{
    return put(aURLPath.c_str());
}

int HttpClient::put(const char* aURLPath, const char* aContentType, const char* aBody)
{
    return put(aURLPath, aContentType, strlen(aBody),  (const byte*)aBody);
}

int HttpClient::put(const String& aURLPath, const String& aContentType, const String& aBody)
{
    return put(aURLPath.c_str(), aContentType.c_str(), aBody.length(), (const byte*)aBody.c_str());
}

int HttpClient::put(const char* aURLPath, const char* aContentType, int aContentLength, const byte aBody[])
{
    return startRequest(aURLPath, HTTP_METHOD_PUT, aContentType, aContentLength, aBody);
}

int HttpClient::patch(const char* aURLPath)
{
    return startRequest(aURLPath, HTTP_METHOD_PATCH);
}

int HttpClient::patch(const String& aURLPath)
{
    return patch(aURLPath.c_str());
}

int HttpClient::patch(const char* aURLPath, const char* aContentType, const char* aBody)
{
    return patch(aURLPath, aContentType, strlen(aBody),  (const byte*)aBody);
}

int HttpClient::patch(const String& aURLPath, const String& aContentType, const String& aBody)
{
    return patch(aURLPath.c_str(), aContentType.c_str(), aBody.length(), (const byte*)aBody.c_str());
}

int HttpClient::patch(const char* aURLPath, const char* aContentType, int aContentLength, const byte aBody[])
{
    return startRequest(aURLPath, HTTP_METHOD_PATCH, aContentType, aContentLength, aBody);
}

int HttpClient::del(const char* aURLPath)
{
    return startRequest(aURLPath, HTTP_METHOD_DELETE);
}

int HttpClient::del(const String& aURLPath)
{
    return del(aURLPath.c_str());
}

int HttpClient::del(const char* aURLPath, const char* aContentType, const char* aBody)
{
    return del(aURLPath, aContentType, strlen(aBody),  (const byte*)aBody);
}

int HttpClient::del(const String& aURLPath, const String& aContentType, const String& aBody)
{
    return del(aURLPath.c_str(), aContentType.c_str(), aBody.length(), (const byte*)aBody.c_str());
}

int HttpClient::del(const char* aURLPath, const char* aContentType, int aContentLength, const byte aBody[])
{
    return startRequest(aURLPath, HTTP_METHOD_DELETE, aContentType, aContentLength, aBody);
}

int HttpClient::responseStatusCode()
{
    if (iState < eRequestSent)
    {
        return HTTP_ERROR_API;
    }
    // The first line will be of the form Status-Line:
    //   HTTP-Version SP Status-Code SP Reason-Phrase CRLF
    // Where HTTP-Version is of the form:
    //   HTTP-Version   = "HTTP" "/" 1*DIGIT "." 1*DIGIT

    int c = '\0';
    do
    {
        // Make sure the status code is reset, and likewise the state.  This
        // lets us easily cope with 1xx informational responses by just
        // ignoring them really, and reading the next line for a proper response
        iStatusCode = 0;
        iState = eRequestSent;

        unsigned long timeoutStart = millis();
        // Psuedo-regexp we're expecting before the status-code
        const char* statusPrefix = "HTTP/*.* ";
        const char* statusPtr = statusPrefix;
        // Whilst we haven't timed out & haven't reached the end of the headers
        while ((c != '\n') && 
               ( (millis() - timeoutStart) < iHttpResponseTimeout ))
        {
            if (available())
            {
                c = HttpClient::read();
                if (c != -1)
                {
                    switch(iState)
                    {
                    case eRequestSent:
                        // We haven't reached the status code yet
                        if ( (*statusPtr == '*') || (*statusPtr == c) )
                        {
                            // This character matches, just move along
                            statusPtr++;
                            if (*statusPtr == '\0')
                            {
                                // We've reached the end of the prefix
                                iState = eReadingStatusCode;
                            }
                        }
                        else
                        {
                            return HTTP_ERROR_INVALID_RESPONSE;
                        }
                        break;
                    case eReadingStatusCode:
                        if (isdigit(c))
                        {
                            // This assumes we won't get more than the 3 digits we
                            // want
                            iStatusCode = iStatusCode*10 + (c - '0');
                        }
                        else
                        {
                            // We've reached the end of the status code
                            // We could sanity check it here or double-check for ' '
                            // rather than anything else, but let's be lenient
                            iState = eStatusCodeRead;
                        }
                        break;
                    case eStatusCodeRead:
                        // We're just waiting for the end of the line now
                        break;

                    default:
                        break;
                    };
                    // We read something, reset the timeout counter
                    timeoutStart = millis();
                }
            }
            else
            {
                // We haven't got any data, so let's pause to allow some to
                // arrive
                delay(iHttpWaitForDataDelay);
            }
        }
        if ( (c == '\n') && (iStatusCode < 200 && iStatusCode != 101) )
        {
            // We've reached the end of an informational status line
            c = '\0'; // Clear c so we'll go back into the data reading loop
        }
    }
    // If we've read a status code successfully but it's informational (1xx)
    // loop back to the start
    while ( (iState == eStatusCodeRead) && (iStatusCode < 200 && iStatusCode != 101) );

    if ( (c == '\n') && (iState == eStatusCodeRead) )
    {
        // We've read the status-line successfully
        return iStatusCode;
    }
    else if (c != '\n')
    {
        // We must've timed out before we reached the end of the line
        return HTTP_ERROR_TIMED_OUT;
    }
    else
    {
        // This wasn't a properly formed status line, or at least not one we
        // could understand
        return HTTP_ERROR_INVALID_RESPONSE;
    }
}

int HttpClient::skipResponseHeaders()
{
    // Just keep reading until we finish reading the headers or time out
    unsigned long timeoutStart = millis();
    // Whilst we haven't timed out & haven't reached the end of the headers
    while ((!endOfHeadersReached()) && 
           ( (millis() - timeoutStart) < iHttpResponseTimeout ))
    {
        if (available())
        {
            (void)readHeader();
            // We read something, reset the timeout counter
            timeoutStart = millis();
        }
        else
        {
            // We haven't got any data, so let's pause to allow some to
            // arrive
            delay(iHttpWaitForDataDelay);
        }
    }
    if (endOfHeadersReached())
    {
        // Success
        return HTTP_SUCCESS;
    }
    else
    {
        // We must've timed out
        return HTTP_ERROR_TIMED_OUT;
    }
}

bool HttpClient::endOfHeadersReached()
{
    return (iState == eReadingBody || iState == eReadingChunkLength || iState == eReadingBodyChunk);
};

long HttpClient::contentLength()
{
    // skip the response headers, if they haven't been read already 
    if (!endOfHeadersReached())
    {
        skipResponseHeaders();
    }

    return iContentLength;
}

String HttpClient::responseBody()
{
    int bodyLength = contentLength();
    String response;

    if (bodyLength > 0)
    {
        // try to reserve bodyLength bytes
        if (response.reserve(bodyLength) == 0) {
            // String reserve failed
            return String((const char*)NULL);
        }
    }

    // keep on timedRead'ing, until:
    //  - we have a content length: body length equals consumed or no bytes
    //                              available
    //  - no content length:        no bytes are available
    while (iBodyLengthConsumed != bodyLength)
    {
        int c = timedRead();

        if (c == -1) {
            // read timed out, done
            break;
        }

        if (!response.concat((char)c)) {
            // adding char failed
            return String((const char*)NULL);
        }
    }

    if (bodyLength > 0 && (unsigned int)bodyLength != response.length()) {
        // failure, we did not read in response content length bytes
        return String((const char*)NULL);
    }

    return response;
}

bool HttpClient::endOfBodyReached()
{
    if (endOfHeadersReached() && (contentLength() != kNoContentLengthHeader))
    {
        // We've got to the body and we know how long it will be
        return (iBodyLengthConsumed >= contentLength());
    }
    return false;
}

int HttpClient::available()
{
    if (iState == eReadingChunkLength)
    {
        while (iClient->available())
        {
            char c = iClient->read();

            if (c == '\n')
            {
                iState = eReadingBodyChunk;
                break;
            }
            else if (c == '\r')
            {
                // no-op
            }
            else if (isHexadecimalDigit(c))
            {
                char digit[2] = {c, '\0'};

                iChunkLength = (iChunkLength * 16) + strtol(digit, NULL, 16);
            }
        }
    }

    if (iState == eReadingBodyChunk && iChunkLength == 0)
    {
        iState = eReadingChunkLength;
    }
    
    if (iState == eReadingChunkLength)
    {
        return 0;
    }
    
    int clientAvailable = iClient->available();

    if (iState == eReadingBodyChunk)
    {
        return min(clientAvailable, iChunkLength);
    }
    else
    {
        return clientAvailable;
    }
}


int HttpClient::read()
{
    if (iIsChunked && !available())
    {
        return -1;
    }

    int ret = iClient->read();
    if (ret >= 0)
    {
        if (endOfHeadersReached() && iContentLength > 0)
        {
            // We're outputting the body now and we've seen a Content-Length header
            // So keep track of how many bytes are left
            iBodyLengthConsumed++;
        }

        if (iState == eReadingBodyChunk)
        {
            iChunkLength--;

            if (iChunkLength == 0)
            {
                iState = eReadingChunkLength;
            }
        }
    }
    return ret;
}

bool HttpClient::headerAvailable()
{
    // clear the currently stored header line
    iHeaderLine = "";

    while (!endOfHeadersReached())
    {
        // read a byte from the header
        int c = readHeader();

        if (c == '\r' || c == '\n')
        {
            if (iHeaderLine.length())
            {
                // end of the line, all done
                break;
            } 
            else
            {
                // ignore any CR or LF characters
                continue;
            }
        }

        // append byte to header line
        iHeaderLine += (char)c;
    }

    return (iHeaderLine.length() > 0);
}

String HttpClient::readHeaderName()
{
    int colonIndex = iHeaderLine.indexOf(':');

    if (colonIndex == -1)
    {
        return "";
    }

    return iHeaderLine.substring(0, colonIndex);
}

String HttpClient::readHeaderValue()
{
    int colonIndex = iHeaderLine.indexOf(':');
    int startIndex = colonIndex + 1;

    if (colonIndex == -1)
    {
        return "";
    }

    // trim any leading whitespace
    while (startIndex < (int)iHeaderLine.length() && isSpace(iHeaderLine[startIndex]))
    {
        startIndex++;
    }

    return iHeaderLine.substring(startIndex);
}

int HttpClient::read(uint8_t *buf, size_t size)
{
    int ret =iClient->read(buf, size);
    if (endOfHeadersReached() && iContentLength > 0)
    {
        // We're outputting the body now and we've seen a Content-Length header
        // So keep track of how many bytes are left
        if (ret >= 0)
        {
            iBodyLengthConsumed += ret;
        }
    }
    return ret;
}

int HttpClient::readHeader()
{
    char c = HttpClient::read();

    if (endOfHeadersReached())
    {
        // We've passed the headers, but rather than return an error, we'll just
        // act as a slightly less efficient version of read()
        return c;
    }

    // Whilst reading out the headers to whoever wants them, we'll keep an
    // eye out for the "Content-Length" header
    switch(iState)
    {
    case eStatusCodeRead:
        // We're at the start of a line, or somewhere in the middle of reading
        // the Content-Length prefix
        if (*iContentLengthPtr == c)
        {
            // This character matches, just move along
            iContentLengthPtr++;
            if (*iContentLengthPtr == '\0')
            {
                // We've reached the end of the prefix
                iState = eReadingContentLength;
                // Just in case we get multiple Content-Length headers, this
                // will ensure we just get the value of the last one
                iContentLength = 0;
                iBodyLengthConsumed = 0;
            }
        }
        else if (*iTransferEncodingChunkedPtr == c)
        {
            // This character matches, just move along
            iTransferEncodingChunkedPtr++;
            if (*iTransferEncodingChunkedPtr == '\0')
            {
                // We've reached the end of the Transfer Encoding: chunked header
                iIsChunked = true;
                iState = eSkipToEndOfHeader;
            }
        }
        else if (((iContentLengthPtr == kContentLengthPrefix) && (iTransferEncodingChunkedPtr == kTransferEncodingChunked)) && (c == '\r'))
        {
            // We've found a '\r' at the start of a line, so this is probably
            // the end of the headers
            iState = eLineStartingCRFound;
        }
        else
        {
            // This isn't the Content-Length or Transfer Encoding chunked header, skip to the end of the line
            iState = eSkipToEndOfHeader;
        }
        break;
    case eReadingContentLength:
        if (isdigit(c))
        {
            iContentLength = iContentLength*10 + (c - '0');
        }
        else
        {
            // We've reached the end of the content length
            // We could sanity check it here or double-check for "\r\n"
            // rather than anything else, but let's be lenient
            iState = eSkipToEndOfHeader;
        }
        break;
    case eLineStartingCRFound:
        if (c == '\n')
        {
            if (iIsChunked)
            {
                iState = eReadingChunkLength;
                iChunkLength = 0;
            }
            else
            {
                iState = eReadingBody;
            }
        }
        break;
    default:
        // We're just waiting for the end of the line now
        break;
    };

    if ( (c == '\n') && !endOfHeadersReached() )
    {
        // We've got to the end of this line, start processing again
        iState = eStatusCodeRead;
        iContentLengthPtr = kContentLengthPrefix;
        iTransferEncodingChunkedPtr = kTransferEncodingChunked;
    }
    // And return the character read to whoever wants it
    return c;
}



