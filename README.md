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
#include <MTD2A.h>
using namespace MTD2A_const;

MTD2A_binary_input  FC_51_stop  ("Stop",  10000);   // continous active time (no reactivation)
MTD2A_binary_output train_sound ("Sound", 100, 3000);  // Wait 3 seconds and activate 0,1 second
// Trigger sound board 2 seconds after train detection for duraten of 0,1 seconds

void setup() {
  Serial.begin(9600);
  MTD2A::set_globalDebugPrint(); // Enable debug print for all instantiated classes

  byte FC_51_STOP_PIN  = 2;  // Arduino board input pin 2
  byte TRAIN_SOUND_PIN = 9;  // Arduino board Output pin 9 
  FC_51_stop.initialize  (FC_51_STOP_PIN);
  train_sound.initialize (TRAIN_SOUND_PIN);
  // Danish and English speech from https://ttsmaker.com/
}

void loop() {
  if (FC_51_stop.get_processState() == ACTIVE ) { // Continous activation (no drop outs)
    if (FC_51_stop.get_phaseChange() == true &&  FC_51_stop.get_phaseNumber() == FIRST_TIME_PHASE)
    // Equivalent of: if (FC_51_stop + FC_51_stop)
      train_sound.activate();
  }

  MTD2A::loop_execute();  // Update the state (event) system
} // Stop light and sound message: The train brakes and temporarily stops at a red light.
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
