#ifndef LOAD_CELL_H
#define LOAD_CELL_H

#include "HX711.h"
#include "Config/config.h"
extern DeviceConfig deviceConfig;
/* Function returns weight with applied offsets and scale */
int16_t LCWeight();

/* Initialize crane scale */
void LCSetup();

/* Function sets weight zero */
void LCOffset();

/* Function sets weight zero that will be saved to internal memory */
void LCZero();

/* Function used to calibrate crane scale */
void LCCal(float knownWeight);

#endif