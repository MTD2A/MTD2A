// Two blinking LEDs. One with symmetric interval and another with asymetric interval.
// Jørgen Bo Madsen / may 2025 / https://github.com/jebmdk

#include <MTD2A.h>
using namespace MTD2A_const;

MTD2A_binary_output red_LED   ("Red LED",   400, 400);  // 0.4 sec light, 0.4 sec no light
MTD2A_binary_output green_LED ("Green LED", 300, 700, 0, PWM, 96);  // 0.3 sec light, 0.7 sec no light, PWM dimmed

void setup() {
  Serial.begin(9600);

  red_LED.initialize   (9);   // Output pin 9
  green_LED.initialize (10);  // Output pin 10

  Serial.println("Two LED blink");
}

void loop() {
  if (red_LED.get_processState()   == COMPLETE)   red_LED.activate();
  if (green_LED.get_processState() == COMPLETE) green_LED.activate();

  MTD2A::loop_execute();
} // Two blinking LEDs. One with symmetric interval and another with asymetric interval.

