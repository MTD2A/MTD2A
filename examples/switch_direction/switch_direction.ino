
// Automatic switch to the direction the train is coming from (to avoid derailment)
// Standard LGB switch (turnout) with position switches for feedback 
// Jørgen Bo Madsen / june 2025 / https://github.com/jebmdk  Sensor: https://github.com/MTD2A/FC-51

#include <MTD2A.h>
using namespace MTD2A_const;

// Track sensor pin input
MTD2A_binary_input FC_51_left      ("Left track sensor",  3000);  // 3 seconds
MTD2A_binary_input FC_51_right     ("Right track sensor", 3000);  // 3 seconds
// read current switch position 
MTD2A_binary_input position_left   ("Left feedback position");
MTD2A_binary_input position_right  ("Right feedback position");
// Send 0,5 seconds pulse to switch actuator
MTD2A_binary_output actuator_power ("Power switch actuator", 500);  // 0,5 seconds
MTD2A_binary_output actuator_right ("Right switch actuator", 500);  // Relay polarity right/left

bool trackShiftState = false;

void setup() {
  Serial.begin(9600);
  MTD2A_globalDebugPrint     ();    // Enable debug print for all instantiated classes
  FC_51_left.initialize      (A0);  // Pin input no: Sensor detection moving train left
  FC_51_right.initialize     (A1);  // Pin input no: Sensor detection moving train right
  position_left.initialize   (A3);  // Pin input no: Read switch position left
  position_right.initialize  (A4);  // Pin input no: Read switch position right
  actuator_power.initialize  (9,  INVERTED);  // Pin output no: Relay power on rail switch actuator
  actuator_right.initialize  (10, INVERTED);  // Pin output no: Relay output rail switch actuator right
}

void loop() {
  
  if (FC_51_left.get_processState() == ACTIVE) {
    if (FC_51_left.get_phaseChange() == true  &&  position_left.get_processState() == ACTIVE) {
      trackShiftState = true;
      actuator_right.activate(); // Activate polarity relay: Track switch actuator pulse
      actuator_power.activate(); // Activate relay: Switch actuator power pulse
      Serial.println("Move track switch to the right");
    }
  }
  else {
    if (trackShiftState == true) {
      // Default polarity relay resting state is left
      actuator_power.activate(); // Activate relay: Switch actuator power pulse
      Serial.println("Move track switch back to the left");
      trackShiftState = false;  // last command in the process
    }
  }
  // Mirror fromabove
  if (FC_51_right.get_processState() == ACTIVE) {
    if (FC_51_right.get_phaseChange() == true  &&  position_right.get_processState() == ACTIVE) {
      trackShiftState = true;
      // Default relay resting state is left
      actuator_power.activate(); // Activate relay: Switch actuator power pulse
      Serial.println("Move track switch to the left");
    }
  }
  else {
    if (trackShiftState == true) {
      actuator_right.activate(); // Activate polarity relay: Track switch actuator pulse
      actuator_power.activate(); // Activate relay: Switch actuator power pulse
      Serial.println("Move track switch back to the right");
      trackShiftState = false;  // last command in the process
    }
  }

  MTD2A_loop_execute();  // Update the state (event) system
} // Automatic track switch to the direction the train is coming from
