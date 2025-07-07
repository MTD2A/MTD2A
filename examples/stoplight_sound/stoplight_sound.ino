
// Stop light and sound message: The train brakes and temporarily stops at a red light.
// Speaker announcement is broadcast to the passengers. If the train is passing by, no announcement is broadcast.
// Jørgen Bo Madsen / june 2025 / https://github.com/jebmdk  Sensor: https://github.com/MTD2A/FC-51

#include <MTD2A.h>
using namespace MTD2A_const;

MTD2A_binary_input  FC_51_stop  ("Stop train",  2000);   
// continuous active time. No incorrect reactivation due to gaps between the train wagons.
MTD2A_binary_output train_sound ("Sound", 100, 8000, 8000); 
// Trigger "DFplayer mini" "Buton activation" method for duraten of 0,1 seconds. Not less - not more.
// Check if the train is stationary for 8 seconds or more after detection. waith 8 seconds to complete.

void setup() {
  Serial.begin(9600);
  while (!Serial) { delay(10); } // ESP32 Serial Monitor ready delay
  byte FC_51_STOP_PIN  = A0; // Arduino board input analog and digtal pin A0
  byte TRAIN_SOUND_PIN = 2;  // Arduino board output pin 2
  FC_51_stop.initialize  (FC_51_STOP_PIN);
  train_sound.initialize (TRAIN_SOUND_PIN, INVERTED); // trigger = HIGH to LOW
}

void loop() {
  if (FC_51_stop.get_processState() == ACTIVE ) { 
    train_sound.activate();  // Speaker announcement is broadcast to the passengers. Looping.
  }
  else {
    train_sound.reset();  // Danish and English speech from https://ttsmaker.com/
  }

  MTD2A_loop_execute();  // Update the state (event) system
} // Stop light and sound message: The train brakes and temporarily stops at a red light.
