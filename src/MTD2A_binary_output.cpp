/*
 ******************************************************************************
 * @file    MTD2A_binary_output.cpp
 * @author  Joergen Bo Madsen
 * @version V1.1.6
 * @date    7.july 2025
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
#include "MTD2A_base.h"
#include "MTD2A_binary_output.h"


// Specific global constants from MTD2A_binary_output.h (MTD2A_const.h)
constexpr uint8_t MTD2A_binary_output::RESET_PHASE; 
constexpr uint8_t MTD2A_binary_output::BEGIN_PHASE;
constexpr uint8_t MTD2A_binary_output::OUTPUT_PHASE; 
constexpr uint8_t MTD2A_binary_output::END_PHASE;
constexpr uint8_t MTD2A_binary_output::COMPLETE_PHASE;
    // PWM curves
constexpr uint8_t MTD2A_binary_output::MIN_PWM_VALUE;
constexpr uint8_t MTD2A_binary_output::MAX_PWM_VALUE; 
constexpr uint8_t MTD2A_binary_output::NO_CURVE;
// RIsing
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
  : pinOutputMode {setPinOutputMode}, 
    pinBeginValue {setPinBeginValue}, 
    pinEndValue   {setPinEndValue},
    // Instatiated funtion pointer
    MTD2A{[](MTD2A* funcPtr) { static_cast<MTD2A_binary_output*>(funcPtr)->loop_fast(); }}
  {
    MTD2A_add_function_pointer_loop_fast(this);
    objectName    = MTD2A_set_object_name(setObjectName);
    outputTimeMS  = check_set_time  (setOutputTimeMS);
    beginDelayMS  = check_set_time  (setBeginDelayMS);
    endDelayMS    = check_set_time  (setEndDelayMS);
    pinBeginValue = check_pin_value (pinBeginValue);
    pinEndValue   = check_pin_value (pinEndValue);
    if (outputTimeMS == 0)
      errorNumber = 150;
    if (outputTimeMS == 0 && beginDelayMS == 0 && endDelayMS == 0)
      errorNumber = 151; // No timing configured
  }
// MTD2A_binary_output


uint32_t MTD2A_binary_output::check_set_time (const uint32_t &setCheckTimeMS) {
  if (setCheckTimeMS > 0 && setCheckTimeMS < globalDelayTimeMS) {
    errorNumber = 9; 
    MTD2A_print_error_text ((debugPrint == ENABLE || errorPrint == ENABLE), errorNumber, pinNumber);
    return globalDelayTimeMS;
  }
  else
    return setCheckTimeMS;
} // check_set_time


void MTD2A_binary_output::initialize (const uint8_t &setPinNumber, const bool &setPinNomalOrInverted, const uint8_t &setStartPinValue) {
  // Check for instantiate object error
  if (errorNumber > 0)
    MTD2A_print_error_text ((debugPrint == ENABLE || errorPrint == ENABLE), errorNumber, pinNumber);
  uint8_t initPinFlags {DIGITAL_FLAG_0 | OUTPUT_FLAG_4};
  if (pinOutputMode == P_W_M)
     initPinFlags = initPinFlags | PWM_FLAG_5;
  errorNumber = MTD2A_reserve_and_check_pin (setPinNumber, initPinFlags);
  if (errorNumber == 0) {
    pinWrite  = ENABLE;
    pinNumber = setPinNumber;
    pinOutput = setPinNomalOrInverted;
    startPinValue = check_pin_value (setStartPinValue);
    pinMode(pinNumber, OUTPUT);
    write_pin_value (startPinValue);
  }
  else {
    MTD2A_print_error_text ((debugPrint == ENABLE || errorPrint == ENABLE), errorNumber, setPinNumber); 
    pinWrite = DISABLE;
    pinNumber = PIN_ERROR_NO;
  }
} // initialize


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
}
void MTD2A_binary_output::activate (const uint8_t &setPinBeginValue, const uint8_t &setPinEndValue, const uint8_t &setPWMcurveType, const bool &LoopFastOnce) {
  if (processState == COMPLETE) {
    pinOutputMode = P_W_M;
    pinBeginValue = check_pin_value (setPinBeginValue);
    pinEndValue   = check_pin_value (setPinEndValue);
    PWMcurveType  = check_PWM_curve (setPWMcurveType);
    if (PWMcurveType != NO_CURVE)
      PWM_curve_begin_end ();
    activate_process ();
    if (LoopFastOnce == ENABLE)
      loop_fast();
  }
}  // Activate


void MTD2A_binary_output::activate_process () {
  processState = ACTIVE;
  phaseChange  = true;
  // Determine starting phase based on configured delays
  if (beginDelayMS > 0)
      phaseNumber = BEGIN_PHASE; 
  else if (outputTimeMS > 0)
    phaseNumber = OUTPUT_PHASE;
  else if (endDelayMS > 0)
    phaseNumber = END_PHASE;
  else
    phaseNumber = COMPLETE_PHASE;
} // activate_process



void MTD2A_binary_output::set_pinWrite (const bool &setPinEnableOrDisable, const bool &LoopFastOnce) {
  if (setPinEnableOrDisable == ENABLE  &&  pinNumber == PIN_ERROR_NO) {
    errorNumber = 1; 
    MTD2A_print_error_text ((debugPrint == ENABLE || errorPrint == ENABLE), errorNumber, pinNumber);  
  }
  else {
    pinWrite = setPinEnableOrDisable;
    if (LoopFastOnce == ENABLE)
      loop_fast();
  }
}  // set_pinWrite


void MTD2A_binary_output::set_pinOutput (const bool &setPinNomalOrInverted, const bool &LoopFastOnce) {
  if (pinNumber != PIN_ERROR_NO) {
    pinOutput = setPinNomalOrInverted;
    if (LoopFastOnce == ENABLE)
      loop_fast();
  }
  else {
    errorNumber = 1; 
    MTD2A_print_error_text ((debugPrint == ENABLE || errorPrint == ENABLE), errorNumber, pinNumber);
  }
} // set_pinOutput


void MTD2A_binary_output::set_setPinValue  (const uint8_t &setSetPinValue, const bool &LoopFastOnce) {
  if (pinWrite == ENABLE) {
    setPinValue = check_pin_value (setSetPinValue);
    write_pin_value (setPinValue);
    if (LoopFastOnce == ENABLE)
      loop_fast();
  }
  else {
    errorNumber = 11; 
    MTD2A_print_error_text ((debugPrint == ENABLE || errorPrint == ENABLE), errorNumber, pinNumber);
  }
} // set_setPinValue


void MTD2A_binary_output::set_outputTimer (const bool &changeOutputTimer, const bool &LoopFastOnce) {
  if (changeOutputTimer == STOP_TIMER) 
    stopOutputTM = ENABLE;
  else { // Restart timer
    print_phase_line (RESTART_TIMER);
    setOutputMS = globalSyncTimeMS;
  }
  if (LoopFastOnce == ENABLE)
    loop_fast();
} // set_outputTimer


void MTD2A_binary_output::set_beginTimer (const bool &changeBeginTimer, const bool &LoopFastOnce) {
  if (changeBeginTimer == STOP_TIMER) 
    stopBeginTM = ENABLE;
  else { // Restart timer
    print_phase_line (RESTART_TIMER);
    setBeginMS = globalSyncTimeMS;
  }
  if (LoopFastOnce == ENABLE)
    loop_fast();
} // set_beginTimer


void MTD2A_binary_output::set_endTimer (const bool &changeEndTimer, const bool &LoopFastOnce) {
  if (changeEndTimer == STOP_TIMER) 
    stopEndTM = ENABLE;
  else { // Restart timer
    print_phase_line (RESTART_TIMER);
    setEndMS = globalSyncTimeMS;
  }
  if (LoopFastOnce == ENABLE)
    loop_fast();
} // set_endTimer


void MTD2A_binary_output::set_debugPrint (const bool &setEnableOrDisable, const bool &LoopFastOnce) {
  debugPrint = setEnableOrDisable;
  if (LoopFastOnce == ENABLE)
    loop_fast();
} // set_debugPrint


bool const &MTD2A_binary_output::get_pinWrite () const {
  return pinWrite;
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


uint8_t MTD2A_binary_output::check_pin_value (const uint8_t &checkPinValue) {
    if (pinOutputMode == BINARY  &&  checkPinValue > 1) {
      errorNumber = 152; 
      MTD2A_print_error_text ((debugPrint == ENABLE || errorPrint == ENABLE), errorNumber, pinNumber);
      return 1;
    }
    else
      return checkPinValue;
} // check_pin_value



void MTD2A_binary_output::write_pin_value (const uint8_t &writePinValue) {
  if (pinWrite == ENABLE  && pinNumber != PIN_ERROR_NO) {
    if (pinOutputMode == BINARY) {
      if (pinOutput == NORMAL) {
        digitalWrite(pinNumber, writePinValue);
      }
      else { // INVERTED
        digitalWrite(pinNumber, !writePinValue);
      }
    } 
    else {
      if (pinOutput == NORMAL) {
        analogWrite(pinNumber, writePinValue);
      }
      else { // INVERTED
        analogWrite(pinNumber, MAX_BYTE_VALUE - writePinValue);
      }
    }
  }
} // write_pin_value


uint8_t MTD2A_binary_output::check_PWM_curve (const uint8_t &checkPWMcurveType) {
  if (checkPWMcurveType > MAX_PWM_CURVES) {
    errorNumber = 153; 
    MTD2A_print_error_text ((debugPrint == ENABLE || errorPrint == ENABLE), errorNumber, pinNumber);
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
      errorNumber = 154; 
      MTD2A_print_error_text ((debugPrint == ENABLE || errorPrint == ENABLE), errorNumber, pinNumber);
    }
  }
  else {
    PWMoffstePoint = pinEndValue;
    PWMscaleFactor = ((double)pinBeginValue - (double)pinEndValue) / (double)MAX_BYTE_VALUE;
    if (PWMcurveType >= RISING_XY && PWMcurveType <= RISING_SM5) {
      errorNumber = 155; 
      MTD2A_print_error_text ((debugPrint == ENABLE || errorPrint == ENABLE), errorNumber, pinNumber);
    }
  }
  PWMtimeStep = (double)outputTimeMS / (double)MAX_BYTE_VALUE;
  PWMlastPoint = MAX_BYTE_VALUE;
  PWMcurrPoint = 0;
} // PWM_curve_begin_end


uint8_t MTD2A_binary_output::PWM_scale_point (const double &curvePointY) {
int16_t scalePointY; 
  scalePointY = (int16_t)PWMoffstePoint + (int16_t)round(curvePointY * PWMscaleFactor);
  if (scalePointY > MAX_BYTE_VALUE)
    return MAX_BYTE_VALUE; 
    // inaccuracy in the calculation formula
  else
    return (uint8_t) scalePointY;
} // constrain_point_Y 


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


void MTD2A_binary_output::PWM_curve_step () {
  uint8_t PWMpinValue;
  PWMcurrPoint = (uint8_t)round((globalSyncTimeMS - setOutputMS) / PWMtimeStep);
  if (PWMcurrPoint != PWMlastPoint) {
    PWMlastPoint = PWMcurrPoint;
    PWMpinValue = PWM_curve_point (PWMcurrPoint, PWMcurveType);
    write_pin_value (PWMpinValue);
  }
} // PWM_curve_step


void MTD2A_binary_output::loop_fast () {
  if (processState == ACTIVE) {
    // RESET_PHASE: do nothing
    if (phaseNumber == BEGIN_PHASE) 
      loop_fast_begin ();
    else if (phaseNumber == OUTPUT_PHASE) {  
      loop_fast_out_begin (); 
      loop_fast_out_end ();  
    }
    else if (phaseNumber == END_PHASE)
      loop_fast_end ();
    else if (phaseNumber == COMPLETE_PHASE)
      loop_fast_complete ();
  }
} // loop_fast


void MTD2A_binary_output::loop_fast_begin () {
  if (phaseChange == true) {
    phaseChange = false;
    setBeginMS  = globalSyncTimeMS;
    print_phase_line ();
  }
  if ((globalSyncTimeMS - setBeginMS) >= beginDelayMS  ||  stopBeginTM == ENABLE) {
    phaseChange = true;
    stopBeginTM = DISABLE;
    if (outputTimeMS > 0)
      phaseNumber = OUTPUT_PHASE;
    else if (endDelayMS > 0)
      phaseNumber = END_PHASE;
    else
      phaseNumber = COMPLETE_PHASE;
  }
} // loop_fast_begin


void MTD2A_binary_output::loop_fast_out_begin () { 
  if (phaseChange == true) {
    phaseChange = false;
    setOutputMS = globalSyncTimeMS;
    write_pin_value (pinBeginValue);
    print_phase_line ();
  }
} // loop_fast_out_begin


void MTD2A_binary_output::loop_fast_out_end () {
  if ((globalSyncTimeMS - setOutputMS) >= outputTimeMS  ||  stopOutputTM == ENABLE) {
    phaseChange = true;
    stopOutputTM = DISABLE;
    // Do not write the same value twice
    if (pinBeginValue != pinEndValue) 
      write_pin_value (pinEndValue);
    if (endDelayMS > 0)
      phaseNumber = END_PHASE;
    else
      phaseNumber = COMPLETE_PHASE;
  }
  else if (pinOutputMode == P_W_M  && PWMcurveType != NO_CURVE) {
    PWM_curve_step();
  }
} // loop_fast_out_end


void MTD2A_binary_output::loop_fast_end () {
    if (phaseChange == true) {
      phaseChange = false;
      setEndMS = globalSyncTimeMS;
      print_phase_line ();
    }
    if ((globalSyncTimeMS - setEndMS) >= endDelayMS  ||  stopEndTM == ENABLE) {
      phaseNumber = COMPLETE_PHASE;
      phaseChange = true;
      stopEndTM = DISABLE;
    }
} // loop_fast_end


void MTD2A_binary_output::loop_fast_complete () {
  processState = COMPLETE;
  phaseChange  = false;
  print_phase_line ();
} // loop_fast_complete


void MTD2A_binary_output::reset () {
  setPinValue   = LOW;
  processState  = COMPLETE;
  setBeginMS    = 0;
  setOutputMS   = 0;
  setEndMS      = 0;
  errorNumber   = 0;
  phaseChange   = false;
  phaseNumber   = RESET_PHASE;
  stopOutputTM  = DISABLE;
  stopBeginTM   = DISABLE;
  stopEndTM     = DISABLE;
  if (pinNumber == PIN_ERROR_NO)
    pinWrite = DISABLE;
  else {
    pinWrite = ENABLE;
    write_pin_value (startPinValue);
  }
  print_phase_line ();
}  // reset


void MTD2A_binary_output::print_phase_text() {
  switch(phaseNumber) {
    case 0: PortPrintln(F("[0] Reset"));         break;
    case 1: PortPrintln(F("[1] First time"));    break;
    case 2: PortPrintln(F("[2] Last time"));     break;
    case 3: PortPrintln(F("[3] Pin blocking"));  break;
    case 4: PortPrintln(F("[4] Complete"));      break;
  }
} // print_phase_text


void MTD2A_binary_output::print_phase_line (const bool &printRestartTimer) {
  if (debugPrint == ENABLE  ||  globalDebugPrint == ENABLE) {
    MTD2A_print_object_name (objectName);
    if (printRestartTimer == RESTART_TIMER)
      PortPrint(F(": Restart timer "));
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
  PortPrint  (F("  pinBeginValue: ")); MTD2A_print_value_binary (pinOutputMode, pinBeginValue);
  PortPrint  (F("  pinEndValue  : ")); MTD2A_print_value_binary (pinOutputMode, pinEndValue);
  // pin and input setup
  MTD2A_print_pin_number (pinNumber);
  PortPrint  (F("  pinWrite     : ")); MTD2A_print_enable_disable (pinWrite);
  PortPrint  (F("  pinOutput    : ")); MTD2A_print_normal_inverted (pinOutput);
  PortPrint  (F("  startPinValue: ")); MTD2A_print_value_binary (pinOutputMode, startPinValue);
  PortPrint  (F("  setPinValue  : ")); MTD2A_print_value_binary (pinOutputMode, setPinValue);
  // timers
  PortPrint  (F("  setBeginMS   : ")); PortPrintln(setBeginMS);
  PortPrint  (F("  setOutputMS  : ")); PortPrintln(setOutputMS);
  PortPrint  (F("  setEndMS     : ")); PortPrintln(setEndMS);
} // print_conf 
