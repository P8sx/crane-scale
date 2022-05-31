#pragma once
#include "HX711.h"
#include "Preferences.h"
#include "config.h"


uint16_t LCWeight();
void LCSetup();
void LCTare();
void LCBeginCal();
float LCCal(float knownWeight);