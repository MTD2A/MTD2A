
// Stop light and sound message: The train brakes and temporarily stops at a red light.
// Jørgen Bo Madsen / may 2025 / https://github.com/jebmdk

#include <MTD2A.h>

MTD2A_binary_input  FC_51_stop  ("Stop",  10000, lastTrigger, timeDelay);   // continous active time (no drop outs)
MTD2A_binary_output train_sound ("Sound", 500, 2000);  // Wait 2 seconds and activate 0,5 second
// Trigger sound board 2 seconds after train detection for duraten of 0,5 seconds

bool trainSoundState = false;

void setup() {
  Serial.begin(9600);
  MTD2A::set_allDebugPrint(enable); // Enable debug print for all instantiated classes
  FC_51_stop.initialize   (2);  // input pin 2
  train_sound.initialize  (9);  // Output pin 9 Danish and English speech from https://ttsmaker.com/
}

void loop() {
  if (FC_51_stop.get_processState() == active ) { // Continous activation (no drop outs)
    if (FC_51_stop.get_phaseChange() == true &&  FC_51_stop.get_phaseNumber() == firstTimePhase)
    // Equivalent of: if (FC_51_stop + FC_51_stop)
      train_sound.activate();
  }

  MTD2A::loop_execute();  // Update the state (event) system
} // Stop light and sound message: The train brakes and temporarily stops at a red light.
