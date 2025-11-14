/**
 ******************************************************************************
 * @file    MTD2A_base.h
 * @author  Joergen Bo Madsen
 * @version V1.1.8
 * @date    16 october 2025
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

// Development an debug
#define PortPrint(x)   Serial.print(x)
#define PortPrintln(x) Serial.println(x)
// Optimized production
// #define PortPrint(x)
// #define PortPrintln(x)


#include "Arduino.h"
#include "MTD2A_const.h"


class MTD2A  // base class
{ 
  // Static class that makes the attributes or methods belong to the class itself instead of to instances of the class. 
  public:

    friend class MTD2A_timer;
    friend class MTD2A_binary_output;
    friend class MTD2A_binary_input;

  private:
    static constexpr uint8_t  NO_PRINT_PIN   {254};
    static constexpr uint8_t  WARNING_START  {128};
    static constexpr uint8_t  MAX_PWM_CURVES {16};
    static constexpr uint16_t MS_to_US       {1000};
    // Global constants from MTD2A_const.h
    static constexpr uint8_t  MAX_BYTE_VALUE {MTD2A_const::MAX_BYTE_VALUE};
    static constexpr uint8_t  PIN_ERROR_NO   {MTD2A_const::PIN_ERROR_NO};
    //
    static constexpr bool     ENABLE     {MTD2A_const::ENABLE};
    static constexpr bool     DISABLE    {MTD2A_const::DISABLE};
    static constexpr bool     ACTIVE     {MTD2A_const::ACTIVE};
    static constexpr bool     COMPLETE   {MTD2A_const::COMPLETE};
    static constexpr bool     NORMAL     {MTD2A_const::NORMAL};
    static constexpr bool     INVERTED   {MTD2A_const::INVERTED};
    static constexpr bool     PULSE      {MTD2A_const::PULSE};
    static constexpr bool     FIXED      {MTD2A_const::FIXED};
    static constexpr bool     BINARY     {MTD2A_const::BINARY};
    static constexpr bool     P_W_M      {MTD2A_const::P_W_M};
    static constexpr uint8_t  DELAY_10MS {MTD2A_const::DELAY_10MS};
    static constexpr uint8_t  DELAY_5MS  {MTD2A_const::DELAY_5MS};
    static constexpr uint8_t  DELAY_1MS  {MTD2A_const::DELAY_1MS};
    //
    static const uint8_t DIGITAL_FLAG_0 {1}, ANALOG_FLAG_1 {2}, INPUT_FLAG_2 {4}, PULLUP_FLAG_3    {8}, 
                         OUTPUT_FLAG_4 {16}, PWM_FLAG_5 {32},   TONE_FLAG_6 {64}, INTERRUPT_FLAG_7 {128};
    // 
    static bool     globalDebugPrint;
    static bool     globalErrorPrint;
    static uint32_t globalSyncTimeMS;
    static uint8_t  globalDelayTimeMS;
    static uint8_t  globalObjectCount;
    //
    static uint32_t delayTimeUS;
    static uint32_t elapsedTimeUS;
    static uint32_t maxElapsedTimeUS;
    static uint32_t timeOverrunCount;  
    static uint32_t endTimeUS;
    static uint32_t beginTimeUS;

  public:
    virtual ~MTD2A() = default;


    // Setters -------------------------------------------------------------


    /**
     * @brief Enable print phase state number and phase state text for all instantiated classes
     * @name set_globalDebugPrint
     * @param ( {ENABLE | DISABLE} );
     * @return none
     */
    static void set_globalDebugPrint (const bool &setEnableOrDisable = ENABLE);


    /**
     * @brief Enable print error and warning text for all instantiated classes
     * @name set_globalErrorPrint
     * @param ( {ENABLE | DISABLE} );
     * @return none
     */
    static void set_globalErrorPrint (const bool &setEnableOrDisable = ENABLE);


    /**
     * @brief Set main loop delay in milliseconds in function MTD2A_loop_execute();
     * @name set_globalDelayTimeMS
     * @param ( {1 - 10} or { DELAY_10MS | DELAY_5MS | DELAY_1MS } );
     * @return none
     */
    static void set_globalDelayTimeMS (const uint8_t &setGlobalDelayTimeMS = DELAY_10MS);


    // getters -------------------------------------------------------------


    /**
     * @brief Get main loop delay in milliseconds
     * @name get_globalDelayTimeMS();
     * @param none
     * @return uint8_t ( {1 - 10} or { DELAY_10MS | DELAY_5MS | DELAY_1MS } );
     */
    static uint8_t get_globalDelayTimeMS ();

 
    /**
     * @brief Get current common reference time for all instantiated objects
     * @name get_globalSyncTimeMS();
     * @param none
     * @return unit32_t milliseconds
     */ 
    static uint32_t get_globalSyncTimeMS ();


    /**
     * @brief Get max MTD2A code, user code and other library loop execution time and reset measurement
     * @name get_maxElapsedTimeMS ();
     * @param none
     * @return unit32_t milliseconds
     */
    static uint32_t get_maxElapsedTimeUS ();


    /**
     * @brief Get number times loop time exceeds globalDelayTimeMS 
     * @name get_timeOverrunCount ();
     * @param none
     * @return unit32_t milliseconds
     */
    static uint32_t get_timeOverrunCount ();

    
    /**
     * @brief Get number of instatiated MTD2A objects 
     * @name get_globalObjectCount();
     * @param none
     * @return unit8_t count
     */
    static uint8_t get_globalObjectCount ();


    /**
     * @brief print configuration parameters, timers and state logic.
     * @name print_conf ();
     * @param none
     * @return none
     */
    static void print_conf ();


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
    static char    *MTD2A_set_object_name       (const char     *setObjectName);
    static void     MTD2A_print_object_name     (const char     *printObjectName);
    static uint8_t  MTD2A_reserve_and_check_pin (const uint8_t  &checkPinNumber,    const uint8_t &checkPinFlags);

    // Error and debug print
    static void     MTD2A_print_error_text      (const bool     &DebugOrErrorPrint, const uint8_t &printErrorNumber, const uint8_t &printPinNumber);
    static void     MTD2A_print_debug_error     (const bool     &printDebugPrint,   const bool    &printErrorPrint,  const uint8_t &printErrorNumber);
    static void     MTD2A_print_pin_number      (const uint8_t  &printPinNumber);
    static void     MTD2A_print_name_state      (const char     *printObjectName,   const bool    &printProcessState);
    static void     MTD2A_print_value_binary    (const bool     &binaryOrP_W_M,     const uint8_t &PrintValue);
    static void     MTD2A_print_enable_disable  (const bool     &enableOrDisable);
    static void     MTD2A_print_normal_inverted (const bool     &normalOrInverted);
    static void     MTD2A_print_pulse_fixed     (const bool     &pulseOrFixed);
    
};  // MTD2A


