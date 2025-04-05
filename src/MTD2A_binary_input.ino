/*
 ******************************************************************************
 * @file    MTD2A.h
 * @author  Jørgen Bo Madsen
 * @version V1.1
 * @date    4.april 2025
 * @brief   Abstract Class for MTD2A (Mobile Train Detection And Action)
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Open source free for all</center></h2>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 ******************************************************************************
 */

#ifndef _MTD2A_H_
#define _MTD2A_H_

//Easy understanding
#define yes true
#define Yes true
#define no  false
#define No  false
//
#define enable  true
#define Enable  true
#define disable false
#define Disable false
//
#define on  true
#define On  true
#define off false
#define Off false
//
#define active  true
#define Active  true
#define pending false
#define Pending false
//
#define firstTrigger true
#define FirstTrigger true
#define LastTrigger  false
#define lastTrigger  false
//
#define timeDelay  true
#define TimeDelay  true
#define monoStable false
#define MonoStable false
// 
#define normal true
#define Normal true
#define inverted false
#define Inverted false
//
#define pulse true
#define Pulse true 
#define fixed false
#define Fixed false

// Planlagte klasser:
class MTD2A {}; // synchronization and cadance control
class MTD2A_delay {};  // milliseconds (delayTmeMS, {normal | pulse} )
class MTD2A_binary_output {}; // on_ms, [on_freq if = 0 -> PWM] {timeDelay | monostable}
class MTD2A_astable {}; // highMS, LowMS, [(highFreq, lowFfreq) if != 0 -> PWM], numCycles, NumSubCycles, {pulse | fixed}
class MTD2A_flip_flop {};  // (bistable) set, reset, {normal | pulse} (monstable)
class MTD2A_passive_buzzer; // ptr frq list, ptr timer list

// -----------------------------------------------------------------------------------

#define BIreset    0
#define BIactive   1
#define BIlastTime 2
#define BIblocking 3
#define BIpending  4


class MTD2A_binary_input
{
  public:
    // Debug help text
    const char *phaseText[5] = {"[0] Reset", "[1] Active", "[2] Set last time", "[3] Pin blocking", "[4] Pending"};

  private:
    // Arguments
    char *   objectName;   // Constructor default (User defined name to display identification using print and debug)
    uint32_t delayTimeMS;  // Constructor default (Milliseconds)
    bool     triggerMode;  // Constructor default (firstTrigger / lastTrigger)
    bool     timerMode;    // Constructor default (monoStable / timeDelay)
    uint32_t pinBlockMS;   // Constructor default (Milliseconds)
    // pin and input setup
    uint8_t  pinNumber     {255};          // Init_setup () default
    uint8_t  pinType       {INPUT_PULLUP}; // Open collector or switch to ground (LOW)
    bool     pinRead       {Disable};      // Init_setup () default / enable
    bool     pinInput      {normal};       // set_PinInput () default / inverted
    bool     inputMode     {pulse};        // set_InputState () default / Fixed
    // Other
    bool     debugPrint    {Off};     // set_debugPrint default / on
    uint8_t  errorNumber   {0};       // Error number
    // Timers    
    uint32_t firstTimeMS   {0};       // Milliseconds (firstTrigger)
    uint32_t lastTimeMS    {0};       // Milliseconds (lastTrigger)
    uint32_t endTimeMS     {0};       // Milliseconds (delay end time)
    uint32_t BlockTimeMS   {0};       // Milliseconds (pin input bloscking time)
    // State control
    bool     pinState      {HIGH};    // HIGH / LOW (trigger)
    bool     inputState    {HIGH};    // HIGH / LOW (manual program trigger)
    bool     outputState   {pending}; // Active (trigger activated)
    bool     pinBlockState {off};     // End pin blocking time state
    bool     currentState  {HIGH};    // Loop timing states
    bool     lastState     {HIGH};    // Loop timing states
    bool     phaseChange   {false};   // HIGH = change in timing state
    uint8_t  phaseNumber   {0};       // initialize and reset= 0, active = 1, Set last time = 2, Pin block = 3, Pending = 4 
    bool     inputGoLow    {false};   // Falling edge
    bool     inputGoHigh   {false};   // Rising edge

