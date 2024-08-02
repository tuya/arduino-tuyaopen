#ifndef _BLEDEV_H_
#define _BLEDEV_H_

#include "BLEAdvertising.h"
#include "BLEServer.h"

class BLEDEV {
public:
	static int ble_common_connect_handle;
	
	static BLEServer*  createServer();   
	static BLEAdvertising* 	getAdvertising();
    static void  init(); 

private:
	static BLEServer*	g_pServer;
	static BLEAdvertising* g_bleAdvertising;

}; 

#endif
