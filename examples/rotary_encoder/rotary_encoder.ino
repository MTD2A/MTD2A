/*
Rotary encoder with and without MTD2A. 
Using KY-040 Specific incremental, quadrature and relative encoder.
  Uses two pulsed output signals and one signal from the puch button.
  20 steps (detents) pr. 360 degrees revolution, builtin push button switch, 
  5 volts, 10 KΩ on board pull-up resistors for Clock. Some versions lave the Switch resistor unpopulated.
  2 bit gray code: Two digital outputs, four states and only one bit changes at each step
The contacts are mechanical and bounce. A state-table decoder like #7 and #8 below handles this, 
because a bounce just steps back and forth between two neighbouring states and cancels itself out.
Two LEDs: Green and Red shows PN status or encoding output status
The push button switch is connected to passive buzzer and the frequency is controlled by tone()

Examples are written for the Arduino Nano pinout (Nano family, Uno, Mega)
https://github.com/MTD2A/MTD2A/blob/main/doc/MTD2A_binary_input_english.pdf
Short DEMO: 
Joergen Bo Madsen / November 2026 / https://github.com/jebmdk
*/

#include <MTD2A.h>
using namespace MTD2A_const;


#define section 1
// 1: Test CLK (clock) bouncing
// 2: Test DT  (data)  bouncing
// 3: Test SW  (push button) bouncing
// 5: Arduino simple rotary counter. No debounce example
// 6: MTD2A   simple rotary counter with short timed debounce example
// 7: Arduino correct and bounce resistant rotary counter example
// 8: MTD2A   correct and bounce resistant rotary counter example

// Rotary Encoder Inputs
int ROTARY_CLK_PIN = 14;
int ROTARY_DT_PIN  = 15;
int ROTARY_SW_PIN  = 16;

 // LED Outputs
int BUZZER_PIN       =  5;
int GREEN_LED_CW_PIN =  9;
int RED_LED_CCW_PIN  = 10;


// ----------------------------------------------------------------------------------------------------------


#if (section == 1 || section == 2 || section == 3)


bool buttonDown      = false;
bool timerState      = false;
int  printCount      = 0;
int  currButtonState = HIGH;
int  lastButtonState = HIGH;
unsigned long changeCount = 0;
unsigned long beginTime   = 0;


void setup() { 
  // Set encoder pins as inputs (KY-040 has pull-ups on CLK and DT)
  pinMode (ROTARY_CLK_PIN, INPUT);
  pinMode (ROTARY_DT_PIN,  INPUT);
  pinMode (ROTARY_SW_PIN,  INPUT);
  // Some version of KY-040 does not use pull-up resistor
  // pinMode (ROTARY_DT_PIN,  INPUT_PULLUP);  // No pull-up on switch

  // Set LED pins as outputs
  pinMode (GREEN_LED_CW_PIN, OUTPUT);
  pinMode (RED_LED_CCW_PIN,  OUTPUT);
  
  // Setup Serial Monitor
  Serial.begin(9600);
  while (!Serial) { delay(10); } // ESP32 Serial Monitor ready delay

  Serial.println (F("Count number bounces (Twice is correct: falling edge and rising edge)."));
  Serial.println (F("Turn/press the knob one step only and waith for the result on the monitor. Start over"));
  #if   section == 1
    Serial.println (F("(1) CLK (clock) test."));
  #elif section == 2
    Serial.println (F("(2) DT (data) test."));
  #elif section == 3
    Serial.println (F("(3) SW (push button switch) test."));
  #endif
} // setup


