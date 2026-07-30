/**
 ******************************************************************************
 * @file    MTD2A_const.h
 * @author  Joergen Bo Madsen
 * @version 1.3.1
 * @date    15. july 2026
 * @brief   Global constants for MTD2A (Model Train Detection And Action)
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

 
#ifndef MTD2A_CONST_H_
#define MTD2A_CONST_H_


namespace MTD2A_const {
  // Easy understanding global definitions
  constexpr bool     ACTIVE         = true,  COMPLETE     = false;
  constexpr bool     FIRST_TRIGGER  = true,  LAST_TRIGGER = false;
  constexpr bool     TIME_DELAY     = true,  MONO_STABLE  = false;
  constexpr bool     ENABLE         = true,  DISABLE      = false;
  constexpr bool     NORMAL         = true,  INVERTED     = false;
  constexpr bool     PULSE          = true,  FIXED        = false;
  constexpr bool     BINARY         = true,  P_W_M        = false;
  // Global timer
  constexpr uint8_t  DELAY_10MS     = 10,    DELAY_5MS    = 5,    DELAY_2MS      = 2,   DELAY_1MS = 1;
  constexpr uint32_t MAX_TIME_MS    = 4200000; // 70 minutes
  // PIN
  constexpr uint8_t  MAX_BYTE_VALUE = 255,   NO_PIN       = 255;
  // Timer phases
  constexpr uint8_t  RESET_TIMER      = 0,  START_TIMER     = 1,  PAUSE_TIMER    = 2,  STOP_TIMER = 3;
  // Process phases
  constexpr uint8_t  RESET_PHASE      = 0; 
  constexpr uint8_t  FIRST_TIME_PHASE = 1,  LAST_TIME_PHASE = 2,  BLOCKING_PHASE = 3; // binary_input
  constexpr uint8_t  BEGIN_PHASE      = 1,  OUTPUT_PHASE    = 2,  END_PHASE      = 3; // binary_output
  constexpr uint8_t  COMPLETE_PHASE   = 4;
  // PWM rising curves - binary_output
  constexpr uint8_t  MIN_PWM_VALUE = 0,   MAX_PWM_VALUE = 255;  
  constexpr uint8_t  NO_CURVE      = 0; 
  constexpr uint8_t  RISING_XY     = 1;
  constexpr uint8_t  RISING_B05    = 2,   RISING_B025    = 3;
  constexpr uint8_t  RISING_E05    = 4,   RISING_E025    = 5;
  constexpr uint8_t  RISING_SM8    = 6,   RISING_SM5     = 7;
  constexpr uint8_t  RISING_BZ1    = 8,   RISING_BZ2     = 9;
  constexpr uint8_t  RISING_SIN0   = 10,  RISING_SIN270  = 11;
  constexpr uint8_t  RISING_LED    = 12;
  // PWM falling curves
  constexpr uint8_t  FALLING_XY    = 13;
  constexpr uint8_t  FALLING_B05   = 14,  FALLING_B025   = 15;
  constexpr uint8_t  FALLING_E05   = 16,  FALLING_E025   = 17;
  constexpr uint8_t  FALLING_SM8   = 18,  FALLING_SM5    = 19;
  constexpr uint8_t  FALLING_BZ1   = 20,  FALLING_BZ2    = 21;
  constexpr uint8_t  FALLING_SIN90 = 22,  FALLING_SIN180 = 23;
  constexpr uint8_t  FALLING_LED   = 24;

  // Error numbers {1-127} --------------------------------------------
  constexpr uint8_t  ERR_OBJECT_INSTANT    =   0;  // Header line: object instantiation error/warning
  constexpr uint8_t  ERR_PIN_NOT_DEFINED   =   1;  // Pin number not defined (255)
  constexpr uint8_t  ERR_DIGITAL_PIN_RANGE =   2;  // Digital pin number out of range
  constexpr uint8_t  ERR_ANALOG_PIN_RANGE  =   3;  // Analog pin number out of range
  constexpr uint8_t  ERR_OUTPUT_PIN_IN_USE =   4;  // Output pin already in use
  constexpr uint8_t  ERR_NO_PWM_SUPPORT    =   5;  // Pin does not support PWM
  constexpr uint8_t  ERR_TONE_PWM_CONFLICT =   6;  // tone() conflicts with PWM pin
  constexpr uint8_t  ERR_NO_INT_SUPPORT    =   7;  // Pin does not support interrupt
  constexpr uint8_t  ERR_INPUT_TYPE        =   8;  // Must be INPUT or INPUT_PULLUP
  constexpr uint8_t  ERR_TIME_BELOW_DELAY  =   9;  // Timer set to globalDelayTimeMS
  constexpr uint8_t  ERR_TIME_ABOVE_MAX    =  10;  // Timer set to MAX_TIME_MS
  constexpr uint8_t  ERR_PIN_WRITE_DISABLED=  11;  // Pin write is disabled
  constexpr uint8_t  ERR_NOT_COMPLETE      =  12;  // Process state must be COMPLETE
  constexpr uint8_t  ERR_STOP_OR_RESET     =  13;  // Select STOP_TIMER or RESET_TIMER
  constexpr uint8_t  ERR_TIMER_ARGUMENT    =  14;  // Unknown TIMER argument
  constexpr uint8_t  ERR_DELAY_RANGE       =  15;  // globalDelayTimeMS must be 1-10 MS
  constexpr uint8_t  ERR_NOT_ACTIVE        =  16;  // Process state must be ACTIVE
  constexpr uint8_t  ERR_UNFORESEEN_ERROR  =  17;  // Unforeseen system error
  constexpr uint8_t  ERR_TIMER_NOT_IN_USE  =  18;  // Must be ACTIVE and timer configured (> 0)
  constexpr uint8_t  ERR_ALREADY_INIT      =  19;  // Already initialized

  // Warning numbers {128-255} ----------------------------------------
  constexpr uint8_t  WARN_DIGITAL_NO_CHECK = 128;  // Digital pin check not possible
  constexpr uint8_t  WARN_ANALOG_NO_CHECK  = 129;  // Analog pin check not possible
  constexpr uint8_t  WARN_PIN_REUSED       = 130;  // Pin used more than once
  constexpr uint8_t  WARN_PWM_NO_CHECK     = 131;  // PWM pin check not possible
  constexpr uint8_t  WARN_INT_NO_CHECK     = 132;  // Interrupt pin check not possible
  constexpr uint8_t  WARN_TIMER_ZERO       = 140;  // Timer value is zero
  constexpr uint8_t  WARN_TIME_PAUSE_MAX   = 141;  // Time + pause exceeds MAX_TIME_MS
  constexpr uint8_t  WARN_COUNTDOWN_IGNORED= 142;  // setCountDownMS argument is ignored
  constexpr uint8_t  WARN_OUT_TIMER_ZERO   = 150;  // Output timer value is zero
  constexpr uint8_t  WARN_ALL_TIMERS_ZERO  = 151;  // All three timers are zero
  constexpr uint8_t  WARN_BINARY_VALUE     = 152;  // Binary pin value > 1. Set to HIGH
  constexpr uint8_t  WARN_UNDEF_PWM_CURVE  = 153;  // Undefined PWM curve
  constexpr uint8_t  WARN_USE_RISING       = 154;  // Use RISING curve instead of FALLING
  constexpr uint8_t  WARN_USE_FALLING      = 155;  // Use FALLING curve instead of RISING
  constexpr uint8_t  WARN_NO_CURVE_VALUE   = 156;  // beginValue = endValue => NO_CURVE
  constexpr uint8_t  WARN_NO_CURVE_TIME    = 157;  // Time = globalDelayTime => NO_CURVE
  constexpr uint8_t  WARN_PAUSE_ACTIVE     = 158;  // PAUSE already active

} // namespace MTD2A_const

#endif
