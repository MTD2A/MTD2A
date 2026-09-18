// Two flashing LEDs. One with symmetric interval and another with asymmetric interval.
// Examples are written for the Arduino Nano pinout (Nano family, Uno, Mega)
// Short DEMO: https://youtu.be/eyGRazX9Bko
// Jørgen Bo Madsen / updated september 2026 / https://github.com/jebmdk

#include <MTD2A.h>
using namespace MTD2A_const;

MTD2A_binary_output red_LED   ("Red LED",   400, 400);  // 0.4 sec light, 0.4 sec no light
MTD2A_binary_output green_LED ("Green LED", 300, 700, 0, P_W_M, 96);  // 0.3 sec light, 0.7 sec no light, PWM dimmed

#define section 1
// 1: Re-activated blink 
// 2: Automated continuously blink
// 3: switch-example: Soft blink (like an incandescent bulb) blink using PWM curves
// 4: if-example:     Soft blink (like an incandescent bulb) blink using PWM curves
// 5: Switching red and green Soft blink (like an incandescent bulb) blink using PWM curves

void setup() {
  Serial.begin(9600);
  while (!Serial) { delay(10); } // ESP32 Serial Monitor ready delay

  byte RED_LED_PIN   = 9;
  byte GREEN_LED_PIN = 10;

  red_LED.initialize   (RED_LED_PIN);
  green_LED.initialize (GREEN_LED_PIN);

  Serial.println("Two blinking LED");
}


// ----------------------------------------------------------------------------------------

#if section == 1

void loop() {
  if (red_LED.get_processState () == COMPLETE) {
    red_LED.activate ();
  }
  if (green_LED.get_processState () == COMPLETE) {
    green_LED.activate ();
  }

  MTD2A_loop_execute ();
} // Two flashing LEDs. One with symmetric interval and another with asymmetric interval.


// ----------------------------------------------------------------------------------------


#elif section == 2

long loopCount = 0;

void loop() {
  switch (loopCount) {
    case  300: // 3 seconds
      Serial.println("START Red and Green blink");
      red_LED.set_loopActivate   (ENABLE);
      green_LED.set_loopActivate (ENABLE);
      red_LED.activate   ();
      green_LED.activate ();
    break;
    case 1000: // 10 seconds (7 seconds blink)
      Serial.println("STOP blink");
      red_LED.set_loopActivate   (DISABLE);
      green_LED.set_loopActivate (DISABLE);
      loopCount = 0;
    break;
  } // switch

  loopCount++;
  MTD2A_loop_execute();
} // Two flashing LEDs. One with symmetric interval and another with asymmetric interval.


// ----------------------------------------------------------------------------------------


#elif section == 3

int softStep = 0;

void loop() {
  switch (softStep) {
    case 0:
      Serial.println (F("switch-example: Soft blink (like an incandescent bulb) blink using PWM curves"));
      softStep = 1;
      break;
    case 1:
      if (red_LED.get_processState () == COMPLETE) {  // waith for case 2 process to end
        // Light up during 200 milliseconds and keep full lightning for 300 milliseconds
        red_LED.set_timers (200, 0, 300); 
        red_LED.activate(MIN_PWM_VALUE, MAX_PWM_VALUE, RISING_LED);
        softStep = 2;
      }
      break;
    case 2:
      if (red_LED.get_processState () == COMPLETE) {  // waith for case 1 process to end
        // Light down during 200 milliseconds and keep zero lightning for 300 milliseconds
        red_LED.set_timers (200, 0, 300); 
        red_LED.activate(MAX_PWM_VALUE, MIN_PWM_VALUE, FALLING_LED);
        softStep = 1;
      }
      break;      
  } // switch 
  MTD2A_loop_execute();
} // loop


// ----------------------------------------------------------------------------------------


#elif section == 4

bool switchUpDown = true, printFlag = true;

void loop() {
  if (printFlag) {
    Serial.println (F("if-example: Soft blink (like an incandescent bulb) blink using PWM curves"));
    printFlag = false;
  }
  //
  if (switchUpDown) {
    if (red_LED.get_processState () == COMPLETE) {
      // Light up during 200 milliseconds and keep full lightning for 300 milliseconds
      red_LED.set_timers (200, 0, 300);
      red_LED.activate(MIN_PWM_VALUE, MAX_PWM_VALUE, RISING_LED);
      switchUpDown = false;
    }
  }
  else {
    if (red_LED.get_processState () == COMPLETE) {
      // Light down during 200 milliseconds and keep zero lightning for 300 milliseconds      
      red_LED.set_timers (200, 0, 300);
      red_LED.activate(MAX_PWM_VALUE, MIN_PWM_VALUE, FALLING_LED);
      switchUpDown = true;
    }
  } // switchUpDown
  MTD2A_loop_execute();
} // loop


// ----------------------------------------------------------------------------------------


#elif section == 5

bool switchUpDown = true, printFlag = true;

void loop() {
  if (printFlag) {
    Serial.println (F("Switching red and green Soft blink (like an incandescent bulb) blink using PWM curves"));
    printFlag = false;
  }
  //
  if (switchUpDown) {
    if (red_LED.get_processState () == COMPLETE) {
      // Light up during 200 milliseconds and keep full lightning for 300 milliseconds
      red_LED.set_timers (200, 0, 300);
      red_LED.activate(MIN_PWM_VALUE, MAX_PWM_VALUE, RISING_LED);
      green_LED.set_timers (200, 0, 300);
      green_LED.activate(MAX_PWM_VALUE, MIN_PWM_VALUE, FALLING_LED);
      switchUpDown = false;
    }
  }
  else {
    if (red_LED.get_processState () == COMPLETE) {
      // Light down during 200 milliseconds and keep zero lightning for 300 milliseconds      
      red_LED.set_timers (200, 0, 300);
      red_LED.activate(MAX_PWM_VALUE, MIN_PWM_VALUE, FALLING_LED);
      green_LED.set_timers (200, 0, 300);
      green_LED.activate(MIN_PWM_VALUE, MAX_PWM_VALUE, RISING_LED);
      switchUpDown = true;
    }
  } // switchUpDown
  MTD2A_loop_execute();
} // loop


#endif