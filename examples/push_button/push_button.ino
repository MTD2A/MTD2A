// Push button detection and debounce handling.
// Demonstration of debounce counting, measuring button down time and button down counter within time period.
// Examples are written for the Arduino Nano pinout (Nano family, Uno, Mega)
// https://github.com/MTD2A/MTD2A/blob/main/doc/MTD2A_binary_input_english.pdf
// Short DEMO: 
// Joergen Bo Madsen / September 2026 / https://github.com/jebmdk
#include <MTD2A.h>
using namespace MTD2A_const;

#define section 6
// 1: Count number of push button debounce.
// 2: Arduino standard: Measure push button down time.
// 3: MTD2A library:    Measure push button down time.
// 4: Short, long and extra long button presses.
// 5: Count number of button presses within 2 seconds and variations thereof.
// 6: Count number of two button presses in parallel within 4 seconds.

int BUTTON_PIN = 2; // Digital PIN 2


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
  Serial.println (F("Count number of push button debounce."));
  pinMode(BUTTON_PIN, INPUT_PULLUP);
} // setup


void loop() {
  // read the input pin:
  lastButtonState = currButtonState;
  currButtonState = digitalRead(BUTTON_PIN);
  // Check whether the button has been pressed.
  if (currButtonState == LOW) {
    if (buttonDown == false) {
       buttonDown = true;
       timerState = true;
       beginTime  = millis();
    }
  }

  if (timerState == true) {
    if (currButtonState != lastButtonState) {
      changeCount++;
    }
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
  pinMode(BUTTON_PIN, INPUT_PULLUP);
} // setup


void loop() {
  // read the input pin:
  lastButtonState = currButtonState;
  currButtonState = digitalRead(BUTTON_PIN);
  // Check whether the button has been pressed.
  if (currButtonState == LOW) {
    if (buttonDown == false) {
       buttonDown = true;
       timerState = true;
       beginTime  = millis();
       delay (DEBOUNCE_MS); // DEBOUNCE_MS = 50 milliseconds
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

MTD2A_binary_input buttonTimer ("Button timer", DEBOUNCE_MS, FIRST_TRIGGER, TIME_DELAY);

void setup() {
  Serial.begin(9600); 
  while (!Serial) { delay(10); } // ESP32 Serial Monitor ready delay
  Serial.println (); Serial.println (F("MTD2A library: Measure push button down time."));
  MTD2A::set_globalDelayTimeMS (DELAY_10MS); // { DELAY_10MS | DELAY_5MS | DELAY_2MS | DELAY_1MS }
  buttonTimer.initialize (BUTTON_PIN);  // Default: NORMAL & INPUT_PULLUP
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

MTD2A_binary_input buttonTimer ("Button timer", DEBOUNCE_MS, FIRST_TRIGGER, TIME_DELAY);

void setup() {
  Serial.begin(9600); 
  while (!Serial) { delay(10); } // ESP32 Serial Monitor ready delay
  Serial.println (); Serial.println (F("MTD2A library: Short, long and extra long button presses."));
  MTD2A::set_globalDelayTimeMS (DELAY_10MS); // { DELAY_10MS | DELAY_5MS | DELAY_2MS | DELAY_1MS }
  buttonTimer.initialize (BUTTON_PIN);  // Default: NORMAL & INPUT_PULLUP
}

void loop() {
  if (buttonTimer) { // Equivalent to (get_phaseChange() == true  &&  get_phaseNumber() == COMPLETE_PHASE)
    unsigned long activeTimeMS = buttonTimer.get_activeTimeMS ();
    if (activeTimeMS > 50  && activeTimeMS <= 500) {  // DEBOUNCE_MS = 50
      Serial.print (F("SHORT button press: ")); Serial.println (activeTimeMS);
    }
    else if (activeTimeMS > 500  && activeTimeMS <= 1500) {
      Serial.print (F("LONG button press: ")); Serial.println (activeTimeMS);
    }
    else if (activeTimeMS > 1500  && activeTimeMS <= 2500) {
      Serial.print (F("EXTRA long button press: ")); Serial.println (activeTimeMS);
    }
  }
  MTD2A_loop_execute ();
} // loop

#endif


// ------------------------------------------------------------------------------------------------


#if section == 5

// A: Count number of button presses within 2 seconds
MTD2A_binary_input buttonCounter ("Button first press counter", 2000, FIRST_TRIGGER, TIME_DELAY);
// B: Count number of button presses within 2 seconds, resstarting timer after each button press. 
// MTD2A_binary_input buttonCounter ("Button last press counter", 2000, LAST_TRIGGER, TIME_DELAY);
// C: Immediately detects exactly one button press
// MTD2A_binary_input buttonCounter ("One fast Button press", 0);

void setup() {
  Serial.begin(9600); 
  while (!Serial) { delay(10); } // ESP32 Serial Monitor ready delay
  Serial.println (); Serial.println (F("MTD2A library: Count number of button presses within 2 seconds and variations thereof.")); 
  MTD2A::set_globalDelayTimeMS (DELAY_10MS); // { DELAY_10MS | DELAY_5MS | DELAY_2MS | DELAY_1MS }
  buttonCounter.initialize (BUTTON_PIN);  // Default: NORMAL & INPUT_PULLUP
}

void loop() {
  if (buttonCounter) { // Equivalent to (get_phaseChange() == true  &&  get_phaseNumber() == COMPLETE_PHASE)
    printCount++; 
    Serial.print (printCount); Serial.print (F("  Number of button pressed: ")); Serial.println (buttonCounter.get_inputCount ());
    // buttonCounter.print_conf ();
  }
  MTD2A_loop_execute ();
} // loop

#endif


// ------------------------------------------------------------------------------------------------


#if section == 6

int BUTTON_PIN_2 = 3; // Digital PIN 3

// Count number of button presses within 4 seconds
MTD2A_binary_input buttonCounter  ("Button first press counter",   4000, FIRST_TRIGGER, TIME_DELAY);
MTD2A_binary_input buttonCounter2 ("Button first press counter 2", 4000, FIRST_TRIGGER, TIME_DELAY);

void setup() {
  Serial.begin(9600); 
  while (!Serial) { delay(10); } // ESP32 Serial Monitor ready delay
  Serial.println (); Serial.println (F("MTD2A library: Count number of two button presses in parallel within 4 seconds.")); 
  MTD2A::set_globalDelayTimeMS (DELAY_10MS); // Default: DELAY_10MS { DELAY_10MS | DELAY_5MS | DELAY_2MS | DELAY_1MS }
  buttonCounter.initialize  (BUTTON_PIN);     // Default: NORMAL & INPUT_PULLUP
  buttonCounter2.initialize (BUTTON_PIN_2);   // Default: NORMAL & INPUT_PULLUP
}

void loop() {
  // Button 1
  if (buttonCounter) { // Equivalent to (get_phaseChange() == true  &&  get_phaseNumber() == COMPLETE_PHASE)
    printCount++; 
    Serial.print (printCount); Serial.print (F("  Number of button pressed: ")); Serial.println (buttonCounter.get_inputCount ());
  }
  // Button 2
  if (buttonCounter2) { // Equivalent to (get_phaseChange() == true  &&  get_phaseNumber() == COMPLETE_PHASE)
    printCount++; 
    Serial.print (printCount); Serial.print (F("  Number of button 2 pressed: ")); Serial.println (buttonCounter2.get_inputCount ());
  }
  MTD2A_loop_execute ();
} // loop


#endif

