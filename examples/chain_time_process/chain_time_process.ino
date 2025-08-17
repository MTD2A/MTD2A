// Chain processing 4 cascading roud robin LEDs
// https://github.com/MTD2A/MTD2A/blob/main/doc/MTD2A_binary_output_english.pdf
// Jørgen Bo Madsen / july 2025 / https://github.com/jebmdk

#include <MTD2A.h>
using namespace MTD2A_const;

// 0,5 sec light, 0,5 sec delay
MTD2A_binary_output green_LED_1 ("Green LED 1", 500);  
MTD2A_binary_output green_LED_2 ("Green LED 2", 500);  
MTD2A_binary_output red_LED_1   ("Red LED 1",   500);
MTD2A_binary_output red_LED_2   ("Red LED 2",   500);  

long loopCount  = 0;
long stepCount  = 0;

void setup() {
  // Serial.begin(9600); // Writing to Serial Output will cause time delays. choose the fastest speed possible!
  Serial.begin(250000); 
  while (!Serial) { delay(10); } // ESP32 Serial Monitor ready delay

  byte GREEN_LED_PIN_1 = 5;
  byte GREEN_LED_PIN_2 = 6;
  byte RED_LED_PIN_1   = 9;
  byte RED_LED_PIN_2   = 10;

  green_LED_1.initialize (GREEN_LED_PIN_1);
  green_LED_2.initialize (GREEN_LED_PIN_2);
  red_LED_1.initialize   (RED_LED_PIN_1);
  red_LED_2.initialize   (RED_LED_PIN_2);
  MTD2A_globalDebugPrint (); // Write state information to Serial output.

  Serial.print("Loop time: "); Serial.print(MTD2A::get_globalDelayTimeMS()); Serial.println(" Millisecond");
}

void loop() {

  // Timed cascading. Time critical.
  switch (loopCount) {
    case   0: green_LED_1.activate(); break;
    case  50: green_LED_2.activate(); break;
    case 100: red_LED_1.activate();   break;
    case 150: red_LED_2.activate();   break;
  }
  loopCount++;
  if (loopCount >= 200) {
    loopCount = 0;
  }


  /*
  // Step cascading. Time / delay independent
  switch (stepCount) {
    case 0:
      if (red_LED_2.get_processState() == COMPLETE) {
        green_LED_1.activate();
        stepCount = 1;
    }
    break;
    case 1:
      if (green_LED_1.get_processState() == COMPLETE) {
        green_LED_2.activate();
        stepCount = 2;
      }
      break;
    case 2:
      if (green_LED_2.get_processState() == COMPLETE) {
        red_LED_1.activate();
        stepCount = 3;
      }
      break;
    case 3:
      if (red_LED_1.get_processState() == COMPLETE) {
        red_LED_2.activate();
        stepCount = 0;
      }
      break;
  }
  */

  MTD2A_loop_execute();
}