  public: 
    // Constructor inittializers
    MTD2A_binary_input (const char *setObjectName, const uint32_t setDelayTimeMS, 
                        const bool setFirstOrLast, const bool setTimeOrMono, 
                        const uint32_t setPinBlockMS);

    // Destructor
    ~MTD2A_binary_input() {delete [] objectName;};
 
    // Operator oveloading
    bool operator==(const MTD2A_binary_input &obj) const {
      return (outputState == obj.outputState);
    }
    bool operator!=(const MTD2A_binary_input &obj) const {
      return (outputState != obj.outputState);
    }
    bool operator>(const MTD2A_binary_input &obj) const {
      return (firstTimeMS > obj.firstTimeMS);
    }
    bool operator<(const MTD2A_binary_input &obj) const {
      return (firstTimeMS < obj.firstTimeMS);
    }
    bool operator>>(const MTD2A_binary_input &obj) const {
      return (lastTimeMS > obj.lastTimeMS);
    }
    bool operator<<(const MTD2A_binary_input &obj) const {
      return (lastTimeMS < obj.lastTimeMS);
    }

  public: // Functions
    void init_setup  (const uint8_t &setPinNumber, const bool &setNomalOrInverted);
    void reset_timer ();
    void loop_fast   ();
    void print_conf  ();
    // Setters and getters
    void     set_pinRead     (const bool &setPinRead);
    void     set_pinInput    (const bool &setPinNomalOrInverted);
    void     set_inputState  (const bool &setInputState, const bool &setPulseOrFixed);
    void     set_debugPrint  (const bool &setonOrOff);
    bool     get_outputState ();
    bool     get_phaseChange ();
    uint8_t  get_phaseNumber ();
    bool     get_inputGoLow  ();
    bool     get_inputGoHigh ();
    uint32_t get_firstTimeMS ();
    uint32_t get_lastTimeMS  ();
    uint32_t get_endTimeMS   ();
    uint8_t  get_reset_error ();

  private: // Functions
    void loop_fast_input  ();
    void loop_fast_binary ();
    void loop_fast_first  ();
    void loop_fast_last   ();
    void debug_print      ();
    void active_state     ();
    void ending_state     ();
    void pending_state    ();
    void loop_fast_ptr    ();
}; // class MTD2A_binary_input

#endif // _MTD2A_H_


// MTD2A.cpp  ------------------------------------------------------------------------------------------------------------------

// #include "MTD2A.h"

// defaults and setup -----------------------------------------------------------------------------------------------------------

/*
 * @brief Create object and set configuration parameters or use defaults
 * @name MTD2A_binary_input object_name 
 * @param ( "Object Name", delayTimeMS, {firstTrigger | lastTrigger}, {timeDelay | monoStable}, pinBlocTimeMS );
 * @param delayTimeMS & pinBlockTimeMS {0 - 4294967295} milliseconds
 * @return none
*/
MTD2A_binary_input::MTD2A_binary_input
  (
    const char     *setObjectName = "Object name", 
    const uint32_t setDelayTimeMS = 0, 
    const bool     setFirstOrLast = firstTrigger, 
    const bool     setTimeOrMono  = timeDelay, 
    const uint32_t setPinBlockMS  = 0
  )
  : triggerMode{setFirstOrLast}, delayTimeMS{setDelayTimeMS}, timerMode{setTimeOrMono}, pinBlockMS{setPinBlockMS}
  {
    if (setObjectName == nullptr) {
      objectName = new char[1];
      *objectName = '\0';
    } 
    else {
      objectName = new char[strlen(setObjectName) + 1];
        strcpy(objectName, setObjectName);
    }
  } // objectName
// MTD2A_binary_input class


/*
 * @brief Initalize and configure input pin and dedebug. If setPinNumber > NUM_DIGITAL_PINS, pin reading is disabled! 
 * @name object_name.init_setup 
 * @param ( {0 - NUM_DIGITAL_PINS | 255}, {normal | inverted} );
 * @return none
 */
