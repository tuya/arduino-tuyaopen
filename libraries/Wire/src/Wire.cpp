
#include "Wire.h"

#include "tuya_cloud_types.h"
#include "tal_memory.h"

#include "tkl_pinmux.h"
#include "tkl_i2c.h"

TwoWire::TwoWire(uint8_t port) 
  : _i2cPort(port), _clkHz(100000),
  _rxBuffer(nullptr), _rxIndex(0), _rxLength(0),
  _txBuffer(nullptr), _txLength(0), _txAddress(0),
  _timeOutMillis(50)
{

}

TwoWire::~TwoWire() {
  end();
}

void TwoWire::setClock(uint32_t freq) {
  _clkHz = freq;
}


bool TwoWire::setSDA(pin_size_t sda) {
  _sda = sda;
}

bool TwoWire::setSCL(pin_size_t scl) {
  _scl = scl;
}

void TwoWire::begin() {
  int rt = OPRT_OK;

  if (_isBegin) {
    return;
  }

  if (_i2cPort >= TUYA_I2C_NUM_MAX) {
    Serial.println("Invalid port");
    return;
  }

  setBufferSize(WIRE_BUFFER_SIZE);

  tkl_io_pinmux_config(_scl, TUYA_IIC0_SCL);
  tkl_io_pinmux_config(_sda, TUYA_IIC0_SDA);

  TUYA_IIC_BASE_CFG_T i2cCfg;
  i2cCfg.role = TUYA_IIC_MODE_MASTER;
  i2cCfg.addr_width = TUYA_IIC_ADDRESS_7BIT;
  switch (_clkHz) {
    case 400000:
      i2cCfg.speed = TUYA_IIC_BUS_SPEED_400K;
      break;
    case 1000000:
      i2cCfg.speed = TUYA_IIC_BUS_SPEED_1M;
      break;
    case 3400000:
      i2cCfg.speed = TUYA_IIC_BUS_SPEED_3_4M;
      break;
    default:
      i2cCfg.speed = TUYA_IIC_BUS_SPEED_100K;
      break;
  }

  rt = tkl_i2c_init(static_cast<TUYA_I2C_NUM_E>(_i2cPort), &i2cCfg);
  if (OPRT_OK == rt) {
    _isBegin = 1;
  } else {
    Serial.println("Failed to init i2c: " + String(rt));
  }

  if (!_isBegin) {
    freeWireBuffer();
  }

  return;
}

void TwoWire::begin(uint8_t address) {
  _txAddress = address;
  begin();
}

void TwoWire::end() {
  if (!_isBegin) {
    return;
  }

  freeWireBuffer();

  tkl_i2c_deinit(static_cast<TUYA_I2C_NUM_E>(_i2cPort));
  _isBegin = 0;
}

void TwoWire::beginTransmission(uint8_t address) {
  _txAddress = address;

  _txLength = 0;
}

uint8_t TwoWire::endTransmission(bool stopBit) {
  int rt = OPRT_OK;

  if (!_isBegin) {
    return 4; // other error
  }

  if (nullptr == _txBuffer) {
    Serial.println("TX buffer not allocated");
    return 4;
  }

  rt = tkl_i2c_master_send(static_cast<TUYA_I2C_NUM_E>(_i2cPort), _txAddress, _txBuffer, _txLength, !stopBit);

  return ((OPRT_OK == rt) ? 0 : 4);
}

uint8_t TwoWire::endTransmission(void) {
  return endTransmission(true);
}

size_t TwoWire::requestFrom(uint8_t address, size_t len, bool stopBit) {
  int rt = OPRT_OK;

  if (_rxBuffer == nullptr || _txBuffer == nullptr) {
    Serial.println("NULL buffer pointer");
    return 0;
  }

  _rxIndex = 0;
  _rxLength = 0;

  len = (len > _bufferSize) ? _bufferSize : len;

  rt = tkl_i2c_master_receive(static_cast<TUYA_I2C_NUM_E>(_i2cPort), address, _rxBuffer, len, !stopBit);
  if (OPRT_OK != rt) {
    return 0;
  }
  _rxLength = len;

  return len;
}

size_t TwoWire::requestFrom(uint8_t address, size_t len) {
  return requestFrom(address, len, true);
}

size_t TwoWire::write(uint8_t data) {
  if (_txBuffer == nullptr) {
    Serial.println("NULL TX buffer pointer");
    return 0;
  }
  if (_txLength >= _bufferSize) {
    return 0;
  }
  _txBuffer[_txLength++] = data;
  return 1;
}

size_t TwoWire::write(const uint8_t *data, size_t quantity) {
  for (size_t i = 0; i < quantity; ++i) {
    if (!write(data[i])) {
      return i;
    }
  }
  return quantity;
}

int TwoWire::available()
{
  if (_rxBuffer == nullptr) {
    Serial.println("NULL RX buffer pointer");
    return 0;
  }
  return _rxLength - _rxIndex;
}

int TwoWire::read() {
  int value = -1;
  if (_rxBuffer == nullptr) {
    Serial.println("NULL RX buffer pointer");
    return value;
  }
  if (_rxIndex < _rxLength) {
    value = _rxBuffer[_rxIndex++];
  }
  return value;
}

int TwoWire::peek() {
  int value = -1;
  if (_rxBuffer == nullptr) {
    Serial.println("NULL RX buffer pointer");
    return value;
  }
  if (_rxIndex < _rxLength) {
    value = _rxBuffer[_rxIndex];
  }
  return value;
}

void TwoWire::flush() {
  _rxIndex = 0;
  _rxLength = 0;
  _txLength = 0;
}

void TwoWire::onReceive(void(*)(int)) {
  // Not support slave mode
}

void TwoWire::onRequest(void(*)(void)) {
  // Not support slave mode
}

size_t TwoWire::setBufferSize(size_t bSize)
{
  if (bSize == 0) {
    freeWireBuffer();
    return 0;
  }

  if (_txBuffer && _rxBuffer) {
    if (_bufferSize == bSize) {
      return bSize;
    }

    freeWireBuffer();

    _rxBuffer = (uint8_t *)tal_malloc(bSize);
    if (nullptr == _rxBuffer) {
      return 0;
    }

    _txBuffer = (uint8_t *)tal_malloc(bSize);
    if (nullptr == _txBuffer) {
      tal_free(_rxBuffer);
      _rxBuffer = nullptr;
      return 0;
    }

    _bufferSize = bSize;
  } else {
    freeWireBuffer();

    _rxBuffer = (uint8_t *)tal_malloc(bSize);
    if (nullptr == _rxBuffer) {
      return 0;
    }

    _txBuffer = (uint8_t *)tal_malloc(bSize);
    if (nullptr == _txBuffer) {
      tal_free(_rxBuffer);
      _rxBuffer = nullptr;
      return 0;
    }

    _bufferSize = bSize;
  }

  return _bufferSize;
}

void TwoWire::freeWireBuffer() {
  if (nullptr != _txBuffer) {
    tal_free(_txBuffer);
    _txBuffer = nullptr;
    _rxIndex = 0;
    _rxLength = 0;
  }

  if (nullptr != _rxBuffer) {
    tal_free(_rxBuffer);
    _rxBuffer = nullptr;
    _txLength = 0;
  }
}

TwoWire Wire = TwoWire(0);
