#include "Log.h"

void setup(void)
{
    Serial.begin(115200);
    Log.begin(1024);
}

void loop()
{
    PR_DEBUG("Hello");
    delay(1000);
}