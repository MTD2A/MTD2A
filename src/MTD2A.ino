/*
 ******************************************************************************
 * @file    MTD2A.h
 * @author  Jørgen Bo Madsen
 * @version V1.0
 * @date    15 march 2025
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
#define Yes true
#define yes true
#define No  false
#define no  false
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


class MTD2A {}; // synchronization and cadance control
class MTD2A_delay {};  // milliseconds
class MTD2A_binary_output {}; // on_ms, [on_freq if = 0 -> PWM]
class MTD2A_astable {}; // on_ms, off_ms, [(on_freq, off_freq) if = 0 -> PWM]
class MTD2A_flip_flop {};  // (bistable) set, reset, pulse (monstable)
class MTD2A_passive_buzzer; // ptr frq list, ptr timer list

// -----------------------------------------------------------------------------------


class MTD2A_binary_input
{
  public:
    uint8_t  errorNumber  {0};   // Error number
    bool     debugPrint   {off}; // On

  private:
    // Arguments
    char *   objectName;  // Constructor default (User defined name to display identification using print and debug)
    uint32_t delayTimeMS; // Constructor default (Milliseconds)
    bool     triggerMode; // Constructor default (firstTrigger / lastTrigger)
    bool     timerMode;   // Constructor default (monoStable / timeDelay)
    // pin and input setup
    uint8_t  pinNumber    {255};          // init_setup default
    uint8_t  pinType      {INPUT_PULLUP}; // Open collector or switch to low.
    bool     pinInput     {normal};       // normal / inverted
    bool     inputMode    {pulse};        // Fixed
    // Timers    
    uint32_t firstTimeMS  {0};   // Milliseconds
    uint32_t lastTimeMS   {0};   // Milliseconds
    uint32_t endTimeMS    {0};   // Milliseconds
    // State control
    bool     pinState     {HIGH};         // HIGH / LOW (trigger)
    bool     inputState   {HIGH};         // HIGH / LOW (manual program trigger)
    bool     outputState  {pending};      // Active (trigger activated)
    bool     currState    {HIGH};         // Loop timing states
    bool     lastState    {HIGH};         // Loop timing states
    bool     stateChange  {false};        // HIGH = change in timing state
    bool     inputGoLow   {false};        // Falling edge
    bool     inputGoHigh  {false};        // Rising edge

  public:
    // Constructor inittializers
    MTD2A_binary_input (const char *setObjectName, const uint32_t setDelayTimeMS, 
                        const bool setFirstOrLast, const bool setTimeOrMono);

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
    void init_setup  (const uint8_t &setPinNumber, const bool &setDebugOnOrOff, const bool &setNomalOrInverted);
    void reset_timer ();
    void loop_fast   ();
    void print_conf  ();
    // Setters and getters
    void     set_inputState  (const bool &setInputState, const bool &setPulseOrFixed);
    bool     get_outputState ();
    bool     get_stateChange ();
    uint32_t get_firstTimeMS ();
    uint32_t get_lastTimeMS  ();
    uint32_t get_endTimeMS   ();

  private: // Functions
    void loop_fast_input ();
    void loop_fast_binary();
    void loop_fast_first ();
    void loop_fast_last  ();
    void loop_last_timer ();
    void debug_print     ();
    void active_state    ();
    void pending_state   ();
    void loop_fast_ptr   ();
}; // class MTD2A_binary_input

#endif // _MTD2A_H_


// MTD2A.cpp  ------------------------------------------------------------------------------------------------------------------

// #include "MTD2A.h"

// defaults and setup -----------------------------------------------------------------------------------------------------------

/*
 * @brief Create object and set configuration parameters or use defaults
 * @param MTD2A_binary_input object_name ( "Object Name", {0 - 4294967295}, {firstTrigger | lastTrigger}, {timeDelay | monoStable} );
*/
MTD2A_binary_input::MTD2A_binary_input (const char *setObjectName = "Object name", const uint32_t setDelayTimeMS = 0, 
                                        const bool setFirstOrLast = firstTrigger, const bool setTimeOrMono = timeDelay)
                                        : triggerMode{setFirstOrLast}, delayTimeMS{setDelayTimeMS}, timerMode{setTimeOrMono} 
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
 * @brief Initalize and configure input pin 
 * @param object_name.init_setup ( 0 - NUM_DIGITAL_PINS, {on | off}, {normal | inverted} );
 */
