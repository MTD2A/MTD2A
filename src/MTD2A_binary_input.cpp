/*
 ******************************************************************************
 * @file    MTD2A_binary_input.cpp
 * @author  Joergen Bo Madsen
 * @version V1.1
 * @date    12. maj 2025
 * @brief   functions for MTD2A.h (Mobile Train Detection And Action)
 * 
 * Supporting a vast variety of input sensors and output devices 
 * Simple to use to build complex solutions 
 * Non blocking simple yet efficient state machine
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
#include "MTD2A.h"


// Constructor
MTD2A_binary_input::MTD2A_binary_input
  (
    const char    *setObjectName, 
    const uint32_t setDelayTimeMS, 
    const bool     setFirstOrLast, 
    const bool     setTimeOrMono, 
    const uint32_t setPinBlockMS
  )
  : triggerMode{setFirstOrLast}, 
    delayTimeMS{setDelayTimeMS}, 
    timerMode{setTimeOrMono}, 
    pinBlockMS{setPinBlockMS}
  {
    objectName = MTD2A_set_object_name(setObjectName);
    if (delayTimeMS == 0) {
      errorNumber = 130;  // Warning
    }
  }
// MTD2A_binary_input


void MTD2A_binary_input::initialize (const uint8_t &setPinNumber, const bool &setPinNomalOrInverted, const uint8_t &setPinPullupOrInput) {
  errorNumber = MTD2A_reserve_and_check_pin (setPinNumber, digitalFlag_0 + inputFlag_2 + pullupFlag_3, debugPrint);
  if (errorNumber == 0) {
    pinRead = enable;
    pinNumber = setPinNumber;
    if (setPinPullupOrInput != 1  & setPinPullupOrInput != 2) {
      errorNumber = 8; MTD2A_print_error_text (errorNumber);
    }
    pinMode(pinNumber, pinType);
  }
  else {
    pinRead = disable;
    pinNumber = 255;
  }
} // initialize


void MTD2A_binary_input::set_pinRead (const bool &setPinEnableOrDisable, const bool &LoopFastOnce) {
  if (setPinEnableOrDisable == enable  &&  pinNumber == 255) {
    errorNumber = 9; MTD2A_print_error_text (errorNumber);  
  }
  else {
    pinRead = setPinEnableOrDisable;
    if (LoopFastOnce == enable)
      loop_fast();
  }
} // set_pinRead


void MTD2A_binary_input::set_pinInput (const bool &setPinNomalOrInverted, const bool &LoopFastOnce) {
  if (pinNumber = !255) {
    pinInput = setPinNomalOrInverted;
    if (LoopFastOnce == enable)
      loop_fast();
  }
  else {
    errorNumber = 9; MTD2A_print_error_text (errorNumber);
  }
}


void MTD2A_binary_input::set_inputState (const bool &setInputLowOrHigh, const bool &setPulseOrFixed, const bool &LoopFastOnce) {
  inputState = setInputLowOrHigh;
  inputMode = setPulseOrFixed;
  if (LoopFastOnce == enable)
    loop_fast();
}


void MTD2A_binary_input::set_debugPrint (const bool &setEnableOrDisable, const bool &LoopFastOnce) {
  debugPrint = setEnableOrDisable;
  if (LoopFastOnce == enable)
    loop_fast();
}


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
}


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
    begin_state ();
  if (inputGoHigh)
    end_state ();
}  // loop_fast_binary


void MTD2A_binary_input::loop_fast_first () {
  if (inputGoLow)
    if (processState == pending)
      begin_state ();
  //
  if (processState == active) {
    if ((millis() - firstTimeMS) >= delayTimeMS) {
      if (timerMode == monoStable) 
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
    if (processState == pending)
      begin_state ();
  }
  //
  if (processState == active) {
    if (inputGoHigh) {
      phaseChange = true;
      phaseNumber = lastTimePhase;
      lastTimeMS  = millis();
      MTD2A_print_phase_line (debugPrint, objectName, phaseText[lastTimePhase]);
    }
    //
    if ((millis() - lastTimeMS) >= delayTimeMS) {
      if (timerMode == monoStable) 
        end_state ();
      else {
        if (currentState == HIGH)
          end_state ();
      }
    }
  }
} // loop_fast_last


void MTD2A_binary_input::begin_state () {
  processState = active;
  firstTimeMS  = millis();
  lastTimeMS   = firstTimeMS;
  phaseChange  = true;
  phaseNumber  = firstTimePhase;
  MTD2A_print_phase_line (debugPrint, objectName, phaseText[firstTimePhase]);
} // begin_state


void MTD2A_binary_input::end_state () {
  if (pinBlockMS > 0) {
    if (pinBlockState == disable) {
      pinBlockState = enable;
      pinRead       = disable;
      BlockTimeMS   = millis();
      phaseChange   = true;
      phaseNumber   = blockingPhase;
      MTD2A_print_phase_line (debugPrint, objectName, phaseText[blockingPhase]); 
    }
    if ((millis() - BlockTimeMS) >= pinBlockMS) {
      pinBlockState = disable;
      pinRead = enable;
      pending_state ();
    }
  }
  else
    pending_state ();    
} // ending_state


void MTD2A_binary_input::pending_state () {
  processState = pending;
  endTimeMS    = millis();
  phaseChange  = true;
  phaseNumber  = pendingPhase;
  MTD2A_print_phase_line (debugPrint, objectName, phaseText[pendingPhase]);
} // ending_state


void MTD2A_binary_input::reset () {
  firstTimeMS   = 0;
  lastTimeMS    = 0;
  endTimeMS     = 0;
  BlockTimeMS   = 0;
  errorNumber   = 0;
  pinState      = HIGH;
  pinRead       = enable;
  pinInput      =  normal;
  inputState    = LOW;
  inputMode     = pulse;
  processState  = pending;
  currentState  = HIGH;
  lastState     = HIGH;
  pinBlockState = disable;
  phaseChange   = false;
  phaseNumber   = resetPhase; 
  inputGoLow    = false;
  inputGoHigh   = false;
  MTD2A_print_phase_line (debugPrint, objectName, phaseText[resetPhase]);
}  // reset


void MTD2A_binary_input::print_conf () {
  Serial.println(F("MTD2A_binary_input: "));
  MTD2A_print_generic_info (objectName, phaseChange, phaseText[phaseNumber]);
  MTD2A_print_debug_error (errorNumber, debugPrint);
  //
  Serial.print  (F("  triggerMode : ")); if (triggerMode == firstTrigger) Serial.println(F("First")); else Serial.println(F("Last"));
  Serial.print  (F("  timerMode   : ")); if (timerMode == timeDelay)      Serial.println(F("Delay")); else Serial.println(F("Mono"));
  // Setup
  Serial.print  (F("  pinNumber   : ")); MTD2A_print_pin_number (pinNumber);
  Serial.print  (F("  pinType     : ")); if (pinType == INPUT_PULLUP) Serial.println(F("INPUT_PULLUP")); else Serial.println(F("INPUT"));
  Serial.print  (F("  pinRead     : ")); MTD2A_print_enable_disable (pinRead);
  Serial.print  (F("  pinInput    : ")); if (pinInput == normal)      Serial.println(F("Normal"));       else Serial.println(F("Inverted"));
  Serial.print  (F("  inputMode   : ")); MTD2A_print_pulse_fixed (inputMode);
  // timers
  Serial.print  (F("  delayTimeMS : ")); Serial.println(delayTimeMS);
  Serial.print  (F("  firstTimeMS : ")); Serial.println(firstTimeMS);
  Serial.print  (F("  lastTimeMS  : ")); Serial.println(lastTimeMS);
  Serial.print  (F("  endTimeMS   : ")); Serial.println(endTimeMS);
  Serial.print  (F("  blockTimeMS : ")); Serial.println(BlockTimeMS);
  // state loggic
  Serial.print  (F("  pinState    : ")); MTD2A_print_value_binary (binary, pinState);
  Serial.print  (F("  inputState  : ")); MTD2A_print_value_binary (binary, inputState);
} // print_conf 



/*
void MTD2A_binary_input::loop_fast_ptr () {
    void (MTD2A_binary_input::*loop_pointer) () = &MTD2A_binary_input::loop_fast;
    (this->*loop_pointer) ();
//    typedef void (MTD2A_binary_input::*loop_pointer) ();
//    loop_pointer LP =  &MTD2A_binary_input::loop_fast;
//    (this->*LP) ();
}
*/
