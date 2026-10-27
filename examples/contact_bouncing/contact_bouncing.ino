// contact_bouncing - correct push button detection and debounce handling.
// Demonstration of debounce handling, measuring button push holding down time and push button down counter within time period.
// Examples are written for the Arduino Nano pinout (Nano family, Uno, Mega)
// https://github.com/MTD2A/MTD2A/blob/main/doc/MTD2A_binary_input_english.pdf
// Short DEMO: https://youtu.be/LPrWA3gDReY
// Joergen Bo Madsen / September 2026 / https://github.com/jebmdk

#include <MTD2A.h>
using namespace MTD2A_const;

#define section 1
// 1: Count number of push button debounce.
// 2: Arduino standard: Measure push button down time.
// 3: MTD2A library:    Measure push button down time.
// 4: Short, long and extra long button presses.
// 5: Count number of button presses within 2 seconds and variations thereof.
// 6: Count number of two button presses in parallel within 4 seconds.

int GREEN_BUTTON_PIN =  2;
int RED_BUTTON_PIN   =  3;
int GREEN_LED_PIN    =  9;
int RED_LED_PIN      = 10;

bool buttonDown      = false;
bool timerState      = false;
int  printCount      = 0;
int  currButtonState = HIGH;
int  lastButtonState = HIGH;
unsigned long changeCount = 0;
unsigned long beginTime   = 0;


// ------------------------------------------------------------------------------------------------


#if section == 1

void setup() {
  Serial.begin(9600);
  while (!Serial) { delay(10); } // ESP32 Serial Monitor ready delay
  pinMode(GREEN_BUTTON_PIN, INPUT_PULLUP);
  pinMode(RED_BUTTON_PIN, INPUT_PULLUP);
  Serial.println (F("Count number of push button debounce."));
} // setup


void loop() {
  // read the input pin:
  lastButtonState = currButtonState;
  //currButtonState = digitalRead(GREEN_BUTTON_PIN);
  currButtonState = digitalRead(RED_BUTTON_PIN);
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
  Serial.println (); Serial.println (F("Arduino standard: Measure green push button down time (minimum DEBOUNCE_MS)."));
  pinMode(GREEN_BUTTON_PIN, INPUT_PULLUP);
  // pinMode(RED_BUTTON_PIN, INPUT_PULLUP);
} // setup


void loop() {
  // read the input pin:
  lastButtonState = currButtonState;
  currButtonState = digitalRead(GREEN_BUTTON_PIN);
  // currButtonState = digitalRead(RED_BUTTON_PIN);
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
    Serial.print (printCount); Serial.print (F("  Green button holding down milliseconds: ")); Serial.println(millis() - beginTime);
    buttonDown = false;
  }
  delay (1); // waith 1 millisecond before next loop
} // loop

#endif


// ------------------------------------------------------------------------------------------------


#if section == 3

MTD2A_binary_input  greenButton ("Green button", DEBOUNCE_MS, FIRST_TRIGGER, TIME_DELAY);
MTD2A_binary_output greenLED    ("Green LED");


void setup() {
  Serial.begin(9600); 
  while (!Serial) { delay(10); } // ESP32 Serial Monitor ready delay
  Serial.println (); Serial.println (F("MTD2A library: Measure green push holding button down time."));
  MTD2A::set_globalDelayTimeMS (DELAY_10MS); // { DELAY_10MS | DELAY_5MS | DELAY_2MS | DELAY_1MS }
  greenButton.initialize (GREEN_BUTTON_PIN);  // Default: NORMAL & INPUT_PULLUP
  greenLED.initialize    (GREEN_LED_PIN);
}

void loop() {
  if (greenButton) { // Equivalent to (get_phaseChange() == true  &&  get_phaseNumber() == COMPLETE_PHASE)
    printCount++; 
    Serial.print (printCount); Serial.print (F("  Green button holding down milliseconds: ")); Serial.println (greenButton.get_activeTimeMS ());
    if (greenLED.get_processState() == COMPLETE) { // Check if button is pressed before LED flash time is up
      greenLED.set_outputTimeMS(greenButton.get_activeTimeMS ());
      greenLED.activate ();
    }
  }
  MTD2A_loop_execute ();
} // loop

#endif


// ------------------------------------------------------------------------------------------------


#if section == 4

MTD2A_binary_input  greenButton ("Green button", DEBOUNCE_MS, FIRST_TRIGGER, TIME_DELAY);
MTD2A_binary_output greenLED    ("Green LED");

void setup() {
  Serial.begin(9600); 
  while (!Serial) { delay(10); } // ESP32 Serial Monitor ready delay
  Serial.println (); Serial.println (F("MTD2A library: Short, long and extra long button presses."));
  MTD2A::set_globalDelayTimeMS (DELAY_10MS); // { DELAY_10MS | DELAY_5MS | DELAY_2MS | DELAY_1MS }
  greenButton.initialize (GREEN_BUTTON_PIN);  // Default: NORMAL & INPUT_PULLUP
  greenLED.initialize    (GREEN_LED_PIN);
}

