// example_blink_LED.ino ----------------------------------------------------------------------------- 

// Blinking LED with 1 second interval

#include <MTD2A.h>

MTD2A_binary_output red_LED ("Red LED", 500, 500);  // 0.5 sec light, 0.5 sec no light

void setup() {
  Serial.begin(250000);
  red_LED.set_debugPrint (enable); // Enable debug print for all instantiated classes
  red_LED.initialize (9);  // Output pin 9 and loop process
  Serial.println("LED blink");
}

void loop() {
  if (red_LED.get_processState() == pending) red_LED.activate();
  red_LED.loop_fast();
  delay (10);
} // Blinking LED with 1 second interval
