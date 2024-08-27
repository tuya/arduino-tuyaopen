#ifndef __TICKER_H__
#define __TICKER_H__

extern "C" {
#include "tal_sw_timer.h"
}


class Ticker {
public:
  Ticker();
  ~Ticker();

  typedef void (*callback_with_arg_t)(void* arg);
  typedef void (*callback_function_t)(void);

  void attach(float seconds, callback_function_t callback) {
    _callback_function = callback;

    _arg = nullptr;
    _callback_function_with_arg = nullptr;
    _attach_ms(1000ULL * seconds, true, _static_callback, this);
  }

  void attach_ms(uint64_t milliseconds, callback_function_t callback) {
    _callback_function = callback;

    _arg = nullptr;
    _callback_function_with_arg = nullptr;
    _attach_ms(milliseconds, true, _static_callback, this);
  }

  template<typename TArg> void attach(float seconds, void (*callback)(TArg), TArg arg) {
    _callback_function = nullptr;

    _arg = reinterpret_cast<void *>(arg);
    _callback_function_with_arg = reinterpret_cast<callback_with_arg_t>(callback);
    _attach_ms(1000ULL * seconds, true, _static_callback, this);
  }

  template<typename TArg> void attach_ms(uint64_t milliseconds, void (*callback)(TArg), TArg arg) {
    _callback_function = nullptr;

    _arg = reinterpret_cast<void *>(arg);
    _callback_function_with_arg = reinterpret_cast<callback_with_arg_t>(callback);
    _attach_ms(milliseconds, true, _static_callback, this);
  }

  void once(float seconds, callback_function_t callback) {
    _callback_function = callback;

    _arg = nullptr;
    _callback_function_with_arg = nullptr;
    _attach_ms(1000ULL * seconds, false, _static_callback, this);
  }

  void once_ms(uint64_t milliseconds, callback_function_t callback) {
    _callback_function = callback;

    _arg = nullptr;
    _callback_function_with_arg = nullptr;
    _attach_ms(milliseconds, false, _static_callback, this);
  }

  template<typename TArg> void once(float seconds, void (*callback)(TArg), TArg arg) {
    _callback_function = nullptr;

    _arg = reinterpret_cast<void *>(arg);
    _callback_function_with_arg = reinterpret_cast<callback_with_arg_t>(callback);
    _attach_ms(1000ULL * seconds, false, _static_callback, this);
  }

  template<typename TArg> void once_ms(uint64_t milliseconds, void (*callback)(TArg), TArg arg) {
    _callback_function = nullptr;

    _arg = reinterpret_cast<void *>(arg);
    _callback_function_with_arg = reinterpret_cast<callback_with_arg_t>(callback);
    _attach_ms(milliseconds, false, _static_callback, this);
  }

  void detach();
  bool active() const;

protected:
  static void _static_callback(TIMER_ID timer_id, void *arg);
  TIMER_ID _timer_handle;

  callback_function_t _callback_function;

  void* _arg;
  callback_with_arg_t _callback_function_with_arg;
private:
  void _attach_ms(uint64_t ms, bool repeat, TAL_TIMER_CB callback, void *arg);
};

#endif //__TICKER_H__
