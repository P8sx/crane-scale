#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>
#include "config.h"
#include "Preferences.h"

uint8_t displayBrightness(uint8_t brightness);
void displaySetup();
void displayWeight(uint16_t weight);
void displayBattery(uint8_t batteryPercentage);
void displayBLEStatusBar(bool status);
void displayBatteryStatusBar(bool status, uint8_t batteryPercentage);
void displayTareStatusBar(bool status);