void MTD2A_binary_input::init_setup (const uint8_t &setPinNumber = 255, const bool &setDebugOnOrOff = off, 
                                     const bool &setNomalOrInverted = normal) {
  debugPrint = setDebugOnOrOff;
  pinInput  = setNomalOrInverted;
  if (setPinNumber > NUM_DIGITAL_PINS) {  // github/arduino pins_arduino.h.
    pinNumber = 255;
    if (setPinNumber != 255)
      errorNumber = 1;
  }
  else {
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


void MTD2A_binary_input::set_inputState (const bool &setInputState, const bool &setPulseOrFixed) {
  inputState = setInputState;
  inputMode = setPulseOrFixed;
}


/*
 * @brief Get outpuState {active | pending} 
 * @param none
 */
bool MTD2A_binary_input::get_outputState () {
  return outputState; 
}


/*
 * @brief get StateChange {true | false} 
 * @param none
 */
bool MTD2A_binary_input::get_stateChange () {
  return stateChange;
}


/*
 * @brief Get firstTimeMS Milliseconds 
 * @param none
 */
uint32_t MTD2A_binary_input::get_firstTimeMS () {
  return firstTimeMS;
}


/*
 * @brief Get lastTimeMS Milliseconds
 * @param none
 */
uint32_t MTD2A_binary_input::get_lastTimeMS () {
  return lastTimeMS;
}


/*
 * @brief  Get endTimeMS Milliseconds 
 * @param none
 */
uint32_t MTD2A_binary_input::get_endTimeMS () {
  return endTimeMS;
}


/*
 * @brief update functions and state logic every 1 - 10 millisseconds (delay(10);)
 * @param none.
 */
void MTD2A_binary_input::loop_fast () {
  stateChange = false;
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
  lastState = currState;
} // loop_fast


void MTD2A_binary_input::loop_fast_input () {
  if (pinNumber != 255) {
    pinState = digitalRead(pinNumber);
    if (pinInput == inverted) 
      pinState = !pinState;
  }
  if (pinState == LOW || inputState == LOW)
    currState = LOW;
  else
    currState = HIGH;
  if (inputMode == pulse && inputState == LOW)
    inputState = HIGH;
  //
  inputGoLow  = (lastState == HIGH && currState == LOW);
  inputGoHigh = (lastState == LOW && currState == HIGH);
} // loop_fast_input


void MTD2A_binary_input::loop_fast_binary () {
  if (inputGoLow)
    active_state ();
  if (inputGoHigh)
    pending_state ();
}  // loop_fast_binary


void MTD2A_binary_input::loop_fast_first () {
  if (inputGoLow)
    if (outputState == pending)
      active_state ();
  //
  if (outputState == active) {
    if ((millis() - firstTimeMS) >= delayTimeMS) {
      if (timerMode == monoStable) 
        pending_state ();
      else {
        if (currState == HIGH)
          pending_state ();
      }
    }
  }
} // loop_fast_first


void MTD2A_binary_input::loop_fast_last () {
  if (inputGoLow) {
    if (outputState == pending)
      active_state ();
    if (timerMode == monoStable) {
      lastTimeMS = millis();
      stateChange = true;
      if (debugPrint == on) { 
        debug_print(); Serial.println("Set last time first");
      }
    }
  }
  //
  if (outputState == active)
    loop_last_timer ();
} // loop_fast_last


void MTD2A_binary_input::loop_last_timer () {
  if (timerMode == timeDelay) {
    if (currState == LOW) {
      lastTimeMS = millis();
    }
    if (inputGoHigh) {
      if (debugPrint == on) 
        debug_print(); Serial.println("Set last time");
      stateChange = true;
    }
  }
  //
  if ((millis() - lastTimeMS) >= delayTimeMS) {
    if (timerMode == monoStable) 
      pending_state ();
    else {
      if (currState == HIGH)
        pending_state ();
    }
  }
} // loop_fast_last_timer


/*
 * @brief Reset all timers and state logic. Ready to start.
 * @param none
 */
void MTD2A_binary_input::reset_timer () {
  firstTimeMS = 0;
  lastTimeMS  = 0;
  endTimeMS   = 0;
  inputState  = HIGH;
  outputState = pending;
  currState   = HIGH;
  lastState   = HIGH;
  stateChange = false;
  inputGoLow  = false;
  inputGoHigh = false;
  if (debugPrint == on) {
    debug_print(); Serial.println(F("Reset"));
  }
}  // reset_timer


void MTD2A_binary_input::active_state () {
  outputState = active;
  firstTimeMS = millis();
  lastTimeMS = firstTimeMS;
  stateChange = true;
  if (debugPrint == on) {
    debug_print(); Serial.println(F("Active"));
  }
} // active_state


void MTD2A_binary_input::pending_state () {
  outputState = pending;
  endTimeMS   = millis();
  stateChange = true;
  if (debugPrint == on) {
    debug_print(); Serial.println(F("Pending"));
  }
} // pending_state


void MTD2A_binary_input::print_conf () {
  Serial.println(F("MTD2A_binary_input: "));
  for (size_t i {1}; i < 20; i++) Serial.print(F("-")); Serial.println();
  Serial.print  (F("  objectName  : ")); Serial.println(objectName);
  Serial.print  (F("  pinNumber   : ")); Serial.println(pinNumber);
  Serial.print  (F("  pinType     : ")); if (pinType == INPUT_PULLUP) Serial.println(F("INPUT_PULLUP")); else Serial.println(F("INPUT"));
  Serial.print  (F("  pinInput    : ")); if (pinInput == normal) Serial.println(F("Normal")); else Serial.println(F("Inverted"));
  Serial.print  (F("  inputMode   : ")); if (inputMode == normal) Serial.println(F("Pulse")); else Serial.println(F("Fixed"));
  Serial.print  (F("  triggerMode : ")); if (triggerMode) Serial.println(F("First")); else Serial.println(F("Last"));
  Serial.print  (F("  timerMode   : ")); if (timerMode)   Serial.println(F("Delay")); else Serial.println(F("Mono"));
  Serial.print  (F("  errorNumber : ")); Serial.println(errorNumber);
  // timers
  Serial.print  (F("  delayTimeMS : ")); Serial.println(delayTimeMS);
  Serial.print  (F("  firstTimeMS : ")); Serial.println(firstTimeMS);
  Serial.print  (F("  lastTimeMS  : ")); Serial.println(lastTimeMS);
  Serial.print  (F("  endTimeMS   : ")); Serial.println(endTimeMS);
  // state loggic
  Serial.print  (F("  pinState    : ")); if (pinState)    Serial.println(F("High"));   else Serial.println(F("Low"));
  Serial.print  (F("  inputState  : ")); if (inputState)  Serial.println(F("High"));   else Serial.println(F("Low"));
  Serial.print  (F("  outputState : ")); if (outputState) Serial.println(F("Active")); else Serial.println(F("Pending"));
  Serial.print  (F("  stateChange : ")); if (stateChange) Serial.println(F("True"));   else Serial.println(F("False"));
  Serial.print  (F("  debugPrint  : ")); if (debugPrint)  Serial.println(F("On"));     else Serial.println(F("Off"));
} // print_conf 


void MTD2A_binary_input::debug_print() {
    Serial.print(millis()); Serial.print(F(" - ")); Serial.print(objectName); Serial.print(F(": "));
} // debug_print


// Arduino sketch  --------------------------------------------------------------------



MTD2A_binary_input FC_51_test_1;
MTD2A_binary_input FC_51_test_2  ("TEST_2") ;
MTD2A_binary_input FC_51_test_3  ("TEST_3", 0, firstTrigger);
MTD2A_binary_input FC_51_right   ("Right", 5000, lastTrigger, timeDelay);

// MTD2A_binary_input FC_51_left  ("Left", 0);
// MTD2A_binary_input FC_51_left  ("Left", 10000, firstTrigger, timeDelay);  // OK
   MTD2A_binary_input FC_51_left  ("Left", 10000, lastTrigger,  timeDelay);  // OK
// MTD2A_binary_input FC_51_left  ("Left", 10000, firstTrigger, monoStable); // OK
// MTD2A_binary_input FC_51_left  ("Left", 10000, lastTrigger,  monoStable); // OK


int count {0};

void setup() {
  Serial.begin(250000); 
  FC_51_test_1.init_setup();
  FC_51_test_1.print_conf();

  FC_51_left.init_setup(2, on);
  FC_51_left.print_conf();
//  FC_51_test.print_conf();
//  FC_51_right.print_conf();
}


void loop() {
  bool X = (FC_51_left == FC_51_right);
  //  Serial.println(X);
  if (FC_51_left.get_stateChange()) {
    Serial.print("firstTimeMS: "); Serial.println(FC_51_left.get_firstTimeMS ());
    Serial.print("lastTimeMS : ");  Serial.println(FC_51_left.get_lastTimeMS ());
    Serial.print("endTimeMS  : ");   Serial.println(FC_51_left.get_endTimeMS ());
  }
  FC_51_left.loop_fast();
//  FC_51_right.loop_fast(); 
   delay (100);
   count++;
   if (count == 100) {
    FC_51_left.reset_timer();
    FC_51_left.set_inputState (LOW, pulse);
   }
}





/*

MTD2A_binary_input object_name ( "Object Name", {0 - 4294967295}, {firstTrigger | lastTrigger}, {timeDelay | monoStable} );

object_name.init_setup(Nano pin: 0 - 20, {on | off}, {normal | inverted} ); 

MTD2A_binary_input FC_51_left ("FC-51 left", 5000, lastTrigger, timeDelay);
// "FC-51 left" = Sensor (object) navn, som vises sammen med tilstandsbeskeder.
// 5000 = Tidsforsinkelse i milliseskunder (5 sekunder).
// lastTrigger = Tidspunkt for start på tidstagning. Her sidste impuls i tidsperioden. Alternativt første gang.  
// timeDelay = benyt tidsforsinkelse. Alterntivt monostabilt.

void setup() {
  Serial.begin(250000);  // Nødvendigt og først, hvis der skal vises tilstandsbeskeder.
  FC_51_left.init_setup(2, on); 
  // 2 = Arduino board ben nummer.
  // on = Vis tilstandsbeskeder.
}

void loop() {
  FC_51_left.loop_fast();
  // Opdatere klasseobjektet. Typisk mellem 1 - 10 millisekunder
  delay (10);
}

*/