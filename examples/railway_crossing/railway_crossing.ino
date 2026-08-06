// Danish railway crossing. Read two sensors, activate red blinking warning light, start bell, lower boom. 
// Wait for the train to pass, raise boom and stop red blink.
// Examples are written for the Arduino Nano pinout (Nano family, Uno, Mega)
// https://docs.arduino.cc/libraries/servo/
// https://github.com/MTD2A/MTD2A/blob/main/sounds/Bells/railroad-crossing-bell-denmark-1-sec.mp3
// Short DEMO: https://youtu.be/VaXmki2oLrk
// Jørgen Bo Madsen / Updated august 2026 / https://github.com/jebmdk

#include <MTD2A.h>
#if defined(ESP32)
  #include <ESP32Servo.h>
#else
  #include <Servo.h>
#endif
using namespace MTD2A_const;


// choose between hard (binary) blink and soft (like an incandescent bulb) blink using PWM
bool softBlink = true; // false = hard (normal) blink


MTD2A_binary_input  FC_51_left  ("FC-51 left" , 3000); // 3 seconds LAST_TIME delay
MTD2A_binary_input  FC_51_right ("FC-51 right", 3000); // 3 seconds LAST_TIME delay
MTD2A_binary_output red_LED_1   ("Red LED 1"  , 500, 500); // 0,5 sec on, 0,5 sec off
MTD2A_binary_output red_LED_2   ("Red LED 2"  , 500, 500); // 0,5 sec on, 0,5 sec off
MTD2A_binary_output bell_sound  ("Bell sound" , 100, 900); // 0,1 sec activation and wait 0,9 sec 
MTD2A_binary_output boom_angel  ("Boom angel" , 3000); // 3 seconds slow movement UP and DOWN
Servo boom_servo_1; // SG92R front
Servo boom_servo_2; // SG92R back

// Arduino board pins
byte FC_51_LEFT_PIN    = 14; // A0 input
byte FC_51_RIGHT_PIN   = 15; // A1 input
byte BOOM_SERVO_1_PIN  =  3; // output
byte BOOM_SERVO_2_PIN  =  5; // output
byte RED_LED_1_PIN     =  6; // Two LED in parallel (220 Ohm ~ 30 mA)
//byte RED_LED_2_PIN     =  9; // Two LED in parallel (220 Ohm ~ 30 mA)
byte BELL_SOUND_PIN    = 10; // output
// Servo is disabling PWM on PIN 9 and 10 (timer 1) on NANO.
// Change wiring from PIN 9 to 11 when using soft blink. 
byte RED_LED_2_PIN     =  11; 
// Boom on servo
byte BOOM_UP     = 0;  // degrees
byte BOOM_DOWN   = 90; // degrees
// Control flags
bool leftActive  = false;
bool rightActive = false;
bool beginFlag   = false;
bool endFlag     = false;
bool softStop    = false;
int  softStep    = 0;
int  waitStep    = 0;

// Time counters
long beginCount  = 0; // Default MTD2A loop time step is 10 milliseconds
long endCount    = 0; // Default MTD2A loop time step is 10 milliseconds

void setup() {
  Serial.begin(9600);
  while (!Serial) { delay(10); } // ESP32 Serial Monitor ready delay

  FC_51_left.initialize  (FC_51_LEFT_PIN);
  FC_51_right.initialize (FC_51_RIGHT_PIN);
  red_LED_1.initialize   (RED_LED_1_PIN);
  red_LED_2.initialize   (RED_LED_2_PIN);
  bell_sound.initialize  (BELL_SOUND_PIN, INVERTED);

  boom_angel.initialize ();  // pin not used

  // Two SG92R standard micro servo. Range: 0 - 180 degrees
  boom_servo_1.attach (BOOM_SERVO_1_PIN);  // Attach servo 1 to board pin
  boom_servo_1.write  (BOOM_UP);  // Initialize servo position to position UP
  boom_servo_2.attach (BOOM_SERVO_2_PIN);  // Attach servo 2 to board pin
  boom_servo_2.write  (BOOM_UP);  // Initialize servo position to position UP

  delay(1000);  // Give time for servo to find UP position
}


void loop() {
  detect_train_direction ();
  begin_process ();
  end_process ();
  soft_blink ();

  MTD2A_loop_execute();  // Update the state (event) system
} // Realway crossing


