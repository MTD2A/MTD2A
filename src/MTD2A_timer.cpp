/**
 ******************************************************************************
 * @file    MTD2A_timers.cpp
 * @author  Joergen Bo Madsen
 * @version V1.1.1
 * @date    31. august 2025
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


// MTD2A_timer ===========================================================================


// Constructor
MTD2A_timer::MTD2A_timer
  (const char *setObjectName, const uint32_t setCountDownMS)
  : countDownMS {setCountDownMS}, 
    // Instatiated funtion pointer
    MTD2A{[](MTD2A* funcPtr) { static_cast<MTD2A_timer*>(funcPtr)->loop_fast(); }}
  { 
    MTD2A_add_function_pointer_loop_fast(this);
    objectName = MTD2A_set_object_name(setObjectName);
  }
// MTD2A_timer


// Timer function overloading
void MTD2A_timer::timer (const uint8_t &startStopPauseReset) {
  set_timer_state (startStopPauseReset);
}
void MTD2A_timer::timer (const uint8_t &startStopPauseReset, const uint32_t &setCountDownMS) {
  if (startStopPauseReset == START_TIMER  ||  startStopPauseReset == RESET_TIMER) {
    countDownMS = setCountDownMS;
  }
  else {
    print_error_text (15);
  }
  set_timer_state (startStopPauseReset);
}  // timer


void MTD2A_timer::set_timer_state (const uint8_t &setStartStopPauseReset) {
  countDownMS = check_set_time (countDownMS);
  if (countDownMS == 0) {
      print_error_text (140);
  }
  //
  switch (setStartStopPauseReset) {
    case START_TIMER: start_timer ();  break;
    case RESET_TIMER: reset_timer ();  break;
    case PAUSE_TIMER: pause_timer ();  break;
    case STOP_TIMER:  stop_timer  ();  break;
    default:
      print_error_text (14);
    break;
  }
} // set_timer_state


void MTD2A_timer::start_timer () {
  if (processState == COMPLETE) {
    processState = ACTIVE;
    phaseChange  = true;
    phaseNumber  = START_TIMER;
    startProcess = true;
    stopProcess  = true;
    remainTimeMS = countDownMS;
  }
  if (processState == ACTIVE  &&  phaseNumber == PAUSE_TIMER) {
    phaseChange  = true;
    phaseNumber  = START_TIMER;
    stopPause    = true;
  }
} // start_timer


void MTD2A_timer::reset_timer () {
  if (processState == ACTIVE) {
    phaseChange  = true;
    phaseNumber  = RESET_TIMER;
    startProcess = true;
    stopProcess  = true;
    remainTimeMS = countDownMS;
    pauseTimeMS  = 0;
  }
} // reset_timer


void MTD2A_timer::pause_timer () {
  if (processState == ACTIVE  && phaseNumber == START_TIMER) {
    phaseChange  = true;
    phaseNumber  = PAUSE_TIMER;
    startPause   = true;
  }
} // pause_timer


void MTD2A_timer::stop_timer () {
  if (processState == ACTIVE) {
    processState = COMPLETE;
    phaseChange  = true;
    phaseNumber  = STOP_TIMER;
    stopProcess  = true;
    if (phaseNumber == PAUSE_TIMER) {
      stopPause = true;
    }
  }
} // stop_timer


// Setters -------------------------------------------------------------


void MTD2A_timer::set_countDownMS (const uint32_t &setCountDownMS) {
  if (processState == COMPLETE) {
    if (setCountDownMS == 0)
      print_error_text (140);
    else
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
  return remainTimeMS;
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
  loop_fast_pause ();
  loop_fast_start ();
  loop_fast_timer ();
} // loop_fast


void MTD2A_timer::loop_fast_pause () {
  if (startPause == true) {
    startPause    = false;
    phaseChange   = true;
    beginPauseMS  = globalSyncTimeMS;
    print_phase_line ();
  }
  if (stopPause == true) {
    stopPause   = false;
    phaseChange = true;
    endPauseMS  = globalSyncTimeMS;
    pauseTimeMS = pauseTimeMS + endPauseMS - beginPauseMS;
    print_phase_line ();
  }
} // loop_fast_pause


void MTD2A_timer::loop_fast_start () {
  if (startProcess == true) {
    startProcess = false;
    phaseChange  = true;
    startTimeMS  = globalSyncTimeMS;
    stopTimeMS   = 0;
    pauseTimeMS  = 0;
    print_phase_line ();
    phaseNumber  = START_TIMER; // override RESET_TIME
  }
  else if (phaseNumber != PAUSE_TIMER) {
    // checks for negative numbers
    if ((globalSyncTimeMS - pauseTimeMS - startTimeMS)  >  countDownMS) {
      remainTimeMS  = 0;
      elapsedTimeMS = countDownMS;
    }
    else {
      elapsedTimeMS  = globalSyncTimeMS - pauseTimeMS - startTimeMS;
      remainTimeMS = countDownMS - elapsedTimeMS;
    }
  }
} // loop_fast_start


void MTD2A_timer::loop_fast_timer () {
  if ((remainTimeMS) == 0  ||  phaseNumber == STOP_TIMER ) {
    if (stopProcess == true) {
      stopProcess  = false;
      phaseChange  = true;
      processState = COMPLETE;
      phaseNumber  = STOP_TIMER; 
      stopTimeMS   = globalSyncTimeMS;
      if (phaseNumber != STOP_TIMER) { 
        remainTimeMS = 0;
      }
      print_phase_line ();
    }
  }
} // loop_fast_timer


uint32_t MTD2A_timer::check_set_time (const uint32_t &setCheckTimeMS) {
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
  PortPrint  (F("  startTimeMS  : ")); PortPrintln(startTimeMS);
  PortPrint  (F("  stopTimeMS   : ")); PortPrintln(stopTimeMS);
  PortPrint  (F("  remainTimeMS : ")); PortPrintln(remainTimeMS);
  PortPrint  (F("  elapsedTimeMS: ")); PortPrintln(elapsedTimeMS);
  PortPrint  (F("  pauseTimeMS  : ")); PortPrintln(pauseTimeMS);
  PortPrint  (F("  beginPauseMS : ")); PortPrintln(beginPauseMS);
  PortPrint  (F("  endPauseMS   : ")); PortPrintln(endPauseMS);
} // print_conf 

#endif