void MTD2A_binary_input::init_setup (const uint8_t &setPinNumber = 255, const bool &setPinNomalOrInverted = normal) {
  pinInput  = setPinNomalOrInverted;
  if (setPinNumber > NUM_DIGITAL_PINS) {  // github/arduino pins_arduino.h.
    pinRead = disable;
    if (setPinNumber != 255)
      errorNumber = 1;
  }
  else {
    pinRead = enable;
    pinNumber = setPinNumber;
    pinMode(pinNumber, pinType);
  }
} // init_setup


// Functions -----------------------------------------------------------------------------------------------------------------


void MTD2A_binary_input::loop_fast_ptr () {
    void (MTD2A_binary_input::*loop_pointer) () = &MTD2A_binary_input::loop_fast;
    (this->*loop_pointer) ();
//    typedef void (MTD2A_binary_input::*loop_pointer) ();
//    loop_pointer LP =  &MTD2A_binary_input::loop_fast;
//    (this->*LP) ();
}


/*
 * @brief Enable or disable pin reading.
 * @name object_name.set_pinRead 
 * @param ( {enable | disable} );
 * @return none
 */
void MTD2A_binary_input::set_pinRead (const bool &setPinRead = enable) {
  pinRead = setPinRead;
}


/*
 * @brief Configure pin input trigger mode = LOW or HIGH (inverted)
 * @name object_name.set_pinInput
 * @param ( {normal | inverted} );
 * @return non
 */
void MTD2A_binary_input::set_pinInput (const bool &setPinNomalOrInverted = normal) {
  pinInput  = setPinNomalOrInverted;
}


/*
 * @brief Activate input state and set input mode
 * @name object_name.set_pinState
 * @param ( {HIGH | LOW}, {pulse | fixed} );
 * @return none
 */
void MTD2A_binary_input::set_inputState (const bool &setInputState = LOW, const bool &setPulseOrFixed = pulse) {
  inputState = setInputState;
  inputMode = setPulseOrFixed;
}


/*
 * @brief activate print state information (Pending, Active etc).
 * @name object_name.set_debugPrint
 * @param ( {on | off} );
 * @return none
 */
void MTD2A_binary_input::set_debugPrint (const bool &setOnOrOff = Off) {
  debugPrint = setOnOrOff;
}


/*
 * @brief Get outputState  
 * @name object_name.get_outputState ();
 * @param none
 * @return bool {active | pending}
 */
bool MTD2A_binary_input::get_outputState () {
  return outputState; 
}


/*
 * @brief get phaseChange 
 * @name object_name.get_phaseChange ();
 * @param none
 * @return bool {true | false}
 */
bool MTD2A_binary_input::get_phaseChange () {
  return phaseChange;
}


/*
 * @brief get phase number. 
 * @name object_name.get_phaseNumber ();
 * @param none
 * @return uint8_t Initialize and reset = 0, active = 1, Set last time = 2, Pin block = 3, Pending = 4
 */
uint8_t MTD2A_binary_input::get_phaseNumber () {
  return phaseNumber;
}


/*
 * @brief Get firstTimeMS
 * @name object_name.get_firstTimeMS ();
 * @param none
 * @return uint32_t Milliseconds 
 */
uint32_t MTD2A_binary_input::get_firstTimeMS () {
  return firstTimeMS;
}


/*
 * @brief Get lastTimeMS
 * @name object_name.get_lastTimeMS ();
 * @param none
 * @return uint32_t Milliseconds
 */
uint32_t MTD2A_binary_input::get_lastTimeMS () {
  return lastTimeMS;
}


/*
 * @brief  Get endTimeMS Milliseconds 
 * @name object_name.get_endTimeMS ();
 * @param none
 * @return uint32_t Milliseconds
 */
uint32_t MTD2A_binary_input::get_endTimeMS () {
  return endTimeMS;
}


/*
 * @brief get inputGoLow
 * @name object_name.get_inputGoLow ();
 * @param none
 * @return bool {true | false} 
 */
bool MTD2A_binary_input::get_inputGoLow () {
  return inputGoLow;
}


