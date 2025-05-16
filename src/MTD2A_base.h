/*
 ******************************************************************************
 * @file    MTD2A_base.h
 * @author  Joergen Bo Madsen
 * @version V1.1
 * @date    12. maj 2025
 * @brief   Abstract base Class for MTD2A (Model Train Detection And Action)
 * 
 * Supporting a vast variety of input sensors and output devices 
 * Simple to use to build complex solutions 
 * Non blocking, simple, yet efficient event-driven state machine
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

 
#ifndef _MTD2A_base_H_
#define _MTD2A_base_H_


//Easy understanding global definitions
#define enable  true
#define Enable  true
#define disable false
#define Disable false
//
#define active   true
#define Active   true
#define pending  false
#define Pending  false
#define complete false
#define Complete false
//
#define firstTrigger true
#define FirstTrigger true
#define LastTrigger  false
#define lastTrigger  false
//
#define timeDelay  true
#define TimeDelay  true
#define monoStable false
#define MonoStable false
// 
#define normal   true
#define Normal   true
#define inverted false
#define Inverted false
//
#define pulse  true
#define Pulse  true 
#define fixed  false
#define Fixed  false
//
#define binary true
#define Binary true
#define pwm    false
#define PWM    false


// Common phase
const uint8_t resetPhase {0};

class MTD2A  // base class
{ 
  // Static class that makes the attributes or method belong to the class itself instead of to instances of the class. 
  // synchronization and cadance control and common functions
  public:

    friend class MTD2A_binary_output;
    friend class MTD2A_binary_input;
    friend class MTD2A_delay; 
    friend class MTD2A_astable;
    friend class MTD2A_flip_flop;
    friend class MTD2A_sound;
    friend class MTD2A_servo;
    friend class MTD2A_DCC_input;

    static const uint8_t digitalFlag_0 {1}, analogFlag_1 {2}, inputFlag_2 {4}, pullupFlag_3 {8}, 
                         outputFlag_4 {16}, PWMFlag_5 {32}, toneFlag_6 {64}, interruptFlag_7 {128};

    static void    set_allDebugPrint           (const bool    &EnableOrDisable);
    
  private:
    static bool    allDebugPrint;
    //
    static char   *MTD2A_set_object_name       (const char    *setObjectName);
    static uint8_t MTD2A_reserve_and_check_pin (const uint8_t &checkPinNumber, const uint8_t &checkPinFlags, const bool &checkDebugPrint);
    static void    MTD2A_print_phase_line      (const bool    &printDebugPrint, const char *printObjectName, const char *printPhaseText);
    //Error and debug print
    static void    MTD2A_print_pin_error       (const uint8_t &printErrorNumber, const uint8_t &printPinNumber);
    static void    MTD2A_print_error_text      (const uint8_t &printErrorNumber);
    static void    MTD2A_print_debug_error     (const uint8_t &printErrorNumber, const bool    &printDebugPrint);
    static void    MTD2A_print_pin_number      (const uint8_t &printPinNumber);
    static void    MTD2A_print_generic_info    (const char    *printObjectName,  const bool    &printProcessState, const char *printPhaseText);
    static void    MTD2A_print_value_binary    (const bool    &binaryOrPWM,      const uint8_t &PrintValue);
    static void    MTD2A_print_enable_disable  (const bool    &enableOrDisable);
    static void    MTD2A_print_pulse_fixed     (const bool    &pulseOrFixed);

};  // MTD2A


#endif
