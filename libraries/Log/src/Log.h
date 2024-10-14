#pragma once

#include "Arduino.h"

extern "C" {
#include "tal_log.h"
}

namespace arduino {

#define OPEN_SDK_LOG_BUFFER_SIZE (512)

class LogClass {
public:
  enum LogLevel {
    ERROR, WARN, NOTICE, INFO, DEBUG, TRACE
  };
  enum ColorMode {
    MODE_DEFAULT = 0, MODE_HIGH_LIGHT, MODE_UNDER_LINE = 4, MODE_FLASH = 5, MODE_REVERSE = 7
  };
  enum ColorFont {
    FONT_BLACK = 30, FONT_RED, FONT_GREEN, FONT_YELLOW, FONT_BLUE, FONT_PURPLE, FONT_CYAN, FONT_WHITE, FONT_DEFAULT = 39
  };
  enum ColorBG { // background color
    BG_BLACK = 40, BG_RED, BG_GREEN, BG_YELLOW, BG_BLUE, BG_PURPLE, BG_CYAN, BG_WHITE, BG_DEFAULT = 49
  };

  LogClass(int buffer);
  ~LogClass();

  void setLevel(LogLevel level);

  void enableMsInfo(bool enable);

  void enableColor(bool enable);
  void setColor(LogLevel level, ColorMode mode, ColorFont font, ColorBG background);
};

} // namespace arduino

extern arduino::LogClass Log;
