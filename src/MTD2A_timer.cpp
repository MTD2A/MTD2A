/**
 ******************************************************************************
 * @file    MTD2A_timer.cpp
 * @author  Joergen Bo Madsen
 * @version 1.2.1
 * @date    15. july 2026
 * @brief   functions for MTD2A_timer.h base class (Model Train Detection And Action)
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
 * NOTE: Commands take effect at the next MTD2A_loop_execute ().
 * Multiple commands in the same loop are combined in issue order.
 * State getters reflect the last executed loop.
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


#include "MTD2A_timer.h"

// Global constants from MTD2A_base.h (MTD2A_const.h) 
using namespace MTD2A_const;


// Constructor
MTD2A_timer::MTD2A_timer
  (const char *setObjectName, uint32_t setCountDownMS)
  :
  // Instantiated Function pointer 
  MTD2A{[](MTD2A* funcPtr) { static_cast<MTD2A_timer*>(funcPtr)->loop_fast(); }}
  { 
    MTD2A_add_function_pointer_loop_fast(this);
    objInstantiated = false;
    objectName      = MTD2A_set_object_name (setObjectName);
    countDownUS     = check_set_MS_to_US (setCountDownMS);
    objectError     = errorNumber;
    objInstantiated = true;
  }
// MTD2A_timer


// Timer function overloading -----------------------------------------------------------------------------
void MTD2A_timer::timer (uint8_t setStartStopPauseReset) {
  if (setStartStopPauseReset <= STOP_TIMER) {
    if (setStartStopPauseReset == START_TIMER || setStartStopPauseReset == RESET_TIMER) {
      if (objectError > 0) {
        print_error_text (ERR_OBJECT_INSTANT);
        print_error_text (objectError);
        objectError = 0;
      }
    }
    set_timer_state (setStartStopPauseReset);
  }
  else {
    print_error_text (ERR_TIMER_ARGUMENT);
  }   
}
void MTD2A_timer::timer (uint8_t setStartStopPauseReset, uint32_t setCountDownMS) {
  if (setStartStopPauseReset <= STOP_TIMER) {
    switch (setStartStopPauseReset) {
      case START_TIMER:
        if (pending_state () == COMPLETE) {
          countDownUS = check_set_MS_to_US (setCountDownMS);
          objectError = 0;
        }
        else if (setPhaseNumber == PAUSE_TIMER) {
          print_error_text (WARN_COUNTDOWN_IGNORED);
        }
        break;
      case STOP_TIMER:
      case PAUSE_TIMER:
        print_error_text (WARN_COUNTDOWN_IGNORED);
        break;
      case RESET_TIMER:
        countDownUS = check_set_MS_to_US (setCountDownMS);  // Reset restarts regardless of state
        objectError = 0;
        break;
    }
    set_timer_state (setStartStopPauseReset);
  }
  else{
    print_error_text (ERR_TIMER_ARGUMENT);
  }
}  // timer


// Timer function overloading -----------------------------------------------------------------------------


void MTD2A_timer::set_timer_state (uint8_t setStartResetPauseStop) {
    switch (setStartResetPauseStop) {
      case START_TIMER: start_timer ();  break;
      case RESET_TIMER: reset_timer ();  break;
      case PAUSE_TIMER: pause_timer ();  break;
      case STOP_TIMER:  stop_timer  ();  break;
      default: print_error_text (ERR_TIMER_ARGUMENT);
    }
} // set_timer_state


void MTD2A_timer::start_timer () {
  if (pending_state () == COMPLETE) {
      startProcess   = true;
      endPause       = false;
      stopProcess    = false;
      setPhaseNumber = START_TIMER;
  }
  else { // ACTIVE
    if (setPhaseNumber == PAUSE_TIMER) {
      endPause       = true;
      calcProcess    = true;
      setPhaseNumber = START_TIMER; 
    }
    else {
      print_error_text (ERR_NOT_COMPLETE);
    }
  }
} // start_timer


void MTD2A_timer::reset_timer () {
  // Reset no matter processState
  startProcess   = true;
  beginPause     = false;
  endPause       = false;
  stopProcess    = false;
  setPhaseNumber = RESET_TIMER;

} // reset_timer


void MTD2A_timer::pause_timer () {
  if (pending_state () == ACTIVE) {
    if (setPhaseNumber == START_TIMER  ||  setPhaseNumber == RESET_TIMER) {  
      beginPause     = true;
      calcProcess    = false;
      setPhaseNumber = PAUSE_TIMER; 
    }
    else {
      print_error_text (WARN_PAUSE_ACTIVE);
    }
  }
  else {
    print_error_text (ERR_NOT_ACTIVE);
  }
} // pause_timer


void MTD2A_timer::stop_timer () {
  if (pending_state () == ACTIVE) {
    stopProcess    = true;
    calcProcess    = true;
    if (setPhaseNumber == PAUSE_TIMER) {
      endPause = true;
    }
    setPhaseNumber = STOP_TIMER;
  }
  else {
    print_error_text (ERR_NOT_ACTIVE);  
  }
} // stop_timer


bool MTD2A_timer::pending_state () const {
  // Effective processState including commands queued since the last loop.
  // stopProcess is checked FIRST: start_timer()/reset_timer() always clear a
  // stale stopProcess when accepted, so if both flags are set, the stop was
  // issued after the start and wins.
  if (stopProcess  == true)  return COMPLETE;
  if (startProcess == true)  return ACTIVE;
  return processState;
} // pending_state


// Setters -------------------------------------------------------------


void MTD2A_timer::set_countDownMS (uint32_t setCountDownMS) {
  if (pending_state () == COMPLETE) {
    countDownUS = check_set_MS_to_US (setCountDownMS);
  }
  else {
    print_error_text (ERR_NOT_COMPLETE);
  }
} // set_countDownMS


void MTD2A_timer::set_debugPrint (bool setEnableOrDisable) {
  debugPrint = setEnableOrDisable;
} // set_debugPrint


void MTD2A_timer::set_errorPrint (bool setEnableOrDisable) {
  errorPrint = setEnableOrDisable;
} // set_errorPrint


// getters -------------------------------------------------------------


uint32_t MTD2A_timer::get_startTimeMS () const {
  return MTD2A_round_US_to_MS (startTimeUS);
}

uint32_t MTD2A_timer::get_stopTimeMS () const {
  return MTD2A_round_US_to_MS (stopTimeUS);
}


uint32_t MTD2A_timer::get_pauseTimeMS () const {
  return MTD2A_round_US_to_MS (pauseTimeUS);
}


uint32_t MTD2A_timer::get_remaingTimeMS () const {
  return MTD2A_round_US_to_MS (remaingTimeUS);
}


uint32_t MTD2A_timer::get_elapsedTimeMS () const {
  return MTD2A_round_US_to_MS (elapsedTimeUS);
}


bool MTD2A_timer::get_processState () const {
  return processState; 
}


bool MTD2A_timer::get_phaseChange () const {
  return phaseChange;
}


uint8_t MTD2A_timer::get_phaseNumber () const {
  return phaseNumber;
}


uint8_t MTD2A_timer::get_reset_error () {
  uint8_t tempErrorNumber = errorNumber;
  if (objectError > 0) {
    print_error_text (0);
    print_error_text (objectError);
    objectError = 0;
  }
  errorNumber = 0;
  return tempErrorNumber;
} // get_reset_error


// Internal -------------------------------------------------------------


void MTD2A_timer::loop_fast () {
  phaseChange = false;
  phaseNumber = setPhaseNumber;
    loop_fast_start_reset ();
    loop_fast_pause_begin ();
    loop_fast_pause_guard (); 
    loop_fast_pause_end   ();
    loop_fast_calc_time   ();
    loop_fast_timer_stop  ();
} // loop_fast



void MTD2A_timer::loop_fast_start_reset () {
  if (startProcess == true) {
    startProcess  = false;
    if (countDownUS > 0) {
      calcProcess = (beginPause == false);
    }
    else {
      calcProcess = false;
      stopProcess = true;
    }
    phaseChange   = true;
    processState  = ACTIVE;
    startTimeUS   = globalSyncTimeUS;
    remaingTimeUS = countDownUS;
    elapsedTimeUS = 0;
    pauseTimeUS   = 0;
    pauseBeginUS  = 0;
    pauseEndUS    = 0;
    stopTimeUS    = 0;
    print_phase_line ();
  }
} // loop_fast_start_reset


void MTD2A_timer::loop_fast_pause_begin () {
  if (beginPause == true) {
    beginPause    = false;
    phaseChange   = true;
    pauseBeginUS  = globalSyncTimeUS;
    if (endPause == false) {
      print_phase_line ();
    }
  }
} // loop_fast_begin_pause_begin


void MTD2A_timer::loop_fast_pause_guard () {
  // elapsedTimeUS = (globalSyncTimeUS - pauseTimeUS) - startTimeUS is only
  // wrap-safe while the wall-clock span (countdown + all pauses) stays below
  // 2^32 us (~71.6 min). Enforce budget: countDown + pause <= MAX_TIME_US,
  // which leaves ~95 s margin. Checked every loop while paused.
  if (processState == ACTIVE  &&  phaseNumber == PAUSE_TIMER  &&  endPause == false) {
    uint32_t currentPauseUS = globalSyncTimeUS - pauseBeginUS;  // wrap-safe subtraction
    if ((currentPauseUS + pauseTimeUS) >= (MAX_TIME_US - countDownUS)) {
      print_error_text (WARN_TIME_PAUSE_MAX);
      endPause    = true;   // close pause bookkeeping this same loop
      calcProcess = true;   // compute final elapsed/remaining time with valid numbers
      stopProcess = true;   // then finalize via loop_fast_timer_stop ()
    }
  }
} // loop_fast_pause_guard


void MTD2A_timer::loop_fast_pause_end () {
  if (endPause == true) {
    endPause    = false;
    phaseChange = true;
    pauseEndUS  = globalSyncTimeUS;
    pauseTimeUS = (pauseEndUS - pauseBeginUS) + pauseTimeUS; // subtraction first, safe across overflow
    if (phaseNumber == PAUSE_TIMER  ||  phaseNumber == START_TIMER) {
      print_phase_line ();
    }
  }
} // loop_fast_end_pause_end


void MTD2A_timer::loop_fast_calc_time () {
  if (calcProcess == true) {
    elapsedTimeUS = (globalSyncTimeUS - pauseTimeUS) - startTimeUS;
    if (elapsedTimeUS + MARGIN_TIME_US >= countDownUS) {
      remaingTimeUS = 0;
      elapsedTimeUS = countDownUS;
      calcProcess   = false;
      stopProcess   = true;
    } 
    else {
      remaingTimeUS = countDownUS - elapsedTimeUS;
    }
  }
} // loop_fast_calc_time


void MTD2A_timer::loop_fast_timer_stop () {
  if (stopProcess == true) {
    stopProcess    = false;
    startProcess   = false;
    calcProcess    = false;
    beginPause     = false;
    endPause       = false;      
    phaseChange    = true;
    processState   = COMPLETE;
    phaseNumber    = STOP_TIMER;
    setPhaseNumber = STOP_TIMER; 
    stopTimeUS     = globalSyncTimeUS;
    print_phase_line ();
  }
} // loop_fast_timer_stop


uint32_t MTD2A_timer::check_set_MS_to_US (uint32_t setCheckTimeMS) {
  if (setCheckTimeMS > 0  &&  setCheckTimeMS < globalDelayTimeMS) {
    print_error_text (ERR_TIME_BELOW_DELAY);
    return globalDelayTimeMS * MS_to_US;
  }
  if (setCheckTimeMS > MAX_TIME_MS) {
    print_error_text (ERR_TIME_ABOVE_MAX);
    return MAX_TIME_MS * MS_to_US;
  }
  if (setCheckTimeMS == 0) {
    print_error_text (WARN_TIMER_ZERO);
  }
  return setCheckTimeMS * MS_to_US;
} // check_set_MS_to_US


void MTD2A_timer::print_phase_line () {
  if (debugPrint == ENABLE  ||  globalDebugPrint == ENABLE) {
    MTD2A_print_object_name (objectName);
    PortPrint(F(" "));
    print_phase_text ();
  }
} // print_phase_line


void MTD2A_timer::print_phase_text () {
  switch(phaseNumber) {
    case  0: PortPrint(F("[0] Reset"));  break;
    case  1: PortPrint(F("[1] Start"));  break;
    case  2: PortPrint(F("[2] Pause"));  break;
    case  3: PortPrint(F("[3] Stop"));   break;
    default: print_error_text (ERR_UNFORESEEN_ERROR);
  }
  PortPrintln(F(" timer"));    
} // print_phase_text


void MTD2A_timer::print_error_text (uint8_t setErrorNumber) {
  errorNumber = setErrorNumber;
  if (objInstantiated == true) {
    MTD2A_print_error_text (objectName, (debugPrint == ENABLE || errorPrint == ENABLE), errorNumber, NO_PIN);
  }
} // print_error_text


void MTD2A_timer::print_conf () {
  PortPrintln(F("MTD2A_timer: "));
  MTD2A_print_name_state (objectName, processState);
  PortPrint  (F("  phaseText    : ")); print_phase_text ();
  MTD2A_print_debug_error (debugPrint, errorPrint, errorNumber);
  // timers
  PortPrint  (F("  countDownMS  : ")); PortPrintln(MTD2A_round_US_to_MS (countDownUS));
  PortPrint  (F("  elapsedTimeMS: ")); PortPrintln(MTD2A_round_US_to_MS (elapsedTimeUS));
  PortPrint  (F("  remaingTimeMS: ")); PortPrintln(MTD2A_round_US_to_MS (remaingTimeUS));
  PortPrint  (F("  startTimeMS  : ")); PortPrintln(MTD2A_round_US_to_MS (startTimeUS));
  PortPrint  (F("  stopTimeMS   : ")); PortPrintln(MTD2A_round_US_to_MS (stopTimeUS));
  PortPrint  (F("  pauseTimeMS  : ")); PortPrintln(MTD2A_round_US_to_MS (pauseTimeUS));
  PortPrint  (F("  pauseBeginMS : ")); PortPrintln(MTD2A_round_US_to_MS (pauseBeginUS));
  PortPrint  (F("  pauseEndMS   : ")); PortPrintln(MTD2A_round_US_to_MS (pauseEndUS));
} // print_conf 