/*
 * @brief get inputGoHigh
 * @name object_name.get_inputGoHigh ();
 * @param none
 * @return bool {true | false}
 */
bool MTD2A_binary_input::get_inputGoHigh () {
  return inputGoHigh;
}


/*
 * @brief  Get and clear error number 
 * @name object_name.get_reset_error ();
 * @param none
 * @return uint8_t Error number
 */
uint8_t MTD2A_binary_input::get_reset_error () {
  uint8_t tempErrorNumber;
  tempErrorNumber = errorNumber;
  errorNumber = 0;
  return tempErrorNumber;
}


/*
 * @brief update functions and state logic every 10 millisseconds (delay(10);)
 * @name object_name.loop_fast ();
 * @param none
 * @return none
 */
void MTD2A_binary_input::loop_fast () {
  phaseChange = false;
  loop_fast_input ();
  if (delayTimeMS == 0) {
    loop_fast_binary ();
  }
  else {
    if (triggerMode == FirstTrigger) {
      loop_fast_first ();
    }
    else { // lastTrigger
      loop_fast_last ();
    }
  }
  lastState = currentState;
} // loop_fast


void MTD2A_binary_input::loop_fast_input () {
  if (pinRead == enable) {
    pinState = digitalRead(pinNumber);
    if (pinInput == inverted) 
      pinState = !pinState;
  }
  if (pinState == LOW || inputState == LOW)
    currentState = LOW;
  else
    currentState = HIGH;
  if (inputMode == pulse && inputState == LOW)
    inputState = HIGH;
  //
  inputGoLow  = (lastState == HIGH && currentState == LOW);
  inputGoHigh = (lastState == LOW && currentState == HIGH);
} // loop_fast_input


void MTD2A_binary_input::loop_fast_binary () {
  if (inputGoLow)
    active_state ();
  if (inputGoHigh)
    ending_state ();
}  // loop_fast_binary


void MTD2A_binary_input::loop_fast_first () {
  if (inputGoLow)
    if (outputState == pending)
      active_state ();
  //
  if (outputState == active) {
    if ((millis() - firstTimeMS) >= delayTimeMS) {
      if (timerMode == monoStable) 
        ending_state ();
      else {
        if (currentState == HIGH)
          ending_state ();
      }
    }
  }
} // loop_fast_first


void MTD2A_binary_input::loop_fast_last () {
  if (inputGoLow) {
    if (outputState == pending)
      active_state ();
  }
  //
  if (outputState == active) {
    if (inputGoHigh) {
      phaseChange = true;
      phaseNumber = BIlastTime;
      lastTimeMS = millis();
      if (debugPrint == on) 
        debug_print(); Serial.println(phaseText[phaseNumber]); //set last time
    }
    //
    if ((millis() - lastTimeMS) >= delayTimeMS) {
      if (timerMode == monoStable) 
        ending_state ();
      else {
        if (currentState == HIGH)
          ending_state ();
      }
    }
  }
} // loop_fast_last


/*
 * @brief Reset all timers and state logic. Ready to start.
 * @name object_name.reset_timer ();
 * @param none
 * @return none
 */
void MTD2A_binary_input::reset_timer () {
  firstTimeMS   = 0;
  lastTimeMS    = 0;
  endTimeMS     = 0;
  BlockTimeMS   = 0;
  pinState      = HIGH;
  inputState    = HIGH;
  outputState   = pending;
  currentState  = HIGH;
  lastState     = HIGH;
  pinBlockState = off;
  phaseChange   = false;
  phaseNumber   = BIreset; 
  inputGoLow    = false;
  inputGoHigh   = false;
  if (debugPrint == on) {
    debug_print(); Serial.println(phaseText[phaseNumber]);  // reset
  }
}  // reset_timer


void MTD2A_binary_input::active_state () {
  outputState = active;
  firstTimeMS = millis();
  lastTimeMS = firstTimeMS;
  phaseChange = true;
  phaseNumber = BIactive;
  if (debugPrint == on) {
    debug_print(); Serial.println(phaseText[phaseNumber]); //active
  }
} // active_state


