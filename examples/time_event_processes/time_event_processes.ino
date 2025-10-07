// 7 time and event processes examples, to inspire how to code state machine process flows.
// https://github.com/MTD2A/MTD2A/blob/main/doc/MTD2A_english.pdf
// Joergen Bo Madsen / September 2025 / https://github.com/jebmdk

#include <MTD2A.h>
using namespace MTD2A_const;

#define section 1
// 1, 2, 3, 4, 5, 6, 7

// 0,5 sec light, 0,5 sec delay
MTD2A_binary_output green_LED_1 ("Green LED 1", 500); // LED on 0,5 second
MTD2A_binary_output green_LED_2 ("Green LED 2", 500); // LED on 0,5 second
MTD2A_binary_output red_LED_1   ("Red LED 1",   500); // LED on 0,5 second
MTD2A_binary_output red_LED_2   ("Red LED 2",   500); // LED on 0,5 second
MTD2A_binary_input  IR_sensor   ("IR FC-51");         // Infrarede input sensor

MTD2A_timer timer_GL1 ("timer GL1"); 
MTD2A_timer timer_GL2 ("Timer GL2");
MTD2A_timer timer_RL1 ("Timer RL1"); 
MTD2A_timer timer_RL2 ("Timer RL2");

unsigned long loopCount = 0;
unsigned long stepCount = 0;
unsigned long startTime = 0;
unsigned long testTime  = 0;


void setup() {
  // Serial.begin(9600); // Writing to Serial Output can cause time delays. choose the fastest speed possible!
  Serial.begin(250000); 
  while (!Serial) { delay(10); } // ESP32 Serial Monitor ready delay

  byte GREEN_LED_PIN_1 = 5;
  byte GREEN_LED_PIN_2 = 6;
  byte RED_LED_PIN_1   = 9;
  byte RED_LED_PIN_2   = 10;
  byte SENSOR_PIN_A0   = A0;

  green_LED_1.initialize (GREEN_LED_PIN_1);
  green_LED_2.initialize (GREEN_LED_PIN_2);
  red_LED_1.initialize   (RED_LED_PIN_1);
  red_LED_2.initialize   (RED_LED_PIN_2);
  IR_sensor.initialize   (SENSOR_PIN_A0);
  // MTD2A_globalDebugPrint (ENABLE);

  Serial.print("Loop executing time (milliseconds): "); Serial.println(MTD2A::get_globalDelayTimeMS());
}

