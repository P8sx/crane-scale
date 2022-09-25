#include "load_cell.h"

static HX711 scale;
static long offset = 0;




int16_t LCWeight(){
    long value = scale.read_average(WEIGHT_SAMPLES);
    Serial.print("Read val:");
    Serial.print(value);Serial.print(", offset:");
    Serial.print(offset);Serial.print(", zero:");
    Serial.print(deviceConfig.zero);Serial.print(", scale:");
    Serial.print(deviceConfig.scale);

    value = value - deviceConfig.zero - offset;
    value = value / deviceConfig.scale;
    Serial.print(", calculated value:");
    Serial.println((int16_t)value);
    return (int16_t)value;  
}
void LCSetup(){
    scale.begin(DOUT_PIN, SCK_PIN);
}
void LCOffset(){
    if(deviceConfig.zero == 0){
        offset = scale.read_average(WEIGHT_SAMPLES);
    }
    else{
        offset = scale.read_average(WEIGHT_SAMPLES) - deviceConfig.zero;
    }
}
void LCZero(){
    deviceConfig.zero = scale.read_average(WEIGHT_SAMPLES);
    saveDeviceConfig();
}


void LCCal(float knownWeight){   
    float measuredValue = scale.read_average(WEIGHT_SAMPLES);
    deviceConfig.scale = measuredValue/knownWeight;
    saveDeviceConfig();
}