void MTD2A_binary_input::ending_state () {
  if (pinBlockMS > 0) {
    if (pinBlockState == off) {
      pinBlockState = on;
      pinRead = disable;
      BlockTimeMS = millis();
      phaseChange = true;
      phaseNumber = BIblocking;
      if (debugPrint == on) {
        debug_print(); Serial.println(phaseText[phaseNumber]); // pin blocking
      }
    }
    if ((millis() - BlockTimeMS) >= pinBlockMS) {
      pinBlockState = off;
      pinRead = enable;
      pending_state ();
    }
  }
  else
    pending_state ();    
} // ending_state


void MTD2A_binary_input::pending_state () {
  outputState = pending;
  endTimeMS   = millis();
  phaseChange = true;
  phaseNumber = BIpending;
  if (debugPrint == on) {
    debug_print(); Serial.println(phaseText[phaseNumber]); // pending
  }
} // ending_state

/*
 * @brief print configuration parameters, timers and state logic.
 * @name object_name.print_cont ();
 * @param none
 * @return none
 */
void MTD2A_binary_input::print_conf () {
  Serial.println(F("MTD2A_binary_input: "));
  for (size_t i {1}; i < 20; i++) Serial.print(F("-")); Serial.println();
  Serial.print  (F("  objectName  : ")); Serial.println(objectName);
  Serial.print  (F("  triggerMode : ")); if (triggerMode == firstTrigger) Serial.println(F("First")); else Serial.println(F("Last"));
  Serial.print  (F("  timerMode   : ")); if (timerMode == timeDelay)      Serial.println(F("Delay")); else Serial.println(F("Mono"));
  // Setup
  Serial.print  (F("  pinNumber   : ")); Serial.print(pinNumber); 
  if (pinNumber == 255) Serial.println(F(" (Program input)")); else Serial.println(F(" (Digital input)"));
  Serial.print  (F("  pinType     : ")); if (pinType == INPUT_PULLUP) Serial.println(F("INPUT_PULLUP")); else Serial.println(F("INPUT"));
  Serial.print  (F("  pinRead     : ")); if (pinRead == enable)       Serial.println(F("Enable"));   else Serial.println(F("Disable"));
  Serial.print  (F("  pinInput    : ")); if (pinInput == normal)      Serial.println(F("Normal")); else Serial.println(F("Inverted"));
  Serial.print  (F("  inputMode   : ")); if (inputMode == pulse)      Serial.println(F("Pulse")); else Serial.println(F("Fixed"));
  // Other
  Serial.print  (F("  errorNumber : ")); Serial.println(errorNumber);
  Serial.print  (F("  debugPrint  : ")); if (debugPrint == on) Serial.println(F("On"));     else Serial.println(F("Off"));
  // timers
  Serial.print  (F("  delayTimeMS : ")); Serial.println(delayTimeMS);
  Serial.print  (F("  firstTimeMS : ")); Serial.println(firstTimeMS);
  Serial.print  (F("  lastTimeMS  : ")); Serial.println(lastTimeMS);
  Serial.print  (F("  endTimeMS   : ")); Serial.println(endTimeMS);
  Serial.print  (F("  BlockTimeMS : ")); Serial.println(BlockTimeMS);
  // state loggic
  Serial.print  (F("  pinState    : ")); if (pinState == HIGH)      Serial.println(F("High"));   else Serial.println(F("Low"));
  Serial.print  (F("  inputState  : ")); if (inputState == HIGH)    Serial.println(F("High"));   else Serial.println(F("Low"));
  Serial.print  (F("  outputState : ")); if (outputState == active) Serial.println(F("Active")); else Serial.println(F("Pending"));
  Serial.print  (F("  phaseChange : ")); if (phaseChange == true)   Serial.println(F("True"));   else Serial.println(F("False"));
  Serial.print  (F("  phaseNumber : ")); Serial.println(phaseText[phaseNumber]); 
} // print_conf 


void MTD2A_binary_input::debug_print() {
    Serial.print(millis()); Serial.print(F(" - ")); Serial.print(objectName); Serial.print(F(": "));
} // debug_print


// Arduino sketch TEST TEST TEST  --------------------------------------------------------------------


