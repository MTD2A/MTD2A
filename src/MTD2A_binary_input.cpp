/**
 ******************************************************************************
 * @file    MTD2A_binary_input.cpp
 * @author  Joergen Bo Madsen
 * @version V1.1.7
 * @date    31. august 2025
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


// Specific global constants from MTD2A_binary_input.h (MTD2A_const.h)
constexpr bool    MTD2A_binary_input::FIRST_TRIGGER;
constexpr bool    MTD2A_binary_input::LAST_TRIGGER;
constexpr bool    MTD2A_binary_input::MONO_STABLE;
constexpr bool    MTD2A_binary_input::TIME_DELAY;
// Timer
constexpr uint8_t MTD2A_binary_input::RESET_TIMER;
// Phases
constexpr uint8_t MTD2A_binary_input::RESET_PHASE; 
constexpr uint8_t MTD2A_binary_input::FIRST_TIME_PHASE;
constexpr uint8_t MTD2A_binary_input::LAST_TIME_PHASE; 
constexpr uint8_t MTD2A_binary_input::BLOCKING_PHASE;
constexpr uint8_t MTD2A_binary_input::COMPLETE_PHASE;


// Constructor
MTD2A_binary_input::MTD2A_binary_input
  (
    const char    *setObjectName, 
    const uint32_t setDelayTimeMS, 
    const bool     setFirstOrLast, 
    const bool     setTimeOrMono, 
    const uint32_t setPinBlockMS
  )
  : delayTimeMS {setDelayTimeMS}, 
    triggerMode {setFirstOrLast}, 
    timerMode   {setTimeOrMono},
    pinBlockMS  {setPinBlockMS},
    // Instatiated funtion pointer
    MTD2A{[](MTD2A* funcPtr) { static_cast<MTD2A_binary_input*>(funcPtr)->loop_fast(); }}
  {
    MTD2A_add_function_pointer_loop_fast(this);
    objectName  = MTD2A_set_object_name(setObjectName);
  }
// MTD2A_binary_input


void MTD2A_binary_input::initialize (const uint8_t &setPinNumber, const bool &setPinNomalOrInverted, const uint8_t &setPinPullupOrInput) {
  // Check for instantiated object error
  delayTimeMS = check_set_time (delayTimeMS);
  pinBlockMS  = check_set_time (pinBlockMS);
  //
  errorNumber = MTD2A_reserve_and_check_pin (setPinNumber, DIGITAL_FLAG_0 | INPUT_FLAG_2 | PULLUP_FLAG_3);
  if (errorNumber == 0) {
    pinReadToggl = ENABLE;
    pinNumber = setPinNumber;
    pinReadMode = setPinNomalOrInverted;
    if (setPinPullupOrInput == INPUT  ||  setPinPullupOrInput == INPUT_PULLUP) 
      pinType = setPinPullupOrInput;
    else {
      print_error_text (8);
      pinType = INPUT_PULLUP;
    }
    pinMode(pinNumber, pinType);
  }
  else {
    print_error_text (errorNumber);
    pinReadToggl = DISABLE;
    pinNumber    = PIN_ERROR_NO;
  }
} // initialize


void MTD2A_binary_input::set_pinReadToggl (const bool &setPinEnableOrDisable) {
  if (setPinEnableOrDisable == ENABLE  &&  pinNumber == PIN_ERROR_NO) {
    print_error_text (1);
  }
  else {
    pinReadToggl = setPinEnableOrDisable;
  }
} // set_pinReadToggl


void MTD2A_binary_input::set_pinReadMode (const bool &setPinNomalOrInverted) {
  if (pinNumber != PIN_ERROR_NO) {
    pinReadMode= setPinNomalOrInverted;
  }
  else {
    print_error_text (1);
  }
} // set_pinReadMode


void MTD2A_binary_input::set_inputState (const bool &setInputLowOrHigh, const bool &setPulseOrFixed) {
  inputState = setInputLowOrHigh;
  inputMode = setPulseOrFixed;
} // set_inputState


void MTD2A_binary_input::set_delayTimeMS (const uint32_t &setDelayTimeMS) {
  if (processState == COMPLETE) {
    delayTimeMS = check_set_time (setDelayTimeMS);
  } 
  else {
    print_error_text (12);
  }
} // set_delayTimeMS


void MTD2A_binary_input::set_pinBlockMS (const uint32_t &setPinBlockMS) {
  if (processState == COMPLETE) {
    pinBlockMS = check_set_time (setPinBlockMS);
  } 
  else {
    print_error_text (12);
  }
} // set_pinBlockMS


void MTD2A_binary_input::set_stopDelayTimer () {
  stopDelayTM = ENABLE;
} // et_stopDelayTimer


void MTD2A_binary_input::set_stopBlockTimer () {
  stopBlockTM = ENABLE;
} // set_stopBlockTimer


void MTD2A_binary_input::set_debugPrint (const bool &setEnableOrDisable) {
  debugPrint = setEnableOrDisable;
} // set_debugPrint 


void MTD2A_binary_input::set_errorPrint (const bool &setEnableOrDisable) {
  errorPrint = setEnableOrDisable;
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


// Internal -----------------------------------------------


void MTD2A_binary_input::loop_fast () {
  phaseChange = false;
  loop_fast_input ();
  if (delayTimeMS == 0) {
    loop_fast_binary ();
  }
  else { //LastTrigger
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
  if (pinReadToggl == ENABLE) {
    pinState = digitalRead(pinNumber);
    if (pinReadMode== INVERTED) 
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
    if ((globalSyncTimeMS - firstTimeMS) >= delayTimeMS  ||  stopDelayTM == ENABLE) {
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
      lastTimeMS  = globalSyncTimeMS;
      print_phase_line ();
    }
    //
    if ((globalSyncTimeMS - lastTimeMS) >= delayTimeMS  ||  stopDelayTM == ENABLE) {
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
  firstTimeMS  = globalSyncTimeMS;
  lastTimeMS   = firstTimeMS;
  phaseChange  = true;
  phaseNumber  = FIRST_TIME_PHASE;
  print_phase_line ();
} // begin_state


void MTD2A_binary_input::end_state () {
  if (pinBlockMS > 0) {
    if (pinBlockState == DISABLE) {
      pinBlockState = ENABLE;
      pinReadToggl  = DISABLE;
      blockTimeMS   = globalSyncTimeMS;
      phaseChange   = true;
      phaseNumber   = BLOCKING_PHASE;
      print_phase_line (); 
    }
    if ((globalSyncTimeMS - blockTimeMS) >= pinBlockMS  ||  stopBlockTM == ENABLE) {
      stopBlockTM   = DISABLE;
      pinBlockState = DISABLE;
      pinReadToggl  = ENABLE;
      complete_state ();
    }
  }
  else
    complete_state ();    
} // ending_state


void MTD2A_binary_input::complete_state () {
  endTimeMS    = globalSyncTimeMS;
  phaseChange  = true;
  processState = COMPLETE;
  phaseNumber  = COMPLETE_PHASE;
  print_phase_line();
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
  processState  = COMPLETE;
  pinBlockState = DISABLE;
  currentState  = HIGH;
  lastState     = HIGH;
  phaseChange   = true;
  phaseNumber   = RESET_PHASE; 
  inputGoLow    = false;
  inputGoHigh   = false;
  if (pinNumber == PIN_ERROR_NO)
    pinReadToggl = DISABLE;
  else
    pinReadToggl = ENABLE;
  print_phase_line ();
}  // reset



uint32_t MTD2A_binary_input::check_set_time (const uint32_t &setCheckTimeMS) {
  if (setCheckTimeMS > 0  &&  setCheckTimeMS < globalDelayTimeMS) {
    print_error_text (9);
    return globalDelayTimeMS;
  }
  else
    return setCheckTimeMS;
} // check_set_time


void MTD2A_binary_input::print_error_text (const uint8_t setErrorNumber) {
  errorNumber = setErrorNumber;
  MTD2A_print_object_name (objectName);
  MTD2A_print_error_text ((debugPrint == ENABLE || errorPrint == ENABLE), errorNumber, pinNumber);
} // print_error_text


void MTD2A_binary_input::print_phase_text() {
  switch(phaseNumber) {
    case 0: PortPrintln(F("[0] Reset"));         break;
    case 1: PortPrintln(F("[1] First time"));    break;
    case 2: PortPrintln(F("[2] Last time"));     break;
    case 3: PortPrintln(F("[3] Pin blocking"));  break;
    case 4: PortPrintln(F("[4] Complete"));      break;
  }
} // print_phase_text


void MTD2A_binary_input::print_phase_line (const uint8_t &printRestartTimer) {
  if (debugPrint == ENABLE  ||  globalDebugPrint == ENABLE) {
    MTD2A_print_object_name (objectName);
    if (printRestartTimer == RESET_TIMER) {
      PortPrint(F(": Reset timer >"));
    }
    PortPrint(F(" "));
    print_phase_text ();
  }
} // print_phase_line


void MTD2A_binary_input::print_conf () {
  PortPrintln(F("MTD2A_binary_input: "));
  MTD2A_print_name_state (objectName, processState);
  PortPrint  (F("  phaseText    : ")); print_phase_text ();
  MTD2A_print_debug_error (debugPrint, errorPrint, errorNumber);
  //
  PortPrint  (F("  triggerMode  : ")); if (triggerMode == LAST_TRIGGER) PortPrintln(F("LAST_TRIGGER")); else PortPrintln(F("FIRST_TRIGGER"));
  PortPrint  (F("  timerMode    : ")); if (timerMode == TIME_DELAY)     PortPrintln(F("TIME_DELAY"));   else PortPrintln(F("MONO_STABLE"));
  // Setup
  MTD2A_print_pin_number (pinNumber);
  PortPrint  (F("  pinType      : ")); if (pinType == INPUT_PULLUP) PortPrintln(F("INPUT_PULLUP")); else PortPrintln(F("INPUT"));
  PortPrint  (F("  pinReadToggl : ")); MTD2A_print_enable_disable  (pinReadToggl);
  PortPrint  (F("  pinReadMode  : ")); MTD2A_print_normal_inverted (pinReadMode);
  PortPrint  (F("  inputMode    : ")); MTD2A_print_pulse_fixed     (inputMode);
  // timers
  PortPrint  (F("  delayTimeMS  : ")); PortPrintln(delayTimeMS);
  PortPrint  (F("  firstTimeMS  : ")); PortPrintln(firstTimeMS);
  PortPrint  (F("  lastTimeMS   : ")); PortPrintln(lastTimeMS);
  PortPrint  (F("  endTimeMS    : ")); PortPrintln(endTimeMS);
  PortPrint  (F("  blockTimeMS  : ")); PortPrintln(blockTimeMS);
  // state loggic
  PortPrint  (F("  pinState     : ")); MTD2A_print_value_binary (BINARY, pinState);
  PortPrint  (F("  inputState   : ")); MTD2A_print_value_binary (BINARY, inputState);
} // print_conf 

