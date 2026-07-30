/**
 ******************************************************************************
 * @file    MTD2A_binary_input.cpp
 * @author  Joergen Bo Madsen
 * @version 1.2.1
 * @date    15. july 2026
 * @brief   Functions for MTD2A_binary_input.h (Model Train Detection And Action)
 * 
 * MTD2A is a collection of user friendly advanced and functional C++ classes - 
 * building blocks - for time-controlled handling of input and output. 
 * The library is intended for Arduino enthusiasts without much programming experience, 
 * who are interested in electronics control and automation, and model trains as a hobby.

 * Common to all building blocks are:
 * ----------------------------------
 * - Built on a state machine system for parallel processing and synchronous time management
 * - Support a wide range of input sensors and output devices
 * - Are simple to use to build complex solutions with few commands
 * - Operate non-blocking, process-oriented and time controlled
 * - Offers extensive control and troubleshooting information
 * - Thoroughly documented with many examples
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


#include "MTD2A_binary_input.h"

// Global constants from MTD2A_base.h (MTD2A_const.h) 
using namespace MTD2A_const;


// Constructor
MTD2A_binary_input::MTD2A_binary_input
  (
    const char *setObjectName, 
    uint32_t    setDelayTimeMS, 
    bool        setFirstOrLast, 
    bool        setTimeOrMono, 
    uint32_t    setPinBlockMS
  )
  :
    // Instantiated Function pointer 
    MTD2A{[](MTD2A* funcPtr) { static_cast<MTD2A_binary_input*>(funcPtr)->loop_fast(); }},
    triggerMode {setFirstOrLast}, 
    timerMode   {setTimeOrMono}
  {
    MTD2A_add_function_pointer_loop_fast(this);
    objInstantiated = false;
    objectName      = MTD2A_set_object_name (setObjectName);
    delayTimeUS     = check_set_MS_to_US    (setDelayTimeMS);
    pinBlockUS      = check_set_MS_to_US    (setPinBlockMS);
    objectError     = errorNumber;
    objInstantiated = true;
  }
// MTD2A_binary_input


void MTD2A_binary_input::initialize (uint8_t setPinNumber, bool setPinNormalOrInverted, uint8_t setPinPullupOrInput) {
  if (initializedOnce == false) {
    if (objectError > 0) {
      print_error_text (ERR_OBJECT_INSTANT);
      print_error_text (objectError);
      objectError = 0;
    }
    initializedOnce = true;
    pinReadMode     = setPinNormalOrInverted;
    check_pin_init (setPinNumber, setPinPullupOrInput);
  }
  else {
    print_error_text (ERR_ALREADY_INIT);
  }
} // initialize


// original function
void MTD2A_binary_input::check_pin_init (uint8_t checkPinNumber, uint8_t checkPinPullupOrInput) {
  if (checkPinNumber != NO_PIN) {
    pinNumber = checkPinNumber; 
    errorNumber = MTD2A_reserve_and_check_pin (checkPinNumber, DIGITAL_FLAG_0 | INPUT_FLAG_2 | PULLUP_FLAG_3);
    if (errorNumber == 0  ||  errorNumber >= WARNING_START) {
      if (errorNumber >= WARNING_START) {
        print_error_text (errorNumber);  // e.g. WARN_PIN_REUSED (130) - warn but continue
      }
      pinReadToggl = ENABLE;
      if (checkPinPullupOrInput == (uint8_t)INPUT  ||  checkPinPullupOrInput == (uint8_t)INPUT_PULLUP)
        pinType = checkPinPullupOrInput;
      else {
        print_error_text (ERR_INPUT_TYPE);
        pinType = INPUT_PULLUP;
      }
      pinMode(pinNumber, pinType);
    }
    else {
      print_error_text (errorNumber);
      pinReadToggl = DISABLE;
      pinNumber    = NO_PIN;
    }
  }
  else {
    pinReadToggl = DISABLE;
  }  
} // check_pin_init


void MTD2A_binary_input::set_pinReadToggl (bool setPinEnableOrDisable) {
  if (setPinEnableOrDisable == DISABLE) {
    if (pinBlockState == ENABLE) {
      pinReadSaved = DISABLE;   // takes effect when block ends
    }
    else {
      pinReadToggl = DISABLE;
    }  
  }
  else {
    if (pinNumber != NO_PIN) {
      if (pinBlockState == ENABLE) {
        pinReadSaved = ENABLE;   // takes effect when block ends
      }
      else {
        pinReadToggl = ENABLE;
      }  
    }
    else {
      print_error_text (ERR_PIN_NOT_DEFINED);
    }
  }
} // set_pinReadToggl


void MTD2A_binary_input::set_pinReadMode (bool setPinNormalOrInverted) {
  if (pinNumber != NO_PIN) {
    pinReadMode = setPinNormalOrInverted;
  }
  else {
    print_error_text (ERR_PIN_NOT_DEFINED);
  }
} // set_pinReadMode


void MTD2A_binary_input::set_inputState (bool setInputLowOrHigh, bool setPulseOrFixed) {
  inputState = setInputLowOrHigh;
  inputMode = setPulseOrFixed;
} // set_inputState


void MTD2A_binary_input::set_delayTimeMS (uint32_t setDelayTimeMS) {
  if (processState == COMPLETE) {
    delayTimeUS = check_set_MS_to_US (setDelayTimeMS);
  } 
  else {
    print_error_text (ERR_NOT_COMPLETE);
  }
} // set_delayTimeMS


void MTD2A_binary_input::set_pinBlockMS (uint32_t setPinBlockMS) {
  if (processState == COMPLETE) {
    pinBlockUS = check_set_MS_to_US (setPinBlockMS);
  } 
  else {
    print_error_text (ERR_NOT_COMPLETE);
  }
} // set_pinBlockMS


//  Setters ----------------------------------------------------------------------------


void MTD2A_binary_input::set_stopDelayTimer () {
  if (processState == ACTIVE  &&  delayTimeUS > 0) {
    stopDelayTM = ENABLE;
  } 
  else {
    print_error_text (ERR_TIMER_NOT_IN_USE);
  }
} // set_stopDelayTimer


void MTD2A_binary_input::set_stopBlockTimer () {
  if (processState == ACTIVE  &&  pinBlockUS > 0) {
    stopBlockTM = ENABLE;
  }
  else {
    print_error_text (ERR_TIMER_NOT_IN_USE);
  }
} // set_stopBlockTimer


void MTD2A_binary_input::set_debugPrint (bool setEnableOrDisable) {
  debugPrint = setEnableOrDisable;
} // set_debugPrint 


void MTD2A_binary_input::set_errorPrint (bool setEnableOrDisable) {
  errorPrint = setEnableOrDisable;
} // set_errorPrint 



//  Getters ----------------------------------------------------------------------------


bool MTD2A_binary_input::get_processState () const {
  return processState; 
}


bool MTD2A_binary_input::get_pinState () const {
  return pinState; 
}


bool MTD2A_binary_input::get_phaseChange () const {
  return phaseChange;
}


uint8_t MTD2A_binary_input::get_phaseNumber () const {
  return phaseNumber;
}


uint32_t MTD2A_binary_input::get_firstTimeMS () const {
  return MTD2A_round_US_to_MS (firstTimeUS);
}


uint32_t MTD2A_binary_input::get_lastTimeMS () const {
  return MTD2A_round_US_to_MS (lastTimeUS);
}


uint32_t MTD2A_binary_input::get_endTimeMS () const {
  return MTD2A_round_US_to_MS (endTimeUS);
}


bool MTD2A_binary_input::get_inputGoLow () const {
  return inputGoLow;
}


bool MTD2A_binary_input::get_inputGoHigh () const {
  return inputGoHigh;
}


uint8_t MTD2A_binary_input::get_reset_error () {
  uint8_t tempErrorNumber = errorNumber;
  if (objectError > 0) {
    print_error_text (ERR_OBJECT_INSTANT);
    print_error_text (objectError);
    objectError = 0;
  }
  errorNumber = 0;
  return tempErrorNumber;
} // get_reset_error


// Internal -----------------------------------------------


void MTD2A_binary_input::loop_fast () {
  phaseChange = false;
  loop_fast_input ();
  if (pinBlockState == ENABLE) {
    // BLOCKING_PHASE short-circuit: poll block expiry every loop and ignore all
    // triggers - independent of trigger mode, timer mode and (manual) input state.
    end_state ();
  }
  else if (delayTimeUS == 0) {
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
    if (pinReadMode == INVERTED) 
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


// AAAAAAAAAAAAAA
void MTD2A_binary_input::loop_fast_binary () {
  if (inputGoLow) {
    if (processState == COMPLETE)   // no retrigger while ACTIVE (same guard as first/last)
      begin_state ();
  }
  if (inputGoHigh)                  // blocking expiry is polled in loop_fast ()
    end_state ();
} // loop_fast_binary


void MTD2A_binary_input::loop_fast_first () {
  if (inputGoLow)
    if (processState == COMPLETE)
      begin_state ();
  //
  if (processState == ACTIVE) {
    if ((globalSyncTimeUS - firstTimeUS) >= (delayTimeUS - MARGIN_TIME_US)  ||  stopDelayTM == ENABLE) {
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
      lastTimeUS  = globalSyncTimeUS;
      print_phase_line ();
    }
    //
    if ((globalSyncTimeUS - lastTimeUS) >= (delayTimeUS - MARGIN_TIME_US)  ||  stopDelayTM == ENABLE) {
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
  firstTimeUS  = globalSyncTimeUS;
  lastTimeUS   = firstTimeUS;
  phaseChange  = true;
  phaseNumber  = FIRST_TIME_PHASE;
  print_phase_line ();
} // begin_state


void MTD2A_binary_input::end_state () {
  if (pinBlockUS > 0) {
    if (pinBlockState == DISABLE) {
      pinBlockState = ENABLE;
      pinReadSaved  = pinReadToggl;   // remember caller's setting (DISABLE if no pin)      
      pinReadToggl  = DISABLE;
      blockTimeUS   = globalSyncTimeUS;
      phaseChange   = true;
      phaseNumber   = BLOCKING_PHASE;
      print_phase_line (); 
    }
    if ((globalSyncTimeUS - blockTimeUS) >= (pinBlockUS - MARGIN_TIME_US)  ||  stopBlockTM == ENABLE) {
      stopBlockTM   = DISABLE;
      pinBlockState = DISABLE;
      pinReadToggl  = pinReadSaved;   // restore
      complete_state ();
    }
  }
  else
    complete_state ();    
} // end_state


void MTD2A_binary_input::complete_state () {
  stopDelayTM  = DISABLE;   // discard unconsumed stop requests -
  stopBlockTM  = DISABLE;   // they must not leak into the next cycle
  endTimeUS    = globalSyncTimeUS;
  phaseChange  = true;
  processState = COMPLETE;
  phaseNumber  = COMPLETE_PHASE;
  print_phase_line();
} // complete_state


void MTD2A_binary_input::reset () {
  firstTimeUS   = 0;
  lastTimeUS    = 0;
  endTimeUS     = 0;
  blockTimeUS   = 0;
  errorNumber   = 0;
  stopDelayTM   = DISABLE;
  stopBlockTM   = DISABLE;
  pinState      = HIGH;
  if (pinBlockState == ENABLE) {      // reset issued during BLOCKING_PHASE
    pinReadToggl = pinReadSaved;      // restore pin reading
  }
  pinBlockState = DISABLE;
  processState  = COMPLETE;
  inputState    = HIGH;
  currentState  = HIGH;
  lastState     = HIGH;
  phaseChange   = true;
  phaseNumber   = RESET_PHASE; 
  inputGoLow    = false;
  inputGoHigh   = false;
  print_phase_line ();
}  // reset


uint32_t MTD2A_binary_input::check_set_MS_to_US (uint32_t setCheckTimeMS) {
  if (setCheckTimeMS > 0  &&  setCheckTimeMS < globalDelayTimeMS) {
    print_error_text (ERR_TIME_BELOW_DELAY);
    return globalDelayTimeMS * MS_to_US;
  }
  if (setCheckTimeMS > MAX_TIME_MS) {
    print_error_text (ERR_TIME_ABOVE_MAX);
    return MAX_TIME_MS * MS_to_US;
  }
  return setCheckTimeMS * MS_to_US;
} // check_set_MS_to_US


void MTD2A_binary_input::print_error_text (uint8_t setErrorNumber) {
  errorNumber = setErrorNumber;
  if (objInstantiated == true) {
    MTD2A_print_error_text (objectName, (debugPrint == ENABLE || errorPrint == ENABLE), errorNumber, pinNumber);
  }
} // print_error_text


void MTD2A_binary_input::print_phase_text() {
  switch(phaseNumber) {
    case  0: PortPrintln(F("[0] Reset"));         break;
    case  1: PortPrintln(F("[1] First time"));    break;
    case  2: PortPrintln(F("[2] Last time"));     break;
    case  3: PortPrintln(F("[3] Pin blocking"));  break;
    case  4: PortPrintln(F("[4] Complete"));      break;
    default: print_error_text (ERR_UNFORESEEN_ERROR);
  }
} // print_phase_text


void MTD2A_binary_input::print_phase_line () {
  if (debugPrint == ENABLE  ||  globalDebugPrint == ENABLE) {
    MTD2A_print_object_name (objectName);
    print_phase_text ();
  }
} // print_phase_line


void MTD2A_binary_input::print_conf () {
  PortPrintln(F("MTD2A_binary_input: "));
  MTD2A_print_name_state (objectName, processState);
  PortPrint  (F("  phaseText    : ")); print_phase_text ();
  MTD2A_print_debug_error (debugPrint, errorPrint, errorNumber);
  //
  PortPrint  (F("  triggerMode  : ")); if (triggerMode == LAST_TRIGGER) PortPrintln (F("LAST_TRIGGER")); else PortPrintln (F("FIRST_TRIGGER"));
  PortPrint  (F("  timerMode    : ")); if (timerMode == TIME_DELAY)     PortPrintln( F("TIME_DELAY"));   else PortPrintln (F("MONO_STABLE"));
  // Setup
  MTD2A_print_pin_number (pinNumber);
  PortPrint  (F("  pinType      : ")); if (pinType == INPUT_PULLUP) PortPrintln (F("INPUT_PULLUP")); else PortPrintln (F("INPUT"));
  PortPrint  (F("  pinReadToggl : ")); MTD2A_print_enable_disable  (pinReadToggl);
  PortPrint  (F("  pinReadMode  : ")); MTD2A_print_normal_inverted (pinReadMode);
  PortPrint  (F("  inputMode    : ")); MTD2A_print_pulse_fixed     (inputMode);
  // timers
  PortPrint  (F("  delayTimeMS  : ")); PortPrintln (MTD2A_round_US_to_MS (delayTimeUS));
  PortPrint  (F("  firstTimeMS  : ")); PortPrintln (MTD2A_round_US_to_MS (firstTimeUS));
  PortPrint  (F("  lastTimeMS   : ")); PortPrintln (MTD2A_round_US_to_MS (lastTimeUS));
  PortPrint  (F("  endTimeMS    : ")); PortPrintln (MTD2A_round_US_to_MS (endTimeUS));
  PortPrint  (F("  blockTimeMS  : ")); PortPrintln (MTD2A_round_US_to_MS (blockTimeUS));
  // state loggic
  PortPrint  (F("  pinState     : ")); MTD2A_print_value_binary (BINARY, pinState);
  PortPrint  (F("  inputState   : ")); MTD2A_print_value_binary (BINARY, inputState);
} // print_conf



