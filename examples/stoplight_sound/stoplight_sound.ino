
// Stop light and sound message: The train brakes and temporarily stops at a red light.
// Jørgen Bo Madsen / may 2025 / https://github.com/jebmdk

#include <MTD2A.h>
using namespace MTD2A_const;

MTD2A_binary_input  FC_51_stop  ("Stop train",  10000);   
// continuous active time. No incorrect reactivation due to gaps between the wagons.
MTD2A_binary_output train_sound ("Sound", 100, 5000);  // Wait 5 seconds and activate 0,1 second
// Trigger sound board - non-blocking - 5 seconds after train detection for duraten of 0,1 seconds

void setup() {
  Serial.begin(9600);
  MTD2A_globalDebugPrint(); // Enable debug print for all instantiated classes

  byte FC_51_STOP_PIN  = 2;   // Arduino board input pin 2
  byte TRAIN_SOUND_PIN = A0;  // Arduino board Output pin Analog and digtal pin A0 
  FC_51_stop.initialize  (FC_51_STOP_PIN);
  train_sound.initialize (TRAIN_SOUND_PIN, INVERTED);
  // Danish and English speech from https://ttsmaker.com/
}

void loop() {
  if (FC_51_stop.get_processState() == ACTIVE ) { // Continous activation (no drop outs)
    if (FC_51_stop.get_phaseChange() == true &&  FC_51_stop.get_phaseNumber() == FIRST_TIME_PHASE) {
      train_sound.activate();
    }
  }

  MTD2A_loop_execute();  // Update the state (event) system
} // Stop light and sound message: The train brakes and temporarily stops at a red light.
