
// Determining the direction of travel of the train
// Jørgen Bo Madsen / may 2025 / https://github.com/jebmdk

#include <MTD2A.h>

MTD2A_binary_input  FC_51_left      ("FC 51 left",  5000, lastTrigger, timeDelay); // 5 sec
MTD2A_binary_input  FC_51_right     ("FC 51 right", 5000, lastTrigger, timeDelay); // 5 sec
MTD2A_binary_output red_LED_left    ("Red LED left",    3000);  // 3 seconds
MTD2A_binary_output green_LED_right ("Green LED right", 3000);  // 3 seconds

void setup() {
  Serial.begin(9600);
  MTD2A::set_allDebugPrint   (enable); // Enable debug print for all instantiated classes
  FC_51_left.initialize      (2);  // input pin 2
  FC_51_right.initialize     (3);  // input pin 3
  red_LED_left.initialize    (9);  // Output pin 9
  green_LED_right.initialize (10); // output pin 10
}

void loop() {
  if (FC_51_left.get_processState() == active  &&  FC_51_right.get_processState() == pending) {
  // Equivalent to: if (FC_51_left > FC_51_right) {
    if (FC_51_left.get_phaseChange() == true  &&  FC_51_left.get_phaseNumber() == firstTimePhase) {
    // Equivalent to: if (FC_51_left + FC_51_left) {  
      red_LED_left.activate();
      Serial.println("Moving left");
    }
  }
  else
    if (FC_51_left.get_processState() == pending  &&  FC_51_right.get_processState() == active) {  
    // equivalent to: if (FC_51_left < FC_51_right) {
      if (FC_51_right.get_phaseChange() == true  &&  FC_51_right.get_phaseNumber() == firstTimePhase) {
      // Equivalent to: if(FC_51_left + FC_51_left) {
        green_LED_right.activate();
        Serial.println("Moving rigth");
      }
    }
  
  FC_51_left.loop_fast();
  FC_51_right.loop_fast();
  red_LED_left.loop_fast();
  green_LED_right.loop_fast();
  delay (10);
} // Determining the direction of travel of the train