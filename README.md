# MTD2A

### Model Train Detection And Action - Arduino library
 * Supporting a vast variety of input sensors and output devices 
 * Simple to use to build complex solutions 
 * Non blocking, simple, yet efficient event-driven state machine

### Example

```
// Blinking LED with 0,5 second interval

#include <MTD2A.h>

MTD2A_binary_output red_LED ("Red LED", 500, 500);  // 0.5 sec light, 0.5 sec no light

void setup() {
  Serial.begin(250000);
  red_LED.set_debugPrint (enable); // Enable debug print for instantiated class
  red_LED.initialize (9);  // Output pin 9 and loop process
  Serial.println("LED blink");
}

void loop() {
  if (red_LED.get_processState() == pending) red_LED.activate();
  red_LED.loop_fast();
  delay (10);
}   

```

### Work in proggress
