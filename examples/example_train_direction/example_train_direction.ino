
// Determining the direction of travel of the train
// Jørgen Bo Madsen / may 2025 / https://github.com/jebmdk

#include <MTD2A.h>
using namespace MTD2A_const;

MTD2A_binary_input  FC_51_left      ("FC 51 left",  3000, LAST_TRIGGER, TIME_DELAY); // 3 sec
MTD2A_binary_input  FC_51_right     ("FC 51 right", 3000, LAST_TRIGGER, TIME_DELAY); // 3 sec
MTD2A_binary_output red_LED_left    ("Red LED left",    3000);  // 3 seconds
MTD2A_binary_output green_LED_right ("Green LED right", 3000);  // 3 seconds

void setup() {
  Serial.begin(9600);
  MTD2A::set_globalDebugPrint ();   // Enable debug print for all instantiated classes
  FC_51_left.initialize       (2);  // input Arduino board pin 2
  FC_51_right.initialize      (3);  // input Arduino board pin 3
  red_LED_left.initialize     (9);  // Output Arduino board pin 9
  green_LED_right.initialize  (10); // output Arduino board pin 10
}

void loop() {
  if (FC_51_left.get_processState() == ACTIVE  &&  FC_51_right.get_processState() == COMPLETE) {
  // Equivalent to: if (FC_51_left > FC_51_right) {
    if (FC_51_left.get_phaseChange() == true  &&  FC_51_left.get_phaseNumber() == FIRST_TIME_PHASE) {
    // Equivalent to: if (FC_51_left + FC_51_left) {  
      red_LED_left.activate();
      Serial.println("Moving left");
    }
  }
  else
    if (FC_51_left.get_processState() == COMPLETE  &&  FC_51_right.get_processState() == ACTIVE) {  
    // equivalent to: if (FC_51_left < FC_51_right) {
      if (FC_51_right.get_phaseChange() == true  &&  FC_51_right.get_phaseNumber() == FIRST_TIME_PHASE) {
      // Equivalent to: if(FC_51_left + FC_51_left) {
        green_LED_right.activate();
        Serial.println("Moving rigth");
      }
    }
  
    MTD2A::loop_execute();  // Update the state (event) system
} // Determining the direction of travel of the train