/*
MTD2A_binary_input FC_51_test_1;
MTD2A_binary_input FC_51_test_2  ("TEST_2") ;
MTD2A_binary_input FC_51_test_3  ("TEST_3", 0, firstTrigger);
MTD2A_binary_input FC_51_right   ("Right", 5000, lastTrigger, timeDelay);

// MTD2A_binary_input FC_51_left ("Left", 0);
// MTD2A_binary_input FC_51_left ("Left", 10000, firstTrigger, timeDelay);  // OK
// MTD2A_binary_input FC_51_left ("Left", 10000, lastTrigger,  timeDelay);  // OK
// MTD2A_binary_input FC_51_left ("Left", 10000, firstTrigger, monoStable); // OK
// MTD2A_binary_input FC_51_left ("Left", 10000, lastTrigger,  monoStable); // OK

// MTD2A_binary_input FC_51_left ("Left", 10000, firstTrigger, timeDelay, 5000);  // OK
// MTD2A_binary_input FC_51_left ("Left", 10000, lastTrigger,  timeDelay, 5000);  // OK
// MTD2A_binary_input FC_51_left ("Left", 10000, firstTrigger, monoStable, 5000); // OK
// MTD2A_binary_input FC_51_left ("Left", 10000, lastTrigger,  monoStable, 5000); // OK

int count {0};

void setup() {
  Serial.begin(250000); 
  FC_51_test_1.init_setup();
  FC_51_test_1.set_debugPrint(on);
  FC_51_test_1.print_conf();

  FC_51_left.init_setup(2);
  FC_51_left.set_debugPrint(on);
  FC_51_left.print_conf();
//  FC_51_test.print_conf();
//  FC_51_right.print_conf();
}


void loop() {
  bool X = (FC_51_left == FC_51_right);
  //  Serial.println(X);
  if (FC_51_left.get_phaseChange()) {
//    Serial.print("\tphaseNumber: "); Serial.println(FC_51_left.get_phaseNumber());
  }
  FC_51_left.loop_fast();
//  FC_51_right.loop_fast(); 
   delay (100);
   count++;
   if (count == 100) {
    FC_51_left.reset_timer();
   // FC_51_left.set_inputState (LOW, pulse);
   }
}
*/

/*
// Eksample stoplight_sound.ino  ------------------------------------------------------------------------------- 

// Stop light and sound message: The train brakes and temporarily stops at a red light.

MTD2A_binary_input FC_51_stop ("Stop",  10000, lastTrigger, timeDelay);   // continous active time (no drop outs)
MTD2A_binary_input wait_time  ("Sound",  5000, firstTrigger, monoStable); // Mono pulse and no pin assignment
MTD2A_binary_input sound_time ("Time",   3000, firstTrigger, monoStable); // Mono pulse and no pin assignment

int soundPin = 9; // Danish and english speech from https://ttsmaker.com/

void setup() {
  Serial.begin(250000); 
  FC_51_stop.init_setup (2);  // input pin 2
  // Debug information
  FC_51_stop.set_debugPrint  (on);
  wait_time.set_debugPrint (on);
  sound_time.set_debugPrint (on);
  // Output
  pinMode (soundPin, OUTPUT);

  bool trainSoundState = false;
}

void loop() {
  if (FC_51_stop.get_outputState() == active ) {
    // Continous activation (no drop outs)
    if (FC_51_stop.get_phaseChange() == true  &&  FC_51_stop.get_phaseNumber() == BIactive) {
      wait_time.set_inputState(LOW);
      wait_time.loop_fast();  // updtate setting (active)
    }
    // Waiting time before sound activation
    if (wait_time.get_phaseChange() == true  &&  wait_time.get_phaseNumber() == BIpending) {
      digitalWrite(soundPin, HIGH);  // write once
      Serial.println("Start sound track");
      sound_time.set_inputState(LOW);
    }
    // Sound duration
    if (sound_time.get_phaseChange() == true  &&  sound_time.get_phaseNumber() == BIpending) {
      digitalWrite(soundPin, LOW);  // write once
      Serial.println("End sound track");
    }
  }

  FC_51_stop.loop_fast();
  wait_time.loop_fast();
  sound_time.loop_fast();
  delay (10);
} // Stop light and sound message: The train brakes and temporarily stops at a red light.
*/


