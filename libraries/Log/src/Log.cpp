#include "Log.h"

using namespace arduino;

extern "C" void _log_output_cb(const char *str)
{
  Serial.print(str);
}

LogClass::LogClass(void)
{

}

LogClass::~LogClass()
{

}

void LogClass::begin(int buffer_size)
{
  tal_log_init(TAL_LOG_LEVEL_DEBUG, buffer_size, (TAL_LOG_OUTPUT_CB)_log_output_cb);
  tal_log_color_enable_set(FALSE);
}

void LogClass::begin()
{
  begin(1024);
}

void LogClass::setLevel(LogLevel level)
{
  tal_log_set_level(static_cast<TAL_LOG_LEVEL_E>(level));
}

void LogClass::enableMsInfo(bool enable)
{
  tal_log_set_ms_info(enable);
}

void LogClass::enableColor(bool enable)
{
  tal_log_color_enable_set(enable);
}

void LogClass::setColor(LogLevel level, ColorMode mode, ColorFont font, ColorBG background)
{
  tal_log_color_set(static_cast<TAL_LOG_LEVEL_E>(level), static_cast<TAL_LOG_DISPLAY_MODE_E>(mode), static_cast<TAL_LOG_FONT_COLOR_E>(font), static_cast<TAL_LOG_BACKGROUND_COLOR_E>(background));
}

LogClass Log;
