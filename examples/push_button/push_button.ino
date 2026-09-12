// Push button detection and debounce handling.
// Demonstration of debounce counting, measuring button down time and button down counter within time period.
// Examples are written for the Arduino Nano pinout (Nano family, Uno, Mega)
// https://github.com/MTD2A/MTD2A/blob/main/doc/MTD2A_binary_input_english.pdf
// Short DEMO: 
// Joergen Bo Madsen / September 2026 / https://github.com/jebmdk
#include <MTD2A.h>
using namespace MTD2A_const;

#define section 4
// 1: Count number of push button debounces.
// 2: Arduino standard: Measure push button down time.
// 3: MTD2A library: Measure push button down time.
// 4: Count number of button presses within LONG_PRESS_MS milliseconds and variations thereof.

int pushButton = 2; // Digital PIN 2


bool buttonDown = false;
bool timerState = false;
int  printCount = 0;
int  currButtonState = HIGH;
int  lastButtonState = HIGH;
unsigned long changeCount = 0;
unsigned long beginTime   = 0;


// ------------------------------------------------------------------------------------------------


#if section == 1

void setup() {
  Serial.begin(9600);
  while (!Serial) { delay(10); } // ESP32 Serial Monitor ready delay
  Serial.println (F("Count number of push button debounces."));
  pinMode(pushButton, INPUT_PULLUP);
} // setup


void loop() {
  // read the input pin:
  lastButtonState = currButtonState;
  currButtonState = digitalRead(pushButton);
  // Detect push down button
  if (currButtonState == LOW) {
    if (buttonDown == false) {
       buttonDown = true;
       timerState = true;
       beginTime  = millis();
    }
  }

  if (timerState == true) {
    if (currButtonState != lastButtonState) changeCount++;
    if (millis() - beginTime > 500) {
      timerState = false;
      buttonDown = false;
      printCount++;
      Serial.print (printCount); Serial.print (F("  Number of button changes = ")); Serial.println(changeCount);
      beginTime   = 0;
      changeCount = 0;
    }
  }
} // loop

#endif


// ------------------------------------------------------------------------------------------------


#if section == 2

void setup() {
  Serial.begin(9600);
  while (!Serial) { delay(10); } // ESP32 Serial Monitor ready delay
  Serial.println (); Serial.println (F("Arduino standard: Measure push button down time."));
  pinMode(pushButton, INPUT_PULLUP);
} // setup


void loop() {
  // read the input pin:
  lastButtonState = currButtonState;
  currButtonState = digitalRead(pushButton);
  // Detect push down button
  if (currButtonState == LOW) {
    if (buttonDown == false) {
       buttonDown = true;
       timerState = true;
       beginTime  = millis();
       // delay (20); // Debounce time
       delay (50); // DEBOUNCE_MS
    }
  }
  // print out push button downn time (LOW)
  if (currButtonState == HIGH && lastButtonState == HIGH &&  buttonDown == true) {
    printCount++;
    Serial.print (printCount); Serial.print (F("  Button down milliseconds: ")); Serial.println(millis() - beginTime);
    buttonDown = false;
  }
  delay (1); // waith 1 millisecond before next loop
} // loop

#endif


// ------------------------------------------------------------------------------------------------


#if section == 3

MTD2A_binary_input buttonTimer ("Button timer", DEBOUNCE_MS, LAST_TRIGGER, TIME_DELAY);

void setup() {
  Serial.begin(9600); 
  while (!Serial) { delay(10); } // ESP32 Serial Monitor ready delay
  Serial.println (); Serial.println (F("MTD2A library: Measure push button down time."));
  MTD2A::set_globalDelayTimeMS (DELAY_10MS); // { DELAY_10MS | DELAY_5MS | DELAY_2MS | DELAY_1MS }
  buttonTimer.initialize (pushButton);  // default: NORMAL & INPUT_PULLUP
}

void loop() {
  if (buttonTimer) { // Equivalent to (get_phaseChange() == true  &&  get_phaseNumber() == COMPLETE_PHASE)
    printCount++; 
    Serial.print (printCount); Serial.print (F("  Button down milliseconds: ")); Serial.println (buttonTimer.get_activeTimeMS ());
  }
  MTD2A_loop_execute ();
} // loop

#endif


// ------------------------------------------------------------------------------------------------


#if section == 4

// A: Count number of button presses within LONG_PRESS_MS milliseconds
MTD2A_binary_input buttonCounter ("Button first press counter", LONG_PRESS_MS, FIRST_TRIGGER, TIME_DELAY);
// B: Count number of button presses within LONG_PRESS_MS milliseconds, resstarting timer after each button press. 
// MTD2A_binary_input buttonCounter ("Button last press counter", LONG_PRESS_MS, LAST_TRIGGER, TIME_DELAY);
// C: Immediately detects exactly one button press
// MTD2A_binary_input buttonCounter ("One fast Button press", 0);

void setup() {
  Serial.begin(9600); 
  while (!Serial) { delay(10); } // ESP32 Serial Monitor ready delay
  Serial.println (); Serial.print (F("MTD2A library: Count number of button presses within ")); 
  Serial.print (LONG_PRESS_MS); Serial.println (F(" milliseconds"));
  Serial.print (F("The Debounce time: "));  Serial.print (DEBOUNCE_MS); 
  Serial.println (F(" milliseconds is skipped, and is not counted.")); 
  MTD2A::set_globalDelayTimeMS (DELAY_10MS); // { DELAY_10MS | DELAY_5MS | DELAY_2MS | DELAY_1MS }
  buttonCounter.initialize (pushButton);  // default: NORMAL & INPUT_PULLUP
}

void loop() {
  if (buttonCounter) { // Equivalent to (get_phaseChange() == true  &&  get_phaseNumber() == COMPLETE_PHASE)
    printCount++; 
    Serial.print (printCount); Serial.print (F("  Number of button presses: ")); Serial.println (buttonCounter.get_inputCount ());
  }
  MTD2A_loop_execute ();
} // loop

#endif


