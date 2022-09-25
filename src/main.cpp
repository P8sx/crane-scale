#include <Arduino.h>
#include <driver/adc.h>


#include "Config/config.h"



#include "LoadCell/load_cell.h"
#include "Display/display.h"
#include "BLE/ble.h"


DeviceConfig deviceConfig;

TaskHandle_t data_task = NULL;
TaskHandle_t display_task = NULL;
TaskHandle_t input_task = NULL;

QueueHandle_t vbat_queue = NULL;
QueueHandle_t weight_queue = NULL;

extern BLECharacteristic *pWeightChar;
extern BLECharacteristic *pBatteryChar;


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



void dataTask(void * parameter){
  uint8_t vbat = 0;
  int32_t weight = 0;
  uint32_t taskNotificationFlag_u32; 

  for(;;){
    if(xTaskNotifyWait(pdFALSE, ULONG_MAX, &taskNotificationFlag_u32, 0) == pdPASS){
      if((taskNotificationFlag_u32 & NOTIFICATION_TARE_BIT) == NOTIFICATION_TARE_BIT){
        LCOffset();
      }
      if((taskNotificationFlag_u32 & NOTIFICATION_ZERO_BIT) == NOTIFICATION_ZERO_BIT){
        LCZero(); 
        }
    }
    weight = LCWeight();
    xQueueSend(weight_queue, &weight, 0);
    
    vbat = VBAT();
    xQueueSend(vbat_queue, &vbat, 0);

    vTaskDelay(500/portTICK_PERIOD_MS);
  }
}


void displayTask(void * parameter){
  uint8_t vbat = 0;
  uint32_t weight = 0;
  uint32_t taskNotificationFlag_u32;
  uint8_t timeLeft = 0;
  displayBattery(VBAT());
  vTaskDelay(2000/portTICK_PERIOD_MS);  

  for(;;){

    if(xQueueReceive(weight_queue, &weight, 0) == pdTRUE){
      /* Update BLE characteristic */
      pWeightChar->setValue((uint8_t *)&weight, sizeof(weight));
      pWeightChar->notify();

      displayWeight(weight);
    }

    if(xQueueReceive(vbat_queue, &vbat, 0) == pdTRUE){
      /* Update BLE characteristic */
      pBatteryChar->setValue((uint8_t *)&vbat, sizeof(vbat));
      pBatteryChar->notify();

      #ifdef STATUS_LED
        displayBatteryStatusLED(true, vbat);
      #endif
    } 

    #ifdef STATUS_LED
      displayBLEStatusLED(BLEConnected());
      if(xTaskNotifyWait(pdFALSE, ULONG_MAX, &taskNotificationFlag_u32, 0) == pdPASS){
        if((taskNotificationFlag_u32 & NOTIFICATION_TARE_DISPLAY_BIT) == NOTIFICATION_TARE_DISPLAY_BIT){
          displayTareZeroStatusLED(true);
        }
        else if((taskNotificationFlag_u32 & NOTIFICATION_ZERO_DISPLAY_BIT) == NOTIFICATION_ZERO_DISPLAY_BIT){
          displayTareZeroStatusLED(true, true);
        }
        else if((taskNotificationFlag_u32 & NOTIFICATION_OFF_DISPLAY_BIT) == NOTIFICATION_OFF_DISPLAY_BIT){
          displayTareZeroStatusLED(false);
        }
      }

    #endif


    /* Display battery status on main screen */
    if(xTaskNotifyWait(pdFALSE, ULONG_MAX, &taskNotificationFlag_u32, 0) == pdPASS && (taskNotificationFlag_u32 & NOTIFICATION_BATTERY_DISPLAY_BIT) == NOTIFICATION_BATTERY_DISPLAY_BIT){
      displayBattery(vbat);
      vTaskDelay(BATTERY_STATUS_SCREEN_TIME/portTICK_PERIOD_MS);
    }

    vTaskDelay(200/portTICK_PERIOD_MS);
  }
}

void inputTask(void * parameter){
  uint8_t tareButtonHoldTime = 0;
  uint8_t timeLeft = 0;
  for(;;){
    
    /* Tare button handling */
    while(digitalRead(TARE_PIN) == LOW){
      tareButtonHoldTime++;
      vTaskDelay(50/portTICK_PERIOD_MS);
      if(tareButtonHoldTime == 1) xTaskNotify(display_task, NOTIFICATION_TARE_DISPLAY_BIT, eSetBits);
      else if(tareButtonHoldTime >= 60) xTaskNotify(display_task, NOTIFICATION_ZERO_DISPLAY_BIT, eSetBits);
    }

    if(tareButtonHoldTime >= 1 && tareButtonHoldTime < 60){
      xTaskNotify(data_task, NOTIFICATION_TARE_BIT, eSetBits);
      timeLeft = 40;
    }
    else if(tareButtonHoldTime >= 60){
      xTaskNotify(data_task, NOTIFICATION_ZERO_BIT, eSetBits);
      timeLeft = 40;
    }
    tareButtonHoldTime = 0;
    if(timeLeft >= 1) timeLeft--;
    else if (timeLeft == 0) xTaskNotify(display_task, NOTIFICATION_OFF_DISPLAY_BIT, eSetBits);
    
    /* Display button handling */
    if(digitalRead(BAT_PIN) == LOW){
      xTaskNotify(display_task, NOTIFICATION_BATTERY_DISPLAY_BIT, eSetBits);
      vTaskDelay(2000/portTICK_PERIOD_MS);
    }

    vTaskDelay(50/portTICK_PERIOD_MS);
  }
}

void setup() {
  Serial.begin(9600);


  Serial.println("Booting");
  loadDeviceConfig();

  adc1_config_width(ADC_WIDTH_12Bit);
  adc1_config_channel_atten(ADC1_CHANNEL_5, ADC_ATTEN_11db);
  pinMode(TARE_PIN,INPUT_PULLUP);
  pinMode(BAT_PIN,INPUT_PULLUP);


  displaySetup();
  BLESetup();
  LCSetup();

  while(digitalRead(BAT_PIN) == LOW && digitalRead(TARE_PIN) == LOW){
    delay(1000); Serial.println("Recovery mode");
  }

  vbat_queue = xQueueCreate(2, sizeof(uint8_t));
  weight_queue = xQueueCreate(2, sizeof(int32_t));

  xTaskCreatePinnedToCore(dataTask, "DATA_TASK", 10000, NULL, 1, &data_task, 0);
  xTaskCreatePinnedToCore(inputTask, "INPUT_TASK",10000, NULL, 1, &input_task, 1);
  xTaskCreatePinnedToCore(displayTask, "MAIN_TASK", 10000, NULL, 1, &display_task, 1);
}


void loop() {

}


