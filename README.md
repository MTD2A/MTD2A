# MTD2A

Work in progress

### Model Train Detection And Action - Arduino library
 * Supporting a vast variety of input sensors and output devices 
 * Simple to use to build complex solutions 
 * Non blocking, simple, yet efficient event-driven state machine
 * Comprehensive control, state and debug information

### Example

```
// Two blinking LEDs. One with symmetric interval and another with asymetric interval.

#include <MTD2A.h>

MTD2A_binary_output red_LED   ("Red LED",   400, 400);              // 0.4 sec light, 0.4 sec no light
MTD2A_binary_output green_LED ("Green LED", 300, 700, 0, PWM, 96);  // 0.3 sec light, 0.7 sec no light, PWM dimmed

void setup() {
  Serial.begin(9600);

  red_LED.initialize (9);     // Output pin 9
  green_LED.initialize (10);  // Output pin 10

  Serial.println("Two LED blink");
}

void loop() {
  if (red_LED.get_processState()   == pending)   red_LED.activate();
  if (green_LED.get_processState() == pending) green_LED.activate();

  MTD2A::loop_execute();
} 
```
### documentation 
[https://github.com/MTD2A/MTD2A/tree/main/doc](https://github.com/MTD2A/MTD2A/blob/main/doc/README.md)

<br/>

### New to eletronics and Arduino?
https://makeabilitylab.github.io/physcomp/

<br/>

# Installation til Arduino IDE

Download the entire MTD2A repository as a ZIP file.

![](/image/MTD2A-download.png)

Unzip the ZIP file and rename the directory name to MTD2A.

Move everything to the Arduino home directory under libraries: **Arduino/libraries/MTD2A**

The examples can now be selected from the list of examples in the Arduino IDE **Examples**

![](/image/Arduino-examples.png)
