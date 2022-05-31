#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>
#include "config.h"
#include "load_cell.h"
#include "display.h"


void BLESetup();
bool BLEConnected();
class ServerCallbacks: public BLEServerCallbacks{
    void onConnect(BLEServer* pServer);
    void onDisconnect(BLEServer* pServer);
};

class CalibrationCallbacks: public BLECharacteristicCallbacks{
    void onWrite(BLECharacteristic* pCharacteristic);
    void onRead(BLECharacteristic* pCharacteristic);
};
class TareCallbacks: public BLECharacteristicCallbacks{
    void onWrite(BLECharacteristic* pCharacteristic);
    void onRead(BLECharacteristic* pCharacteristic);
};
class DisplayCallbacks: public BLECharacteristicCallbacks{
    void onWrite(BLECharacteristic* pCharacteristic);
    void onRead(BLECharacteristic* pCharacteristic);
};




