#include <string.h>
#include <stdlib.h>
#include "tkl_bluetooth.h"
#include "BLECharacteristic.h"
#include "BLEService.h"
#include "BLEDEV.h"

BLECharacteristic::BLECharacteristic(std::string uuid, uint32_t properties) 
{
    int ntf_idx = 0;
    int prop = 0;
    int perm = 0;
    if(uuid.length() == 4 )
    {
        (BLEService::ble_service_char + BLEService::char_index )->char_uuid.uuid_type = TKL_BLE_UUID_TYPE_16;
        hexstr2bin(uuid.data(),(uint8_t*)&(BLEService::ble_service_char + BLEService::char_index )->char_uuid.uuid.uuid16,uuid.length()/2);
    }
    else if(uuid.length() == 8 )
    {
        (BLEService::ble_service_char + BLEService::char_index )->char_uuid.uuid_type = TKL_BLE_UUID_TYPE_32;
        hexstr2bin(uuid.data(),(uint8_t*)&(BLEService::ble_service_char + BLEService::char_index )->char_uuid.uuid.uuid32,uuid.length()/2);
    }
    else if(uuid.length() == 32)
    {
        (BLEService::ble_service_char + BLEService::char_index )->char_uuid.uuid_type = TKL_BLE_UUID_TYPE_128;
        hexstr2bin(uuid.data(),(uint8_t*)&(BLEService::ble_service_char + BLEService::char_index )->char_uuid.uuid.uuid128,uuid.length()/2);
    }   

    if(properties & PROPERTY_WRITE)
    {
        prop |= TKL_BLE_GATT_CHAR_PROP_WRITE;
        perm |=  (TKL_BLE_GATT_PERM_READ | TKL_BLE_GATT_PERM_WRITE);
    }
    if(properties & PROPERTY_NOTIFY)
    {
        prop |= TKL_BLE_GATT_CHAR_PROP_NOTIFY;
        perm |=  (TKL_BLE_GATT_PERM_READ | TKL_BLE_GATT_PERM_WRITE);
    }
    if(properties & PROPERTY_READ)
    {
        prop |= TKL_BLE_GATT_CHAR_PROP_READ;
        perm |=  TKL_BLE_GATT_PERM_READ;  
    }
    if(properties & PROPERTY_INDICATE)
    {
        prop |= TKL_BLE_GATT_CHAR_PROP_INDICATE;
        perm |=  (TKL_BLE_GATT_PERM_READ | TKL_BLE_GATT_PERM_WRITE);     

    }  
    ntf_idx = BLEService::char_index;
    (BLEService::ble_service_char + BLEService::char_index )->property = prop;
    (BLEService::ble_service_char + BLEService::char_index )->permission = perm;
    (BLEService::ble_service_char + BLEService::char_index )->handle = TKL_BLE_GATT_INVALID_HANDLE;
    (BLEService::ble_service_char + BLEService::char_index )->value_len = 244;
    BLEService::char_index ++;
    notify_handle = ntf_idx;
}

void BLECharacteristic::setValue(uint8_t* data, size_t length) 
{
    if(length < 128)
    {
        memcpy(notify_data,data,length);   
        notify_data_len = length;   
    }
    else 
    {
        memcpy(notify_data,data,128);   
        notify_data_len = 128;   
    }
} 

void BLECharacteristic::notify() 
{
    if (notify_data_len != 0)
    {
        tkl_ble_gatts_value_notify(BLEDEV::ble_common_connect_handle, (BLEService::ble_service_char + notify_handle )->handle, notify_data, notify_data_len);
    } 
}

void BLECharacteristic::indicate() 
{
    if (notify_data_len != 0)
    {
        tkl_ble_gatts_value_indicate(BLEDEV::ble_common_connect_handle, (BLEService::ble_service_char + notify_handle )->handle, notify_data, notify_data_len);
    } 
}