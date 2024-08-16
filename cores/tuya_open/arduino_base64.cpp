
#include "Arduino.h"
extern "C" {
#include "libb64/cdecode.h"
#include "libb64/cencode.h"
}
#include "arduino_base64.h"

/**
 * convert input data to base64
 * @param data const uint8_t *
 * @param length size_t
 * @return String
 */
String base64::encode(const uint8_t * data, size_t length)
{
    size_t size = base64_encode_expected_len(length) + 1;
    char * buffer = (char *) malloc(size);
    if(buffer) {
        base64_encodestate _state;
        base64_init_encodestate(&_state);
        int len = base64_encode_block((const char *) &data[0], length, &buffer[0], &_state);
        len = base64_encode_blockend((buffer + len), &_state);

        String base64 = String(buffer);
        free(buffer);
        return base64;
    }
    return String("-FAIL-");
}

/**
 * convert input data to base64
 * @param text const String&
 * @return String
 */
String base64::encode(const String& text)
{
    return base64::encode((uint8_t *) text.c_str(), text.length());
}

