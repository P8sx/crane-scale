#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>
#include "Config/config.h"

extern DeviceConfig deviceConfig;

void setDisplayBrightness(uint8_t brightness);

void displaySetup();
void displayWeight(int16_t weight);
void displayBattery(uint8_t batteryPercentage);

#ifdef STATUS_LED
void displayBLEStatusLED(bool status);
void displayBatteryStatusLED(bool status, uint8_t batteryPercentage);
void displayTareZeroStatusLED(bool status, bool zero = false);
#endif

#endif