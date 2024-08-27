#include "SerialUART.h"

static SerialUART* __serialPtr[TUYA_UART_NUM_MAX] = {nullptr};

extern "C" void __uartRxCallback(TUYA_UART_NUM_E port_id)
{
  uint8_t c = 0;
  int rt = 0;

  SerialUART *p = __serialPtr[port_id];

  if (p == nullptr) {
    return;
  }

  while (1) {
    rt = tkl_uart_read(port_id, &c, 1);
    if (rt != 1) {
      break;
    }
    p->__rxBufWrite(c);
  }

  return;
}

SerialUART::SerialUART(uint8_t id)
{
  __uartID = id;

  return;
}

SerialUART::~SerialUART()
{
  end();
}

void SerialUART::begin(unsigned long baudrate, uint16_t config)
{
  TUYA_UART_BASE_CFG_T uartConfig;

  if (__uartID >= TUYA_UART_NUM_MAX) {
    return;
  }

  __serialPtr[__uartID] = this;

  uartConfig.baudrate = baudrate;

  // data bits
  switch (config & SERIAL_DATA_MASK)
  {
  case SERIAL_DATA_5:
    uartConfig.databits = TUYA_UART_DATA_LEN_5BIT;
    break;
  case SERIAL_DATA_6:
    uartConfig.databits = TUYA_UART_DATA_LEN_6BIT;
    break;
  case SERIAL_DATA_7:
    uartConfig.databits = TUYA_UART_DATA_LEN_7BIT;
    break;
  case SERIAL_DATA_8:
    uartConfig.databits = TUYA_UART_DATA_LEN_8BIT;
    break;
  default:
    uartConfig.databits = TUYA_UART_DATA_LEN_8BIT;
    break;
  }

  // stop bits
  switch (config & SERIAL_STOP_BIT_MASK)
  {
  case SERIAL_STOP_BIT_1:
    uartConfig.stopbits = TUYA_UART_STOP_LEN_1BIT;
    break;
  case SERIAL_STOP_BIT_1_5:
    uartConfig.stopbits = TUYA_UART_STOP_LEN_1_5BIT1;
    break;
  case SERIAL_STOP_BIT_2:
    uartConfig.stopbits = TUYA_UART_STOP_LEN_2BIT;
    break;
  default:
    uartConfig.stopbits = TUYA_UART_STOP_LEN_1BIT;
    break;
  }

  // parity
  switch (config & SERIAL_PARITY_MASK)
  {
  case SERIAL_PARITY_NONE:
    uartConfig.parity = TUYA_UART_PARITY_TYPE_NONE;
    break;
  case SERIAL_PARITY_EVEN:
    uartConfig.parity = TUYA_UART_PARITY_TYPE_EVEN;
    break;
  case SERIAL_PARITY_ODD:
    uartConfig.parity = TUYA_UART_PARITY_TYPE_ODD;
    break;
  default:
    uartConfig.parity = TUYA_UART_PARITY_TYPE_NONE;
    break;
  }

  // flowctrl
  uartConfig.flowctrl = TUYA_UART_FLOWCTRL_NONE;

  tkl_uart_init(static_cast<TUYA_UART_NUM_E>(__uartID), &uartConfig);

  tkl_uart_rx_irq_cb_reg(static_cast<TUYA_UART_NUM_E>(__uartID), __uartRxCallback);

  tal_mutex_create_init(&__mutex);

  return;
}

void SerialUART::begin(unsigned long baudrate)
{
  begin(baudrate, SERIAL_8N1);
  return;
}

void SerialUART::end()
{
  __serialPtr[__uartID] = nullptr;

  tkl_uart_deinit(static_cast<TUYA_UART_NUM_E>(__uartID));
  tal_mutex_release(&__mutex);
  _rxBuffer.clear();
  return;
}

int SerialUART::__rxBufWrite(uint8_t c)
{
  if (_rxBuffer.availableForStore()) {
    _rxBuffer.store_char(c);
  }

  return 0;
}

int SerialUART::available(void)
{
  int rt = 0;

  tal_mutex_lock(__mutex);

  rt = _rxBuffer.available();

  tal_mutex_unlock(__mutex);

  return rt;
}

int SerialUART::peek(void)
{
  int rt = 0;

  tal_mutex_lock(__mutex);

  rt = _rxBuffer.peek();

  tal_mutex_unlock(__mutex);

  return rt;
}

int SerialUART::read(void)
{
  int rt = 0;

  tal_mutex_lock(__mutex);

  rt = _rxBuffer.read_char();

  tal_mutex_unlock(__mutex);

  return rt;
}

void SerialUART::flush(void)
{
  return;
}

size_t SerialUART::write(uint8_t c)
{
  if (__serialPtr[__uartID] == nullptr) {
    return 0;
  }

  return tkl_uart_write(static_cast<TUYA_UART_NUM_E>(__uartID), &c, 1);
}

size_t SerialUART::write(const uint8_t* c, size_t len)
{
  uint8_t *p = const_cast<uint8_t*>(c);

  if (__serialPtr[__uartID] == nullptr) {
    return 0;
  }

  return tkl_uart_write(static_cast<TUYA_UART_NUM_E>(__uartID), p, len);
}

SerialUART::operator bool() {
  return (__uartID < TUYA_UART_NUM_MAX) ? true : false;
}

SerialUART _SerialUART0_(defaultSerial);
