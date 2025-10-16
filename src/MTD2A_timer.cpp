/**
 ******************************************************************************
 * @file    MTD2A_timers.cpp
 * @author  Joergen Bo Madsen
 * @version V1.1.3
 * @date    16. september 2025
 * @brief   functions for MTD2A_base.h base class (Model Train Detection And Action)
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


#ifndef _MTD2A_timer_CPP_
#define _MTD2A_timer_CPP_


#include "Arduino.h"
#include "MTD2A_base.h"
#include "MTD2A_timer.h"


// Specific global constants from MTD2A_binary_output.h (MTD2A_const.h)
constexpr uint8_t  MTD2A_timer::RESET_TIMER;
constexpr uint8_t  MTD2A_timer::START_TIMER;
constexpr uint8_t  MTD2A_timer::PAUSE_TIMER;
constexpr uint8_t  MTD2A_timer::STOP_TIMER;
// MTD2A_base
constexpr uint8_t  MTD2A_timer::NO_PRINT_PIN;


// Constructor
MTD2A_timer::MTD2A_timer
  (const char *setObjectName, const uint32_t setCountDownMS)
  : countDownMS {setCountDownMS}, 
    // Instatiated funtion pointer
    MTD2A{[](MTD2A* funcPtr) { static_cast<MTD2A_timer*>(funcPtr)->loop_fast(); }}
  { 
    remainTimeMS  = countDownMS;
    MTD2A_add_function_pointer_loop_fast(this);
    objectName = MTD2A_set_object_name(setObjectName);
  }
// MTD2A_timer


// Timer function overloading
void MTD2A_timer::timer (const uint8_t &setStartStopPauseReset) {
  if (check_timer_arg (setStartStopPauseReset) == true) {
    set_timer_state (setStartStopPauseReset);
  }
}
void MTD2A_timer::timer (const uint8_t &setStartStopPauseReset, const uint32_t &setCountDownMS) {
  countDownMS = setCountDownMS;
  if (check_timer_arg (setStartStopPauseReset) == true) {
    set_timer_state (setStartStopPauseReset);
  }
}  // timer


bool MTD2A_timer::check_timer_arg (const uint8_t &argStartStopPauseReset) {
  if (argStartStopPauseReset <= STOP_TIMER) {
    if (argStartStopPauseReset == START_TIMER  ||  argStartStopPauseReset == RESET_TIMER) {
      countDownMS = check_set_time (countDownMS);
    }
    return true;
  }
  else {
    print_error_text (14);
    return false;
  }
} // check_timer_arg


void MTD2A_timer::set_timer_state (const uint8_t &setStartRestPauseStop) {
    setPhaseNumber = setStartRestPauseStop;
    switch (setStartRestPauseStop) {
      case START_TIMER: start_timer ();  break;
      case RESET_TIMER: reset_timer ();  break;
      case PAUSE_TIMER: pause_timer ();  break;
      case STOP_TIMER:  stop_timer  ();  break;
    }
} // set_timer_state


void MTD2A_timer::start_timer () {
  if (processState == COMPLETE) {
    startProcess  = true;
  }
  else { // ACTIVE
    if (phaseNumber == PAUSE_TIMER) {
      endPause = true;
    }
    else{
      print_error_text (12);
    }
  }
} // start_timer


void MTD2A_timer::reset_timer () {
  // Reset no matter processState
  startProcess  = true;
} // reset_timer


void MTD2A_timer::pause_timer () {
  if (processState == ACTIVE) {
    if (phaseNumber == START_TIMER  ||  phaseNumber == RESET_TIMER) {
      beginPause = true;
    }
  }
  else {
    print_error_text (16);
  }
} // pause_timer


void MTD2A_timer::stop_timer () {
  if (processState == ACTIVE) {
    stopProcess  = true;
    if (phaseNumber == PAUSE_TIMER) {
      endPause = true;
    }
  }
  else {
    print_error_text (16);  }
} // stop_timer


// Setters -------------------------------------------------------------


void MTD2A_timer::set_countDownMS (const uint32_t &setCountDownMS) {
  if (processState == COMPLETE) {
    countDownMS = check_set_time (setCountDownMS);
  }
  else {
    print_error_text (12);
  }
} // set_countDownMS


void MTD2A_timer::set_debugPrint (const bool &setEnableOrDisable) {
  debugPrint = setEnableOrDisable;
} // set_debugPrint


void MTD2A_timer::set_errorPrint (const bool &setEnableOrDisable) {
  errorPrint = setEnableOrDisable;
} // set_errorPrint


// getters -------------------------------------------------------------


uint32_t const &MTD2A_timer::get_startTimeMS () const {
  return startTimeMS;
}

uint32_t const &MTD2A_timer::get_stopTimeMS () const {
  return stopTimeMS;
}


uint32_t const &MTD2A_timer::get_pauseTimeMS () const {
  return pauseTimeMS;
}

uint32_t const &MTD2A_timer::get_remainTimeMS () const {
  return remainTimeMS;
}


uint32_t const &MTD2A_timer::get_elapsedTimeMS () const {
  return elapsedTimeMS;
}


bool const &MTD2A_timer::get_processState () const {
  return processState; 
}


bool const &MTD2A_timer::get_phaseChange () const {
  return phaseChange;
}


uint8_t const &MTD2A_timer::get_phaseNumber () const {
  return phaseNumber;
}


uint8_t const &MTD2A_timer::get_reset_error () {
  static uint8_t tempErrorNumber;
  tempErrorNumber = errorNumber;
  errorNumber = 0;
  return tempErrorNumber;
} // get_reset_error


// Internal -------------------------------------------------------------


void MTD2A_timer::loop_fast () {
  phaseChange = false;
  phaseNumber = setPhaseNumber;
 switch (phaseNumber) {
    case START_TIMER:
    case RESET_TIMER:
         loop_fast_start_reset ();
         loop_fast_pause_end   ();
         loop_fast_calc_time   ();
         loop_fast_timer_stop  ();
    break;
    case PAUSE_TIMER:
         loop_fast_pause_begin ();
    break;
    case STOP_TIMER:
         loop_fast_pause_end   ();
         loop_fast_calc_time   ();
         loop_fast_timer_stop  ();
    break;
 }
} // loop_fast


void MTD2A_timer::loop_fast_start_reset () {
  if (startProcess == true) {
    startProcess  = false;
    stopProcess   = true;
    beginPause    = false;
    endPause      = false;
    phaseChange   = true;
    processState  = ACTIVE;
    startTimeMS   = globalSyncTimeMS;
    remainTimeMS  = countDownMS;
    elapsedTimeMS = 0;
    pauseTimeMS   = 0;
    stopTimeMS    = 0;
    print_phase_line ();
  }
} // loop_fast_start


void MTD2A_timer::loop_fast_pause_begin () {
  if (beginPause == true) {
    beginPause    = false;
    phaseChange   = true;
    pauseBeginMS  = globalSyncTimeMS;
    print_phase_line ();
  }
} // loop_fast_begin_pause


void MTD2A_timer::loop_fast_pause_end () {
  if (endPause == true) {
    endPause   = false;
    phaseChange = true;
    pauseEndMS  = globalSyncTimeMS;
    pauseTimeMS = pauseTimeMS + pauseEndMS - pauseBeginMS;
    print_phase_line ();
  }
} // loop_fast_end_pause


void MTD2A_timer::loop_fast_calc_time () {
  if (phaseNumber != PAUSE_TIMER  &&  stopProcess == true) {
    // checks for negative numbers
    elapsedTimeMS = globalSyncTimeMS - pauseTimeMS - startTimeMS;
    if (elapsedTimeMS  >=  countDownMS) {
      remainTimeMS = 0;
      elapsedTimeMS = countDownMS;
    }
    else {
      remainTimeMS = countDownMS - elapsedTimeMS;
    }
  }
} // loop_fast_calc_time


void MTD2A_timer::loop_fast_timer_stop () {
  if ((remainTimeMS) == 0  ||  phaseNumber == STOP_TIMER ) {
    if (stopProcess == true) {
      stopProcess  = false;
      phaseChange  = true;
      processState = COMPLETE;
      phaseNumber  = STOP_TIMER; 
      stopTimeMS   = globalSyncTimeMS;
      print_phase_line ();
    }
  }
} // loop_fast_timer


uint32_t MTD2A_timer::check_set_time (const uint32_t &setCheckTimeMS) {
  if (countDownMS == 0) {
    print_error_text (140);
  }
  if (setCheckTimeMS > 0  &&  setCheckTimeMS < globalDelayTimeMS) {
    print_error_text (9);
    return globalDelayTimeMS;
  }
  else
    return setCheckTimeMS;
} // check_set_time


void MTD2A_timer::print_phase_line () {
  if (debugPrint == ENABLE  ||  globalDebugPrint == ENABLE) {
    MTD2A_print_object_name (objectName);
    PortPrint(F(" "));
    print_phase_text ();
  }
} // print_phase_line


void MTD2A_timer::print_phase_text () {
  switch(phaseNumber) {
    case 0: PortPrint(F("[0] Reset"));  break;
    case 1: PortPrint(F("[1] Start"));  break;
    case 2: PortPrint(F("[2] Pause"));  break;
    case 3: PortPrint(F("[3] Stop"));   break;
  }
  PortPrintln(F(" timer"));    
} // print_phase_text


void MTD2A_timer::print_error_text (const uint8_t setErrorNumber) {
  errorNumber = setErrorNumber;
  MTD2A_print_object_name (objectName);
  MTD2A_print_error_text ((debugPrint == ENABLE || errorPrint == ENABLE), errorNumber, NO_PRINT_PIN);
} // print_error_text


void MTD2A_timer::print_conf () {
  PortPrintln(F("MTD2A_timer: "));
  MTD2A_print_name_state (objectName, processState);
  PortPrint  (F("  phaseText    : ")); print_phase_text ();
  MTD2A_print_debug_error (debugPrint, errorPrint, errorNumber);
  // timers
  PortPrint  (F("  countDownMS  : ")); PortPrintln(countDownMS);
  PortPrint  (F("  remainTimeMS : ")); PortPrintln(remainTimeMS);
  PortPrint  (F("  elapsedTimeMS: ")); PortPrintln(elapsedTimeMS);
  PortPrint  (F("  startTimeMS  : ")); PortPrintln(startTimeMS);
  PortPrint  (F("  stopTimeMS   : ")); PortPrintln(stopTimeMS);
  PortPrint  (F("  pauseTimeMS  : ")); PortPrintln(pauseTimeMS);
  PortPrint  (F("  pauseBeginMS : ")); PortPrintln(pauseBeginMS);
  PortPrint  (F("  pauseEndMS   : ")); PortPrintln(pauseEndMS);
} // print_conf 

#endif
