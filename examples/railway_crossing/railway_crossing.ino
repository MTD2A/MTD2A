// Danish railway crossing. Read two sensors, activate red blinking warning light, start bell, lower boom. 
// Wait for the train to pass, raise boom and stop red blink.
// https://docs.arduino.cc/libraries/servo/
// https://github.com/MTD2A/MTD2A/blob/main/sounds/Bells/railroad-crossing-bell-denmark-1-sec.mp3
// Short DEMO: https://youtu.be/VaXmki2oLrk
// Jørgen Bo Madsen / july 2025 / https://github.com/jebmdk

#include <MTD2A.h>
#include <Servo.h>
using namespace MTD2A_const;

MTD2A_binary_input  FC_51_left  ("FC-51 left" , 3000); // 3 seconds LAST_TIME delay
MTD2A_binary_input  FC_51_right ("FC-51 right", 3000); // 3 seconds LAST_TIME delay
MTD2A_binary_output red_LED_1   ("Red LED 1"  ,   500, 500); // 0,5 sec on, 0,5 sec off
MTD2A_binary_output red_LED_2   ("Red LED 2"  ,   500, 500); // 0,5 sec on, 0,5 sec off
MTD2A_binary_output bell_sound  ("Bell sound" ,   100, 900); // 0,1 sec activation and wait 0,9 sec 
MTD2A_binary_output boom_angel  ("Boom angel" , 3000); // 3 seconds slow movement UP and DOWN
Servo boom_servo_1; // SG92R front
Servo boom_servo_2; // SG92R back

// Arduino board pins
byte FC_51_LEFT_PIN    = A0; // input
byte FC_51_RIGHT_PIN   = A1; // input
byte BOOM_SERVO_1_PIN  = 3;  // output
byte BOOM_SERVO_2_PIN  = 5;  // output
byte RED_LED_1_PIN     = 8;  // Two LED in parallel (220 Ohm ~ 30 mA)
byte RED_LED_2_PIN     = 9;  // Two LED in parallel (220 Ohm ~ 30 mA)
byte BELL_SOUND_PIN    = 10; // output
// Boom on servo
byte BOOM_UP     = 0;  // degrees
byte BOOM_DOWN   = 90; // degrees
// Control flags
bool leftActive  = false;
bool rightActive = false;
bool beginFlag   = false;
bool endFlag     = false;
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

  boom_angel.set_pinWriteToggl (DISABLE);
  boom_angel.initialize ();  // pin not used

  // Two SG92R standard micro servo. Range: 0 - 180 degrees
  boom_servo_1.attach (BOOM_SERVO_1_PIN);  // Attach servo 1 to board pin
  boom_servo_1.write  (BOOM_UP);  // Initialize servo position to position UP
  boom_servo_2.attach (BOOM_SERVO_2_PIN);  // Attach servo 2 to board pin
  boom_servo_2.write  (BOOM_UP);  // Initialize servo position to position UP

  delay(1000);  // Give time for servo to find UP position
}

void loop() {

  if (rightActive == false) {
    // LEFT sensor detect first car / locomotive. Set values one time.
    if (FC_51_left.get_processState() == ACTIVE  &&  FC_51_right.get_processState() == COMPLETE) {
      if (leftActive == false) {
        Serial.println ("Train coming from the LEFT!");
        leftActive = true;
        beginFlag  = true;
        endFlag    = false;
        beginCount = 0;
      }
    }
    // Last car / locomotive passede RIGHT sensor. Set values one time.
    if (leftActive == true  && FC_51_right.get_phaseChange() == true && FC_51_right.get_phaseNumber() == COMPLETE_PHASE) {
      Serial.println ("Train from the LEFT has passed");
      beginFlag = false;
      endFlag   = true;
      endCount  = 0;
    }
  } // rightActive == false


  if (leftActive == false) {
    // RIGHT sensor detect first car / locomotive. Set values one time.
    if (FC_51_right.get_processState() == ACTIVE  &&  FC_51_left.get_processState() == COMPLETE) {
      if (rightActive == false) {
        Serial.println ("Train coming from the RIGHT");
        rightActive = true;
        beginFlag   = true;
        endFlag     = false;
        beginCount  = 0;
      }
    }
    // Last car / locomotive passede LEFT sensor. Set values one time.
    if (rightActive == true  && FC_51_left.get_phaseChange() == true && FC_51_left.get_phaseNumber() == COMPLETE_PHASE) {
      Serial.println ("Train from the RIGHT has passed");
      beginFlag = false;
      endFlag   = true;
      endCount  = 0;
    }
  } // leftActive == false


  if (beginFlag == true) {  // Begin phase
    // Default MTD2A loop time step is 10 milliseconds.
    // Red LED blink every second
    if (beginCount >= 80) { // compensate for the MP3 player's slow response to start
      red_LED_1.activate();
      red_LED_2.activate();
    }
    // play sound for 6 seconds and stop when boom is down
    if (beginCount <= 600) { // 600 * 10 = 6000 Milliseconds or 6 seconds.
      bell_sound.activate ();
    }
    // wait 3 seconds and raise the boom over a period of 3 seconds
    if (beginCount == 300) {
      boom_angel.activate (BOOM_UP, BOOM_DOWN, RISING_XY); // Alternatively: RISING_SM8
    }
    if (boom_angel.get_processState() == ACTIVE) {
      boom_servo_1.write( boom_angel.get_pinOutputValue() ); 
      boom_servo_2.write( boom_angel.get_pinOutputValue() ); 
    }
    // Check if the train has been removed or if the "begin" sensor has been activated by mistake
    if (beginCount == 30000) { // 5 minutes delay
      beginFlag = false;
      endFlag   = true;
      endCount  = 0;
    }
    beginCount++; // 10 Millisecond step
  } // beginFlag == true


if (endFlag == true) { // End phase
  // Default MTD2A loop time step is 10 milliseconds.
  // Red LED blink until boom is up
  if (endCount <= 300) {  // 300 * 10 = 3000 Milliseconds or 3 seconds.
    red_LED_1.activate();
    red_LED_2.activate();
  }
  // Lower the boom over a period of 3 seconds
  if (endCount == 0) {  // start immediately
    boom_angel.activate (BOOM_DOWN, BOOM_UP, FALLING_XY); //  Alternatively: FALLING_SM8
  }
  if (boom_angel.get_processState() == ACTIVE) {
    boom_servo_1.write( boom_angel.get_pinOutputValue() ); 
    boom_servo_2.write( boom_angel.get_pinOutputValue() ); 
  }
  // Finish end phase and redy to start
  if (endCount == 300) {
    endFlag     = false;
    leftActive  = false;
    rightActive = false;
  }
  endCount++; // 10 Millisecond step
} // endFlag == true

  MTD2A_loop_execute();  // Update the state (event) system
} // Realway crossing
