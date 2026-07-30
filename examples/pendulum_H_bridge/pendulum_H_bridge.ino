
// Advanced automatic pendulum operation on model railway system using PWM controlled H-bridge
// to power train engine and direction. A simple H-bridge is used to control track switch and direction. 
// One IR sensor is used to calibrate time and distance offset. Finaly red and green train station signal.
// The train route is controlled by advanced mathematical PWM curves that compensate for inertia and 
// friction, and thus ensure smooth acceleration and deceleration.
// Can easily be expanded with more tracks and sensors. Sensor: https://github.com/MTD2A/FC-51
// MTD2A_binary_output: https://github.com/MTD2A/MTD2A/tree/main/doc#mtd2a_binary_output
// Short DEMO: https://youtu.be/1i-cGc6Dk4E
// Jørgen Bo Madsen / Update 26 april 2026 / https://github.com/jebmdk  

#include <MTD2A.h>
using namespace MTD2A_const;


#define section 2  // 1 = case step process and 2 = for loop process.


// Red LED train signal STOP
MTD2A_binary_output red_LED        ("Red LED signal");  // Constant light
// Green LED train signal drive 
MTD2A_binary_output green_LED      ("Geen LED signal", 500, 0, 500); // Blink
// IR sensor pin input at side track end. 
MTD2A_binary_input  IR_sensor      ("IR sensor");
// Train power and direction. PWM controlled BTS7960 H-bridge
MTD2A_binary_output train_forward  ("Train forward");
MTD2A_binary_output train_backward ("Train backward");
// Track switch actuator power and direction. PWM controlled DRV8871 H-bridge
MTD2A_binary_output switch_right   ("Switch right", 500);
MTD2A_binary_output switch_left    ("Switch left",  500);
// Timer
MTD2A_timer         train_timer    ("Timer"); // Generic timer

// Driving speeds PWM {0 - 255}. 
// Voltage = actual speed devided by 255 multiplied by power supply voltage. Default 18 Volts.
const byte zeroSpeed   =   0; //   0% voltage
const byte snailSpeed  =  51; //  20% voltage
const byte slowSpeed   = 102; //  40% voltage
const byte mediumSpeed = 153; //  60% voltage
const byte fastSpeed   = 204; //  80% voltage
const byte fullSpeed   = 255; // 100% voltage

// Process control
byte stepCount = 0;


void setup() {
  Serial.begin(9600);
  while (!Serial) { delay(10); } // ESP32 Serial Monitor ready delay
  //
  IR_sensor.initialize       (A0);
  green_LED.initialize       (2);
  red_LED.initialize         (3);
  train_forward.initialize   (5); 
  train_backward.initialize  (6);
  switch_right.initialize    (9);
  switch_left.initialize    (10);
  //
  Serial.println(F("Advanced PWM controlled automatic pendulum operation. Activate sensor to start loop"));
} // setup


#if section == 1


// ------------------------------------------------------------------------------------------------


