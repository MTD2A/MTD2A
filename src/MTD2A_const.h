/**
 ******************************************************************************
 * @file    MTD2A_const.h
 * @author  Joergen Bo Madsen
 * @version V1.1.3
 * @date    30. august 2025
 * @brief   Global constants for MTD2A (Model Train Detection And Action)
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

 
#ifndef _MTD2A_const_H_
#define _MTD2A_const_H_

#include "Arduino.h"

namespace MTD2A_const {
  // Easy understanding global definitions
  constexpr bool     ENABLE         = true,  DISABLE      = false;
  constexpr bool     ACTIVE         = true,  COMPLETE     = false;
  constexpr bool     FIRST_TRIGGER  = true,  LAST_TRIGGER = false;
  constexpr bool     TIME_DELAY     = true,  MONO_STABLE  = false;
  constexpr bool     NORMAL         = true,  INVERTED     = false;
  constexpr bool     PULSE          = true,  FIXED        = false;
  constexpr bool     BINARY         = true,  P_W_M        = false;
  constexpr uint8_t  DELAY_10MS     = 10,    DELAY_5MS    = 5,    DELAY_1MS      = 1;
  constexpr uint8_t  MAX_BYTE_VALUE = 255,   PIN_ERROR_NO = 255;
  // Timers
  constexpr uint8_t  RESET_TIMER      = 0,  START_TIMER     = 1,  PAUSE_TIMER    = 2,  STOP_TIMER = 3;
  // Process phases
  constexpr uint8_t  RESET_PHASE      = 0; 
  constexpr uint8_t  BEGIN_PHASE      = 1,  OUTPUT_PHASE    = 2,  END_PHASE      = 3; // binary_input 
  constexpr uint8_t  FIRST_TIME_PHASE = 1,  LAST_TIME_PHASE = 2,  BLOCKING_PHASE = 3; // binary_output
  constexpr uint8_t  COMPLETE_PHASE   = 4;
  // PWM rising curves - binary_output
  constexpr uint8_t  MIN_PWM_VALUE = 0,   MAX_PWM_VALUE = 255;  
  constexpr uint8_t  NO_CURVE      = 0; 
  constexpr uint8_t  RISING_XY     = 1;
  constexpr uint8_t  RISING_B05    = 2,   RISING_B025   = 3;
  constexpr uint8_t  RISING_E05    = 4,   RISING_E025   = 5;
  constexpr uint8_t  RISING_SM8    = 6,   RISING_SM5    = 7;
  constexpr uint8_t  RISING_LED    = 8;
  // PWM falling curves
  constexpr uint8_t  FALLING_XY    = 9;
  constexpr uint8_t  FALLING_B05   = 10,  FALLING_B025  = 11;
  constexpr uint8_t  FALLING_E05   = 12,  FALLING_E025  = 13;
  constexpr uint8_t  FALLING_SM8   = 14,  FALLING_SM5   = 15;
  constexpr uint8_t  FALLING_LED   = 16;
} // namespace _MTD2A_const - binary_output

#endif
