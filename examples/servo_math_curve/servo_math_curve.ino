// Use PWM math curve to move server from 0 to 180 degrees
// https://docs.arduino.cc/libraries/servo/ 
// https://github.com/MTD2A/MTD2A/tree/main/doc
// Jørgen Bo Madsen / july 2025 / https://github.com/jebmdk

#include <MTD2A.h>
#include <Servo.h>
using namespace MTD2A_const;

 // 4 seconds slow movement and 1 second pause
MTD2A_binary_output boom_angel ("Boom angel", 3000, 1000);
Servo boom_servo;

// Arduino board pins
byte BOOM_SERVO_PIN = 5;   // output
bool changeFlag     = false;
long servoMicroSec  = 0;

void setup() {
  Serial.begin(9600);
  while (!Serial) { delay(10); } // ESP32 Serial Monitor ready delay

  boom_angel.set_pinWriteToggl (DISABLE);
  boom_angel.initialize ();  // pin not used

  boom_servo.attach (BOOM_SERVO_PIN);  // Attach servo to borad pin
  // SG92R standard micro servo. Range: 0 - 180 degrees
  for (byte loopIndex = 0; loopIndex < 2; loopIndex++) {
    boom_servo.write  (0);   // Left
    delay(1000);
    boom_servo.write  (180); // Right
    delay(1000);
  }
}


void loop() {
  if (boom_angel.get_processState() == COMPLETE) {
    if (changeFlag == true) {
      boom_angel.activate (180, 0, FALLING_SM5); // FALING_XY
      changeFlag = false;
    } 
    else {
      boom_angel.activate (0, 180, RISING_SM5);  // RISING XY
      changeFlag = true;
    }
  }
  else {
    // Move servo left-right-pause-right-left-pause ....
    boom_servo.write(boom_angel.get_pinOutputValue()); 
  }
 
  MTD2A_loop_execute();  // Update the state (event) system
} 

