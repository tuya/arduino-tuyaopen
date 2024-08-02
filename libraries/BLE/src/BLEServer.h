#ifndef _BK_BLESERVER_H_
#define _BK_BLESERVER_H_

#include "BLEService.h"
#include <string>

class BLEServerCallbacks {
public:
	virtual ~BLEServerCallbacks() {};
	virtual void onConnect() {};
	virtual void onDisconnect() {};
	virtual void onwrite(uint8_t* data,uint16_t data_len){};
	virtual void notify_done() {};

}; 

class  BLEServer {
public:
	BLEService*     createService(const char* uuid);	
	void            setCallbacks(BLEServerCallbacks* pCallbacks);
	static BLEServerCallbacks* getCallbacks();

private:
	friend class BLEService;
}; 

#endif 
