// Library to simplify HTTP fetching on Arduino
// (c) Copyright Arduino. 2019
// Released under Apache License, version 2.0

#ifndef URL_ENCODER_H
#define URL_ENCODER_H

#include <Arduino.h>

class URLEncoderClass
{
public:
    URLEncoderClass();
    virtual ~URLEncoderClass();

    static String encode(const char* str);
    static String encode(const String& str);

private:
    static String encode(const char* str, int length);
};

extern URLEncoderClass URLEncoder;

#endif
