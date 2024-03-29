#include "ble.h"


BLEServer *pServer;
BLEService *pService;
BLECharacteristic *pWeightChar;
BLECharacteristic *pCalibrationChar;
BLECharacteristic *pTareChar;

BLECharacteristic *pBatteryChar;
BLECharacteristic *pDisplayBrightnessChar;
extern TaskHandle_t display_task;

void BLESetup(){
  BLEDevice::init("P8 - Waga");
  esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_DEFAULT, ESP_PWR_LVL_P9); 
  esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_ADV, ESP_PWR_LVL_P9);
  esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_SCAN ,ESP_PWR_LVL_P9);
  
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new ServerCallbacks());
  pService = pServer->createService(SERVICE_CUUID);


  pWeightChar = pService->createCharacteristic(
    WEIGHT_CUUID,
    BLECharacteristic::PROPERTY_READ |
    BLECharacteristic::PROPERTY_BROADCAST |
    BLECharacteristic::PROPERTY_NOTIFY |
    BLECharacteristic::PROPERTY_INDICATE
  );
  pWeightChar->setValue("none");
  pWeightChar->addDescriptor(new BLE2902());

  
  pCalibrationChar = pService->createCharacteristic(
    CAL_CUUID,
    BLECharacteristic::PROPERTY_READ |
    BLECharacteristic::PROPERTY_WRITE
  );
  pCalibrationChar->setValue("none");
  pCalibrationChar->setCallbacks(new CalibrationCallbacks());
  pCalibrationChar->addDescriptor(new BLE2902());


  pTareChar = pService->createCharacteristic(
    TARE_CUUID,
    BLECharacteristic::PROPERTY_WRITE
  );
  pTareChar->setCallbacks(new TareCallbacks());
  pTareChar->addDescriptor(new BLE2902());


  pBatteryChar = pService->createCharacteristic(
    BATTERY_CUUID,
    BLECharacteristic::PROPERTY_READ |
    BLECharacteristic::PROPERTY_BROADCAST |
    BLECharacteristic::PROPERTY_NOTIFY |
    BLECharacteristic::PROPERTY_INDICATE
  );
  pBatteryChar->setValue("none");
  pBatteryChar->addDescriptor(new BLE2902());


  pDisplayBrightnessChar = pService->createCharacteristic(
    BRIGHTNESS_CUUID,
    BLECharacteristic::PROPERTY_READ |
    BLECharacteristic::PROPERTY_WRITE
  );

  pDisplayBrightnessChar->setValue(&deviceConfig.displayBrightness, sizeof(uint8_t));
  pDisplayBrightnessChar->setCallbacks(new DisplayCallbacks);
  pDisplayBrightnessChar->addDescriptor(new BLE2902());



  pService->start();
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_CUUID);
  pAdvertising->setScanResponse(false);
  pAdvertising->setMinPreferred(0x0);
  BLEDevice::startAdvertising();
}


void ServerCallbacks::onConnect(BLEServer* pServer){
    Serial.println("New device connected");
}
void ServerCallbacks::onDisconnect(BLEServer* pServer){
    Serial.println("Device disconnected");
    BLEDevice::startAdvertising();
}


void CalibrationCallbacks::onWrite(BLECharacteristic* pCharacteristic){
  std::string rxValue = pCharacteristic->getValue(); 
  LCCal(atof(rxValue.c_str()));
  // pCharacteristic->setValue((uint8_t *)&result, sizeof(float));
  
}
void CalibrationCallbacks::onRead(BLECharacteristic* pCharacteristic){
   
}



void TareCallbacks::onWrite(BLECharacteristic* pCharacteristic){
  xTaskNotifyFromISR(display_task, NOTIFICATION_TARE_BIT, eSetBits, NULL);
}
void TareCallbacks::onRead(BLECharacteristic* pCharacteristic){
   
}



void DisplayCallbacks::onWrite(BLECharacteristic* pCharacteristic){
  uint8_t brightness = *(uint8_t *)pCharacteristic->getValue().c_str();
  setDisplayBrightness(brightness);
  pCharacteristic->setValue((uint8_t *)&brightness, sizeof(uint8_t));
}
void DisplayCallbacks::onRead(BLECharacteristic* pCharacteristic){
   
}


bool BLEConnected(){
    if(pServer->getConnectedCount()>0) return true;
    return false;
}

