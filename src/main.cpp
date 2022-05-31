#include <Arduino.h>
#include "Preferences.h"
#include "load_cell.h"
#include "display.h"
#include "ble.h"
#include <driver/adc.h>
#include "Preferences.h"
#include "config.h"

Preferences preferences;
portMUX_TYPE mutex = portMUX_INITIALIZER_UNLOCKED;
TaskHandle_t data_task = NULL;
TaskHandle_t main_task = NULL;

TimerHandle_t tare_timer = NULL;

QueueHandle_t vbat_queue = NULL;
QueueHandle_t weight_queue = NULL;

extern BLECharacteristic *pWeightChar;
extern BLECharacteristic *pBatteryChar;

bool tare_flag = false;

uint8_t VBAT()
{
    long sum = 0;                 
    float voltage = 0.0;           
    float output = 0.0;          
    const float battery_max = 4.2;
    const float battery_min = 3.6; 

    float R1 = 60000.0;
    float R2 = 60000.0;

    for (int i = 0; i < 500; i++)
    {
        sum += adc1_get_raw(ADC1_CHANNEL_5);
    }
    voltage = sum / (float)500;
    voltage = (voltage * 3.4) / 4095.0;
    voltage = voltage / (R2/(R1+R2));
    voltage = roundf(voltage * 100) / 100;
    output = ((voltage - battery_min) / (battery_max - battery_min)) * 100;
    if (output < 100)
        return output;
    else
        return 100;
}

void tareTimerCallback(TimerHandle_t xTimer){
  static uint8_t blink_count = 0;
  tare_flag = !tare_flag;

  if(++blink_count > 3){
    blink_count = 0;
    tare_flag = false;
    xTimerStop(tare_timer, 0);
  }
}

void dataTask(void * parameter){
  uint8_t vbat = 0;
  uint32_t weight = 0;
  uint32_t notification; 
  for(;;){
    if(xTaskNotifyWait(pdFALSE, ULONG_MAX, &notification, 0) == pdPASS){
      if((notification & TARE_BIT) != 0){
        LCTare();
      }      
    }

    weight = LCWeight();
    xQueueSend(weight_queue, &weight, 0);
    
    vbat = VBAT();
    xQueueSend(vbat_queue, &vbat, 0);

    vTaskDelay(500/portTICK_PERIOD_MS);
  }
}


void mainTask(void * parameter){
  uint8_t vbat = 0;
  uint32_t weight = 0;
  uint32_t notification;

  displayBattery(VBAT());
  vTaskDelay(2000/portTICK_PERIOD_MS);  

  for(;;){

    if(xQueueReceive(weight_queue, &weight, 0) == pdTRUE){
      displayWeight(weight);
      pWeightChar->setValue((uint8_t *)&weight, sizeof(weight));
      pWeightChar->notify();
    }

    if(xQueueReceive(vbat_queue, &vbat, 0) == pdTRUE){
      displayBatteryStatusBar(true, vbat);
      pBatteryChar->setValue((uint8_t *)&vbat, sizeof(vbat));
      pBatteryChar->notify();
    } 

    displayBLEStatusBar(BLEConnected());

    displayTareStatusBar(tare_flag);


    if(digitalRead(TARE_PIN) == LOW || (xTaskNotifyWait(pdFALSE, ULONG_MAX, &notification, 0) == pdPASS && (notification & TARE_BIT) != 0)){
      xTimerStart(tare_timer, 0);
      xTaskNotify(data_task, TARE_BIT, eSetBits);
    }

    if(digitalRead(BAT_PIN) == LOW){
      displayBattery(vbat);
      vTaskDelay(3000/portTICK_PERIOD_MS);
    }
    vTaskDelay(50/portTICK_PERIOD_MS);
  }
}

void setup() {
  adc1_config_width(ADC_WIDTH_12Bit);
  adc1_config_channel_atten(ADC1_CHANNEL_5, ADC_ATTEN_11db);
  pinMode(TARE_PIN,INPUT_PULLUP);
  pinMode(BAT_PIN,INPUT_PULLUP);

  Serial.begin(9600);
  displaySetup();
  BLESetup();
  LCSetup();

  while(digitalRead(BAT_PIN) == LOW && digitalRead(TARE_PIN) == LOW){
    delay(1000); Serial.println("Recovery mode");
  }
  vbat_queue = xQueueCreate(2, sizeof(uint8_t));
  weight_queue = xQueueCreate(2, sizeof(uint32_t));

  xTaskCreatePinnedToCore(dataTask, "DATA_TASK", 10000, NULL, 1, &data_task, 0);
  xTaskCreatePinnedToCore(mainTask, "MAIN_TASK", 10000, NULL, 1, &main_task, 1);
  tare_timer = xTimerCreate("TARE_TIMER", pdMS_TO_TICKS(400), pdTRUE, NULL, tareTimerCallback);
}


void loop() {

}


