// Test and compare lineary and power PWM curves with heavy train load and light train load
// Jørgen Bo Madsen / november 2025 / https://github.com/jebmdk

#include <MTD2A.h>
using namespace MTD2A_const;

MTD2A_binary_output train_forward  ("Train forward");
MTD2A_binary_output train_backward ("Train backward");
MTD2A_binary_output red_LED        ("Red LED signal");
MTD2A_binary_output green_LED      ("Geen LED signal");
MTD2A_timer         train_timer    ("Timer");

int  stepCount   =    0;
int  AccelTime   =  1500; //  1,5 second
int  driveTime   =  1500; //  1,5 second
int  bacwardTime = 11000; //   10 seconds
byte zeroSpeed   =     0; //   0% voltage
byte snailSpeed  =    85; //  33% voltage
byte fullSpeed   =   255; // 100% voltage

void setup() {
  Serial.begin(9600);
  while (!Serial) { delay(10); } // ESP32 Serial Monitor ready delay
  //
  green_LED.initialize       (2);
  red_LED.initialize         (3);
  train_forward.initialize   (5); 
  train_backward.initialize  (6);
  red_LED.set_pinWriteValue (HIGH);
  Serial.println("Test acceleration comparison: Light train og heavy train");
} // setup

void loop() {
  switch (stepCount) {
    case 0:
      if (train_backward.get_processState() == COMPLETE) {
        Serial.println("Enter test case no [1..4]: ");
        if (Serial.available () > 0) {
          stepCount = Serial.read() - 48; // 1 = ASCII 49
          Serial.read(); // CR = 13
          if (stepCount >= 1  &&  stepCount <=4) {
            Serial.print  ("Testing case no: "); Serial.println(stepCount);
            Serial.println("Get ready - camera on - 5 seconds!");
            delay(5000);
            red_LED.set_pinWriteValue (LOW);
            green_LED.set_pinWriteValue (HIGH);
          }
          else {
            stepCount = 0;
          }
        }
        delay(1000);
      }
    break;
    case 1:
      Serial.println("NO_CURVE");
      train_forward.activate (fullSpeed, zeroSpeed, NO_CURVE, AccelTime);
      stepCount = 0;
    break;
    case 2:
      Serial.println("RISING_XY");
      train_forward.activate (zeroSpeed, fullSpeed, RISING_XY, AccelTime);
      stepCount = 10;
    case 3:
      Serial.println("RISING_B05");
      train_forward.activate (zeroSpeed, fullSpeed, RISING_B05, AccelTime);
      stepCount = 10;
    break;
    case 4:
      Serial.println("RISING_B025");
      train_forward.activate (zeroSpeed, fullSpeed, RISING_B025, AccelTime);
      stepCount = 10;
    break;
    //
    case 10:
      if (train_forward.get_processState() == COMPLETE) {
        green_LED.set_pinWriteValue (LOW);
        red_LED.set_pinWriteValue (HIGH);
        Serial.println("Continue driving");
        train_timer.timer (START_TIMER, driveTime);
        stepCount = 11;
      }
    break;
    case 11:
      if (train_timer.get_processState () == COMPLETE) {
        Serial.println("FALLING_XY");
        train_forward.activate (fullSpeed, zeroSpeed, FALLING_XY, AccelTime);
        stepCount = 12;
      }
    break;
    case 12:
      if (train_forward.get_processState() == COMPLETE) {
        Serial.println("Driving backwards slowly...");
        train_backward.activate (snailSpeed, zeroSpeed, NO_CURVE, bacwardTime);
        stepCount = 0;
      }
    break;
  }

  MTD2A_loop_execute();  // Update the state (event) system

} // test.
