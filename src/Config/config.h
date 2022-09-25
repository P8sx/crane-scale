#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include <Preferences.h>


#define DEVICE_NAME "crane-scale"
#define CONFIG "cfg-1"

#define STATUS_LED

/* WS2812B Matrix */
#define WS2812_PIN      23
/* Input buttons */
#define TARE_PIN        21
#define BAT_PIN         25
/* HX711 Load cell */
#define DOUT_PIN        19
#define SCK_PIN         22

#define BATTERY_STATUS_SCREEN_TIME  3000
#define WEIGHT_SAMPLES              10

/* BLE Characteristic */
#define SERVICE_CUUID               "68da8b4c-d882-11ec-9d64-0242ac120002"
#define WEIGHT_CUUID                "68da8d0e-d882-11ec-9d64-0242ac120002"
#define CAL_CUUID                   "68da8e08-d882-11ec-9d64-0242ac120002"
#define TARE_CUUID                  "68da8eee-d882-11ec-9d64-0242ac120002"
#define BATTERY_CUUID               "68da8fd4-d882-11ec-9d64-0242ac120002"
#define BRIGHTNESS_CUUID            "68da90ba-d882-11ec-9d64-0242ac120002"

#define UNUSED_CUUID_0              "68da91a0-d882-11ec-9d64-0242ac120002"
#define UNUSED_CUUID_1              "68da94e8-d882-11ec-9d64-0242ac120002"
#define UNUSED_CUUID_2              "68da95d8-d882-11ec-9d64-0242ac120002"
#define UNUSED_CUUID_3              "68da96b4-d882-11ec-9d64-0242ac120002"
#define UNUSED_CUUID_4              "68da9790-d882-11ec-9d64-0242ac120002"
#define UNUSED_CUUID_5              "68da986c-d882-11ec-9d64-0242ac120002"
#define UNUSED_CUUID_6              "68da9948-d882-11ec-9d64-0242ac120002"
#define UNUSED_CUUID_7              "68da9a24-d882-11ec-9d64-0242ac120002"
#define UNUSED_CUUID_8              "68da9b00-d882-11ec-9d64-0242ac120002"
#define UNUSED_CUUID_9              "68da9d12-d882-11ec-9d64-0242ac120002"
#define UNUSED_CUUID_11             "68da9df8-d882-11ec-9d64-0242ac120002"
#define UNUSED_CUUID_12             "68da9ede-d882-11ec-9d64-0242ac120002"
#define UNUSED_CUUID_13             "68da9fc4-d882-11ec-9d64-0242ac120002"
#define UNUSED_CUUID_14             "68daa0a0-d882-11ec-9d64-0242ac120002"

/* Task notification bits */
/* Data task */
#define NOTIFICATION_TARE_BIT               0b00000001
#define NOTIFICATION_ZERO_BIT               0b00000010
/* Display task */
#define NOTIFICATION_BATTERY_DISPLAY_BIT    0b00000001
#define NOTIFICATION_TARE_DISPLAY_BIT       0b00000010
#define NOTIFICATION_ZERO_DISPLAY_BIT       0b00000100
#define NOTIFICATION_OFF_DISPLAY_BIT        0b00001000

typedef struct DeviceConfig{
    float       scale                       = 0;
    int64_t     zero                        = 0;  
    uint8_t     displayBrightness           = 50;
} DeviceConfig;

static Preferences pref;
extern DeviceConfig deviceConfig;

void saveDeviceConfig();
void loadDeviceConfig();


#endif

