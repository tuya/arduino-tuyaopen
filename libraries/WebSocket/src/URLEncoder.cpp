// Library to simplify HTTP fetching on Arduino
// (c) Copyright Arduino. 2019
// Released under Apache License, version 2.0

#include "URLEncoder.h"

URLEncoderClass::URLEncoderClass()
{
}

URLEncoderClass::~URLEncoderClass()
{
}

String URLEncoderClass::encode(const char* str)
{
    return encode(str, strlen(str));
}

String URLEncoderClass::encode(const String& str)
{
    return encode(str.c_str(), str.length());
}

String URLEncoderClass::encode(const char* str, int length)
{
    String encoded;

    encoded.reserve(length);

    for (int i = 0; i < length; i++) {
        char c = str[i];

        const char HEX_DIGIT_MAPPER[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

        if (isAlphaNumeric(c) || (c == '-') || (c == '.') || (c == '_') || (c == '~')) {
            encoded += c;
        } else {
            char s[4];

            s[0] = '%';
            s[1] = HEX_DIGIT_MAPPER[(c >> 4) & 0xf];
            s[2] = HEX_DIGIT_MAPPER[(c & 0x0f)];
            s[3] = 0;

            encoded += s;
        }
    }

    return encoded;
}

URLEncoderClass URLEncoder;
