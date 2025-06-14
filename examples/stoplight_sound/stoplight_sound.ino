
// Stop light and sound message: The train brakes and temporarily stops at a red light.
// Speaker announcement is broadcast to the passengers. If the train is passing by, no announcement is broadcast.
// Jørgen Bo Madsen / may 2025 / https://github.com/jebmdk

#include <MTD2A.h>
using namespace MTD2A_const;

MTD2A_binary_input  FC_51_stop  ("Stop train",  2000);   
// continuous active time. No incorrect reactivation due to gaps between the train wagons.
MTD2A_binary_output train_sound ("Sound", 100, 0, 15000); 
// Trigger "DFplayer mini" "Buton activation" method for duraten of 0,1 seconds. Not less - not more.
// Wait 15 seconds i total before next announcement. Loop until train has moved on.
unsigned long timerStart = 0;     // Determine if the train is stationary or is passing by the stop light.
unsigned long DelayTime  = 8000;  // Check if the train is stationary 8 seconds or moere after detection.

void setup() {
  Serial.begin(9600);
  byte FC_51_STOP_PIN  = A0; // Arduino board input analog and digtal pin A0
  byte TRAIN_SOUND_PIN = 2;  // Arduino board output pin 2
  FC_51_stop.initialize  (FC_51_STOP_PIN);
  train_sound.initialize (TRAIN_SOUND_PIN, INVERTED); // trigger = HIGH to LOW
  // Danish and English speech from https://ttsmaker.com/
}

void loop() {
  if (FC_51_stop.get_processState() == ACTIVE ) { 
    if (FC_51_stop.get_phaseChange() == true && FC_51_stop.get_phaseNumber() == FIRST_TIME_PHASE) {
      timerStart = millis();
    }
    if (millis() - timerStart > DelayTime) {
      train_sound.activate();  // Speaker announcement is broadcast to the passengers. Looping.
    }
  }  // Active

  MTD2A_loop_execute();  // Update the state (event) system
} // Stop light and sound message: The train brakes and temporarily stops at a red light.