void loop() {

  switch (stepCount) {
    case 0: 
      if (IR_sensor.get_processState () == ACTIVE) {
        Serial.println(F("0: Sensor activated - Stop train. PWM curve: FALLING_B05 - Time: 1 seconds "));
        red_LED.set_pinWriteValue (HIGH);
        train_forward.activate (snailSpeed, zeroSpeed, FALLING_B05, 1000);
        stepCount = 1;
      }
    break;
    case 1:
      if (train_forward.get_processState () == COMPLETE) {
        Serial.println (F("1: Wait 3 seconds"));
        train_timer.timer (START_TIMER, 3000);
        stepCount = 2;
      }  
    break;


    // Start up from side track -----------------------------------------------------------
    case 2:
      if (train_timer.get_processState () == COMPLETE) {
        set_switch_direction_right ();
        Serial.println (F("2: Start train from the end of side track at IR sensor. PWM curve: RISING_B05 - Time: 2 seconds"));
        train_backward.activate (zeroSpeed, snailSpeed, RISING_B05, 2000); // 2 seconds
        stepCount = 3;
      }
    break;
    case 3:
      if (train_backward.get_processState () == COMPLETE) {
        Serial.println (F("3: Continue driving backwards for 9 seconds"));
        train_timer.timer (START_TIMER, 9000);
        stepCount = 4;
      }
    break;
    case 4:
      if (train_timer.get_processState () == COMPLETE) {
        Serial.println (F("4: Slow down and stop train.  PWM curve: FALLING_B05 - Time: 2 seconds"));
        train_backward.activate (snailSpeed, zeroSpeed, FALLING_B05, 2000); // 2 seconds
        stepCount = 5;
      }
    break;


    // From reverse to forward --------------------------------------------------------------
    case 5:
      if (train_backward.get_processState () == COMPLETE) {
        Serial.println (F("5: Wait 2 seconds"));
        train_timer.timer (START_TIMER, 2000);
        stepCount = 6;       
      }
    break;


    // Towards train station ----------------------------------------------------------------
    case 6:
      if (train_timer.get_processState () == COMPLETE) {
        set_switch_direction_left ();
        Serial.println (F("6: Start train and drive forward towards the train station.  PWM curve: RISING_B05 - Time: 4,5 seconds"));
        train_forward.activate (zeroSpeed, mediumSpeed, RISING_B05, 4500);
        stepCount = 7;
      }
    break;
    case 7:
      if (train_forward.get_processState () == COMPLETE) {
        Serial.println (F("7: Continue driving forwards for 7,3 seconds"));
             train_timer.timer (START_TIMER, 7300);
        stepCount = 8;
      }
    break;
    case 8:
      if (train_timer.get_processState () == COMPLETE) {
        Serial.println (F("8: Stop train at train station.  PWM curve: FALLING_B05 - Time: 5 seconds"));
        train_forward.activate (mediumSpeed, zeroSpeed, FALLING_B05, 5000);
        stepCount = 9;
      }
    break;
    

    // Train Station -------------------------------------------------------------------------
    case 9:
      if (train_forward.get_processState () == COMPLETE) {
        Serial.println (F("9: Wait 3 seconds."));
        train_timer.timer (START_TIMER, 3000);
        stepCount = 10;       
      }
    break;


    // Towards track switch -----------------------------------------------------------------
    case 10:
      if (train_timer.get_processState () == COMPLETE) {
        Serial.println (F("10: Starting up and drive towards track switch.  PWM curve: RISING_B05 - Time: 5 seconds"));
        red_LED.set_pinWriteValue (LOW);
        green_LED.set_pinWriteValue (HIGH);
        Serial.println (F("    Red LED off and Green LED on"));
        train_forward.activate (zeroSpeed, mediumSpeed, RISING_B05, 5000); 
        stepCount = 11;
      }
    break;
    case 11:
      if (train_forward.get_processState () == COMPLETE) {
        Serial.println (F("11: Continue driving forward for 7,5 seconds"));
        train_timer.timer (START_TIMER, 7500);
        stepCount = 12;
      }
    break;

    
    // Full speed on most og the railway ---------------------------------------------------
        case 12:
      if (train_timer.get_processState () == COMPLETE) {
        Serial.println (F("12: Speed up towards the train station.  PWM curve: RISING_XY - Time: 3 seconds"));
        train_forward.activate (mediumSpeed, fullSpeed, RISING_XY, 3000); 
        stepCount = 13;
      }
    break;
    case 13:
      if (train_forward.get_processState () == COMPLETE) {
        Serial.println (F("13: Continue driving full speed for 4 seconds"));
        Serial.println (F("    Green LED blink"));
        train_timer.timer (START_TIMER, 4000);
        stepCount = 14;
      }
      if (green_LED.get_processState () == COMPLETE) {
        green_LED.activate (); // blink: 0,5 on & 0,5 off
      }
    break;
    case 14:
      if (train_timer.get_processState () == COMPLETE) {
        Serial.println (F("14: Slow down from full speed.  PWM curve: FALLING_XY - Time: 3 seconds"));
        Serial.println (F("    Green LED blink"));
        train_forward.activate (fullSpeed, snailSpeed, FALLING_XY, 3000);
        stepCount = 15;
      }
      if (green_LED.get_processState () == COMPLETE) {
        green_LED.activate (); // blink: 0,5 on & 0,5 off
      }
    break;
    case 15:
      red_LED.set_pinWriteValue (HIGH);
      if (train_forward.get_processState () == COMPLETE) {
        Serial.println (F("15: Drive forward towards track switch. Time: Until sesnor stop"));
        set_switch_direction_right ();
        // The train continues to run at the last defined speed
        red_LED.set_pinWriteValue (LOW);  // No red signal
        stepCount = 0;
      }
    break;

  } // switch

  MTD2A_loop_execute();  // Update the state (event) system

} // end loop Automatic pendulum operation on model railway system.


