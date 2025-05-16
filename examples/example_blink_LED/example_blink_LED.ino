// example_blink_LED.ino ----------------------------------------------------------------------------- 

// Two blinking LEDs. One with symmetric interval and another with asymetric interval.

#include <MTD2A.h>

MTD2A_binary_output red_LED   ("Red LED",   400, 400);  // 0.4 sec light, 0.4 sec no light
MTD2A_binary_output green_LED ("Green LED", 300, 700);  // 0.3 sec light, 0.7 sec no light

void setup() {
  Serial.begin(250000);

  red_LED.initialize (9);     // Output pin 9
  green_LED.initialize (10);  // Output pin 10

  Serial.println("Two LED blink");
}

void loop() {
  if (red_LED.get_processState()   == pending)   red_LED.activate();
  if (green_LED.get_processState() == pending) green_LED.activate();

  red_LED.loop_fast();
  green_LED.loop_fast();
  
  delay (10);
} 

