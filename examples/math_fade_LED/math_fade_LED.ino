// Two LEDs fading in and fading out using a linear function and a math potence function
// https://github.com/MTD2A/MTD2A/blob/main/doc/MTD2A_binary_output_english.pdf
// Short DEMO: https://youtu.be/8TV6nOdXBno
// Jørgen Bo Madsen / August 2025 / https://github.com/jebmdk

#include <MTD2A.h>
using namespace MTD2A_const;

// 2 sec light, no time delay before and after
MTD2A_binary_output green_LED_1 ("Green LED 1", 2000);  
MTD2A_binary_output green_LED_2 ("Green LED 2", 2000);  
MTD2A_binary_output red_LED_1   ("Red LED 1",   2000);
MTD2A_binary_output red_LED_2   ("Red LED 2",   2000);  

long loopCount       = 0;
bool changeFlag      = false;
bool LEDchangeFlag   = true;

void setup() {
  Serial.begin(9600);
  while (!Serial) { delay(10); } // ESP32 Serial Monitor ready delay

  byte GREEN_LED_PIN_1 = 5;
  byte GREEN_LED_PIN_2 = 6;
  byte RED_LED_PIN_1   = 9;
  byte RED_LED_PIN_2   = 10;

  green_LED_1.initialize (GREEN_LED_PIN_1);
  green_LED_2.initialize (GREEN_LED_PIN_2);
  red_LED_1.initialize   (RED_LED_PIN_1);
  red_LED_2.initialize   (RED_LED_PIN_2);
}

void loop() {
  if (changeFlag == false) {
    if (red_LED_1.get_processState() == COMPLETE  && red_LED_2.get_processState() == COMPLETE) {
      if (loopCount == 0) {
        Serial.println ("RED: RISING/FALLING XY and RISING/FALLING LED5");
      }
      if (LEDchangeFlag == true) {
        red_LED_1.activate(MIN_PWM_VALUE, MAX_PWM_VALUE, RISING_XY);
        red_LED_2.activate(MIN_PWM_VALUE, MAX_PWM_VALUE, RISING_LED);
        LEDchangeFlag = false;
      } 
      else {
        red_LED_1.activate(MAX_PWM_VALUE, MIN_PWM_VALUE, FALLING_XY);
        red_LED_2.activate(MAX_PWM_VALUE, MIN_PWM_VALUE, FALLING_LED);
        LEDchangeFlag = true;
      }
      loopCount++;
    }  // red COMPLETE
    if (loopCount > 5) {
      changeFlag = true;
      loopCount = 0;
    }
  } 
  else { // changeFlag = true
    if (green_LED_1.get_processState() == COMPLETE  && green_LED_2.get_processState() == COMPLETE) {
      if (loopCount == 0) {
        Serial.println ("GREEN: RISING/FALLING XY and RISING/FALLING E05");
      }
      if (LEDchangeFlag == true) {
        green_LED_1.activate(MIN_PWM_VALUE, MAX_PWM_VALUE, RISING_XY);
        green_LED_2.activate(MIN_PWM_VALUE, MAX_PWM_VALUE, RISING_LED);
        LEDchangeFlag = false;
      } 
      else {
        green_LED_1.activate(MAX_PWM_VALUE, MIN_PWM_VALUE, FALLING_XY);
        green_LED_2.activate(MAX_PWM_VALUE, MIN_PWM_VALUE, FALLING_LED);
        LEDchangeFlag = true;
      }
      loopCount++;
    } // red COMPLETE
    if (loopCount > 5) {
      changeFlag = false;
      loopCount = 0;
    }
  }  // changeFlag = true

  MTD2A_loop_execute();
} // Two smooth fading in and fading out LEDs with different cadance