/*
// Example train_direction.ino ----------------------------------------------------------------------------- 

// Determining the direction of travel of the train

MTD2A_binary_input FC_51_left  ("Left", 5000, lastTrigger, timeDelay);
MTD2A_binary_input FC_51_right ("Right", 5000, lastTrigger, timeDelay);

bool directionActive = false;
int redLEDpin   = 9;
int greenLEDpin = 10;

void setup() {
  Serial.begin(250000); 
  FC_51_left.init_setup  (2);  // input pin 2 (FC-51 left)
  FC_51_right.init_setup (3);  // input pin 3 (Fc-51 right)
  FC_51_left.set_debugPrint  (on);
  FC_51_right.set_debugPrint (on);
  pinMode (redLEDpin, OUTPUT); 
  pinMode (greenLEDpin, OUTPUT);
}

void loop() {
  if (FC_51_left.get_outputState() == active  &&  FC_51_right.get_outputState() == pending) {
    // From left;
    if (directionActive == false) {
      digitalWrite(redLEDpin, HIGH);  // write once
      Serial.println("Moving left");
      directionActive = true;
    }
  }
  else
    if (FC_51_left.get_outputState() == pending  &&  FC_51_right.get_outputState() == active) {
      // From right;
      if (directionActive == false) {
        digitalWrite(greenLEDpin, HIGH);  // write once
        Serial.println("Moving rigth");
        directionActive = true;
      }
    }

 if (FC_51_left.get_outputState() == pending  &&  FC_51_right.get_outputState() == pending  &&  directionActive == true) {
    // Keep output activated for 5 seconds
    Serial.println("Turn off red and green LED");
    digitalWrite(redLEDpin, LOW);
    digitalWrite(greenLEDpin, LOW);
    directionActive = false;
  }

  FC_51_left.loop_fast();
  FC_51_right.loop_fast();
  delay (10);
} // Determining the direction of travel of the train
*/



// Eksample switch_direction.ino ------------------------------------------------------------------------ 

// Automatic switch to the direction the train is coming from (to avoid derailment if the switch direction is wrong)
// Standard LGB switch (turnout) with position switches for feedback is used

// Track sensor pin input
MTD2A_binary_input FC_51_left     ("Left track sensor",  10000, lastTrigger, timeDelay);  // 10 seconds
MTD2A_binary_input FC_51_right    ("Right track sensor", 10000, lastTrigger, timeDelay);  // 10 seconds
// Send 0,5 seconds pulse to switch motor
MTD2A_binary_input motor_left     ("Left switch motor", 500, firstTrigger, monoStable);  // Mono pulse and no pin assignment
MTD2A_binary_input motor_right    ("Right swith motor", 500, firstTrigger, monoStable);  // Mono pulse and no pin assignment
// read current switch position 
MTD2A_binary_input position_left  ("Left feedback position");
MTD2A_binary_input position_right ("Right feedback position");

int RailSwitchLeft   = 9; 
int RailSwitchRight = 10;

bool trackShiftState = false;

void setup() {
  Serial.begin(250000); 
  FC_51_left.init_setup     (2);  // input pin 2
  FC_51_right.init_setup    (3);  // input pin 3
  position_left.init_setup  (4);  // input pin 4 
  position_right.init_setup (5);  // input pin 5

  // Debug print - show what is going on
  FC_51_left.set_debugPrint     (on);
  FC_51_right.set_debugPrint    (on);
  motor_left.set_debugPrint     (on);
  motor_right.set_debugPrint    (on);
  position_left.set_debugPrint  (on);
  position_right.set_debugPrint (on);
  // Switch (turnout)motor
  pinMode (RailSwitchLeft, OUTPUT); 
  pinMode (RailSwitchRight, OUTPUT);
  digitalWrite (RailSwitchLeft, LOW); 
  digitalWrite (RailSwitchRight, LOW);
}

