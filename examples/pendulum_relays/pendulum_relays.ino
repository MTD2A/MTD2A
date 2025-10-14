
// Automatic pendulum operation on model railway system using relays to control track switch, 
// train direction, train and power, and one IR sensor to calibrate time and distance offset.
// When the train reaches its holding area, it is sent back in the opposite direction after the set waiting time. 
// MTD2A_binary_output: https://github.com/MTD2A/MTD2A/tree/main/doc#mtd2a_binary_output
// Jørgen Bo Madsen / october 2025 / https://github.com/jebmdk  Sensor: https://github.com/MTD2A/FC-51

#include <MTD2A.h>
using namespace MTD2A_const;

// IR sensor pin input at side track end. 
MTD2A_binary_input IR_sensor       ("IR sensor");
// Train power relay. Wait 3 second before driving
MTD2A_binary_output train_power    ("Train power",    0, 3000);  // Set outputTimeMS later
// Train direction relay. Wait 2,9 second before driving backward. Default is forward (no voltage).
// Activate 100 milliseconds before train power relay. Add 100 ms to timing in function: bacward_driving.
MTD2A_binary_output train_backward ("Train backward", 0, 2900);  // Set outputTimeMS later
// Track switch actuator direction relay. Send 0,5 second pulse. Default left (no voltage).
MTD2A_binary_output actuator_right ("Right switch actuator", 500);
// Track switch actuator power relay. Wait 0,1 second and send 0,4 seconds pulse
MTD2A_binary_output actuator_power ("Power switch actuator", 400, 100);
// Timer
unsigned int backwardFromSide  = 6200; // Seconds to move bacward from side track
unsigned int forwarFromRear    = 6200; // Seconds to move forward from rear track
unsigned int backwardFromFront = 5600; // Seconds to move backward from front track
unsigned int sideFromRear      = 6800; // Seconds to move side from rear track

// Process
unsigned int stepCount = 0;

void setup() {
  Serial.begin(9600);
  while (!Serial) { delay(10); } // ESP32 Serial Monitor ready delay
  //
  IR_sensor.initialize       (A0);  // Pin input no: Sensor train detection
  train_power.initialize     (5,  INVERTED);  // Relay: Power train on tracks
  train_backward.initialize  (6,  INVERTED);  // Relay: Train direction
  actuator_power.initialize  (9,  INVERTED);  // Relay: Track switch power
  actuator_right.initialize  (10, INVERTED);  // Relay: Track switch direction right
  Serial.println("Automatic pendulum operation. Activate sensor to start loop");
}

void loop() {
  
  switch (stepCount) {
    case 0: 
      if (IR_sensor.get_processState () == ACTIVE) {
        Serial.println("Sensor activated");
        set_switch_direction_right ();
        if (train_power.get_processState () == ACTIVE) {
          Serial.println ("Stop train immediately!");
          train_power.set_outputTimer(STOP_TIMER);
        }
        stepCount = 1;
      }
    break;
    case 1:
      if (train_power.get_processState () == COMPLETE) {
        Serial.println ("Wait 3 seconds - Start train from the end of side track at IR sensor");
        backward_driving (backwardFromSide);  // Seconds train power
        stepCount = 2;
      }
    break;
    case 2:
      if (train_power.get_processState () == COMPLETE) {
        set_switch_direction_left ();
        Serial.println ("Wait 3 seconds - Start train from the rear end of track");
        forward_driving (forwarFromRear);  // Seconds train power
        stepCount = 3;
      }
    break;
    case 3:
      if (train_power.get_processState () == COMPLETE) {
        Serial.println ("Wait 3 seconds - Start train from the front end of track");
        backward_driving (backwardFromFront); // Seconds train power
        stepCount = 4;
      }
    break;
    case 4:
      if (train_power.get_processState () == COMPLETE) {
        set_switch_direction_right ();
        Serial.println ("Wait 3 seconds - Start train from the rear end of track");
        forward_driving (sideFromRear); // Seconds train power
        stepCount = 0;
      }
    break;
  } // switch

  MTD2A_loop_execute();  // Update the state (event) system

} // end loop Automatic pendulum operation on model railway system.


// Helper functions --------------------------------------------------


void set_switch_direction_right () {
  Serial.println("Set track switch direction to the RIGHT");
  actuator_right.activate (); // Track switch actuator direction relay
  actuator_power.activate (); // Track switch actuator train power relay
} // set_switch_direction_right


void set_switch_direction_left () {
  Serial.println("Set track switch direction to the LEFT");
  // Track switch actuator direction is default left (no voltage)
  actuator_power.activate (); // Track switch actuator power relay
} // set_switch_direction_left


void forward_driving (unsigned int drivingTime) {
  // Relay direction default is forward (no voltage)
  train_power.set_outputTimeMS (drivingTime); // seconds train power
  train_power.activate (); // Relay to power train on tracks
} // forward_driving


void backward_driving (unsigned int drivingTime) {
  train_backward.set_outputTimeMS (drivingTime); // seconds backward
  train_power.set_outputTimeMS    (drivingTime); // seconds train power
  train_backward.activate (); // Backward direction relay
  train_power.activate    (); // Power train on tracks
}  // backward_driving