void loop() {

#if section == 1

  // Timed cascading (round robin) 4 LEDs. Loop time controlled.
  // Simpel and easy to use. Suitable for predictable processes flowing a non critical time line
  // however, if the throughput time exceeds 10 milliseconds, unexpected actions may occur
  // timing is a bit faster than loopCcount and and differences grow over time.
  // ATmega328: 1% - 5% and ESP32 below 1%. Use MTD2A_timer () for precision timing.

  switch (loopCount) {
    case   0: green_LED_1.activate(); break;  // Start at once
    case  50: green_LED_2.activate(); break;  // 0.5 second
    case 100: red_LED_1.activate();   break;  // 1   second
    case 150: red_LED_2.activate();   break;  // 1.5 second
  }
  loopCount++;
  if (loopCount >= 200) {
    loopCount = 0;
  }

#endif

// ------------------------------------------------------------------------------------------------

#if section == 2

  // Timed cascading (round robin) 4 LEDs. Precission time controlled.
  // Suitable for predictable process flows where procison timing is important / follows a precise timeline
  if (loopCount == 0) {
    timer_GL1.timer (START_TIMER,  500); // Complete after 0,5 second
    timer_GL2.timer (START_TIMER, 1000); // Complete after 1   second 
    timer_RL1.timer (START_TIMER, 1500); // Complete after 1,5 second
    timer_RL2.timer (START_TIMER, 2000); // Complete after 2   second
    loopCount++;
  }
  //
  if (timer_GL1.get_phaseChange () == true  &&  timer_GL1.get_phaseNumber () == STOP_TIMER) {
    green_LED_1.activate();
  }
  if (timer_GL2.get_phaseChange () == true  &&  timer_GL2.get_phaseNumber () == STOP_TIMER) {
    green_LED_2.activate();
  }
  if (timer_RL1.get_phaseChange () == true  &&  timer_RL1.get_phaseNumber () == STOP_TIMER) {
    red_LED_1.activate();
  }
  if (timer_RL2.get_phaseChange () == true  &&  timer_RL2.get_phaseNumber () == STOP_TIMER) {
    red_LED_2.activate();
    loopCount = 0;
  }

#endif

// ------------------------------------------------------------------------------------------------

#if section == 3

  // millis(); Not recommende with MTD2A library functions. Use MTD2A_timer ()
  // Timed cascading (round robin) 4 LEDs. Time controlled.
  // Time sensitive. Will not work correctly in case of unexpected delays
  // Will NOT always be completely in sync with MTD2A_loop_execute () 
  // due to 10 millisecon loop time and minor time deviations

  if (millis() - startTime > (2000 + DELAY_10MS)) {  // 2 seconds
    startTime = millis();
  }
  //
  testTime = millis() - startTime;
  if (testTime >= 0  &&  testTime <= DELAY_10MS) { // Start at once
    green_LED_1.activate();
  }
  if (testTime >= 500  &&  testTime <= (500 + DELAY_10MS)) { // 0.5 second
    green_LED_2.activate();
  }
  if (testTime >= 1000  &&  testTime <= (1000 + DELAY_10MS)) { // 1 second
    red_LED_1.activate();
  }
  if (testTime >= 1500  &&  testTime <= (1500 + DELAY_10MS)) { // 1.5 second
    red_LED_2.activate();
  }

#endif

// ------------------------------------------------------------------------------------------------

#if section == 4

  // Timed if-condition nested controlled. Not recommended. Can be used for simple solutions
  // Simple step cascading 4 LEDs. Time / delay independent
  // Suitable for predictable and unpredictable process flows and unexpected loop delays
  if (loopCount == 0)
    red_LED_1.activate (); // Start process and state information
  loopCount = 1;
  //
  if (red_LED_1.get_processState () == COMPLETE) {
   if (red_LED_1.get_phaseChange () == true  &&  red_LED_1.get_phaseNumber () == COMPLETE_PHASE) {
      red_LED_2.activate ();
    }
    if (red_LED_2.get_processState () == COMPLETE) {
      if (red_LED_2.get_phaseChange () == true  &&  red_LED_2.get_phaseNumber () == COMPLETE_PHASE) {
        green_LED_1.activate ();
      }
      if (green_LED_1.get_processState () == COMPLETE) {
        if (green_LED_1.get_phaseChange () == true  &&  green_LED_1.get_phaseNumber () == COMPLETE_PHASE) {
          green_LED_2.activate ();
        }
        if (green_LED_2.get_processState () == COMPLETE) {
          if (green_LED_2.get_phaseChange () == true  &&  green_LED_2.get_phaseNumber () == COMPLETE_PHASE) {
            red_LED_1.activate (); 
          }
        } // green_LED_2
      } // green_LED_1
    } // red_LED_2
  } // red_LED_1

#endif

// ------------------------------------------------------------------------------------------------

#if section == 5

  // Timed step controlled. Recommended for complex solutions
  // Simple round robin 2 green LEDs. Time / delay independent
  // Suitable for predictable and unpredictable process flows and unexpected loop delays
  switch (stepCount) {
    case 0:
      green_LED_1.activate();
      stepCount = 1;
      // do something once
    break;
    case 1:
      if (green_LED_1.get_processState() == COMPLETE) {
        stepCount = 2;
        // do something once
      }
      // do something while wating on COMLPETE
    break;
    case 2:
      green_LED_2.activate();
      stepCount = 3;
      // do something once
    break;
    case 3:
      if (green_LED_2.get_processState() == COMPLETE) {
        stepCount = 0;
        // do something once
      }
      // do something while wating on COMLPETE
    break;
  }

#endif
 
// ------------------------------------------------------------------------------------------------

#if section == 6

  // Event if-condition controlled. Recommended for simple solutions
  // Read infrared sensor. Short detection: One LED blink. 
  // continuously detection: continuously LED blink
  // Suitable for Unpredictable process flows such as object detection with a sensor 
  if (IR_sensor.get_processState() == ACTIVE) {
    Serial.println ("Object detected");
    if (timer_GL1.get_processState() == COMPLETE) { 
      timer_GL1.timer (START_TIMER, 1000); // 1   second
      green_LED_1.activate ();             // 0,5 second
    }
  }

#endif

// ------------------------------------------------------------------------------------------------

#if section == 7

  // Event step controlled. Recommended for complex constructions
  // Read infrared sensor. Short detection: One LED blink. 
  // continuously detection: continuously LED blink 
  // Suitable for unpredictable process flows such as object detection with a sensor 
  switch (stepCount) {
    case 0:
      if (IR_sensor.get_processState() == ACTIVE) {
        // do something while detecting
        Serial.println ("Object detected");
        stepCount = 1;
      }
      // do something while wating on ACTIVE
    break;
    case 1:
      // Add 500 milliseconds pause delay to blink LED
      timer_GL1.timer (START_TIMER, 500);
      stepCount = 2;
    break;
    case 2:
      if (timer_GL1.get_processState() == COMPLETE) {
        // do something once
        green_LED_1.activate();
        stepCount = 3;
      }
      // do something while wating on ACTIVE
    break;
    case 3:
      if (green_LED_1.get_processState() == COMPLETE) {
        // do something once
        stepCount = 0;
      }
      // do something while wating on COMLPETE
    break;
  }

#endif


  MTD2A_loop_execute();
}

