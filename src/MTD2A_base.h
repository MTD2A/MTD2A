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


#ifndef _MTD2A_base_H_
#define _MTD2A_base_H_

#include "Arduino.h"
#include "MTD2A_const.h"


class MTD2A  // base class
{ 
  // Static class that makes the attributes or method belong to the class itself instead of to instances of the class. 
  public:

    friend class MTD2A_binary_output;
    friend class MTD2A_binary_input;
    friend class MTD2A_timer; 
    friend class MTD2A_astable;
    friend class MTD2A_flip_flop;
    friend class MTD2A_tone;
    friend class MTD2A_sound;
    friend class MTD2A_servo;
    friend class MTD2A_stepper;
    friend class MTD2A_display;
    friend class MTD2A_ultrasonic;
    friend class MTD2A_laser;
    friend class MTD2A_IR_ranging;
    friend class MTD2A_DCC_input;

  private:
    static constexpr uint8_t  MAX_BYTE_VALUE {255};
    static constexpr uint8_t  PIN_ERROR_NO   {255};
    static constexpr uint8_t  WARNING_START  {128};
    // Global constants
    static constexpr bool ENABLE         {MTD2A_const::ENABLE};
    static constexpr bool DISABLE        {MTD2A_const::DISABLE};
    static constexpr bool ACTIVE         {MTD2A_const::ACTIVE};
    static constexpr bool COMPLETE       {MTD2A_const::COMPLETE};
    static constexpr bool BINARY         {MTD2A_const::BINARY};
    static constexpr bool NORMAL         {MTD2A_const::NORMAL};
    static constexpr bool INVERTED       {MTD2A_const::INVERTED};
    static constexpr bool PULSE          {MTD2A_const::PULSE};
    static constexpr bool MONO_STABLE    {MTD2A_const::MONO_STABLE};
    static constexpr bool LAST_TRIGGER   {MTD2A_const::LAST_TRIGGER};
    static constexpr bool TIME_DELAY     {MTD2A_const::TIME_DELAY};
    static constexpr bool PWM            {MTD2A_const::PWM};
    static constexpr bool STOP_TIMER     {MTD2A_const::STOP_TIMER};
    static constexpr bool RESTART_TIMER  {MTD2A_const::RESTART_TIMER};
    static constexpr uint32_t DELAY_10MS {MTD2A_const::DELAY_10MS};
    static constexpr uint32_t DELAY_1MS  {MTD2A_const::DELAY_1MS};
    //
    static const uint8_t DIGITAL_FLAG_0 {1}, ANALOG_FLAG_1 {2}, INPUT_FLAG_2 {4}, PULLUP_FLAG_3 {8}, 
                         OUTPUT_FLAG_4 {16}, PWM_FLAG_5 {32},   TONE_FLAG_6 {64}, INTERRUPT_FLAG_7 {128};
    // 
    static uint32_t delayTimeMS;
    static bool     globalDebugPrint;
    static uint8_t  objectCount;
    static uint32_t currentTimeMS;
    static uint32_t lastTimeMS;
    static uint32_t loopTimeMS;
    static uint32_t maxLoopTimeMS;


  public:
    virtual ~MTD2A() = default;
    // Global functions
    static void     set_globalDebugPrint (const bool &setEnableOrDisable = ENABLE);
    static void     set_delayTimeMS      (const bool &setDelayTimeMS = DELAY_10MS);
    static uint8_t  get_objectCount      ();
    static uint32_t get_reset_maxLoopMS  ();

  private:
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
    static void    MTD2A_print_phase_line      (const bool    &checkDebugPrint,  const char    *printObjectName,   
                                                const char    *printPhaseText,   const bool    &printRestartTimer = false);
    // Error and debug print
    static void    MTD2A_print_error_text      (const bool    &checkDebugPrint,  const uint8_t &printErrorNumber,  const uint8_t &printPinNumber);
    static void    MTD2A_print_debug_error     (const bool    &printDebugPrint,  const uint8_t &printErrorNumber);
    static void    MTD2A_print_pin_number      (const uint8_t &printPinNumber);
    static void    MTD2A_print_generic_info    (const char    *printObjectName,  const bool    &printProcessState, const char *printPhaseText);
    static void    MTD2A_print_value_binary    (const bool    &binaryOrPWM,      const uint8_t &PrintValue);
    static void    MTD2A_print_enable_disable  (const bool    &enableOrDisable);
    static void    MTD2A_print_normal_inverted (const bool    &normalOrInverted);
    static void    MTD2A_print_pulse_fixed     (const bool    &pulseOrFixed);
    
};  // MTD2A


// User friendly alias and consistent naming ------------------------------------------------------------------------------------

/*
 * @brief Update state mashine for all instantiated classes
 * @name MTD2A_loop_execute();
 * @param none
 * @return none
 */
//inline void MTD2A_loop_execute() {
//  MTD2A::loop_execute();
// }
auto MTD2A_loop_execute = []() {
  MTD2A::loop_execute();
};


/*
 * @brief Enable print phase state number and phase state text for all instantiated classes
 * @name MTD2A_globalDebugPrint
 * @param ( {ENABLE | DISABLE} );
 * @return none
 */
//inline void MTD2A_globalDebugPrint (const bool &setEnableOrDisable = MTD2A_const::ENABLE) {
//  MTD2A::set_globalDebugPrint (setEnableOrDisable);
//}
auto MTD2A_globalDebugPrint = [](const bool &setEnableOrDisable = MTD2A_const::ENABLE) {
    MTD2A::set_globalDebugPrint(setEnableOrDisable);
};


/*
 * @brief Set main loop delay in milliseconds in function MTD2A_loop_execute();
 * @name MTD2A_delayTimeMS
 * @param ( {DELAY_10MS | DELAY_1MS} );
 * @return none
 */
//inline void MTD2A_delayTimeMS (const bool &setDelayTimeMS = MTD2A_const::DELAY_10MS) {
//  MTD2A::set_delayTimeMS(setDelayTimeMS); 
//}

auto MTD2A_delayTimeMS = [](const bool &setDelayTimeMS = MTD2A_const::DELAY_10MS) {
  MTD2A::set_delayTimeMS(setDelayTimeMS); 
};

/*
 * @brief Max MTD2A code, user code and other library loop execution time 
 * @name MTD2A_maxLoopMS();
 * @param none
 * @return unit32_t milliseconds
 */
//inline uint32_t MTD2A_maxLoopMS () {
//  return MTD2A::get_reset_maxLoopMS ();
//}
auto MTD2A_maxLoopMS = []() {
  return MTD2A::get_reset_maxLoopMS ();
};



/*
 * @brief number of instatiated MTD2A objects 
 * @name MTD2A_objectCount();
 * @param none
 * @return unit8_t count
 */
//inline uint8_t MTD2A_objectCount () {
//  return MTD2A::get_objectCount ();
//}
auto MTD2A_objectCount = []() {
  return MTD2A::get_objectCount ();
};

#endif
