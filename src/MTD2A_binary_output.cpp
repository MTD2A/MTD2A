/**
 ******************************************************************************
 * @file    MTD2A_binary_output.cpp
 * @author  Joergen Bo Madsen
 * @version 1.3.1
 * @date    15. july 2026
 * @brief   Functions for MTD2A_binary_output.h (Model Train Detection And Action)
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


#include "math.h"
#include "MTD2A_binary_output.h"

// Global constants from MTD2A_base.h (MTD2A_const.h) 
using namespace MTD2A_const;


// Constructor
MTD2A_binary_output::MTD2A_binary_output
  (
    const char *setObjectName,
    uint32_t    setOutputTimeMS,
    uint32_t    setBeginDelayMS,
    uint32_t    setEndDelayMS,
    bool        setOutputMode,
    uint8_t     setBeginValue, 
    uint8_t     setEndValue,
    bool        setLoopActivate
  )
  :
    // Instantiated function pointer
    MTD2A{[](MTD2A* funcPtr) { static_cast<MTD2A_binary_output*>(funcPtr)->loop_fast(); }},
    outputMode    {setOutputMode}, 
    beginValue    {setBeginValue}, 
    endValue      {setEndValue},
    loopActivate  {setLoopActivate}
    // loopAutoStart {setLoopActivate}  // Starts immediately
  {
    MTD2A_add_function_pointer_loop_fast(this);
    objInstantiated = false;
    objectName      = MTD2A_set_object_name (setObjectName);
    outputTimeUS    = check_set_MS_to_US (setOutputTimeMS);
    beginDelayUS    = check_set_MS_to_US (setBeginDelayMS);
    endDelayUS      = check_set_MS_to_US (setEndDelayMS);
    beginValue      = check_pin_value (setBeginValue);
    endValue        = check_pin_value (setEndValue);
    objectError     = errorNumber;
    objInstantiated = true;
  }
// MTD2A_binary_output


void MTD2A_binary_output::initialize (uint8_t setPinNumber, bool setPinNormalOrInverted, uint8_t setPinStartValue) {
  if (initializedOnce == false) {
    if (objectError > 0) {
      print_error_text (ERR_OBJECT_INSTANT);
      print_error_text (objectError);
      objectError = 0;
    }
    initializedOnce = true;
    pinWriteMode  = setPinNormalOrInverted;
    check_pin_init (setPinNumber, setPinStartValue);
  }
  else {
    print_error_text (ERR_ALREADY_INIT);
  }
} // initialize


void MTD2A_binary_output::check_pin_init (uint8_t checkPinNumber, uint8_t checkpinStartValue) {
  if (checkPinNumber != NO_PIN) {
    uint8_t initPinFlags {DIGITAL_FLAG_0 | OUTPUT_FLAG_4};
    if (outputMode == P_W_M) {
      initPinFlags = initPinFlags | PWM_FLAG_5;
    }
    errorNumber = MTD2A_reserve_and_check_pin (checkPinNumber, initPinFlags);
    if (errorNumber == 0  ||  errorNumber >= WARNING_START) {  // OK or warning {128-255}: continue
      if (errorNumber >= WARNING_START) {
        print_error_text (errorNumber);  // Report warning, but keep the pin enabled
      }
      pinNumber     = checkPinNumber;
      pinWriteToggl = ENABLE;
      pinStartValue = check_pin_value (checkpinStartValue);
      pinMode(pinNumber, OUTPUT);
      write_pin_value (pinStartValue); // if pinWriteToggl == ENABLE
    }
    else { // Hard error {1-127}: disable pin
      print_error_text (errorNumber);
      pinWriteToggl = DISABLE;
      pinNumber     = NO_PIN;
    }
  }
  else {
    pinWriteToggl = DISABLE;
  }  
} // check_pin_init


// Activate function overloading
// Begin -----------------------------------------------------------------------------
void MTD2A_binary_output::activate () {
  if (processState == COMPLETE) {
    activate_check ();
    activate_process ();
  }  
  else {
    print_error_text (ERR_NOT_COMPLETE);
  }
}
void MTD2A_binary_output::activate (uint8_t setBeginValue) {
  if (processState == COMPLETE) {
    beginValue = check_pin_value (setBeginValue);
    activate_check ();
    activate_process ();
  }
  else {
    print_error_text (ERR_NOT_COMPLETE);
  }
}
void MTD2A_binary_output::activate (uint8_t setBeginValue, uint8_t setEndValue) {
  if (processState == COMPLETE) {
    beginValue = check_pin_value (setBeginValue);
    endValue   = check_pin_value (setEndValue);
    activate_check ();
    activate_process ();
  }
  else {
    print_error_text (ERR_NOT_COMPLETE);
  }
}
void MTD2A_binary_output::activate (uint8_t setBeginValue, uint8_t setEndValue, 
                                    uint8_t setPWMcurveType) {
  if (processState == COMPLETE) {
    if (check_PWM_pin_mode () == true) {  // Always true for NO_PIN
      outputMode = P_W_M;  // This overload selects PWM mode: values {0-255}
    }
    // else: pin defined but no PWM support (error 5) - outputMode stays BINARY
    PWMcurveType = check_PWM_curve (setPWMcurveType);  // NO_CURVE is valid: step beginValue -> endValue
    beginValue   = check_pin_value (setBeginValue);
    endValue     = check_pin_value (setEndValue);
    activate_check ();
    activate_process ();
  }
  else {
    print_error_text (ERR_NOT_COMPLETE);
  }
}

void MTD2A_binary_output::activate (uint8_t setBeginValue, uint8_t setEndValue, 
                                    uint8_t setPWMcurveType, uint32_t setOutputTimeMS) {
  if (processState == COMPLETE) {
    if (check_PWM_pin_mode () == true) {  // Always true for NO_PIN
      outputMode = P_W_M;  // This overload selects PWM mode: values {0-255}
    }
    // else: pin defined but no PWM support (error 5) - outputMode stays BINARY
    PWMcurveType = check_PWM_curve (setPWMcurveType);  // NO_CURVE is valid: step beginValue -> endValue
    beginValue   = check_pin_value (setBeginValue);
    endValue     = check_pin_value (setEndValue);
    outputTimeUS = check_set_MS_to_US (setOutputTimeMS);
    activate_check ();
    activate_process ();
  }
  else {
    print_error_text (ERR_NOT_COMPLETE);
  }
} // Activate
// End -----------------------------------------------------------------------------


void MTD2A_binary_output::activate_check () {
  if (outputMode == P_W_M  && PWMcurveType != NO_CURVE) {
    PWMcurveType = check_PWM_curve (PWMcurveType);
    if (beginValue == endValue) {
      PWMcurveType = NO_CURVE;
      print_error_text (WARN_NO_CURVE_VALUE);
      return;
    }
    if (outputTimeUS == (globalDelayTimeMS * MS_to_US)) {
      PWMcurveType = NO_CURVE;
      print_error_text (WARN_NO_CURVE_TIME);
      return;
    }
    if (outputTimeUS == 0) {
      PWMcurveType = NO_CURVE; // A curve needs an output time. Self-silencing:
      print_error_text (WARN_OUT_TIMER_ZERO); // prints once, not once per loop cycle
      return;
    }
    PWMloopCount = 0;
    PWM_curve_begin_end ();
  }
} // activate_check


void MTD2A_binary_output::activate_process (bool userActivate) {
  startPhase    = true;
  setStartPhase = true;
  // Warn on user activation only - stay silent on automatic loop restarts,
  // otherwise the same configuration warning repeats once per loop cycle.
  if (userActivate == true) {
    if (outputTimeUS == 0  &&  beginDelayUS == 0  &&  endDelayUS == 0) {
      print_error_text (WARN_ALL_TIMERS_ZERO); // No timing configured
    }
    else if (outputTimeUS == 0) {
      print_error_text (WARN_OUT_TIMER_ZERO);
    }
  }
  // Determine starting phase based on configured delays and output time
  if (beginDelayUS > 0)
    setPhaseNumber = BEGIN_PHASE; 
  else if (outputTimeUS > 0)
    setPhaseNumber = OUTPUT_PHASE;
  else if (endDelayUS > 0)
    setPhaseNumber = END_PHASE;
  else
    setPhaseNumber = COMPLETE_PHASE;
} // activate_process


bool MTD2A_binary_output::check_PWM_pin_mode () {
  if (pinNumber == NO_PIN) {
    return true;  // No physical pin: mode switch is always safe
  }
  uint8_t checkErrorNumber = MTD2A_reserve_and_check_pin (pinNumber, PWM_FLAG_5);
  if (checkErrorNumber > 0) {
    print_error_text (checkErrorNumber);  // Error 5 or warning 131
  }
  return (checkErrorNumber == 0  ||  checkErrorNumber >= WARNING_START);
}


// Setters ---------------------------------------------------------------------------------


void MTD2A_binary_output::set_outputMode (bool setBinaryOrPWM) {
  if (processState == COMPLETE) {
    if (setBinaryOrPWM == P_W_M) {
      if (check_PWM_pin_mode () == true) {
        outputMode = P_W_M;
      }
      // else: pin does not support PWM (error 5) - outputMode is unchanged
    }
    else {
      outputMode = BINARY;
    }
  }
  else {
    print_error_text (ERR_NOT_COMPLETE);
  }
} // set_outputMode


void MTD2A_binary_output::set_PWMcurveType (uint8_t setPWMcurveType) {
  if (processState == COMPLETE) {
    PWMcurveType = setPWMcurveType;
  }
  else {
    print_error_text (ERR_NOT_COMPLETE);
  }
} // set_PWMcurveType


void MTD2A_binary_output::set_pinWriteValue (uint8_t setPinWriteValue ) {
  if (processState == COMPLETE) {
    if (pinWriteToggl == ENABLE) { // if pinNumber != NO_PIN
      pinWriteValue = check_pin_value (setPinWriteValue);
      write_pin_value (pinWriteValue);
    }
    else {
      print_error_text (ERR_PIN_WRITE_DISABLED);
    }
  }
  else {
    print_error_text (ERR_NOT_COMPLETE);
  }  
}  // overloading
void MTD2A_binary_output::set_pinWriteValue (uint8_t setPinWriteValue, bool setPinBinaryOrPWM) {
  if (processState == COMPLETE) {
    if (pinWriteToggl == ENABLE) { // if pinNumber != NO_PIN
      if (setPinBinaryOrPWM == P_W_M) {
        if (check_PWM_pin_mode () == true) {
          outputMode = P_W_M;
        }
        // else: pin does not support PWM (error 5) - outputMode is unchanged,
        // and the value is written in the current (binary) mode
      }
      else {
        outputMode = BINARY;
      }
      pinWriteValue = check_pin_value (setPinWriteValue);
      write_pin_value (pinWriteValue); 
    }
    else {
      print_error_text (ERR_PIN_WRITE_DISABLED);
    }
  }
  else {
    print_error_text (ERR_NOT_COMPLETE);
  }  
} // set_pinWriteValue


void MTD2A_binary_output::set_pinWriteToggl (bool setPinEnableOrDisable) {
  if (processState == COMPLETE) {
    if (setPinEnableOrDisable == DISABLE) {
      pinWriteToggl = DISABLE;
    }
    else {
      if (pinNumber != NO_PIN) {
        pinWriteToggl = ENABLE;
      }
      else {
        print_error_text (ERR_PIN_NOT_DEFINED);
      }
    }
  }
  else {
    print_error_text (ERR_NOT_COMPLETE);
  }
}  // set_pinWriteToggl


void MTD2A_binary_output::set_pinWriteMode (bool setPinNormalOrInverted) {
  if (processState == COMPLETE) {
    pinWriteMode = setPinNormalOrInverted;
  }
  else {
    print_error_text (ERR_NOT_COMPLETE);
  }
} // set_pinWriteMode


void MTD2A_binary_output::set_outputTimeMS (uint32_t setOutputTimeMS) {
  if (processState == COMPLETE) {
    outputTimeUS = check_set_MS_to_US (setOutputTimeMS);
  }
  else {
    print_error_text (ERR_NOT_COMPLETE);
  }
} // set_outputTimeMS


void MTD2A_binary_output::set_beginDelayMS (uint32_t setBeginDelayMS) {
  if (processState == COMPLETE) {
    beginDelayUS = check_set_MS_to_US (setBeginDelayMS);
  } 
  else {
    print_error_text (ERR_NOT_COMPLETE);
  }
} // set_beginDelayMS


void MTD2A_binary_output::set_endDelayMS (uint32_t setEndDelayMS) {
  if (processState == COMPLETE) {
    endDelayUS = check_set_MS_to_US (setEndDelayMS);
  } 
  else {
    print_error_text (ERR_NOT_COMPLETE);
  }
} // set_endDelayMS


// Function overloading
void MTD2A_binary_output::set_timers (uint32_t setOutputTimeMS) {
  if (processState == COMPLETE) {
    outputTimeUS = check_set_MS_to_US (setOutputTimeMS);
  }
  else {
    print_error_text (ERR_NOT_COMPLETE);
  }
} 
void MTD2A_binary_output::set_timers (uint32_t setOutputTimeMS, uint32_t setBeginDelayMS) {
  if (processState == COMPLETE) {
    outputTimeUS = check_set_MS_to_US (setOutputTimeMS);
    beginDelayUS = check_set_MS_to_US (setBeginDelayMS);
  }
  else {
    print_error_text (ERR_NOT_COMPLETE);
  }
}
void MTD2A_binary_output::set_timers (uint32_t setOutputTimeMS, uint32_t setBeginDelayMS, uint32_t setEndDelayMS) {
  if (processState == COMPLETE) {
    outputTimeUS = check_set_MS_to_US (setOutputTimeMS);
    beginDelayUS = check_set_MS_to_US (setBeginDelayMS);
    endDelayUS   = check_set_MS_to_US (setEndDelayMS);
  }
  else {
    print_error_text (ERR_NOT_COMPLETE);
  }
} // set_timers


void MTD2A_binary_output::set_outputTimer (uint8_t setStopOrReset) {
  if (processState == ACTIVE) {
    if (outputTimeUS > 0) {
      switch (setStopOrReset) {
        case STOP_TIMER : stopOutputTimer  = ENABLE;       break;
        case RESET_TIMER: resetOutputTimer = ENABLE;       break;
        default: print_error_text (ERR_STOP_OR_RESET);  break; 
      }
    }
    else {
      print_error_text (WARN_OUT_TIMER_ZERO);
    }
  }
  else {
    print_error_text (ERR_NOT_ACTIVE);
  }
} // set_outputTimer


void MTD2A_binary_output::set_beginTimer (uint8_t setStopOrReset) {
  if (processState == ACTIVE) {
    if (beginDelayUS > 0) {
      switch (setStopOrReset) {
        case STOP_TIMER : stopBeginTimer  = ENABLE;        break;
        case RESET_TIMER: resetBeginTimer = ENABLE;        break;
        default: print_error_text (ERR_STOP_OR_RESET);  break; 
      }
    }
    else {
      print_error_text (WARN_TIMER_ZERO);
    }
  }
  else {
    print_error_text (ERR_NOT_ACTIVE);
  }  
} // set_beginTimer


void MTD2A_binary_output::set_endTimer (uint8_t setStopOrReset) {
  if (processState == ACTIVE) {
    if (endDelayUS > 0) {
      switch (setStopOrReset) {
        case STOP_TIMER : stopEndTimer  = ENABLE;          break;
        case RESET_TIMER: resetEndTimer = ENABLE;          break;
        default: print_error_text (ERR_STOP_OR_RESET);  break; 
      }
    }
    else {
      print_error_text (WARN_TIMER_ZERO);
    }
  }
  else {
    print_error_text (ERR_NOT_ACTIVE);
  }  
} // set_endTimer


void MTD2A_binary_output::set_loopActivate (bool setEnableOrDisable) {
  loopActivate = setEnableOrDisable;
}


void MTD2A_binary_output::set_outputOptimize (bool setEnableOrDisable) {
  outputOptimize = setEnableOrDisable;
} // set_outputOptimize


void MTD2A_binary_output::set_debugPrint (bool setEnableOrDisable) {
  debugPrint = setEnableOrDisable;
} // set_debugPrint


void MTD2A_binary_output::set_errorPrint (bool setEnableOrDisable) {
  errorPrint = setEnableOrDisable;
} // set_errorPrint


// Getters -----------------------------------------------


bool MTD2A_binary_output::get_pinWriteToggl () const {
  return pinWriteToggl;
}


bool MTD2A_binary_output::get_pinWriteMode () const {
    return pinWriteMode;
}


bool MTD2A_binary_output::get_outputMode () const {
  return outputMode;
}


uint8_t MTD2A_binary_output::get_PWMcurveType () const {
  return PWMcurveType;
}


uint8_t MTD2A_binary_output::get_outputValue () const {
  return outputValue;
}


bool MTD2A_binary_output::get_outputOptimize () const {
  return outputOptimize;
}

bool MTD2A_binary_output::get_outputState () const {
  return outputState;
}


bool MTD2A_binary_output::get_outputProcess () const {
  return outputProcess;
}


bool MTD2A_binary_output::get_processState () const {
  return processState; 
}


bool MTD2A_binary_output::get_phaseChange () const {
  return phaseChange;
}


uint8_t MTD2A_binary_output::get_phaseNumber () const {
  return phaseNumber;
}


bool MTD2A_binary_output::get_loopActivate () const{
  return loopActivate;
}


uint32_t MTD2A_binary_output::get_setBeginMS () const {
  return MTD2A_round_US_to_MS (setBeginUS);
}


uint32_t MTD2A_binary_output::get_setOutputMS () const {
  return MTD2A_round_US_to_MS (setOutputUS);
}


uint32_t MTD2A_binary_output::get_setEndMS () const {
  return MTD2A_round_US_to_MS (setEndUS);
}


uint8_t MTD2A_binary_output::get_reset_error () {
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


void MTD2A_binary_output::write_pin_value (uint8_t writePinValue) {
  outputValue = writePinValue;
  if (outputMode == BINARY) {
    if (pinWriteMode == INVERTED) {
      outputValue = !writePinValue;
    }
  }
  else {
    if (pinWriteMode == INVERTED) {
      outputValue = MAX_BYTE_VALUE - writePinValue;
    }
  }
  // DEBUG
  // PortPrint (outputValue); PortPrint ("-");
  //  
  if (pinNumber != NO_PIN  &&  pinWriteToggl == ENABLE) {
    if (outputMode == BINARY) {
      digitalWrite(pinNumber, outputValue);
    }
    else { // P_W_M
      analogWrite(pinNumber, outputValue);
    }
  }
} // write_pin_value


uint8_t MTD2A_binary_output::check_PWM_curve (uint8_t checkPWMcurveType) {
  if (checkPWMcurveType > MAX_PWM_CURVES) {
    print_error_text (WARN_UNDEF_PWM_CURVE);
    return NO_CURVE;
  }
  else
    return checkPWMcurveType;
} // check_PWM_curve


void MTD2A_binary_output::PWM_curve_begin_end () {
  if (endValue > beginValue) {
    PWMoffsetPoint = beginValue;
    PWMscaleFactor_F = static_cast<float>(endValue - beginValue) / MAX_BYTE_VALUE_F;
    if (PWMcurveType >= FALLING_XY && PWMcurveType <= FALLING_LED) {
      print_error_text (WARN_USE_RISING);
    }
  }
  else {
    PWMoffsetPoint = endValue;
    PWMscaleFactor_F = static_cast<float>(beginValue - endValue) / MAX_BYTE_VALUE_F;
    if (PWMcurveType >= RISING_XY && PWMcurveType <= RISING_LED) {
      print_error_text (WARN_USE_FALLING);
    }
  }
  PWMlastPinValue = beginValue; // skip first point. 
  PWMCurrPinValue = beginValue; // Done here: loop_fast_out_start ()
} // PWM_curve_begin_end


void MTD2A_binary_output::PWM_curve_step () {
  if (outputMode == P_W_M  &&  PWMcurveType != NO_CURVE) {
    // Use deterministic loop count instead of system clock elapsed time.
    // This eliminates jitter-induced interrupts, serial printing etc.

    // Gap correction: catches delays > globalDelayTimeMS that happened mid-output.
    uint32_t PWMelapsedTimeUS = globalSyncTimeUS - setOutputUS;
    uint32_t PWMexpectedLoops = PWMelapsedTimeUS / (globalDelayTimeMS * MS_to_US);
    if (PWMexpectedLoops > PWMloopCount) {
        PWMloopCount = PWMexpectedLoops;
    }
 
    // Multiply first, divide second — avoids float precision loss.
    float elapsedTimeUS_F = static_cast<float>(PWMloopCount) * static_cast<float>(globalDelayTimeMS * MS_to_US);
    float totalTimeUS_F   = static_cast<float>(outputTimeUS);
    PWMtimedPoint_F = roundf(elapsedTimeUS_F * MAX_BYTE_VALUE_F / totalTimeUS_F);

    // Overflow guard
    if (PWMtimedPoint_F > MAX_BYTE_VALUE_F) {
      PWMtimedPoint_F = MAX_BYTE_VALUE_F;
    }

    // Calculate PWM value and write PIN if there is a difference from last value.
    PWMCurrPinValue = PWM_curve_point (PWMtimedPoint_F, PWMcurveType);
    if (outputOptimize == ENABLE) {
      if (PWMCurrPinValue != PWMlastPinValue) {
        outputState = true;
        write_pin_value (PWMCurrPinValue);
      }
    }
    else {
      outputState = true;
      write_pin_value (PWMCurrPinValue);
    }
    PWMlastPinValue = PWMCurrPinValue;
    // Set to 0 when activate and reset 
    PWMloopCount++;
  } // if
} // PWM_curve_step


uint8_t MTD2A_binary_output::PWM_curve_point (const float &curvePointX_F, uint8_t curveType) {
  float curvePointY_F {0};
  //
  switch (curveType) {
  case NO_CURVE    : ; // do nothing
    break;
  case RISING_XY:
    curvePointY_F = curvePointX_F;
    break;
  case RISING_B05:
    curvePointY_F = pow(curvePointX_F, power05_F) * powFactor05_F;
    break;
  case RISING_B025:
    curvePointY_F = pow(curvePointX_F, power025_F) * powFactor025_F;
    break;
  case RISING_E05:
    curvePointY_F = MAX_BYTE_VALUE_F - pow((MAX_BYTE_VALUE_F - curvePointX_F), power05_F) * powFactor05_F;
    break;
  case RISING_E025:
    curvePointY_F = MAX_BYTE_VALUE_F - pow((MAX_BYTE_VALUE_F - curvePointX_F), power025_F) * powFactor025_F;
    break;
  case RISING_SM5:
    curvePointY_F  = PWM_sigmoid_5 (curvePointX_F);
    break;
  case RISING_SM8:
    curvePointY_F  = PWM_sigmoid_8 (curvePointX_F);
    break;
  case RISING_LED:
    curvePointY_F = pow (2, (curvePointX_F / LEDfactor_F)) - 1;
    break;
  case RISING_BZ1:
    curvePointY_F = PWM_rising_BZ1 (curvePointX_F);
    break;
  case RISING_BZ2:
    curvePointY_F = PWM_rising_BZ2 (curvePointX_F);
    break;
  case RISING_SIN0:
    curvePointY_F = sinf(curvePointX_F / MAX_BYTE_VALUE_F * halfPI_F) * MAX_BYTE_VALUE_F;
    break;
  case RISING_SIN270:
    curvePointY_F = MAX_BYTE_VALUE_F - (sinf ((curvePointX_F / MAX_BYTE_VALUE_F * halfPI_F) + halfPI_F) * MAX_BYTE_VALUE_F);
    break;
  // ----------------------------------------------------------------------------------
  case FALLING_XY:
    curvePointY_F = MAX_BYTE_VALUE_F - curvePointX_F;
    break;
  case FALLING_B05:
    curvePointY_F = MAX_BYTE_VALUE_F - pow(curvePointX_F, power05_F) * powFactor05_F;
    break;
  case FALLING_B025:
    curvePointY_F = MAX_BYTE_VALUE_F - pow(curvePointX_F, power025_F) * powFactor025_F;
    break;
  case FALLING_E05:
    curvePointY_F = pow((MAX_BYTE_VALUE_F - curvePointX_F), power05_F) * powFactor05_F;
    break;
  case FALLING_E025:
    curvePointY_F = pow((MAX_BYTE_VALUE_F - curvePointX_F), power025_F) * powFactor025_F;
    break;
  case FALLING_SM5:
    curvePointY_F = MAX_BYTE_VALUE_F - PWM_sigmoid_5 (curvePointX_F);
    break;
  case FALLING_SM8:
    curvePointY_F = MAX_BYTE_VALUE_F - PWM_sigmoid_8 (curvePointX_F);
    break;
  case FALLING_LED:
    curvePointY_F = pow (2, ((MAX_BYTE_VALUE_F - curvePointX_F) / LEDfactor_F)) - 1;
    break;
  case FALLING_BZ1:
    curvePointY_F = PWM_falling_BZ1 (curvePointX_F); 
    break;
  case FALLING_BZ2:
    curvePointY_F = PWM_falling_BZ2 (curvePointX_F); 
    break;
  case FALLING_SIN90:
    curvePointY_F = sinf((curvePointX_F / MAX_BYTE_VALUE_F * halfPI_F) + halfPI_F) * MAX_BYTE_VALUE_F;
    break;
  case FALLING_SIN180:
    curvePointY_F = MAX_BYTE_VALUE_F - (sinf (curvePointX_F / MAX_BYTE_VALUE_F * halfPI_F) * MAX_BYTE_VALUE_F);
    break;
  default: print_error_text (ERR_UNFORESEEN_ERROR);
  }
  return PWM_scale_point (curvePointY_F);
} // PWM_curve_point


float MTD2A_binary_output::PWM_sigmoid_5 (const float &curvePointX5_F) {
  float scalePointX5_F  = (curvePointX5_F * expoScale5_F) - sigmoidConst5_F;
  return           MAX_BYTE_VALUE_F / (1.0F + exp(-scalePointX5_F))
               - ((MAX_BYTE_VALUE_F /  2.0F) - curvePointX5_F) * expoAlign5_F;
} // PWM_sigmoid_5


float MTD2A_binary_output::PWM_sigmoid_8 (const float &curvePointX8_F) { 
  float scalePointX8_F = (curvePointX8_F * expoScale8_F) - sigmoidConst8_F;
  return          MAX_BYTE_VALUE_F / (1.0F + exp(-scalePointX8_F));
} // PWM_sigmoid_8


float MTD2A_binary_output::PWM_rising_BZ1 (const float &curvePointX1_F) {
    float bezierFactor_F = (curvePointX1_F / MAX_BYTE_VALUE_F * bezierConst1_F);
    return (1 - bezierFactor_F) * pow  (bezierFactor_F, 2) * bezierConst2_F * MAX_BYTE_VALUE_F;
} // PWM_rising_BZ1


float MTD2A_binary_output::PWM_falling_BZ1 (const float &curvePointX1_F) {
    float bezierFactor_F = (curvePointX1_F / MAX_BYTE_VALUE_F * (1 - bezierConst1_F)) + bezierConst1_F;
    return (1 - bezierFactor_F) * pow  (bezierFactor_F, 2) * bezierConst2_F * MAX_BYTE_VALUE_F;
} // PWM_faling_BZ1


float MTD2A_binary_output::PWM_rising_BZ2 (const float &curvePointX2_F) {
    float bezierFactor_F = ((MAX_BYTE_VALUE_F - curvePointX2_F) / MAX_BYTE_VALUE_F * (1 - bezierConst1_F)) + bezierConst1_F;
    return (1 - bezierFactor_F) * pow  (bezierFactor_F, 2) * bezierConst2_F * MAX_BYTE_VALUE_F;
} // PWM_rising_BZ2


float MTD2A_binary_output::PWM_falling_BZ2 (const float &curvePointX2_F) {
    float bezierFactor_F = (MAX_BYTE_VALUE_F - curvePointX2_F) / MAX_BYTE_VALUE_F * bezierConst1_F;
    return (1 - bezierFactor_F) * pow  (bezierFactor_F, 2) * bezierConst2_F * MAX_BYTE_VALUE_F;
} // PWM_faling_BZ2


uint8_t MTD2A_binary_output::PWM_scale_point (const float &curvePointY_F) {
  int16_t scalePointY; 
  scalePointY = static_cast<int16_t>(PWMoffsetPoint) + static_cast<int16_t>(roundf(curvePointY_F * PWMscaleFactor_F));
  // Range safety check
  if (scalePointY < 0)
    return 0;
  else if (scalePointY > MAX_BYTE_VALUE)
    return MAX_BYTE_VALUE;
  else
    return static_cast<uint8_t>(scalePointY); 
} // PWM_scale_point


// Main loop -----------------------------------------------


void MTD2A_binary_output::loop_fast () {
  if (setStartPhase == true) {
    setStartPhase = false;
    phaseNumber   = setPhaseNumber;
  }
  //
  outputState   = false;
  if (phaseChange == true) {
      phaseChange = false;
    if (phaseNumber != OUTPUT_PHASE) {
      outputProcess = false;
    }
  }
  //
  switch (phaseNumber) {
    case RESET_PHASE   : break;  // Legal idle state after reset () - wait for activate ()
    case BEGIN_PHASE   : loop_fast_begin_start ();
                         loop_fast_begin_timer ();  break;
    case OUTPUT_PHASE  : loop_fast_out_start   ();
                         loop_fast_out_timer   ();  break;
    case END_PHASE     : loop_fast_end_start   ();
                         loop_fast_end_timer   ();  break;
    case COMPLETE_PHASE: loop_fast_complete    ();  break;
    default: print_error_text (ERR_UNFORESEEN_ERROR);
  }
} // loop_fast


void MTD2A_binary_output::loop_fast_begin_start () {
  if (startPhase == true) {
    startPhase   = false;
    processState = ACTIVE;
    phaseChange  = true;
    setBeginUS   = globalSyncTimeUS;
    print_phase_line ();
  }
  // Check for timer reset
  if (resetBeginTimer == ENABLE) {
    resetBeginTimer = DISABLE;
    print_phase_line (RESET_TIMER);
    setBeginUS = globalSyncTimeUS;
  }
} // loop_fast_begin_start


void MTD2A_binary_output::loop_fast_begin_timer () {
  if ((globalSyncTimeUS - setBeginUS)  >=  (beginDelayUS - MARGIN_TIME_US)  ||  stopBeginTimer  == ENABLE) {
    startPhase  = true;
    stopBeginTimer = DISABLE;
    if (outputTimeUS > 0) {
      phaseNumber = OUTPUT_PHASE;
      loop_fast_out_start ();
    }
    else if (endDelayUS > 0) {
      phaseNumber = END_PHASE;
      loop_fast_end_start ();
    }
    else {
      loop_fast_complete ();
    }
  }
} // loop_fast_begin_timer


void MTD2A_binary_output::loop_fast_out_start () { 
  if (startPhase == true) {
    startPhase    = false;
    processState  = ACTIVE;
    phaseChange   = true;
    outputProcess = ACTIVE;
    outputState   = true;
    setOutputUS   = globalSyncTimeUS;
    PWMloopCount  = 0;    
    write_pin_value (beginValue); // if pinNumber != NO_PIN  &&  pinWriteToggl == ENABLE
    print_phase_line ();
  }
  // Check for timer reset
  if (resetOutputTimer == ENABLE) {
    resetOutputTimer = DISABLE;
    print_phase_line (RESET_TIMER);
    setOutputUS  = globalSyncTimeUS;
    PWMloopCount = 0; 
  }
} // loop_fast_out_start


void MTD2A_binary_output::loop_fast_out_timer () {
  if ((globalSyncTimeUS - setOutputUS) >= (outputTimeUS - MARGIN_TIME_US) ||  stopOutputTimer == ENABLE) {
    startPhase   = true;
    outputState  = true;
    stopOutputTimer = DISABLE;
    // output value
    if ((outputMode == P_W_M)  &&  (PWMcurveType != NO_CURVE)) {    
      if (outputOptimize == ENABLE) { 
        if (PWMCurrPinValue == endValue) {
          outputState = false; 
        }
      }
    }
    else {
      if ((outputOptimize == ENABLE)  &&  (beginValue == endValue)) {
        outputState = false;
      }
    }
    if (outputState == true) {
      write_pin_value (endValue); // if pinNumber != NO_PIN  &&  pinWriteToggl == ENABLE 
    }
    // next phase
    if (endDelayUS > 0) {
        phaseNumber = END_PHASE;
        loop_fast_end_start ();
    }
    else {
      loop_fast_complete ();
    }
  }
  else {
    PWM_curve_step ();
  }
} // loop_fast_out_timer


void MTD2A_binary_output::loop_fast_end_start () {
  if (startPhase == true) {
    startPhase   = false;
    processState = ACTIVE;
    phaseChange  = true;
    setEndUS     = globalSyncTimeUS;
    print_phase_line ();
  }
  // Check for timer reset
  if (resetEndTimer == ENABLE) {
    resetEndTimer = DISABLE;
    print_phase_line (RESET_TIMER);
    setEndUS = globalSyncTimeUS;
  }
} // loop_fast_end_start


void MTD2A_binary_output::loop_fast_end_timer () {
  if ((globalSyncTimeUS - setEndUS) >= (endDelayUS - MARGIN_TIME_US)  ||  stopEndTimer == ENABLE) {
    startPhase  = true;
    stopEndTimer = DISABLE;
    loop_fast_complete ();
  }
} // loop_fast_end_timer


void MTD2A_binary_output::loop_fast_complete () {
  processState = COMPLETE;
  bool restartLoop = (loopActivate == ENABLE  &&  startPhase == true);
  if (restartLoop == true  &&  outputTimeUS == 0  &&  beginDelayUS == 0  &&  endDelayUS == 0) {
    restartLoop = false;  // No timing configured: go idle instead of spinning every loop
    print_error_text (WARN_ALL_TIMERS_ZERO);
  }
  if (restartLoop == true) {
    phaseNumber = COMPLETE_PHASE; // One-loop [4] Complete strobe: operator bool ()
    phaseChange = true;     // fires once per completed loop cycle
    print_phase_line ();    // To many repeating phase lines?
    activate_check ();
    activate_process (false);
  }
  else {
    phaseNumber = COMPLETE_PHASE;
    if (startPhase == true) {
      startPhase  = false;
      phaseChange = true;
      print_phase_line ();
    }
  }
} // loop_fast_complete


void MTD2A_binary_output::reset () {
  processState     = COMPLETE;
  setBeginUS       = 0;
  setOutputUS      = 0;
  setEndUS         = 0;
  //
  errorNumber      = 0;
  startPhase       = false; 
  setStartPhase    = false;
  phaseChange      = true;
  phaseNumber      = RESET_PHASE;
  setPhaseNumber   = RESET_PHASE;
  //
  resetOutputTimer = DISABLE;
  resetBeginTimer  = DISABLE;
  resetEndTimer    = DISABLE;
  //
  stopOutputTimer  = DISABLE;
  stopBeginTimer   = DISABLE;
  stopEndTimer     = DISABLE;
  //
  outputValue      = pinStartValue;
  outputState      = false;
  outputProcess    = COMPLETE;
  pinWriteValue    = LOW;
  // PWM curves
  PWMoffsetPoint   = 0;
  PWMCurrPinValue  = 0;
  PWMlastPinValue  = 0;
  PWMscaleFactor_F = 0.0F;
  PWMloopCount     = 0;    
  write_pin_value  (pinStartValue); // if pinNumber != NO_PIN  &&  pinWriteToggl == ENABLE
  print_phase_line ();
}  // reset


uint32_t MTD2A_binary_output::check_set_MS_to_US (uint32_t setCheckTimeMS) {
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


uint8_t MTD2A_binary_output::check_pin_value (uint8_t checkPinValue) {
  if (outputMode == BINARY  &&  checkPinValue > 1) {
    print_error_text (WARN_BINARY_VALUE);
    return 1;
  }
  else {
    return checkPinValue;
  } 
} // check_pin_value


void MTD2A_binary_output::print_error_text (const uint8_t setErrorNumber) {
  errorNumber = setErrorNumber;
  if (objInstantiated == true) {
    MTD2A_print_error_text (objectName, (debugPrint == ENABLE || errorPrint == ENABLE), errorNumber, pinNumber);
  }
} // print_error_text


void MTD2A_binary_output::print_phase_text () {
  switch(phaseNumber) {
    case  0: PortPrintln(F("[0] Reset"));        break;
    case  1: PortPrintln(F("[1] Begin delay"));  break;
    case  2: PortPrintln(F("[2] Output timer")); break;
    case  3: PortPrintln(F("[3] End delay"));    break;
    case  4: PortPrintln(F("[4] Complete"));     break;
    default: print_error_text (ERR_UNFORESEEN_ERROR);
  }
} // print_phase_text


void MTD2A_binary_output::print_phase_line (uint8_t printResetTimer) {
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
  PortPrint  (F("  outputTimeMS : ")); PortPrintln (MTD2A_round_US_to_MS (outputTimeUS));
  PortPrint  (F("  beginDelayMS : ")); PortPrintln (MTD2A_round_US_to_MS (beginDelayUS));
  PortPrint  (F("  endDelayMS   : ")); PortPrintln (MTD2A_round_US_to_MS (endDelayUS));
  PortPrint  (F("  outputMode   : ")); if (outputMode == P_W_M)  PortPrintln (F("P_W_M"));  else PortPrintln (F("BINARY"));
  PortPrint  (F("  PWMcurveType : ")); if (PWMcurveType == 0) PortPrintln (F("0 (NO_CURVE)")); else PortPrintln (PWMcurveType);
  PortPrint  (F("  PWMloopCount : ")); PortPrintln (PWMloopCount);
  PortPrint  (F("  beginValue   : ")); MTD2A_print_value_binary (outputMode, beginValue);
  PortPrint  (F("  endValue     : ")); MTD2A_print_value_binary (outputMode, endValue);
  PortPrint  (F("  outputValue  : ")); MTD2A_print_value_binary (outputMode, outputValue);
  // pin and input setup
  MTD2A_print_pin_number (pinNumber);
  PortPrint  (F("  pinWriteToggl: ")); MTD2A_print_enable_disable  (pinWriteToggl);
  PortPrint  (F("  pinWriteMode : ")); MTD2A_print_normal_inverted (pinWriteMode);
  PortPrint  (F("  pinStartValue: ")); MTD2A_print_value_binary (outputMode, pinStartValue);
  PortPrint  (F("  pinWriteValue: ")); MTD2A_print_value_binary (outputMode, pinWriteValue);
  // timers
  PortPrint  (F("  setOutputMS  : ")); PortPrintln (MTD2A_round_US_to_MS (setOutputUS));
  PortPrint  (F("  setBeginMS   : ")); PortPrintln (MTD2A_round_US_to_MS (setBeginUS));
  PortPrint  (F("  setEndMS     : ")); PortPrintln (MTD2A_round_US_to_MS (setEndUS));
} // print_conf 

