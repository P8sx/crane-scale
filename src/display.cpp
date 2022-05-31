#include "display.h"

extern Preferences preferences;

static Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(15, 8, WS2812_PIN,  NEO_MATRIX_TOP + NEO_MATRIX_LEFT + NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG, NEO_GRB + NEO_KHZ800);

static const PROGMEM uint8_t small_numbers[10][7] = {{B11111111, B10111111, B10111111, B10111111, B10111111, B10111111, B11111111}, {B00111111, B00111111, B00111111, B00111111, B00111111, B00111111, B00111111}, {B11111111, B00111111, B00111111, B11111111, B10011111, B10011111, B11111111}, {B11111111, B00111111, B00111111, B11111111, B00111111, B00111111, B11111111}, {B10111111, B10111111, B10111111, B11111111, B00111111, B00111111, B00111111}, {B11111111, B10011111, B10011111, B11111111, B00111111, B00111111, B11111111}, {B11111111, B10011111, B10011111, B11111111, B10111111, B10111111, B11111111}, {B11111111, B00111111, B00111111, B00111111, B00111111, B00111111, B00111111}, {B11111111, B10111111, B10111111, B11111111, B10111111, B10111111, B11111111}, {B11111111, B10111111, B10111111, B11111111, B00111111, B00111111, B11111111}};
static const PROGMEM uint8_t big_numbers[10][7] = {{B11111111, B10011111, B10011111, B10011111, B10011111, B10011111, B11111111}, {B00011111, B00011111, B00011111, B00011111, B00011111, B00011111, B00011111}, {B11111111, B00011111, B00011111, B11111111, B10001111, B10001111, B11111111}, {B11111111, B00011111, B00011111, B11111111, B00011111, B00011111, B11111111}, {B10011111, B10011111, B10011111, B11111111, B00011111, B00011111, B00011111}, {B11111111, B10001111, B10001111, B11111111, B00011111, B00011111, B11111111}, {B11111111, B10001111, B10001111, B11111111, B10011111, B10011111, B11111111}, {B11111111, B00011111, B00011111, B00011111, B00011111, B00011111, B00011111}, {B11111111, B10011111, B10011111, B11111111, B10011111, B10011111, B11111111}, {B11111111, B10011111, B10011111, B11111111, B00011111, B00011111, B11111111}};

uint8_t displayBrightness(uint8_t brightness = 0){
    preferences.begin(NAME, false);
    if(brightness == 0){
        brightness = preferences.getUChar(CFG_DISPLAY_BRIGHTNESS, 0);
        preferences.end();
        return brightness;
    }
    matrix.setBrightness(brightness);
    preferences.putUChar(CFG_DISPLAY_BRIGHTNESS, brightness);
    preferences.end();
    return brightness;
}

void displaySetup()
{
    matrix.begin();
    matrix.setBrightness(50);

    matrix.fill(matrix.Color(255, 0, 0));
    matrix.show();
    delay(200);
    matrix.fill(matrix.Color(0, 255, 0));
    matrix.show();
    delay(200);
    matrix.fill(matrix.Color(0, 0, 255));
    matrix.show();
    delay(200);
    matrix.fill(0);
    matrix.show();

    preferences.begin(NAME, false);
    uint8_t brightness = preferences.getUChar(CFG_DISPLAY_BRIGHTNESS, 50);
    matrix.setBrightness(brightness);
    preferences.end();
}


int countDigit(long long n)
{
    if (n / 10 == 0)
        return 1;
    return 1 + countDigit(n / 10);
}

void displayValue(uint16_t number, int16_t x, int16_t y, uint16_t color){
    matrix.fillRect(x-15, y, 15, 7, 0);
    if(number < 1000 && (number < 100 || x > 11)){
        matrix.drawBitmap(x-4*1, y,big_numbers[number%10],4,7,color);
        if(number >= 10) matrix.drawBitmap(x-1-4*2, y,big_numbers[number/10%10],4,7,color);
        if(number >= 100) matrix.drawBitmap(x-2-4*3, y,big_numbers[number/100%10],4,7,color);
    }
    else{
        matrix.drawBitmap(x-3*1, y, small_numbers[number%10], 3, 7, color);
        matrix.drawBitmap(x-1-3*2, y, small_numbers[number/10%10], 3, 7, color);
        matrix.drawBitmap(x-2-3*3, y, small_numbers[number/100%10], 3, 7, color);
        matrix.drawBitmap(x-3-3*4, y, small_numbers[number/1000%10], 3, 7, color);
    }
    matrix.show();
}

void displayWeight(uint16_t weight)
{
    uint16_t color;
    switch(weight){
      case 0 ... 600:
        color = matrix.Color(0,128,255);
      break;
      case 601 ... 1200:
        color = matrix.Color(255,165,0);
      break;
      default:
        color = matrix.Color(255,100,80);
      break;
    }
    displayValue(weight, 15, 0, color);
}

void displayBattery(uint8_t batteryPercentage){

    matrix.fillRect(0,0,15,7,0);
    matrix.drawRect(11,1,4,6,matrix.Color(255,255,255));
    matrix.drawPixel(13,0,matrix.Color(255,255,255));
    matrix.drawPixel(12,0,matrix.Color(255,255,255));

    switch (batteryPercentage){
    case 80 ... 100:
        matrix.drawRect(12,2,2,4,matrix.Color(0,255,0));
        break;
    case 50 ... 79:
        matrix.drawRect(12,3,2,3,matrix.Color(249,215,28));
        break;
    case 25 ... 49:
        matrix.drawRect(12,4,2,2,matrix.Color(255,131,0));
        break;
    default:
        matrix.drawRect(12,5,2,1,matrix.Color(255,0,0));
        break;
    }
    displayValue(batteryPercentage, 10, 0, matrix.Color(255,255,255));
}
void displayBatteryStatusBar(bool status, uint8_t batteryPercentage){
    uint16_t color;
    switch (batteryPercentage){
    case 90 ... 100:
        color = matrix.Color(0,255,0);
        break;
    case 50 ... 89:
        color = matrix.Color(249,215,28);
        break;
    case 25 ... 49:
        color = matrix.Color(255,131,0);
        break;
    default:
        color = matrix.Color(255,0,0);
        break;
    }
    
    if(status)
        matrix.drawLine(5,7,9,7,color);
    else
        matrix.drawLine(5,7,9,7,0); 
    matrix.show();
}
void displayBLEStatusBar(bool status){
    if(status)
        matrix.drawLine(0,7,4,7,matrix.Color(47,141,255));
    else
        matrix.drawLine(0,7,4,7,0);
    matrix.show();
}
void displayTareStatusBar(bool status){
    if(status){
        matrix.drawLine(10,7,15,7,matrix.Color(0,255,255));
    }
    else{
        matrix.drawLine(10,7,15,7,0);
    }
    matrix.show();
}