void detect_train_direction () {
  if (rightActive == false) {
    // LEFT sensor detect first car / locomotive. Set values one time.
    if (FC_51_left.get_processState() == ACTIVE  &&  FC_51_right.get_processState() == COMPLETE) {
      if (leftActive == false) {
        Serial.println (F("Train coming from the LEFT!"));
        leftActive = true;
        beginFlag  = true;
        endFlag    = false;
        beginCount = 0;
      }
    }
    // Last car / locomotive passede RIGHT sensor. Set values one time.
    if (leftActive == true  && FC_51_right.get_phaseChange() == true && FC_51_right.get_phaseNumber() == COMPLETE_PHASE) {
      Serial.println (F("Train from the LEFT has passed"));
      beginFlag = false;
      endFlag   = true;
      endCount  = 0;
    }
  } // rightActive == false


  if (leftActive == false) {
    // RIGHT sensor detect first car / locomotive. Set values one time.
    if (FC_51_right.get_processState() == ACTIVE  &&  FC_51_left.get_processState() == COMPLETE) {
      if (rightActive == false) {
        Serial.println (F("Train coming from the RIGHT"));
        rightActive = true;
        beginFlag   = true;
        endFlag     = false;
        beginCount  = 0;
      }
    }
    // Last car / locomotive passede LEFT sensor. Set values one time.
    if (rightActive == true  && FC_51_left.get_phaseChange() == true && FC_51_left.get_phaseNumber() == COMPLETE_PHASE) {
      Serial.println (F("Train from the RIGHT has passed"));
      beginFlag = false;
      endFlag   = true;
      endCount  = 0;
    }
  } // leftActive == false
} // detect_train_direction


void begin_process () {
  if (beginFlag == true) {  // Begin phase
    if (beginCount == 0) {
      Serial.println (F("Enable repeating bell sound"));
      bell_sound.set_loopActivate (ENABLE);
      bell_sound.activate ();
    }
    // Red LED blink every second
    if (beginCount == 100) { // synchronize with the MP3 player
      soft_or_hard_blink (ENABLE);
    }
    // wait 3 seconds and raise the boom over a period of 3 seconds
    if (beginCount == 300) {
      Serial.println (F("Lower the boom slowly"));
      boom_angel.activate (BOOM_UP, BOOM_DOWN, RISING_XY); // Rising due to opposite direction (0 -> 90)
    }
    if (boom_angel.get_outputState () == true) {
      boom_servo_1.write( boom_angel.get_pinOutputValue() ); 
      boom_servo_2.write( boom_angel.get_pinOutputValue() ); 
    }
    // play sound for 6 seconds and stop when boom is down
    if (beginCount == 600) { // 600 * 10 = 6000 Milliseconds or 6 seconds.
      bell_sound.set_loopActivate (DISABLE);
    }
    // Check if the train has been removed or if the "begin" sensor has been activated by mistake
    if (beginCount == 30000) { // 5 minutes delay
      Serial.println (F("ERROR! End sensor has not detected a train"));
      beginFlag = false;
      endFlag   = true;
      endCount  = 0;
    }
    beginCount++; // 10 Millisecond step
  } // beginFlag == true
} // begin_process


void end_process () {
  if (endFlag == true) { // End phase
    // Raise the boom over a period of 3 seconds
    if (endCount == 0) {  // start immediately
      boom_angel.activate (BOOM_DOWN, BOOM_UP, FALLING_XY); //  Falling due to opposite direction (90 -> 0)
    }
    if (boom_angel.get_outputState () == true) {
      boom_servo_1.write( boom_angel.get_pinOutputValue() ); 
      boom_servo_2.write( boom_angel.get_pinOutputValue() ); 
    }
    // Finish end phase and ready to start
    if (endCount == 300) {
      soft_or_hard_blink (DISABLE);
      endFlag     = false;
      leftActive  = false;
      rightActive = false;
    }
    endCount++; // 10 Millisecond step
  } // endFlag == true
} // end_process


void soft_or_hard_blink (bool enableOrDisable) {
  if (enableOrDisable == ENABLE) {
    Serial.print (F("Start RED blink: "));
    if (softBlink == true) {
      Serial.println (F("SOFT"));
      softStep = 1;
      softStop = false;
    }
    else {
      Serial.println (F("HARD"));
      red_LED_1.set_loopActivate (ENABLE);
      red_LED_2.set_loopActivate (ENABLE);
      red_LED_1.activate();
      red_LED_2.activate();
    }
  }
  else {
    if (softBlink == true) {
      softStop = true;
    }
    else {
      red_LED_1.set_loopActivate (DISABLE);
      red_LED_2.set_loopActivate (DISABLE);
    }
  }
} // soft_or_hard_blink


void soft_blink () {
 switch (softStep) {
    case 0:  break;
    case 1:
        red_LED_2.set_timers (200, 0, 0); // Output, begin, end
        red_LED_2.activate(MIN_PWM_VALUE, MAX_PWM_VALUE, RISING_LED);
        softStep = 2; 
        waitStep = 0;
    case 2:
      if (waitStep == 45) { // wait 450 milliseconds starting from activation (globalDelayTimeMS = DELAY_10MS * 45)
        softStep = 3;
      }
      waitStep++;
      break;
    case 3:
      red_LED_2.set_timers (200, 0, 0); // Output, begin, end
      red_LED_2.activate(MAX_PWM_VALUE, MIN_PWM_VALUE, FALLING_LED);
      softStep = 4;
      waitStep = 0;
      break;
    case 4:
      if (waitStep == 55) { // wait 550 milliseconds starting from activation (globalDelayTimeMS = DELAY_10MS * 55)
        softStep = (softStop) ? 0 : 1;
      }
      waitStep++;
      break;
  }
} // Soft blink
