#ifndef _BLECHARACTERISTIC_H_
#define _BLECHARACTERISTIC_H_

#include <string>

class BLECharacteristic {
public:
	BLECharacteristic(std::string uuid, uint32_t properties = 0);

	void indicate();
	void notify();
	void setValue(uint8_t* data, size_t size);

	static const uint32_t PROPERTY_READ      		= 1<<0;
	static const uint32_t PROPERTY_WRITE     		= 1<<1;
	static const uint32_t PROPERTY_NOTIFY    		= 1<<2;
	static const uint32_t PROPERTY_BROADCAST 		= 1<<3;
	static const uint32_t PROPERTY_INDICATE  		= 1<<4;
	static const uint32_t PROPERTY_WRITE_NO_RSP  	= 1<<5;
	
private:
	uint16_t                    notify_handle;
	uint8_t 					notify_data[128];
	uint16_t 					notify_data_len = 0;
	friend class BLEService;
	friend class BLEDEV;

}; 

extern int hexstr2bin(const char *hex, uint8_t *buf, size_t len);

#endif
