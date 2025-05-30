
// Automatic switch to the direction the train is coming from (to avoid derailment if the switch direction is wrong)
// Standard LGB switch (turnout) with position switches for feedback is used
// Jørgen Bo Madsen / may 2025 / https://github.com/jebmdk

#include <MTD2A.h>
using namespace MTD2A_const;

// Track sensor pin input
MTD2A_binary_input FC_51_left     ("Left track sensor",  5000, LAST_TRIGGER, TIME_DELAY);  // 5 seconds
MTD2A_binary_input FC_51_right    ("Right track sensor", 5000, LAST_TRIGGER, TIME_DELAY);  // 5 seconds
// read current switch position 
MTD2A_binary_input position_left  ("Left feedback position");
MTD2A_binary_input position_right ("Right feedback position");
// Send 0,5 seconds pulse to switch motor
MTD2A_binary_output motor_left     ("Left switch motor",  500);  // 0,5 seconds
MTD2A_binary_output motor_right    ("Right switch motor", 500);  // 0,5 seconds

bool trackShiftState = false;

void setup() {
  Serial.begin(9600);
  MTD2A::set_globalDebugPrint  ();    // Enable debug print for all instantiated classes
  FC_51_left.initialize        (2);   // Pin input no: Sensor detection moving train left
  FC_51_right.initialize       (3);   // Pin input no: Sensor detection moving train right
  position_left.initialize     (4);   // Pin input no: Read switch position left
  position_right.initialize    (5);   // Pin input no: Read switch position right
  motor_left.initialize        (9);   // Pin output no: Output rail motor switch left
  motor_right.initialize       (10);  // Pin output no: Output rail motor switch left
}

void loop() {
  if (FC_51_left.get_processState() == ACTIVE) {
    if (position_right.get_processState() == ACTIVE  &&  motor_left.get_phaseNumber() == FIRST_TIME_PHASE) {
      trackShiftState = true;
      motor_left.activate(); // Activate track switch motor
      Serial.println("Move track switch to the left");
    }
  }
  else {
    if (trackShiftState == true  &&  motor_left.get_phaseNumber() == COMPLETE_PHASE) {
      motor_right.activate(); // Activate track switch motor
      Serial.println("Move track switch back to the right");
      trackShiftState = false;  // last command in the process
    }
  }

  // Track Switch to the right is identical to Track Switch on the left, just mirrored. Not shown here.

  MTD2A::loop_execute();  // Update the state (event) system
} // Automatic track switch to the direction the train is coming from