void loop() {
  unsigned long activeTimeMS = 0;
  if (greenButton) { // Equivalent to (get_phaseChange() == true  &&  get_phaseNumber() == COMPLETE_PHASE)
    activeTimeMS = greenButton.get_activeTimeMS ();
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
  if (greenButton  &&  greenLED.get_processState() == COMPLETE) { // Check if button is pressed before LED flash time is up
    greenLED.set_outputTimeMS(activeTimeMS);
    greenLED.activate ();
  }

  MTD2A_loop_execute ();
} // loop

#endif


// ------------------------------------------------------------------------------------------------


#if section == 5

// A: Immediately detects exactly one button press
// MTD2A_binary_input greenButton ("One fast Green button press", 0);
// B: Count number of button presses within 2 seconds
// MTD2A_binary_input greenButton ("Green button first press counter", 2000, FIRST_TRIGGER, TIME_DELAY);
// C: Count number of button presses within 2 seconds, resstarting timer after each button press. 
MTD2A_binary_input greenButton ("Green button last press counter", 2000, LAST_TRIGGER, TIME_DELAY);
//
MTD2A_binary_output greenLED ("Green LED", 200, 200); // Blink cycle is 400 milliseconds

void setup() {
  Serial.begin(9600); 
  while (!Serial) { delay(10); } // ESP32 Serial Monitor ready delay
  Serial.println (); Serial.println (F("MTD2A library: Count number of green button presses within 2 seconds and variations thereof.")); 
  MTD2A::set_globalDelayTimeMS (DELAY_10MS); // { DELAY_10MS | DELAY_5MS | DELAY_2MS | DELAY_1MS }
  greenButton.initialize (GREEN_BUTTON_PIN);  // Default: NORMAL & INPUT_PULLUP
  greenLED.initialize    (GREEN_LED_PIN);
}

void loop() {
  if (greenButton) { // Equivalent to (get_phaseChange() == true  &&  get_phaseNumber() == COMPLETE_PHASE)
    printCount++; 
    Serial.print (printCount); Serial.print (F("  Number of green button pressed: ")); Serial.println (greenButton.get_inputCount ());
    if (greenLED.get_processState() == COMPLETE) { // Check if button is pressed before LED flash time is up
      greenLED.set_loopActivate(ENABLE, greenButton.get_inputCount ());
      greenLED.activate ();  
    }
    // buttonCounter.print_conf ();
  }
  MTD2A_loop_execute ();
} // loop

#endif


// ------------------------------------------------------------------------------------------------


#if section == 6

// Count number of button presses within 4 seconds
MTD2A_binary_input  greenButton ("Button first press counter",   4000, FIRST_TRIGGER, TIME_DELAY);
MTD2A_binary_input  redButton   ("Button first press counter 2", 4000, FIRST_TRIGGER, TIME_DELAY);
MTD2A_binary_output greenLED    ("Green LED", 200, 200); // Blink cycle is 400 milliseconds
MTD2A_binary_output redLED      ("Green LED", 200, 200);

void setup() {
  Serial.begin(9600); 
  while (!Serial) { delay(10); } // ESP32 Serial Monitor ready delay
  Serial.println (); Serial.println (F("MTD2A library: Count number of two button presses in parallel within 4 seconds.")); 
  MTD2A::set_globalDelayTimeMS (DELAY_10MS); // Default: DELAY_10MS { DELAY_10MS | DELAY_5MS | DELAY_2MS | DELAY_1MS }
  greenButton.initialize (GREEN_BUTTON_PIN); // Default: NORMAL & INPUT_PULLUP
  redButton.initialize   (RED_BUTTON_PIN);
  greenLED.initialize    (GREEN_LED_PIN);
  redLED.initialize      (RED_LED_PIN);
}

void loop() {
  // Button 1
  if (greenButton) { // Equivalent to (get_phaseChange() == true  &&  get_phaseNumber() == COMPLETE_PHASE)
    printCount++; 
    Serial.print (printCount); Serial.print (F("  Number of green button pressed: ")); Serial.println (greenButton.get_inputCount ());
    if (greenLED.get_processState() == COMPLETE) { // Check if button is pressed before LED flash time is up
      greenLED.set_loopActivate(ENABLE, greenButton.get_inputCount ());
      greenLED.activate ();  
    }
  }
  // Button 2
  if (redButton) { // Equivalent to (get_phaseChange() == true  &&  get_phaseNumber() == COMPLETE_PHASE)
    printCount++; 
    Serial.print (printCount); Serial.print (F("  Number of red button  pressed: ")); Serial.println (redButton.get_inputCount ());
    if (redLED.get_processState() == COMPLETE) { // Check if button is pressed before LED flash time is up
      redLED.set_loopActivate(ENABLE, redButton.get_inputCount ());
      redLED.activate ();  
    }
  }
  MTD2A_loop_execute ();
} // loop


#endif

