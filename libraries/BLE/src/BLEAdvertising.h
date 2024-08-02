#ifndef _BLEADVERTISING_H_
#define _BLEADVERTISING_H_

#include "tkl_bluetooth.h"

class BLEAdvertising {
public:
	BLEAdvertising(){advParams.adv_interval_min=160; advParams.adv_interval_max=160;}
	void start();			
	void stop();			
	void setMaxInterval(uint16_t maxinterval);	
	void setMinInterval(uint16_t mininterval);		
	void setAdvertisementData(uint8_t *adv_data,uint8_t len);		
	void setScanResponseData(uint8_t *adv_rsp_data,uint8_t len);	
	void setScanResponse(bool);		

private:
	bool                 scanResp = true;
	TKL_BLE_GAP_ADV_PARAMS_T	 advParams;
};

#endif 