// User friendly alias and consistent naming ------------------------------------------------------------------------------------


/**
 * @brief Update state mashine for all instantiated classes
 * @name MTD2A_loop_execute();
 * @param none
 * @return none
 */
static constexpr auto MTD2A_loop_execute = []() {
    MTD2A::loop_execute();
};

/**
 * @brief Enable print phase state number and phase state text for all instantiated classes
 * @name MTD2A_globalDebugPrint
 * @param ( {ENABLE | DISABLE} );
 * @return none
 */
auto MTD2A_globalDebugPrint = [](const bool &setEnableOrDisable = MTD2A_const::ENABLE) {
    MTD2A::set_globalDebugPrint(setEnableOrDisable);
};


/**
 * @brief Enable print error and warning text for all instantiated classes
 * @name MTD2A_globalErrorPrint
 * @param ( {ENABLE | DISABLE} );
 * @return none
 */
auto MTD2A_globalErrorPrint = [](const bool &setEnableOrDisable  = MTD2A_const::DISABLE) {
  MTD2A::set_globalErrorPrint (setEnableOrDisable);
};


/**
 * @brief print configuration parameters, timers and state logic.
 * @name MTD2A_print_conf ();
 * @param none
 * @return none
 */
auto MTD2A_print_conf = []() {
  MTD2A::print_conf ();
};


#endif