void loop() {
  if (FC_51_left.get_outputState() == active) {
    if (position_right.get_outputState() == active) {
      trackShiftState = true;
      motor_left.set_inputState(LOW); // Activate track switch motor for 500 milliseconds
      // Write only once (a loop) to avoid "flicker"
      if (motor_left.get_outputState() == active  &&  motor_left.get_phaseChange() == true) {
        digitalWrite (RailSwitchLeft, HIGH);
        Serial.println("Move track switch to the left");
      }
      // Write only once (a loop) to avoid "flicker"
      if (motor_left.get_outputState() == pending  &&  motor_left.get_phaseChange() == true)
        digitalWrite (RailSwitchLeft, LOW);
    }
  }
  else {
    if (trackShiftState == true) {
      motor_right.set_inputState(LOW); // Activate track switch motor for 500 millisecond
      // Write only once (a loop) to avoid "flicker"
      if (motor_right.get_outputState() == active  &&  motor_right.get_phaseChange() == true) {
        digitalWrite (RailSwitchRight, HIGH);
        Serial.println("Move track switch back to the right");
      }
      // Write only once (a loop) to avoid "flicker"
      if (motor_right.get_outputState() == pending  &&  motor_right.get_phaseChange() == true) {
        digitalWrite (RailSwitchRight, LOW);
        trackShiftState = false;  // last command in the process
      }
    }
  }

  // Track Switch to the right is identical to Track Switch on the left, just mirrored. Not shown here.

  FC_51_left.loop_fast     ();
  FC_51_right.loop_fast    ();
  motor_left.loop_fast     ();
  motor_right.loop_fast    ();
  position_left.loop_fast  ();
  position_right.loop_fast ();

  delay (10);
} // Automatic track switch to the direction the train is coming from




/*
// hjælpetekster til manualen

 MTD2A_binary_input object_name ( "Object Name", delayTimeMS, {firstTrigger | lastTrigger}, {timeDelay | monoStable}, pinBlockTimeMS );

 delayTimeMS & pinBlockTimeMS {0 - 4294967295} milliseconds

 object_name.init_setup ( pinNumber, {normal | inverted} ); 

 0 - NUM_DIGITAL_PINS

 object_name.loop_fast ();

// Input
 object_name.set_pinRead ( {enable | disable} ); 

 object_name.set_pinInput ( {nomal | inverted} );

 object_name.set_inputState ( {HIGH | LOW}, {pulse | fixed} );

//
  MTD2A_binary_input FC_51_sensor (”FC_51_sensor”);

  FC_51_sensor.init_setup (DigitalPin_2, inverted);

  MTD2A_binary_input FC_51_sensor (”FC_51_sensor”, 5000);

  FC_51_sensor.init_setup (DigitalPin_2);

  MTD2A_binary_input FC_51_sensor (”FC_51_sensor”, 5000, lastTrigger);

  FC_51_sensor.set_debugPrint (on);

  MTD2A_binary_input FC_51_sensor (”FC_51_sensor”, 5000, firstTrigger, monoStable, 12000);

  MTD2A_binary_input FC_51_sensor (”FC_51_sensor”, 3000, lastTrigger, monoStable);

MTD2A_binary_input FC_51_left ("FC-51 left", 5000, lastTrigger, timeDelay);
// "FC-51 left" = Sensor (object) navn, som vises sammen med tilstandsbeskeder.
// 5000 = Tidsforsinkelse i milliseskunder (5 sekunder).
// lastTrigger = Tidspunkt for start på tidstagning. Her sidste impuls i tidsperioden (LOW->HIGH) 
// timeDelay = Benyt tidsforsinkelse (timer funktion)

void setup () {
  Serial.begin (250000);  // Nødvendigt og først, hvis der skal vises tilstandsbeskeder.
  FC_51_left.init_setup (DigitalPin_2); 
  // DigitalPin_2 = Arduino board ben nummer.
  FC_51_left.set_debugPrint (on); 
  // on = Vis tilstandsbeskeder.
}

void loop () {
  FC_51_left.loop_fast();
  // Opdatere klasseobjektet. Typisk 10 millisekunder
  delay (10);
}

*/