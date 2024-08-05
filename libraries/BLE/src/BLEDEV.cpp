#include "BLEDEV.h"
#include "tkl_bluetooth.h"
#include "tal_log.h"

int BLEDEV:: ble_common_connect_handle = TKL_BLE_GATT_INVALID_HANDLE;

static void ble_kernel_gap_event_callback(TKL_BLE_GAP_PARAMS_EVT_T *p_event)
{
    BLEServerCallbacks* callback = BLEServer::getCallbacks();
    if(p_event == NULL) return;

    switch(p_event->type)
    {
        case TKL_BLE_EVT_STACK_INIT: {
            PR_INFO("ble stack ok\n");
        } break;

        case TKL_BLE_EVT_STACK_DEINIT: {
            PR_INFO("ble deinit stack ok\n");
        } break;

        case TKL_BLE_GAP_EVT_CONNECT: {    
            BLEDEV:: ble_common_connect_handle = p_event->conn_handle;
            PR_INFO("addr : %s",p_event->gap_event.connect.peer_addr.addr);
            callback->onConnect();
        } break;

        case TKL_BLE_GAP_EVT_DISCONNECT:{
            if(p_event->gap_event.disconnect.role == TKL_BLE_ROLE_SERVER && p_event->conn_handle == BLEDEV::ble_common_connect_handle ) {
                BLEDEV:: ble_common_connect_handle  = TKL_BLE_GATT_INVALID_HANDLE;
            }
            callback->onDisconnect();
        } break;

        case TKL_BLE_GAP_EVT_ADV_REPORT: {

        } break;

        case TKL_BLE_GAP_EVT_CONN_PARAM_REQ: {

        } break;
     
        case TKL_BLE_GAP_EVT_CONN_PARAM_UPDATE: {
       
        } break;

        case TKL_BLE_GAP_EVT_CONN_RSSI: {
     
        } break;

        default: {

        }return;
    }
}

static void ble_kernel_gatt_event_callback(TKL_BLE_GATT_PARAMS_EVT_T *p_event)
{
    BLEServerCallbacks* callback = BLEServer::getCallbacks();
    if(p_event == NULL) return;

    switch(p_event->type)
    {
        case TKL_BLE_GATT_EVT_MTU_REQUEST: {
    
        } break;

        case TKL_BLE_GATT_EVT_NOTIFY_TX: {
            callback->notify_done();
        } break;

        case TKL_BLE_GATT_EVT_WRITE_REQ: {           
            callback->onwrite(p_event->gatt_event.write_report.report.p_data, p_event->gatt_event.write_report.report.length );
            PR_INFO("write :%02x",p_event->gatt_event.write_report.report.p_data[0]);
        } break;

        case TKL_BLE_GATT_EVT_NOTIFY_INDICATE_RX:{

        } break;

        case TKL_BLE_GATT_EVT_SUBSCRIBE:{
            if(p_event->result == OPRT_OK) {

            }
        }break;

        case TKL_BLE_GATT_EVT_READ_CHAR_VALUE:{

        }break;
        
        default: {
            // Not Support
        } return;
    }
}

BLEAdvertising* BLEDEV::g_bleAdvertising = nullptr;
BLEServer* BLEDEV::g_pServer = nullptr;

BLEAdvertising* BLEDEV::getAdvertising() {
	if(g_bleAdvertising == nullptr) {
		g_bleAdvertising = new BLEAdvertising();
	}
	return g_bleAdvertising; 
}

BLEServer* BLEDEV::createServer() {
	g_pServer = new BLEServer();
	return g_pServer;
}

void BLEDEV::init() 
{
    tkl_ble_gap_callback_register(ble_kernel_gap_event_callback);
    tkl_ble_gatt_callback_register(ble_kernel_gatt_event_callback);
}