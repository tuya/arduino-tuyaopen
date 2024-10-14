#pragma once

#include <Arduino.h>
#include <api/HardwareSPI.h>

extern "C" {
#include "tkl_spi.h"
}

namespace arduino {

class SPIClassTuyaOpen : public HardwareSPI {
public:
  SPIClassTuyaOpen();
  SPIClassTuyaOpen(TUYA_SPI_NUM_E port);
  SPIClassTuyaOpen(TUYA_SPI_NUM_E port, TUYA_SPI_BASE_CFG_T cfg);
  ~SPIClassTuyaOpen();

  uint8_t transfer(uint8_t data);
  uint16_t transfer16(uint16_t data);
  void transfer(void *buf, size_t count);

  // Transaction Functions
  void usingInterrupt(int interruptNumber) override { /* noop */ }
  void notUsingInterrupt(int interruptNumber) override { /* noop */ }
  // void beginTransaction(void) { /* noop */ }
  void beginTransaction(SPISettings settings) override;
  void endTransaction(void) override;

  // SPI Configuration methods
  void attachInterrupt() override { /* noop */ }
  void detachInterrupt() override { /* noop */ }

  void begin();
  void begin(int csPin);
  void end();

  void setCS(int csPin) { _csPin = csPin; }
private:
  void _spiConfigInitAlwaysInline(TUYA_SPI_NUM_E port, TUYA_SPI_BASE_CFG_T cfg) __attribute__((__always_inline__)) {
    this->_port = port;
    memcpy(&_cfg, &cfg, sizeof(TUYA_SPI_BASE_CFG_T));
  }

  TUYA_SPI_NUM_E _port;
  TUYA_SPI_BASE_CFG_T _cfg;

  int _csPin = -1;

  uint8_t _isBeginTransaction = 0;
};

} // namespace arduino

extern arduino::SPIClassTuyaOpen SPI;
