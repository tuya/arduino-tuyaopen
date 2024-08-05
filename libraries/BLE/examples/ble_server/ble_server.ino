#include <BLEDEV.h>

uint8_t adv_data[31];
uint8_t adv_rsp_data[31];
#define SERVICE_UUID        "4fafc2011fb5459e8fccc5c9c331914b"
#define CHARACTERISTIC_UUID1 "FF11"
#define CHARACTERISTIC_UUID2 "FF22"
bool deviceConnected = false;
bool oldDeviceConnected = false;

BLECharacteristic* pCharacteristic1;
BLECharacteristic* pCharacteristic2;
BLEAdvertising *pAdvertising;

uint32_t value = 0;

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect() {
      deviceConnected = true;
      Serial.println("onConnect...");

    };

    void onDisconnect() {
      deviceConnected = false;
      Serial.println("onDisconnect...");
    }

    void onwrite(uint8_t* data,uint16_t data_len) {
        Serial.print(" data_len: ");
        Serial.print( data_len );
        Serial.print(" data: ");
        Serial.print(data[0]);
        Serial.println();
    }

    void notify_done() {
       Serial.println("notify_done...");
    }

};


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  BLEDEV::init();
  BLEServer* pServer = BLEDEV::createServer();
  pAdvertising = BLEDEV::getAdvertising();
  BLEService *pService = pServer->createService(SERVICE_UUID);
  pServer->setCallbacks(new MyServerCallbacks());

  pCharacteristic1 = pService->createCharacteristic(
                                                                      CHARACTERISTIC_UUID1,
                                                                      BLECharacteristic::PROPERTY_READ   |
                                                                      BLECharacteristic::PROPERTY_WRITE  |
                                                                      BLECharacteristic::PROPERTY_NOTIFY 
                                                                    );

  pCharacteristic2 = pService->createCharacteristic(
                                                                       CHARACTERISTIC_UUID2,
                                                                       BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_INDICATE
                                                                    );
  pService->start();

	adv_data[0] = 0x0A;
	adv_data[1] = 0x09;
	memcpy(&adv_data[2], "TEST_BLE", 9);
  pAdvertising->setAdvertisementData((uint8_t*)&adv_data,0xb);

	adv_rsp_data[0] = 0x06;
	adv_rsp_data[1] = 0x08;
	memcpy(&adv_rsp_data[2], "TEST", 5);
  pAdvertising->setScanResponseData((uint8_t*)&adv_rsp_data,7);

  pAdvertising->start();
}

void loop() {
  // put your main code here, to run repeatedly:
    if (deviceConnected) {
        delay(500);
        
        pCharacteristic1->setValue((uint8_t*)&value, 4);
        pCharacteristic1->notify();
        value++;

        delay(500); // bluetooth stack will go into congestion, if too many packets are sent, in 6 hours test i was able to go as low as 3ms
        pCharacteristic2->setValue((uint8_t*)&value, 4);
        pCharacteristic2->indicate();
    }

    // disconnecting
    if (!deviceConnected && oldDeviceConnected) {
        delay(500); // give the bluetooth stack the chance to get things ready
        pAdvertising->start();// restart advertising
        Serial.println("start advertising");
        oldDeviceConnected = deviceConnected;
    }

    // connecting
    if (deviceConnected && !oldDeviceConnected) {
        oldDeviceConnected = deviceConnected;
    }

}
