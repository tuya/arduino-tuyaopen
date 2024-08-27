#include "Ticker.h"

extern "C" {
#include "tal_log.h"
}

Ticker::Ticker() :
_timer_handle(nullptr),
_callback_function(nullptr),
_arg(nullptr),
_callback_function_with_arg(nullptr)
{

}

Ticker::~Ticker() {
  detach();
}

void Ticker::_attach_ms(uint64_t ms, bool repeat, TAL_TIMER_CB callback, void *arg)
{
  OPERATE_RET rt = OPRT_OK;
  TIMER_TYPE timerType = TAL_TIMER_ONCE;

  if (!_timer_handle) {
    rt = tal_sw_timer_create(callback, arg, &_timer_handle);
    if (OPRT_OK != rt) {
      PR_ERR("Create software timer fail, %d", rt);
      return;
    }
  }

  timerType = (repeat) ? (TAL_TIMER_CYCLE) : (TAL_TIMER_ONCE);
  tal_sw_timer_start(_timer_handle, ms, timerType);
}

void Ticker::detach() {
  if (_timer_handle) {
    tal_sw_timer_stop(_timer_handle);
    tal_sw_timer_delete(_timer_handle);
    _timer_handle = nullptr;
    _callback_function = nullptr;
    _arg = nullptr;
    _callback_function_with_arg = nullptr;
  }
}

bool Ticker::active() const {
  if (!_timer_handle) {
    return false;
  }
  return tal_sw_timer_is_running(_timer_handle);
}

void Ticker::_static_callback(TIMER_ID timer_id, void *arg) {
  Ticker *_this = reinterpret_cast<Ticker *>(arg);
  if (_this && _this->_callback_function) {
    _this->_callback_function();
  }
  if (_this && _this->_callback_function_with_arg) {
    _this->_callback_function_with_arg(_this->_arg);
  }
}
