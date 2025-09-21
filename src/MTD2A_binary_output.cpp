/**
 ******************************************************************************
 * @file    MTD2A_binary_output.cpp
 * @author  Joergen Bo Madsen
 * @version V1.1.8
 * @date    31.august 2025
 * @brief   functions for MTD2A_binary_output.h (Model Train Detection And Action)
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
#include "math.h"
#include "MTD2A_base.h"
#include "MTD2A_binary_output.h"


// Specific global constants from MTD2A_binary_output.h (MTD2A_const.h)
constexpr bool    MTD2A_binary_output::P_W_M;
constexpr bool    MTD2A_binary_output::BINARY;
// TIMERS
constexpr uint8_t MTD2A_binary_output::STOP_TIMER;
constexpr uint8_t MTD2A_binary_output::RESET_TIMER;
// Phases
constexpr uint8_t MTD2A_binary_output::RESET_PHASE; 
constexpr uint8_t MTD2A_binary_output::BEGIN_PHASE;
constexpr uint8_t MTD2A_binary_output::OUTPUT_PHASE; 
constexpr uint8_t MTD2A_binary_output::END_PHASE;
constexpr uint8_t MTD2A_binary_output::COMPLETE_PHASE;
// PWM curves
constexpr uint8_t MTD2A_binary_output::NO_CURVE;
// Rising
constexpr uint8_t MTD2A_binary_output::RISING_XY;
constexpr uint8_t MTD2A_binary_output::RISING_B05;
constexpr uint8_t MTD2A_binary_output::RISING_B025;
constexpr uint8_t MTD2A_binary_output::RISING_E05;
constexpr uint8_t MTD2A_binary_output::RISING_E025;
constexpr uint8_t MTD2A_binary_output::RISING_SM8;
constexpr uint8_t MTD2A_binary_output::RISING_SM5;
constexpr uint8_t MTD2A_binary_output::RISING_LED;
// Falling
constexpr uint8_t MTD2A_binary_output::FALLING_XY;
constexpr uint8_t MTD2A_binary_output::FALLING_B05;
constexpr uint8_t MTD2A_binary_output::FALLING_B025;
constexpr uint8_t MTD2A_binary_output::FALLING_E05;
constexpr uint8_t MTD2A_binary_output::FALLING_E025;
constexpr uint8_t MTD2A_binary_output::FALLING_SM8;
constexpr uint8_t MTD2A_binary_output::FALLING_SM5;
constexpr uint8_t MTD2A_binary_output::FALLING_LED;
// Base 
constexpr uint8_t MTD2A_binary_output::MAX_PWM_CURVES;


// Constructor
MTD2A_binary_output::MTD2A_binary_output
  (
    const char    *setObjectName,
    const uint32_t setOutputTimeMS,
    const uint32_t setBeginDelayMS,
    const uint32_t setEndDelayMS,
    const bool     setPinOutputMode,
    const uint8_t  setPinBeginValue, 
    const uint8_t  setPinEndValue
  )
  : outputTimeMS  {setOutputTimeMS},
    beginDelayMS  {setBeginDelayMS},
    endDelayMS    {setEndDelayMS},
    pinOutputMode {setPinOutputMode}, 
    pinBeginValue {setPinBeginValue}, 
    pinEndValue   {setPinEndValue},
    // Instatiated funtion pointer
    MTD2A{[](MTD2A* funcPtr) { static_cast<MTD2A_binary_output*>(funcPtr)->loop_fast(); }}
  {
    MTD2A_add_function_pointer_loop_fast(this);
    objectName = MTD2A_set_object_name(setObjectName);
  }
// MTD2A_binary_output


void MTD2A_binary_output::initialize (const uint8_t &setPinNumber, const bool &setPinNomalOrInverted, const uint8_t &setpinStartValue) {
  pinWriteMode  = setPinNomalOrInverted;
  pinStartValue = check_pin_value (setpinStartValue);
  if (pinWriteToggl == ENABLE)
    check_pin_init (setPinNumber);
  else
    pinNumber = PIN_ERROR_NO;
  // Check for previous instantiated object error
  outputTimeMS  = check_set_time  (outputTimeMS);
  beginDelayMS  = check_set_time  (beginDelayMS);
  endDelayMS    = check_set_time  (endDelayMS);
  pinBeginValue = check_pin_value (pinBeginValue);
  pinEndValue   = check_pin_value (pinEndValue);
  //
  if (outputTimeMS == 0 && beginDelayMS == 0 && endDelayMS == 0) {
    print_error_text (151); // No timing configured
  }
  else if (outputTimeMS == 0) {
    print_error_text (150);
  }
} // initialize


void MTD2A_binary_output::check_pin_init (const uint8_t &checkPinNumber) {
  uint8_t initPinFlags {DIGITAL_FLAG_0 | OUTPUT_FLAG_4};
  if (pinOutputMode == P_W_M)
     initPinFlags = initPinFlags | PWM_FLAG_5;
  errorNumber = MTD2A_reserve_and_check_pin (checkPinNumber, initPinFlags);
  if (errorNumber == 0) {
    pinNumber  = checkPinNumber;
    pinMode(pinNumber, OUTPUT);
    write_pin_value (pinStartValue);
  }
  else {
    print_error_text (errorNumber);
    pinWriteToggl = DISABLE;
    pinNumber     = PIN_ERROR_NO;
  }
} // check_pin_init


// Activate function overloading
void MTD2A_binary_output::activate () {
  if (processState == COMPLETE)
    activate_process ();
}
void MTD2A_binary_output::activate (const uint8_t &setPinBeginValue) {
  if (processState == COMPLETE) {
    pinBeginValue = check_pin_value (setPinBeginValue);
    activate_process ();
  }
}  
void MTD2A_binary_output::activate (const uint8_t &setPinBeginValue, const uint8_t &setPinEndValue) {
  if (processState == COMPLETE) {
    pinBeginValue = check_pin_value (setPinBeginValue);
    pinEndValue   = check_pin_value (setPinEndValue);
    activate_process ();
  }
}
void MTD2A_binary_output::activate (const uint8_t &setPinBeginValue, const uint8_t &setPinEndValue, const uint8_t &setPWMcurveType) {
  if (processState == COMPLETE) {
    pinOutputMode = P_W_M;
    pinBeginValue = check_pin_value (setPinBeginValue);
    pinEndValue   = check_pin_value (setPinEndValue);
    PWMcurveType  = check_PWM_curve (setPWMcurveType);
    if (PWMcurveType != NO_CURVE)
      PWM_curve_begin_end ();
    activate_process ();
  }
 }// Activate


void MTD2A_binary_output::activate_process () {
  processState = ACTIVE;
  phaseChange  = true;
  startPhase   = true;
  // Determine starting phase based on configured delays and output time
  if (beginDelayMS > 0)
      phaseNumber = BEGIN_PHASE; 
  else if (outputTimeMS > 0)
    phaseNumber = OUTPUT_PHASE;
  else if (endDelayMS > 0)
    phaseNumber = END_PHASE;
  else
    phaseNumber = COMPLETE_PHASE;
} // activate_process



// Setters -----------------------------------------------


void MTD2A_binary_output::set_pinWriteValue (const uint8_t &PinValue) {
  if (pinWriteToggl == ENABLE) {
    pinWriteValue = check_pin_value (PinValue);
    write_pin_value (pinWriteValue);
  }
  else {
    print_error_text (11);
  }
} // set_pinWriteValue


void MTD2A_binary_output::set_pinWriteToggl (const bool &setPinEnableOrDisable) {
  if (setPinEnableOrDisable == ENABLE  &&  pinNumber == PIN_ERROR_NO) {
    print_error_text (1);
  }
  else {
    pinWriteToggl = setPinEnableOrDisable;
  }
}  // set_pinWriteToggl


void MTD2A_binary_output::set_pinWriteMode (const bool &setPinNomalOrInverted) {
    pinWriteMode = setPinNomalOrInverted;
} // set_pinWriteMode


void MTD2A_binary_output::set_outputTimeMS (const uint32_t &setOutputTimeMS) {
  if (processState == COMPLETE) {
    if (setOutputTimeMS == 0)
      print_error_text (150);
    else
      outputTimeMS = check_set_time (setOutputTimeMS);
  } 
  else {
    print_error_text (12);
  }
} // set_outputTimeMS


void MTD2A_binary_output::set_beginDelayMS (const uint32_t &setBeginDelayMS) {
  if (processState == COMPLETE) {
    beginDelayMS = check_set_time (setBeginDelayMS);
  } 
  else {
    print_error_text (12);
  }
} // set_beginDelayMS


void MTD2A_binary_output::set_endDelayMS (const uint32_t &setEndDelayMS) {
  if (processState == COMPLETE) {
    endDelayMS = check_set_time  (setEndDelayMS);
  } 
  else {
    print_error_text (12);
  }
} // set_endDelayMS


void MTD2A_binary_output::set_outputTimer (const uint8_t &setOutputTimer) {
  if (outputTimeMS > 0) {
    switch (setOutputTimer) {
      case STOP_TIMER : stopOutputTM  = ENABLE; break;
      case RESET_TIMER: resetOutputTM = ENABLE; break;
      default: print_error_text (13);           break; 
    }
  }
  else {
    print_error_text (150);
  }
} // set_outputTimer


void MTD2A_binary_output::set_beginTimer (const uint8_t &setBeginTimer) {
  if (beginDelayMS > 0) {
    switch (setBeginTimer) {
      case STOP_TIMER : stopBeginTM  = ENABLE; break;
      case RESET_TIMER: resetBeginTM = ENABLE; break;
      default: print_error_text (13);          break; 
    }
  }
  else {
    print_error_text (140);
  }
} // set_beginTimer


void MTD2A_binary_output::set_endTimer (const uint8_t &setEndTimer) {
  if (endDelayMS > 0) {
    switch (setEndTimer) {
      case STOP_TIMER : stopEndTM  = ENABLE; break;
      case RESET_TIMER: resetEndTM = ENABLE; break;
      default: print_error_text (13);        break; 
    }
  }
  else {
    print_error_text (140);
  }
} // set_endTimer


void MTD2A_binary_output::set_debugPrint (const bool &setEnableOrDisable) {
  debugPrint = setEnableOrDisable;
} // set_debugPrint


void MTD2A_binary_output::set_errorPrint (const bool &setEnableOrDisable) {
  errorPrint = setEnableOrDisable;
} // set_errorPrint


// Getters -----------------------------------------------


bool const &MTD2A_binary_output::get_pinWriteToggl () const {
  return pinWriteToggl;
}


bool const &MTD2A_binary_output::get_pinWriteMode () const{
    return pinWriteMode;
} // get_pinWriteMode


uint8_t const &MTD2A_binary_output::get_pinOutputValue () const {
  return pinOutputValue;
}


bool const &MTD2A_binary_output::get_processState () const {
  return processState; 
}


bool const &MTD2A_binary_output::get_phaseChange () const {
  return phaseChange;
}


uint8_t const &MTD2A_binary_output::get_phaseNumber () const {
  return phaseNumber;
}


uint32_t const &MTD2A_binary_output::get_setBeginMS () const {
  return setBeginMS;
}


uint32_t const &MTD2A_binary_output::get_setOutputMS () const {
  return setOutputMS;
}


uint32_t const &MTD2A_binary_output::get_setEndMS () const {
  return setEndMS;
}


uint8_t const &MTD2A_binary_output::get_reset_error () {
  static uint8_t tempErrorNumber;
  tempErrorNumber = errorNumber;
  errorNumber = 0;
  return tempErrorNumber;
} // get_reset_error


// Internal -----------------------------------------------


uint32_t MTD2A_binary_output::check_set_time (const uint32_t &setCheckTimeMS) {
  if (setCheckTimeMS > 0 && setCheckTimeMS < globalDelayTimeMS) {
    print_error_text (9);
    return globalDelayTimeMS;
  }
  else
    return setCheckTimeMS;
} // check_set_time


uint8_t MTD2A_binary_output::check_pin_value (const uint8_t &checkPinValue) {
    if (pinOutputMode == BINARY  &&  checkPinValue > 1) {
      print_error_text (152);
      return 1;
    }
    else
      return checkPinValue;
} // check_pin_value


void MTD2A_binary_output::write_pin_value (const uint8_t &writePinValue) {
  pinOutputValue = writePinValue;
  if (pinOutputMode == BINARY) {
    if (pinWriteMode == INVERTED)
      pinOutputValue = !writePinValue;
  }
  else {
    if (pinWriteMode == INVERTED)
      pinOutputValue = MAX_BYTE_VALUE - writePinValue;
  }
  //  
  if (pinNumber != PIN_ERROR_NO  &&  pinWriteToggl == ENABLE) {
    if (pinOutputMode == BINARY) {
      digitalWrite(pinNumber, pinOutputValue);
    }
    else { 
      analogWrite(pinNumber, pinOutputValue);
    }
  }
} // write_pin_value


uint8_t MTD2A_binary_output::check_PWM_curve (const uint8_t &checkPWMcurveType) {
  if (checkPWMcurveType > MAX_PWM_CURVES) {
    print_error_text (153);
    return NO_CURVE;
  }
  else
    return checkPWMcurveType;
} // check_PWM_curve


void MTD2A_binary_output::PWM_curve_begin_end () {
  if (pinEndValue > pinBeginValue) {
    PWMoffstePoint = pinBeginValue;
    PWMscaleFactor = ((double)pinEndValue - (double)pinBeginValue) / (double)MAX_BYTE_VALUE;
    if (PWMcurveType >= FALLING_XY && PWMcurveType <= FALLING_SM5) {
      print_error_text (154);
    }
  }
  else {
    PWMoffstePoint = pinEndValue;
    PWMscaleFactor = ((double)pinBeginValue - (double)pinEndValue) / (double)MAX_BYTE_VALUE;
    if (PWMcurveType >= RISING_XY && PWMcurveType <= RISING_SM5) {
      print_error_text (155);
    }
  }
  // Devision by zero guard
  if (outputTimeMS == 0) {
    PWMtimeStep = 1;
    print_error_text (150);
  }
  else {
    PWMtimeStep = (double)outputTimeMS / (double)MAX_BYTE_VALUE;
  }
  PWMlastPoint = MAX_BYTE_VALUE;
  PWMcurrPoint = 0;
} // PWM_curve_begin_end


void MTD2A_binary_output::PWM_curve_step () {
  uint8_t PWMpinValue;
  PWMcurrPoint = (uint8_t)round((globalSyncTimeMS - setOutputMS) / PWMtimeStep);
  if (PWMcurrPoint != PWMlastPoint) {
    PWMlastPoint = PWMcurrPoint;
    PWMpinValue = PWM_curve_point (PWMcurrPoint, PWMcurveType);
    write_pin_value (PWMpinValue);
  }
} // PWM_curve_step


uint8_t MTD2A_binary_output::PWM_curve_point (const uint8_t &curvePointX, const uint8_t &curveType) {
  double curvePointY {0};
  //
  switch (curveType) {
  case NO_CURVE    : ; // do nothing
    break;
  case RISING_XY:
    curvePointY = curvePointX;
    break;
  case RISING_B05:
    curvePointY = pow((double)curvePointX, powPotens05) * powFactor05;
    break;
  case RISING_B025:
    curvePointY = pow((double)curvePointX, powPotens025) * powFactor025;
    break;
  case RISING_E05:
    curvePointY = (double)MAX_BYTE_VALUE - pow(((double)MAX_BYTE_VALUE - (double)curvePointX), powPotens05) * powFactor05;
    break;
  case RISING_E025:
    curvePointY = (double)MAX_BYTE_VALUE - pow(((double)MAX_BYTE_VALUE - (double)curvePointX), powPotens025) * powFactor025;
    break;
  case RISING_SM5:
    curvePointY  = PWM_sigmoid_5 (curvePointX);
    break;
  case RISING_SM8:
    curvePointY  = PWM_sigmoid_8 (curvePointX);
    break;
  case RISING_LED:
    curvePointY = pow (2, ((double)curvePointX / LEDfactor)) - 1;
    break;
  //
  case FALLING_XY:
    curvePointY = MAX_BYTE_VALUE - curvePointX;
    break;
  case FALLING_B05:
    curvePointY = (double)MAX_BYTE_VALUE - pow((double)curvePointX, powPotens05) * powFactor05;
    break;
  case FALLING_B025:
    curvePointY = (double)MAX_BYTE_VALUE - pow((double)curvePointX, powPotens025) * powFactor025;
    break;
  case FALLING_E05:
    curvePointY = pow(((double)MAX_BYTE_VALUE - (double)curvePointX), powPotens05) * powFactor05;
    break;
  case FALLING_E025:
    curvePointY = pow(((double)MAX_BYTE_VALUE - (double)curvePointX), powPotens025) * powFactor025;
    break;
  case FALLING_SM5:
    curvePointY = (double)MAX_BYTE_VALUE - PWM_sigmoid_5 (curvePointX);
    break;
  case FALLING_SM8:
    curvePointY = (double)MAX_BYTE_VALUE - PWM_sigmoid_8 (curvePointX);
    break;
  case FALLING_LED:
    curvePointY = pow (2, (((double)MAX_BYTE_VALUE - (double)curvePointX) / LEDfactor)) - 1;
    break;
  }
  return PWM_scale_point (curvePointY);
} // PWM_curve_point


double MTD2A_binary_output::PWM_sigmoid_5 (const uint8_t &curvePointX5) {
  double scalePointX5;
  scalePointX5  = ((double)curvePointX5 * expoScale5) - sigmoid5;
  return          (double)MAX_BYTE_VALUE / (1.0 + exp(-scalePointX5))
              - (((double)MAX_BYTE_VALUE /  2.0) - (double)curvePointX5) * expoAlign5;
} // PWM_sigmoid_5


double MTD2A_binary_output::PWM_sigmoid_8 (const uint8_t &curvePointX8) { 
  double scalePointX8;
  scalePointX8 = ((double)curvePointX8 * expoScale8) - sigmoid8;
  return         (double)MAX_BYTE_VALUE / (1.0 + exp(-scalePointX8));
} // PWM_sigmoid_8


uint8_t MTD2A_binary_output::PWM_scale_point (const double &curvePointY) {
int16_t scalePointY; 
  scalePointY = (int16_t)PWMoffstePoint + (int16_t)round(curvePointY * PWMscaleFactor);
  if (scalePointY > MAX_BYTE_VALUE)
    return MAX_BYTE_VALUE; 
    // inaccuracy in the calculation formula
  else
    return (uint8_t) scalePointY;
} // PWM_scale_point


// Main loop -----------------------------------------------


void MTD2A_binary_output::loop_fast () {
  phaseChange = false;
  switch (phaseNumber) {
    case BEGIN_PHASE   : loop_fast_begin_start ();
                         loop_fast_begin_timer ();  break;
    case OUTPUT_PHASE  : loop_fast_out_start   ();  
                         loop_fast_out_timer   ();  break;
    case END_PHASE     : loop_fast_end_start   ();
                         loop_fast_end_timer   ();  break;
    case COMPLETE_PHASE: loop_fast_complete    ();  break;
    }
} // loop_fast


void MTD2A_binary_output::loop_fast_begin_start () {
  if (startPhase == true) {
    startPhase  = false;
    phaseChange = true;
    setBeginMS  = globalSyncTimeMS;
    print_phase_line ();
  }
  // Check for timer reset
  if (resetBeginTM == ENABLE) {
    resetBeginTM = DISABLE;
    print_phase_line (RESET_TIMER);
    setBeginMS = globalSyncTimeMS;
  }
} // loop_fast_begin_start


void MTD2A_binary_output::loop_fast_begin_timer () {
  // Timer control
  if ((globalSyncTimeMS - setBeginMS) >= beginDelayMS  ||  stopBeginTM == ENABLE) {
    startPhase  = true;
    stopBeginTM = DISABLE;
    if (outputTimeMS > 0) {
      phaseNumber = OUTPUT_PHASE;
      loop_fast_out_start ();
      loop_fast_out_timer ();
    }
    else if (endDelayMS > 0) {
      phaseNumber = END_PHASE;
      loop_fast_end_start ();
      loop_fast_end_timer ();
    }
    else {
      processState = COMPLETE;
      phaseNumber  = COMPLETE_PHASE;
      loop_fast_complete ();
    }
  }
} // loop_fast_begin_timer


void MTD2A_binary_output::loop_fast_out_start () { 
  if (startPhase == true) {
    startPhase  = false;
    phaseChange = true;
    setOutputMS = globalSyncTimeMS;
    if (pinStartValue != pinBeginValue) 
      write_pin_value (pinBeginValue);
    print_phase_line ();
  }
  // Check for timer reset
  if (resetOutputTM == ENABLE) {
    resetOutputTM = DISABLE;
    print_phase_line (RESET_TIMER);
    setOutputMS = globalSyncTimeMS;
  }
} // loop_fast_out_start


void MTD2A_binary_output::loop_fast_out_timer () {
  if ((globalSyncTimeMS - setOutputMS) >= outputTimeMS  ||  stopOutputTM == ENABLE) {
    startPhase  = true;
    stopOutputTM = DISABLE;
    // Do not write the same value twice
    if (pinBeginValue != pinEndValue) 
      write_pin_value (pinEndValue);
    if (endDelayMS > 0) {
        phaseNumber = END_PHASE;
        loop_fast_end_start ();
        loop_fast_end_timer ();
    }
    else {
      processState = COMPLETE;
      phaseNumber  = COMPLETE_PHASE;
      loop_fast_complete ();
    }
  }
  else if (pinOutputMode == P_W_M  && PWMcurveType != NO_CURVE) {
    PWM_curve_step();
  }
} // loop_fast_out_timer


void MTD2A_binary_output::loop_fast_end_start () {
  if (startPhase == true) {
    startPhase  = false;
    phaseChange = true;
    setEndMS    = globalSyncTimeMS;
    print_phase_line ();
  }
  // Check for timer reset
  if (resetEndTM == ENABLE) {
    resetEndTM = DISABLE;
    print_phase_line (RESET_TIMER);
    setEndMS = globalSyncTimeMS;
  }
} // loop_fast_end_start


void MTD2A_binary_output::loop_fast_end_timer () {
  if ((globalSyncTimeMS - setEndMS) >= endDelayMS  ||  stopEndTM == ENABLE) {
    startPhase   = true;
    stopEndTM    = DISABLE;
    processState = COMPLETE;
    phaseNumber  = COMPLETE_PHASE;
    loop_fast_complete ();
  }
} // loop_fast_end_timer


void MTD2A_binary_output::loop_fast_complete () {
  if (startPhase == true) {
    startPhase = false;
    phaseChange = true;
    print_phase_line ();
  } 
} // loop_fast_complete


void MTD2A_binary_output::reset () {
  pinWriteValue  = LOW;
  processState   = COMPLETE;
  setBeginMS     = 0;
  setOutputMS    = 0;
  setEndMS       = 0;
  errorNumber    = 0;
  startPhase     = false; 
  phaseChange    = true;
  phaseNumber    = RESET_PHASE;
  stopOutputTM   = DISABLE;
  stopBeginTM    = DISABLE;
  stopEndTM      = DISABLE;
  pinOutputValue = pinStartValue;
  pinWriteValue  = LOW;
  // PWM curves
  PWMcurveType   = NO_CURVE;
  PWMoffstePoint = 0;
  PWMcurrPoint   = 0;
  PWMlastPoint   = MAX_BYTE_VALUE;
  PWMpinValue    = 0;
  PWMscaleFactor = 0.0;
  PWMtimeStep    = 0.0;
  if (pinNumber != PIN_ERROR_NO  && pinWriteToggl == ENABLE)
    write_pin_value (pinStartValue);
  print_phase_line ();
}  // reset


void MTD2A_binary_output::print_error_text (const uint8_t setErrorNumber) {
  errorNumber = setErrorNumber;
  MTD2A_print_object_name (objectName);
  MTD2A_print_error_text ((debugPrint == ENABLE || errorPrint == ENABLE), errorNumber, pinNumber);
} // print_error_text


void MTD2A_binary_output::print_phase_text () {
  switch(phaseNumber) {
    case 0: PortPrintln(F("[0] Reset"));        break;
    case 1: PortPrintln(F("[1] Begin delay"));  break;
    case 2: PortPrintln(F("[2] Output timer")); break;
    case 3: PortPrintln(F("[3] End delay"));    break;
    case 4: PortPrintln(F("[4] Complete"));     break;
  }
} // print_phase_text


void MTD2A_binary_output::print_phase_line (const uint8_t &printResetTimer) {
  if (debugPrint == ENABLE  ||  globalDebugPrint == ENABLE) {
    MTD2A_print_object_name (objectName);
    if (printResetTimer == RESET_TIMER) {
      PortPrint(F(": Reset timer >"));
    }
    PortPrint(F(" "));  
    print_phase_text ();
  }
} // print_phase_line


void MTD2A_binary_output::print_conf () {
  PortPrintln(F("MTD2A_binary_output: "));
  MTD2A_print_name_state (objectName, processState);
  PortPrint  (F("  phaseText    : ")); print_phase_text ();
  MTD2A_print_debug_error  (debugPrint, errorPrint, errorNumber);
  //
  PortPrint  (F("  outputTimeMS : ")); PortPrintln(outputTimeMS);
  PortPrint  (F("  beginDelayMS : ")); PortPrintln(beginDelayMS);
  PortPrint  (F("  endDelayMS   : ")); PortPrintln(endDelayMS);
  PortPrint  (F("  pinOutputMode: ")); if (pinOutputMode == P_W_M)  PortPrintln(F("P_W_M")); else PortPrintln(F("BINARY"));
  PortPrint  (F("  PWMcurveType : ")); PortPrintln(PWMcurveType);
  PortPrint  (F("  pinBeginValue: ")); MTD2A_print_value_binary (pinOutputMode, pinBeginValue);
  PortPrint  (F("  pinEndValue  : ")); MTD2A_print_value_binary (pinOutputMode, pinEndValue);
  // pin and input setup
  MTD2A_print_pin_number (pinNumber);
  PortPrint  (F("  pinWriteToggl: ")); MTD2A_print_enable_disable  (pinWriteToggl);
  PortPrint  (F("  pinWriteMode : ")); MTD2A_print_normal_inverted (pinWriteMode);
  PortPrint  (F("  pinStartValue: ")); MTD2A_print_value_binary (pinOutputMode, pinStartValue);
  PortPrint  (F("  pinWriteValue: ")); MTD2A_print_value_binary (pinOutputMode, pinWriteValue);
  // timers
  PortPrint  (F("  setOutputMS  : ")); PortPrintln(setOutputMS);
  PortPrint  (F("  setBeginMS   : ")); PortPrintln(setBeginMS);
  PortPrint  (F("  setEndMS     : ")); PortPrintln(setEndMS);
} // print_conf 
