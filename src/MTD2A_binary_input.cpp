/*
 ******************************************************************************
 * @file    MTD2A_binary_input.cpp
 * @author  Joergen Bo Madsen
 * @version V1.1.4
 * @date    30. maj 2025
 * @brief   functions for MTD2A_binary_input.h (Model Train Detection And Action)
 * 
 * Supporting a vast variety of input sensors and output devices 
 * Simple to use to build complex solutions 
 * Non blocking, simple, yet efficient event-driven state machine
 * Comprehensive control, state and debug information
 *
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


#include "Arduino.h"
#include "MTD2A_base.h"
#include "MTD2A_binary_input.h"


// Constructor
MTD2A_binary_input::MTD2A_binary_input
  (
    const char    *setObjectName, 
    const uint32_t setDelayTimeMS, 
    const bool     setFirstOrLast, 
    const bool     setTimeOrMono, 
    const uint32_t setPinBlockMS
  )
  : delayTimeMS{setDelayTimeMS}, 
    triggerMode{setFirstOrLast}, 
    timerMode{setTimeOrMono}, 
    pinBlockMS{setPinBlockMS},
    // Instatiated funtion pointer
    MTD2A{[](MTD2A* funcPtr) { static_cast<MTD2A_binary_input*>(funcPtr)->loop_fast(); }}
  {
    MTD2A_add_function_pointer_loop_fast(this);
    objectName = MTD2A_set_object_name(setObjectName);
    if (delayTimeMS == 0)
      errorNumber = 130;  // Warning
  }
// MTD2A_binary_input


void MTD2A_binary_input::initialize (const uint8_t &setPinNumber, const bool &setPinNomalOrInverted, const uint8_t &setPinPullupOrInput) {
  // Check for instantiated object error
  if (errorNumber > 0)
    MTD2A_print_error_text (debugPrint, errorNumber, pinNumber);
  errorNumber = MTD2A_reserve_and_check_pin (setPinNumber, DIGITAL_FLAG_0 | INPUT_FLAG_2 | PULLUP_FLAG_3);
  if (errorNumber == 0) {
    pinRead = ENABLE;
    pinNumber = setPinNumber;
    pinInput = setPinNomalOrInverted;
    if (setPinPullupOrInput == INPUT  ||  setPinPullupOrInput == INPUT_PULLUP) 
      pinType = setPinPullupOrInput;
    else {
      errorNumber = 8; MTD2A_print_error_text (debugPrint, errorNumber, pinNumber);
      pinType = INPUT_PULLUP;
    }
    pinMode(pinNumber, pinType);
  }
  else {
    pinRead = DISABLE;
    pinNumber = PIN_ERROR_NO;
    MTD2A_print_error_text (debugPrint, errorNumber, pinNumber);
  }
} // initialize


void MTD2A_binary_input::set_pinRead (const bool &setPinEnableOrDisable, const bool &LoopFastOnce) {
  if (setPinEnableOrDisable == ENABLE  &&  pinNumber == PIN_ERROR_NO) {
    errorNumber = 1; MTD2A_print_error_text (debugPrint, errorNumber, pinNumber);  
  }
  else {
    pinRead = setPinEnableOrDisable;
    if (LoopFastOnce == ENABLE)
      loop_fast();
  }
} // set_pinRead


void MTD2A_binary_input::set_pinInput (const bool &setPinNomalOrInverted, const bool &LoopFastOnce) {
  if (pinNumber != PIN_ERROR_NO) {
    pinInput = setPinNomalOrInverted;
    if (LoopFastOnce == ENABLE)
      loop_fast();
  }
  else {
    errorNumber = 1; MTD2A_print_error_text (debugPrint, errorNumber, pinNumber);
  }
} // set_pinInput


void MTD2A_binary_input::set_inputState (const bool &setInputLowOrHigh, const bool &setPulseOrFixed, const bool &LoopFastOnce) {
  inputState = setInputLowOrHigh;
  inputMode = setPulseOrFixed;
  if (LoopFastOnce == ENABLE)
    loop_fast();
} // set_inputState


void MTD2A_binary_input::set_stopDelayTimer (const bool &LoopFastOnce) {
  stopDelayTM = ENABLE;
  if (LoopFastOnce == ENABLE)
    loop_fast();
} // et_stopDelayTimer


void MTD2A_binary_input::set_stopBlockTimer (const bool &LoopFastOnce) {
  stopBlockTM = ENABLE;
  if (LoopFastOnce == ENABLE)
    loop_fast();
} // set_stopBlockTimer


void MTD2A_binary_input::set_debugPrint (const bool &setENABLEOrDisable, const bool &LoopFastOnce) {
  debugPrint = setENABLEOrDisable;
  if (LoopFastOnce == ENABLE)
    loop_fast();
} // set_debugPrint 


bool const &MTD2A_binary_input::get_processState () const {
  return processState; 
}


bool const &MTD2A_binary_input::get_pinState () const {
  return pinState; 
}


bool const &MTD2A_binary_input::get_phaseChange () const {
  return phaseChange;
}


uint8_t const &MTD2A_binary_input::get_phaseNumber () const {
  return phaseNumber;
}


uint32_t const &MTD2A_binary_input::get_firstTimeMS () const {
  return firstTimeMS;
}


uint32_t const &MTD2A_binary_input::get_lastTimeMS () const {
  return lastTimeMS;
}


uint32_t const &MTD2A_binary_input::get_endTimeMS () const {
  return endTimeMS;
}


bool const &MTD2A_binary_input::get_inputGoLow () const {
  return inputGoLow;
}


bool const &MTD2A_binary_input::get_inputGoHigh () const {
  return inputGoHigh;
}


uint8_t const &MTD2A_binary_input::get_reset_error () {
  static uint8_t tempErrorNumber;
  tempErrorNumber = errorNumber;
  errorNumber = 0;
  return tempErrorNumber;
} // get_reset_error


void MTD2A_binary_input::loop_fast () {
  phaseChange = false;
  loop_fast_input ();
  if (delayTimeMS == 0) {
    loop_fast_binary ();
  }
  else {
    if (triggerMode == LAST_TRIGGER) {
      loop_fast_last ();
    }
    else { // FirstTrigger
      loop_fast_first ();
    }
  }
  lastState = currentState;
} // loop_fast


void MTD2A_binary_input::loop_fast_input () {
  if (pinRead == ENABLE) {
    pinState = digitalRead(pinNumber);
    if (pinInput == INVERTED) 
      pinState = !pinState;
  }
  if (pinState == LOW || inputState == LOW)
    currentState = LOW;
  else
    currentState = HIGH;
  if (inputMode == PULSE && inputState == LOW)
    inputState = HIGH;
  //
  inputGoLow  = (lastState == HIGH && currentState == LOW);
  inputGoHigh = (lastState == LOW && currentState == HIGH);
} // loop_fast_input


void MTD2A_binary_input::loop_fast_binary () {
  if (inputGoLow)
    begin_state ();
  if (inputGoHigh)
    end_state ();
}  // loop_fast_binary


void MTD2A_binary_input::loop_fast_first () {
  if (inputGoLow)
    if (processState == COMPLETE)
      begin_state ();
  //
  if (processState == ACTIVE) {
    currentTimeMS = millis();
    if ((currentTimeMS - firstTimeMS) >= delayTimeMS  ||  stopDelayTM == ENABLE) {
      stopDelayTM = DISABLE;
      if (timerMode == MONO_STABLE) 
        end_state ();
      else {
        if (currentState == HIGH)
          end_state ();
      }
    }
  }
} // loop_fast_first


void MTD2A_binary_input::loop_fast_last () {
  if (inputGoLow) {
    if (processState == COMPLETE)
      begin_state ();
  }
  //
  if (processState == ACTIVE) {
    if (inputGoHigh) {
      phaseChange = true;
      phaseNumber = LAST_TIME_PHASE;
      lastTimeMS  = millis();
      MTD2A_print_phase_line (debugPrint, objectName, phaseText[LAST_TIME_PHASE]);
    }
    //
    currentTimeMS = millis();
    if ((currentTimeMS - lastTimeMS) >= delayTimeMS  ||  stopDelayTM == ENABLE) {
      stopDelayTM = DISABLE;
      if (timerMode == MONO_STABLE) 
        end_state ();
      else {
        if (currentState == HIGH)
          end_state ();
      }
    }
  }
} // loop_fast_last


void MTD2A_binary_input::begin_state () {
  processState = ACTIVE;
  firstTimeMS  = millis();
  lastTimeMS   = firstTimeMS;
  phaseChange  = true;
  phaseNumber  = FIRST_TIME_PHASE;
  MTD2A_print_phase_line (debugPrint, objectName, phaseText[FIRST_TIME_PHASE]);
} // begin_state


void MTD2A_binary_input::end_state () {
  if (pinBlockMS > 0) {
    if (pinBlockState == DISABLE) {
      pinBlockState = ENABLE;
      pinRead       = DISABLE;
      blockTimeMS   = millis();
      phaseChange   = true;
      phaseNumber   = BLOCKING_PHASE;
      MTD2A_print_phase_line (debugPrint, objectName, phaseText[BLOCKING_PHASE]); 
    }
    currentTimeMS = millis();
    if ((currentTimeMS - blockTimeMS) >= pinBlockMS  ||  stopBlockTM == ENABLE) {
      stopBlockTM = DISABLE;
      pinBlockState = DISABLE;
      pinRead = ENABLE;
      complete_state ();
    }
  }
  else
    complete_state ();    
} // ending_state


void MTD2A_binary_input::complete_state () {
  processState = COMPLETE;
  endTimeMS    = millis();
  phaseChange  = true;
  phaseNumber  = COMPLETE_PHASE;
  MTD2A_print_phase_line (debugPrint, objectName, phaseText[COMPLETE_PHASE]);
} // ending_state


void MTD2A_binary_input::reset () {
  firstTimeMS   = 0;
  lastTimeMS    = 0;
  endTimeMS     = 0;
  blockTimeMS   = 0;
  errorNumber   = 0;
  stopDelayTM   = DISABLE;
  stopBlockTM   = DISABLE;  
  pinState      = HIGH;
  inputState    = HIGH;
  inputMode     = PULSE;
  processState  = COMPLETE;
  currentState  = HIGH;
  lastState     = HIGH;
  pinBlockState = DISABLE;
  phaseChange   = false;
  phaseNumber   = RESET_PHASE; 
  inputGoLow    = false;
  inputGoHigh   = false;
  if (pinNumber == PIN_ERROR_NO)
    pinRead = DISABLE;
  else
    pinRead = ENABLE;
  MTD2A_print_phase_line (debugPrint, objectName, phaseText[RESET_PHASE]);
}  // reset


void MTD2A_binary_input::print_conf () {
  Serial.println(F("MTD2A_binary_input: "));
  MTD2A_print_generic_info (objectName, phaseChange, phaseText[phaseNumber]);
  MTD2A_print_debug_error (debugPrint, errorNumber);
  //
  Serial.print  (F("  triggerMode  : ")); if (triggerMode == LAST_TRIGGER) Serial.println(F("LAST_TRIGGER")); else Serial.println(F("FIRST_TRIGGER"));
  Serial.print  (F("  timerMode    : ")); if (timerMode == TIME_DELAY)     Serial.println(F("TIME_DELAY"));   else Serial.println(F("MONO_STABLE"));
  // Setup
  MTD2A_print_pin_number (pinNumber);
  Serial.print  (F("  pinType      : ")); if (pinType == INPUT_PULLUP) Serial.println(F("INPUT_PULLUP")); else Serial.println(F("INPUT"));
  Serial.print  (F("  pinRead      : ")); MTD2A_print_enable_disable  (pinRead);
  Serial.print  (F("  pinInput     : ")); MTD2A_print_normal_inverted (pinInput);
  Serial.print  (F("  inputMode    : ")); MTD2A_print_pulse_fixed     (inputMode);
  // timers
  Serial.print  (F("  delayTimeMS  : ")); Serial.println(delayTimeMS);
  Serial.print  (F("  firstTimeMS  : ")); Serial.println(firstTimeMS);
  Serial.print  (F("  lastTimeMS   : ")); Serial.println(lastTimeMS);
  Serial.print  (F("  endTimeMS    : ")); Serial.println(endTimeMS);
  Serial.print  (F("  blockTimeMS  : ")); Serial.println(blockTimeMS);
  // state loggic
  Serial.print  (F("  pinState     : ")); MTD2A_print_value_binary (BINARY, pinState);
  Serial.print  (F("  inputState   : ")); MTD2A_print_value_binary (BINARY, inputState);
} // print_conf 

