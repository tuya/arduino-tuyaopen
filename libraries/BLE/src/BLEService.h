#ifndef _BK_BLESERVICE_H_
#define _BK_BLESERVICE_H_

#include "BLECharacteristic.h"
#include <string>
#include "tkl_bluetooth.h"

class BLEService {
public:
	static TKL_BLE_GATTS_PARAMS_T *  ble_gatt_service;
	static TKL_BLE_SERVICE_PARAMS_T * ble_service;
	static TKL_BLE_CHAR_PARAMS_T * ble_service_char;
	static uint8_t char_index ;
	
	BLECharacteristic* createCharacteristic(const char* uuid, uint32_t properties);
	void               start();

private:
	
	BLEService(std::string uuid);
	friend class BLEServer;
}; 

#endif
