/*
 ******************************************************************************
 * @file    MTD2A_base.h
 * @author  Joergen Bo Madsen
 * @version V1.1.5
 * @date    30. maj 2025
 * @brief   Abstract base Class for MTD2A (Model Train Detection And Action)
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

  namespace MTD2A_const {
  // Easy understanding global definitions
  constexpr bool ENABLE        = true, DISABLE      = false;
  constexpr bool ACTIVE        = true, COMPLETE     = false;
  constexpr bool FIRST_TRIGGER = true, LAST_TRIGGER = false;
  constexpr bool TIME_DELAY    = true, MONO_STABLE  = false;
  constexpr bool NORMAL        = true, INVERTED     = false;  
  constexpr bool PULSE         = true, FIXED        = false;
  constexpr bool BINARY        = true, PWM          = false;

  constexpr uint8_t RESET_PHASE      = 0; 
  constexpr uint8_t BEGIN_PHASE      = 1, OUTPUT_PHASE    = 2, END_PHASE      = 3; // binary_input 
  constexpr uint8_t FIRST_TIME_PHASE = 1, LAST_TIME_PHASE = 2, BLOCKING_PHASE = 3; // binary_output
  constexpr uint8_t COMPLETE_PHASE   = 4;
} // namespace _MTD2A_const 

#endif

using namespace MTD2A_const;

#ifndef _MTD2A_base_H_
#define _MTD2A_base_H_

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

    virtual ~MTD2A() = default;
    // Global functions
    static void  set_globalDebugPrint (const bool     &setEnableOrDisable = ENABLE);
    static void  set_delayTimeMS      (const uint32_t &setDelayTimeMS = DELAY_TIME_MS);

  private:
    static const uint8_t  PIN_ERR_NO    {255};
    static const uint32_t DELAY_TIME_MS {10};
    //
    static uint32_t delayTimeMS;
    static bool     globalDebugPrint;
    //
    static const uint8_t DIGITAL_FLAG_0 {1}, ANALOG_FLAG_1 {2}, INPUT_FLAG_2 {4}, PULLUP_FLAG_3 {8}, 
                         OUTPUT_FLAG_4 {16}, PWM_FLAG_5 {32},   TONE_FLAG_6 {64}, INTERRUPT_FLAG_7 {128};

    // Function pointer linked list -----------------------------------------------------------------------------------
    static void MTD2A_add_function_pointer_loop_fast (MTD2A* object);
    static MTD2A* begin;
    static MTD2A* end;
    MTD2A* next = nullptr;
    using function_type = void (*)(MTD2A*);
    function_type function_pointer = nullptr;
  public:
    MTD2A(const MTD2A&) = delete;
    MTD2A& operator=(const MTD2A&) = delete;
    MTD2A(function_type funcPtr) : function_pointer{funcPtr} {}
    static void loop_execute();
  // Function pointer linked list -----------------------------------------------------------------------------------

  private:
    // Internal functions
    static char   *MTD2A_set_object_name       (const char    *setObjectName);
    static uint8_t MTD2A_reserve_and_check_pin (const uint8_t &checkPinNumber,   const uint8_t &checkPinFlags);
    static void    MTD2A_print_phase_line      (const bool    &checkDebugPrint,  const char    *printObjectName,   const char *printPhaseText);
    // Error and debug print
    static void    MTD2A_print_error_text      (const bool    &checkDebugPrint,  const uint8_t &printErrorNumber,  const uint8_t &printPinNumber);
    static void    MTD2A_print_debug_error     (const bool    &printDebugPrint,  const uint8_t &printErrorNumber);
    static void    MTD2A_print_pin_number      (const uint8_t &printPinNumber);
    static void    MTD2A_print_generic_info    (const char    *printObjectName,  const bool    &printProcessState, const char *printPhaseText);
    static void    MTD2A_print_value_binary    (const bool    &binaryOrPWM,      const uint8_t &PrintValue);
    static void    MTD2A_print_enable_disable  (const bool    &enableOrDisable);
    static void    MTD2A_print_pulse_fixed     (const bool    &pulseOrFixed);

};  // MTD2A


#endif
