#include "config.h"


void saveDeviceConfig(){

    pref.begin(DEVICE_NAME, false);
    uint8_t bytes[sizeof(deviceConfig)];
    memcpy(bytes, &deviceConfig, sizeof(deviceConfig));
    pref.putBytes(CONFIG, bytes, sizeof(deviceConfig));
    pref.end();
}
void loadDeviceConfig(){
    pref.begin(DEVICE_NAME, false);
    uint8_t bytes[sizeof(deviceConfig)];
    pref.getBytes(CONFIG, bytes, sizeof(deviceConfig));
    memcpy(&deviceConfig, bytes, sizeof(deviceConfig));
    pref.end();
}