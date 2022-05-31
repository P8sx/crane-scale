#include "load_cell.h"

static HX711 scale;
extern Preferences preferences;

uint16_t LCWeight(){
    return abs(scale.get_units(10));  
}
void LCSetup(){
    scale.begin(DOUT_PIN, SCK_PIN);
    scale.tare();

    preferences.begin(NAME, false);
    float calVal = preferences.getFloat(CFG_CAL, 0);
    if(calVal!=0) scale.set_scale(calVal);  
    preferences.end();
}
void LCTare(){
    scale.tare();
}
void LCBeginCal(){
    scale.set_scale();
    scale.tare();
}
float LCCal(float knownWeight){   
    float weight = scale.get_units(10);
    float result = weight/knownWeight;
    scale.set_scale(result);

    preferences.begin(NAME, false);
    preferences.putFloat(CFG_CAL, result);
    preferences.end();
    return result;
}