void loop() {
  // read the input pin:
  lastButtonState = currButtonState;
  #if   section == 1
    currButtonState = digitalRead(ROTARY_CLK_PIN);
  #elif section == 2
    currButtonState = digitalRead(ROTARY_DT_PIN);
  #elif section == 3
    currButtonState = digitalRead(ROTARY_SW_PIN);
  #endif
  // Check whether the button has been pressed.
  if (currButtonState == LOW) {
    if (buttonDown == false) {
       buttonDown = true;
       timerState = true;
       beginTime  = millis();
    }
  }
  // show result
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


// ----------------------------------------------------------------------------------------------------------


#if section == 4

int  counter = 0; 
bool currentCLKstate  = HIGH;
bool previousCLKstate = LOW;
bool currentDTstate   = HIGH;
bool previousDTstate  = LOW;

void setup() { 
  // Set encoder pins as inputs (KY-040 has pull-ups on CLK and DT)
  pinMode (ROTARY_CLK_PIN, INPUT);
  pinMode (ROTARY_DT_PIN,  INPUT);
  
  // Set LED pins as outputs
  pinMode (GREEN_LED_CW_PIN, OUTPUT);
  pinMode (RED_LED_CCW_PIN,  OUTPUT);
  
  // Setup Serial Monitor
  Serial.begin(9600);
  while (!Serial) { delay(10); } // ESP32 Serial Monitor ready delay
  
  Serial.println (F("Arduino rotary encoder LED state demonstration."));
  Serial.println (F("If the rotary button is HIGH, the LED lights up, and vice versa."));
  Serial.println (F("Turn the knob very slowly, watch the LEDs and observe the logic step-by-step."));
} 

void loop() { 

  currentCLKstate = digitalRead(ROTARY_CLK_PIN);
  if (currentCLKstate != previousCLKstate) {
    counter++;
    Serial.print(counter); 
    if (currentCLKstate == HIGH) {
      digitalWrite (GREEN_LED_CW_PIN, HIGH);
      Serial.println (F("  Green HIGH"));
    }
    else {
      digitalWrite (GREEN_LED_CW_PIN, LOW);
      Serial.println (F("  Green LOW"));
    } 
      previousCLKstate = currentCLKstate;
  } // CLK

  currentDTstate = digitalRead(ROTARY_DT_PIN);
  if (currentDTstate != previousDTstate) {
    counter++;
    Serial.print(counter);
    if (currentDTstate == HIGH) {
      digitalWrite (RED_LED_CCW_PIN, HIGH);
      Serial.println (F("  Red HIGH"));
    }
    else {
      digitalWrite (RED_LED_CCW_PIN, LOW);
      Serial.println (F("  Red LOW"));
    } 
      previousDTstate = currentDTstate;
  } // DT

} // loop


#endif


// ----------------------------------------------------------------------------------------------------------


#if section == 5


const bool COUNT_STATE = LOW;  // Count on the falling edge (HIGH -> LOW). KY-040 rests HIGH
// If the encoder rests with CLK LOW at the detent, use COUNT_STATE = HIGH instead.

int  counter = 0; 
bool currentCLKstate  = HIGH;
bool previousCLKstate = LOW;


void setup() { 
  // Set encoder pins as inputs (KY-040 has pull-ups on CLK and DT)
  pinMode (ROTARY_CLK_PIN, INPUT);
  pinMode (ROTARY_DT_PIN,  INPUT);
  pinMode (ROTARY_DT_PIN,  INPUT);
  // Some version of KY-040 does not use pull-up resistor
  // pinMode (ROTARY_DT_PIN,  INPUT_PULLUP);  // No pull-up on switch
  
  // Set LED pins as outputs
  pinMode (GREEN_LED_CW_PIN, OUTPUT);
  pinMode (RED_LED_CCW_PIN,  OUTPUT);
  
  // Fast turning can miss edges. Use Serial.begin(115200); and set the Serial Monitor to match.
  Serial.begin(9600);
  // Serial.begin(115200);  
  while (!Serial) { delay(10); } // ESP32 Serial Monitor ready delay
  
  // Read the initial state of inputCLK
  previousCLKstate = digitalRead(ROTARY_CLK_PIN);

  Serial.print (F("Arduino siple rotary encoder demonstration. Rest state is: ")); 
  (COUNT_STATE) ? Serial.println (F("HIGH")) : Serial.println(F("LOW"));
  Serial.println (F("Fast turning can miss edges. Use Serial.begin(115200))"));
  Serial.println (F("Direction: CW = ClockWise - CCW = CounterClockWise"));
} 


void loop() { 
  // Push button 
  if (!digitalRead(ROTARY_SW_PIN)) {
     tone(BUZZER_PIN, 3500, 500);
  }
  
  // Read the current state of inputCLK
  currentCLKstate = digitalRead(ROTARY_CLK_PIN);
  
  // If the previous and the current state of the inputCLK are different then a pulse has occurred
  if (currentCLKstate != previousCLKstate  &&  currentCLKstate == COUNT_STATE) { 
    // If the inputDT state is different than the inputCLK state then 
    if (digitalRead(ROTARY_DT_PIN) != currentCLKstate) { 
      // Clockwice
      counter ++;
      digitalWrite (GREEN_LED_CW_PIN, HIGH);
      digitalWrite (RED_LED_CCW_PIN,   LOW);
      Serial.print(F("CW : "));
    } 
    else {
      // Counterclockwise
      counter --;
      digitalWrite (GREEN_LED_CW_PIN, LOW);
      digitalWrite (RED_LED_CCW_PIN,  HIGH);
      Serial.print(F("CCW: "));
    }
    Serial.println(counter);
  } 
  // Update previousCLKstate with the current state
   previousCLKstate = currentCLKstate;
} // loop


#endif


// ----------------------------------------------------------------------------------------------------------


#if section == 6


const bool COUNT_STATE = LOW; //
// Only act on the falling edge (HIGH -> LOW): one count per detent
// If the encoder rests with CLK LOW at the detent, use REST_STATE = HIGH instead. 
// If the direction comes out reversed, swap the CLK and DT wires or swap ++ and --.

int  counter = 0; 
bool currentCLKstate  = HIGH;
bool previousCLKstate = LOW;

// Set encoder pins as inputs (KY-040 has pull-ups on CLK and DT)
MTD2A_binary_input  rotaryCLK ("ROTARY CLK");
MTD2A_binary_input  rotaryDT  ("ROTARY DT");
MTD2A_binary_input  rotarySW  ("ROTARY SW");
// Some version of KY-040 does not use pull-up resistor
// MTD2A_binary_input  rotarySW  ("ROTARY SW",  NORMAL, INPUT_PULLUP);  

MTD2A_binary_output green_CW ("GREEN CW");
MTD2A_binary_output red_CCW  ("RED CCW");


void setup() { 
   
  // Set encoder pins as inputs (KY-040 has pull-ups on CLK and DT)
  rotaryCLK.initialize (ROTARY_CLK_PIN, NORMAL, INPUT);
  rotaryDT.initialize  (ROTARY_DT_PIN,  NORMAL, INPUT);
  rotarySW.initialize  (ROTARY_SW_PIN,  NORMAL, INPUT);
  // Some version of KY-040 does not use pull-up resistor
  // rotarySW.initialize  (ROTARY_SW_PIN,  NORMAL, INPUT_PULLUP);
  
  // Set LED pins as outputs
  green_CW.initialize (GREEN_LED_CW_PIN);
  red_CCW.initialize  (RED_LED_CCW_PIN);
  pinMode (BUZZER_PIN, OUTPUT);
  
  // Fast turning can miss edges. Use Serial.begin(115200); and set the Serial Monitor to match.
  Serial.begin(9600);
  while (!Serial) { delay(10); } // ESP32 Serial Monitor ready delay
  
  MTD2A::set_globalDelayTimeMS (DELAY_1MS); // DELAY_10MS | DELAY_5MS | DELAY_2MS | DELAY_1MS

  // Read the initial state of input CLK
  previousCLKstate = rotaryCLK.get_pinState ();
  Serial.print (F("MTD2A simple rotary encoder demonstration. Rest state is: ")); 
  (COUNT_STATE) ? Serial.println (F("HIGH")) : Serial.println(F("LOW"));
  Serial.println (F("Fast turning can miss edges. Use Serial.begin(115200)); and MTD2A::set_globalDelayTimeMS (DELAY_1MS);"));
  Serial.println (F("Direction: CW = ClockWise - CCW = CounterClockWise"));
} 


void loop() { 
  // Push button 
  if (!rotarySW.get_pinState ()) {
     tone(BUZZER_PIN, 3500, 500);
  }

  // Read the current state of inputCLK
   currentCLKstate = rotaryCLK.get_pinState ();

  // If the previous and the current state of the inputCLK are different then a pulse has occurred
  if (currentCLKstate != previousCLKstate  &&  currentCLKstate == COUNT_STATE) {
 
    // If the inputDT state is different than the inputCLK state then 
    if (rotaryDT.get_pinState () != currentCLKstate) { 
      // Clockwice
      counter ++;
      green_CW.set_pinWriteValue (HIGH);
      red_CCW.set_pinWriteValue   (LOW);
      Serial.print(F("CW : "));      
    } 
    else {
      // Counterclockwise
      counter --;
      green_CW.set_pinWriteValue (LOW);
      red_CCW.set_pinWriteValue (HIGH);
      Serial.print(F("CCW: "));     
    }
    //
    Serial.println(counter);
   } 
   // Update previousCLKstate with the current state
   previousCLKstate = currentCLKstate; 

   MTD2A_loop_execute();
} // loop

#endif


// ----------------------------------------------------------------------------------------------------------


#if section == 7

/*
How this differs from sketch 5: Arduino simple rotary counter. No debounce.
It reads both pins as one state, instead of watching only CLK edges. Invalid transitions are ignored, 
and the count is taken only when the knob is back in a detent, so you get exactly one count per click.

Sources:
https://www.scribd.com/document/262419062/Reading-Rotary-Encoder-on-Arduino-Circuits-Home
https://forum.arduino.cc/t/reading-rotary-encoders-as-a-state-machine/937388
https://github.com/brianlow/Rotary/blob/master/README.md
https://www.best-microcontroller-projects.com/rotary-encoder.html
*/

// Rotary encoder (KY-040 type) – polled quadrature decoder.
// One count per detent, direction-safe, any pins.

// State at rest in a detent: 0b11 = CLK HIGH, DT HIGH (normal for KY-040).
// Use 0b00 if the encoder rests with both pins LOW.
const uint8_t REST_STATE = 0b11;

// Quadrature lookup: index = (previousState << 2) | newState
// +1 / -1 = valid step, 0 = no change or invalid jump (ignored)
const int8_t QUADRATURE_TABLE[16] = {
   0, -1,  1,  0,
   1,  0,  0, -1,
  -1,  0,  0,  1,
   0,  1, -1,  0
};

int counter = 0;
int currentState;
int previousState;
int subStep = 0;


void setup() {
  // Set encoder pins as inputs (KY-040 has pull-ups on CLK and DT)
  pinMode(ROTARY_CLK_PIN, INPUT);
  pinMode(ROTARY_DT_PIN,  INPUT);
  pinMode(ROTARY_SW_PIN,  INPUT);
  // Some version of KY-040 does not use pull-up resistor
  // pinMode(ROTARY_SW_PIN,  INPUT_PULLUP);

  pinMode(GREEN_LED_CW_PIN, OUTPUT);
  pinMode(RED_LED_CCW_PIN,  OUTPUT);

  Serial.begin(9600);
  // Serial.begin(115200); // fast turns
  while (!Serial) { delay(10); } // ESP32 Serial Monitor ready delay

  // Read the initial state of CLK and DT combined (0 to 3) and assign to previousState variable
  previousState = (digitalRead(ROTARY_CLK_PIN) * 2) + digitalRead(ROTARY_DT_PIN);
  // Bit operation: previousState = (digitalRead(ROTARY_CLK_PIN) << 1) | digitalRead(ROTARY_DT_PIN);
  // CLK is shifted one bit left (bit 1), DT stays in bit 0:  0b00, 0b01, 0b10 or 0b11  

  Serial.print ("Arduino rotary encoder demonstration (state table) Rest state is: ");
  if      (REST_STATE == 0b11) Serial.println (F("HIGH"));
  else if (REST_STATE == 0b00) Serial.println (F("LOW"));
  else    Serial.println(F("Unknown rest state"));
  Serial.println (F("Fast turning can miss edges. Use Serial.begin(115200))"));
  Serial.println (F("Direction: CW = ClockWise - CCW = CounterClockWise"));
}


void loop() {
  // Push button 
  if (!digitalRead(ROTARY_SW_PIN)) {
     tone(BUZZER_PIN, 3500, 500);
  }
  // Read the current state of CLK and DT combined (0 to 3)
  currentState = (digitalRead(ROTARY_CLK_PIN) * 2) + digitalRead(ROTARY_DT_PIN);
  // Bit operation: currentState = (digitalRead(ROTARY_CLK_PIN) << 1) | digitalRead(ROTARY_DT_PIN);
  
  // If the previous and the current state are different then the encoder has moved
  if (currentState != previousState) {

    // Add +1, -1 or 0 from the table. Invalid jumps add 0 and are ignored
    subStep = subStep + QUADRATURE_TABLE[(previousState * 4) + currentState];
    // Bit operation: subStep += QUADRATURE_TABLE[(previousState << 2) | currentState];
    // previousState is shifted two bits left (bits 3-2), currentState fills bits 1-0:  0b0000 to 0b1111

    // Only count when the encoder is back at rest in a detent (one count per click)
    if (currentState == REST_STATE) {
      if (subStep > 2) {  // >= 2 for fast turns and low bounce rotary encoders
        // Clockwise
        counter ++;
        digitalWrite (GREEN_LED_CW_PIN, HIGH);
        digitalWrite (RED_LED_CCW_PIN,   LOW);
        Serial.print (F("CW : "));
        Serial.println (counter);
      }
      else if (subStep < -2) { // <= -2 for fast turns and low bounce rotary encoders
        // Counterclockwise
        counter --;
        digitalWrite (GREEN_LED_CW_PIN, LOW);
        digitalWrite (RED_LED_CCW_PIN,  HIGH);
        Serial.print (F("CCW: "));
        Serial.println (counter);
      }
      // Re-align at every detent
      subStep = 0;
    }
    // Update previousState with the current state
    previousState = currentState;
  }
} // loop


#endif


// ----------------------------------------------------------------------------------------------------------


#if section == 8

/*
How this differs from sketch 6: MTD2A simple rotary counter with short timed debounce
It reads both pins as one state, instead of watching only CLK edges. Invalid transitions are ignored, 
and the count is taken only when the knob is back in a detent, so you get exactly one count per click.

Sources:
https://www.scribd.com/document/262419062/Reading-Rotary-Encoder-on-Arduino-Circuits-Home
https://forum.arduino.cc/t/reading-rotary-encoders-as-a-state-machine/937388
https://github.com/brianlow/Rotary/blob/master/README.md
https://www.best-microcontroller-projects.com/rotary-encoder.html
*/

// Rotary encoder (KY-040 type) – polled quadrature decoder.
// One count per detent, direction-safe, any pins.

// State at rest in a detent: 0b11 = CLK HIGH, DT HIGH (normal for KY-040).
// Use 0b00 if the encoder rests with both pins LOW.
const uint8_t REST_STATE = 0b11;

// Quadrature lookup: index = (previousState << 2) | newState
// +1 / -1 = valid step, 0 = no change or invalid jump (ignored)
const int8_t QUADRATURE_TABLE[16] = {
   0, -1,  1,  0,
   1,  0,  0, -1,
  -1,  0,  0,  1,
   0,  1, -1,  0
};

int counter = 0;
int currentState;
int previousState;
int subStep = 0;

// Set encoder pins as inputs (KY-040 has pull-ups on CLK and)
MTD2A_binary_input  rotaryCLK ("ROTARY CLK");
MTD2A_binary_input  rotaryDT  ("ROTARY DT");
MTD2A_binary_input  rotarySW  ("ROTARY SW");
// Some version of KY-040 does not use pull-up resistor
// MTD2A_binary_input  rotarySW  ("ROTARY SW",  NORMAL, INPUT_PULLUP);  

MTD2A_binary_output green_CW ("GREEN CW");
MTD2A_binary_output red_CCW  ("RED CCW");


void setup() {
  // Set encoder pins as inputs (KY-040 has pull-ups on CLK and DT)
  rotaryCLK.initialize (ROTARY_CLK_PIN, NORMAL, INPUT);
  rotaryDT.initialize  (ROTARY_DT_PIN,  NORMAL, INPUT);
  rotarySW.initialize  (ROTARY_SW_PIN,  NORMAL, INPUT);
  // Some version of KY-040 does not use pull-up resistor
  // rotarySW.initialize  (ROTARY_SW_PIN,  NORMAL, INPUT_PULLUP);
  
  // Set LED pins as outputs
  green_CW.initialize (GREEN_LED_CW_PIN);
  red_CCW.initialize  (RED_LED_CCW_PIN);
  pinMode (BUZZER_PIN, OUTPUT);
  
  // Fast turning can miss edges. Use Serial.begin(115200); and set the Serial Monitor to match.
  Serial.begin(9600);
  while (!Serial) { delay(10); } // ESP32 Serial Monitor ready delay
  
  MTD2A::set_globalDelayTimeMS (DELAY_1MS); // DELAY_10MS | DELAY_5MS | DELAY_2MS | DELAY_1MS

  // Read the initial state of CLK and DT combined (0 to 3) and assign to previousState variable
  previousState = (rotaryCLK.get_pinState () * 2) + rotaryDT.get_pinState ();
  // Bit operation: previousState = (rotaryCLK.get_pinState ()) << 1) | rotaryDT.get_pinState ();
  // CLK is shifted one bit left (bit 1), DT stays in bit 0:  0b00, 0b01, 0b10 or 0b11  

  Serial.print ("Arduino rotary encoder demonstration (state table) Rest state is: ");
  if      (REST_STATE == 0b11) Serial.println (F("HIGH"));
  else if (REST_STATE == 0b00) Serial.println (F("LOW"));
  else    Serial.println(F("Unknown rest state"));
  Serial.println (F("Fast turning can miss edges. Use Serial.begin(115200)); and MTD2A::set_globalDelayTimeMS (DELAY_1MS);"));
  Serial.println (F("Direction: CW = ClockWise - CCW = CounterClockWise"));
}


void loop() {
  // Push button 
  if (!rotarySW.get_pinState ()) {
     tone(BUZZER_PIN, 3500, 500);
  }
  // Read the current state of CLK and DT combined (0 to 3)
  currentState = (rotaryCLK.get_pinState () * 2) + rotaryDT.get_pinState ();
  // Bit operation: currentState = (rotaryCLK.get_pinState ()) << 1) | rotaryDT.get_pinState ();
  
  // If the previous and the current state are different then the encoder has moved
  if (currentState != previousState) {

    // Add +1, -1 or 0 from the table. Invalid jumps add 0 and are ignored
    subStep = subStep + QUADRATURE_TABLE[(previousState * 4) + currentState];
    // Bit operation: subStep += QUADRATURE_TABLE[(previousState << 2) | currentState];
    // previousState is shifted two bits left (bits 3-2), currentState fills bits 1-0:  0b0000 to 0b1111

    // Only count when the encoder is back at rest in a detent (one count per click)
    if (currentState == REST_STATE) {
      if (subStep > 2) { // >= 2 for fast turns and low bounce rotary encoders.
        // Clockwise
        counter ++;
        digitalWrite (GREEN_LED_CW_PIN, HIGH);
        digitalWrite (RED_LED_CCW_PIN,   LOW);
        Serial.print (F("CW  : "));
        Serial.println (counter);
      }
      else if (subStep < -2) {  // <= -2 for fast turns and low bounce rotary encoders
        // Counterclockwise
        counter --;
        digitalWrite (GREEN_LED_CW_PIN, LOW);
        digitalWrite (RED_LED_CCW_PIN,  HIGH);
        Serial.print (F("CCW: "));
        Serial.println (counter);
      }
      // Re-align at every detent
      subStep = 0;
    }
    // Update previousState with the current state
    previousState = currentState;
  }

  MTD2A_loop_execute();
} // loop


#endif
