
// eksample_switch_direction.ino ------------------------------------------------------------------------ 

// Automatic switch to the direction the train is coming from (to avoid derailment if the switch direction is wrong)
// Standard LGB switch (turnout) with position switches for feedback is used

#include <MTD2A.h>

// Track sensor pin input
MTD2A_binary_input FC_51_left     ("Left track sensor",  5000, lastTrigger, timeDelay);  // 5 seconds
MTD2A_binary_input FC_51_right    ("Right track sensor", 5000, lastTrigger, timeDelay);  // 5 seconds
// read current switch position 
MTD2A_binary_input position_left  ("Left feedback position");
MTD2A_binary_input position_right ("Right feedback position");
// Send 0,5 seconds pulse to switch motor
MTD2A_binary_output motor_left     ("Left switch motor",  500);  // 0,5 seconds
MTD2A_binary_output motor_right    ("Right switch motor", 500);  // 0,5 seconds

bool trackShiftState = false;

void setup() {
  Serial.begin(250000);
  MTD2A::set_allDebugPrint  (enable); // Enable debug print for all instantiated classes
  FC_51_left.initialize     (2);   // Sensor detection moving train left
  FC_51_right.initialize    (3);   // Sensor detection moving train right
  position_left.initialize  (4);   // Read switch position left
  position_right.initialize (5);   // Read switch position right
  motor_left.initialize     (9);   // Output rail motor switch left
  motor_right.initialize    (10);  // Output rail motor switch left
}

void loop() {
  if (FC_51_left.get_processState() == active) {
    if (position_right.get_processState() == active &&  motor_left.get_phaseNumber() == firstTimePhase) {
      trackShiftState = true;
      motor_left.activate(); // Activate track switch motor
      Serial.println("Move track switch to the left");
    }
  }
  else {
    if (trackShiftState == true  &&  motor_left.get_phaseNumber() == pendingPhase) {
      motor_right.activate(); // Activate track switch motor
      Serial.println("Move track switch back to the right");
      trackShiftState = false;  // last command in the process
    }
  }

  // Track Switch to the right is identical to Track Switch on the left, just mirrored. Not shown here.

  FC_51_left.loop_fast     ();
  FC_51_right.loop_fast    ();
  position_left.loop_fast  ();
  position_right.loop_fast ();
  motor_left.loop_fast     ();
  motor_right.loop_fast    ();

  delay (10);
} // Automatic track switch to the direction the train is coming from