#endif


// Helper functions ------------------------------------------------------------------------


void set_switch_direction_right () {
  Serial.println(F("   Set track switch direction to the RIGHT"));
  switch_left.set_pinWriteValue (0);
  switch_right.activate (); // Send 500 milliseconds puls
} // set_switch_direction_right


void set_switch_direction_left () {
  Serial.println(F("   Set track switch direction to the LEFT"));
  switch_right.set_pinWriteValue (0);
  switch_left.activate (); // Send 500 milliseconds puls
} // set_switch_direction_left


// ------------------------------------------------------------------------------------------------


#if section == 2


long loopCount     = 0;
const byte maxStep = 8;
const bool FORWARD = true, BACKWARD = false;

struct trainData {
  bool          direction;
  unsigned long delayMS;
  unsigned long curveMS;
  unsigned long maintainMS;
  byte          beginValue;
  byte          endValue;
  byte          PWMcurveType;
};

const trainData stepArray[maxStep] = { 
  { FORWARD,     0,  1000,  3000,  snailSpeed,   zeroSpeed, FALLING_B05 },  // 0
  // Start up from side track --------------------------------------------
  { BACKWARD,    0,  2000,  9000,   zeroSpeed,  snailSpeed,  RISING_B05 },  // 1
  { BACKWARD,    0,  2000,     0,  snailSpeed,   zeroSpeed, FALLING_B05 },  // 2
  // Towards train station -----------------------------------------------
  { FORWARD,     0,  4500,  7300,   zeroSpeed, mediumSpeed,  RISING_B05 },  // 3
  { FORWARD,     0,  5000,  3000, mediumSpeed,   zeroSpeed, FALLING_B05 },  // 4
  // Towards track switch ------------------------------------------------------
  { FORWARD,     0,  5000,  7500,   zeroSpeed, mediumSpeed,  RISING_B05 },  // 5
  // Full speed on most og the railway -----------------------------------------
  { FORWARD,     0,  3000,  4000, mediumSpeed,   fullSpeed,   RISING_XY },  // 6
  { FORWARD,     0,  3000,     0,   fullSpeed,  snailSpeed,  FALLING_XY }   // 7
};


void loop() {

  do { // Waith for sensor to activat
    MTD2A_loop_execute ();
  } while (IR_sensor.get_processState() != ACTIVE);

  for (stepCount = 0; stepCount < maxStep; stepCount++) {
    if (stepArray[stepCount].direction == FORWARD) {
      train_forward.set_timers (stepArray[stepCount].curveMS, stepArray[stepCount].delayMS, stepArray[stepCount].maintainMS);
      train_forward.activate (stepArray[stepCount].beginValue, stepArray[stepCount].endValue, stepArray[stepCount].PWMcurveType); 
      do {
        MTD2A_loop_execute ();
        loop_case ();
      } while (train_forward.get_processState() == ACTIVE);
    }
    else { // BACWARD
      train_backward.set_timers (stepArray[stepCount].curveMS, stepArray[stepCount].delayMS, stepArray[stepCount].maintainMS);
      train_backward.activate (stepArray[stepCount].beginValue, stepArray[stepCount].endValue, stepArray[stepCount].PWMcurveType);
      do {  
        MTD2A_loop_execute ();
        loop_case ();
      } while (train_backward.get_processState() == ACTIVE);
    }
  } // for loop

  MTD2A_loop_execute();  // Update the state (event) system
  loopCount = 0;
} // loop ()


void loop_case () {
  switch (loopCount) {
    case 0:
      red_LED.set_pinWriteValue (HIGH);
      set_switch_direction_right ();
    break;
    case 1700: // 16 seconds
      set_switch_direction_left ();
      break;
    case 3580: // 358 seconds
      red_LED.set_pinWriteValue (LOW);
      green_LED.set_pinWriteValue (HIGH); 
      break;
    case 5030:
      green_LED.set_loopActivate (ENABLE);
      green_LED.activate ();
      break; 
    case 5930:
      green_LED.set_loopActivate (DISABLE);
      set_switch_direction_right ();
      red_LED.set_pinWriteValue (HIGH);
      break;
  } // loop_case
  loopCount++;
}


#endif


