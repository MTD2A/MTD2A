// Read sensor and write phase state information to Arduino IDE serial monitor
// https://github.com/MTD2A/FC-51
// Short DEMO: https://youtu.be/RDFgEbhYUzE
// Jørgen Bo Madsen / may 2025 / https://github.com/jebmdk

#include <MTD2A.h>
using namespace MTD2A_const;

MTD2A_binary_input FC_51_sensor ("FC_51 sensor", 5000);
// "FC-51 sensor" = Sensor (object) name, which is displayed together with status messages
// 5000 = Time delay in milliseconds (5 seconds)
// default: LAST_TRIGGER = Start calculating time from last impulse (LOW->HIGH)
// default: TIME_DELAY = Use time delay (timer function)

void setup () {
  Serial.begin (9600);  // Required and first if status messages are to be displayed
  while (!Serial) { delay(10); } // ESP32 Serial Monitor ready delay

  // responsiveness: Loop time: DELAY_1MS / DELAY_2MS, DELAY_5MS / DELAY_10MS milliseconds
  FC_51_sensor.set_globalDelayTimeMS (DELAY_10MS); // Default: DELAY_10MS

  byte FC_51_SENSOR_PIN = 2; // Arduino board pin 2 input.
  FC_51_sensor.initialize (FC_51_SENSOR_PIN);
  FC_51_sensor.set_debugPrint ();  // Display status messages
  // Every time the sensor is activated, the status will be printed on the Arduino Serial Monitor
}

void loop () {
    MTD2A_loop_execute ();  // Update the state (event) system
} // Read sensor and write phase state information to Arduino IDE serial monitor

