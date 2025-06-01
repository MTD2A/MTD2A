### Model Train Detection And Action - NEW Arduino library

Work in progress

MTD2A is a collection of advanced and functional C++ classes - building blocks - for time-controlled handling of input and output. The library is intended for Arduino enthusiasts without much programming experience, who are interested in electronics control and automation, and model trains as a hobby.

### Common to all building blocks are:
• They support a wide range of input sensors and output devices <br/>
• Are simple to use to build complex solutions with few commands <br/>
• They operate non-blocking, process-oriented and state-driven <br/>
• Offers extensive control and troubleshooting information <br/>
• Thoroughly documented with examples <br>

### Example

```
// Two blinking LEDs. One with symmetric interval and another with asymetric interval.

#include <MTD2A.h>
using namespace MTD2A_const;

MTD2A_binary_output red_LED   ("Red LED",   400, 400);              // 0.4 sec light, 0.4 sec no light
MTD2A_binary_output green_LED ("Green LED", 300, 700, 0, PWM, 96);  // 0.3 sec light, 0.7 sec no light, PWM dimmed

void setup() {
  Serial.begin(9600);

  red_LED.initialize (9);     // Output pin 9
  green_LED.initialize (10);  // Output pin 10

  Serial.println("Two LED blink");
}

void loop() {
  if (red_LED.get_processState()   == PENDING)   red_LED.activate();
  if (green_LED.get_processState() == PENDING) green_LED.activate();

  MTD2A::loop_execute();
}
```
### To do
* class MTD2A_delay
* class MTD2A_astable
* class MTD2A_flip_flop
* class MTD2A_tone
* class MTD2A_sound
* class MTD2A_servo
* class MTD2A_stepper
* class MTD2A_display
* class MTD2A_ultrasonic
* class MTD2A_laser
* class MTD2A_IR_ranging
* class MTD2A_DCC_input

### documentation 
[https://github.com/MTD2A/MTD2A/tree/main/doc](https://github.com/MTD2A/MTD2A/blob/main/doc/README.md)

### New to eletronics and Arduino?
https://makeabilitylab.github.io/physcomp/

# Installation for Arduino IDE
Download the entire MTD2A repository as a ZIP file.

![](/image/MTD2A-download.png)

Unzip the ZIP file and rename the directory name to MTD2A. <br/>
Move everything to the Arduino home directory under libraries: **Arduino/libraries/MTD2A** <br/>
The examples can now be selected from the list of examples in the Arduino IDE **Examples** <br/>

![](/image/Arduino-examples.png)
