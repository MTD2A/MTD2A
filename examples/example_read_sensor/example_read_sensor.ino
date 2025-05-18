
#include <MTD2A.h>

// https://github.com/MTD2A/FC-51

MTD2A_binary_input FC_51_left ("FC-51 left", 5000, lastTrigger, timeDelay);
// "FC-51 left" = Sensor (object) name, which is displayed together with status messages
// 5000 = Time delay in milliseconds (5 seconds)
// lastTrigger = Start calculating time from last impulse (LOW->HIGH)
// timeDelay = Use time delay (timer function)

void setup () {
  Serial.begin (9600);  // Required and first if status messages are to be displayed
  FC_51_left.initialize (2); // Arduino board pin 2.
  FC_51_left.set_debugPrint ();  // Display status messages
}

void loop () {
  // Every time the sensor is activated, the status will be printed on the Arduino Serial Monitor
  FC_51_left.loop_fast();  // Update the state (event) system
  delay (10);  // Update the class object. Typically 1-10 milliseconds
}

