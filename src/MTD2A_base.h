/**
 ******************************************************************************
 * @file    MTD2A_base.h
 * @author  Joergen Bo Madsen
 * @version 1.3.1
 * @date    15. july 2026
 * @brief   Base class for common functions (Model Train Detection And Action)
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


#ifndef MTD2A_BASE_H_
#define MTD2A_BASE_H_


#include "Arduino.h"
#include "MTD2A_print.h"
#include "MTD2A_const.h"


class MTD2A  // base class
{ 
  // Static class that makes the attributes or methods belong to the class itself instead of to instances of the class. 
  public:

    friend class MTD2A_timer;
    friend class MTD2A_binary_output;
    friend class MTD2A_binary_input;

  private:

    static constexpr const char *baseName {"BASE"}; 
    static constexpr uint8_t  WARNING_START  {128};
    static constexpr uint8_t  MAX_PWM_CURVES {24};

    // Global constants from MTD2A_const.h
    static constexpr uint32_t MAX_TIME_MS    {MTD2A_const::MAX_TIME_MS};
    static constexpr uint8_t  MAX_BYTE_VALUE {MTD2A_const::MAX_BYTE_VALUE};
    static constexpr uint8_t  NO_PIN         {MTD2A_const::NO_PIN};
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
    static constexpr uint8_t  DELAY_2MS  {MTD2A_const::DELAY_2MS};
    static constexpr uint8_t  DELAY_1MS  {MTD2A_const::DELAY_1MS};
    //
    static constexpr uint8_t  DIGITAL_FLAG_0 {1}, ANALOG_FLAG_1 {2}, INPUT_FLAG_2 {4}, PULLUP_FLAG_3    {8}, 
                              OUTPUT_FLAG_4 {16}, PWM_FLAG_5 {32},   TONE_FLAG_6 {64}, INTERRUPT_FLAG_7 {128}; 
    // Timing
    static constexpr uint16_t MS_to_US       {1000U}; // Multiplication factor
    static constexpr uint32_t MARGIN_TIME_US {500UL}; // Add measurement margin to time control
    static constexpr uint32_t MAX_TIME_US    {MAX_TIME_MS * MS_to_US};
    // Globals - all MTD2A instantiations
    static bool     globalDebugPrint;   // User friendly status information
    static bool     globalErrorPrint;   // Error messaging
    static uint32_t globalSyncTimeUS;   // Synchronized reference time for all child objects 
    static uint8_t  globalDelayTimeMS;  // Current loop delay time (DELAY_10MS, DELAY_5MS & DELAY_1MS)
    static uint8_t  globalObjectCount;  // Number or instantiated objects (classes)
    // statistics
    static uint32_t elapsedLoopTimeUS;  // Last measured elapsed time
    static uint32_t maxElapsedTimeUS;   // Total loop maximumelapsed time
    static uint32_t timeOverrunCount;   // Number of times delayLoopTimeUS was exceeded
    // control logic
    static bool     epochInitialized;   // First loop detection or reset to avoid overflow
    // loop timing (loop_execute)
    static  int64_t epochTimeUS;        // Absolute time anchor (set on first call and on overflow)
    static uint32_t delayLoopTimeUS;    // Global delay timer
    static uint32_t currentTimeUS;      // Current micros() time.
    static uint32_t beginLoopTimeUS;    // Loop begin micros() time (next loop)
    static uint32_t endLoopTimeUS;      // Loop end micros() time (current loop)
    static uint32_t lastEndLoopTimeUS;  // Last loop end micros() time (current loop)
    // Safe overflow timing
    static uint64_t targetLoopTimeUS;   // When the current loop should end
    static uint64_t totalLoopCount;     // Number of completed loops since reset
    // Loop correction
    static  int32_t remainingTimeUS;    // Remaining loop time
    static uint16_t mainDelayTimeUS;    // Main loop delay time
    static  int32_t correctionTimeUS;   // Correction loop time



  public:

    virtual ~MTD2A();  // Unlinks object from the loop_fast linked list


    // Setters -------------------------------------------------------------


    /**
     * @brief Enable print phase state number and phase state text for all instantiated classes
     * @name set_globalDebugPrint
     * @param ( {ENABLE | DISABLE} );
     * @return none
     */
    static void set_globalDebugPrint (bool setEnableOrDisable = ENABLE);


    /**
     * @brief Enable print error and warning text for all instantiated classes
     * @name set_globalErrorPrint
     * @param ( {ENABLE | DISABLE} );
     * @return none
     */
    static void set_globalErrorPrint (bool setEnableOrDisable = ENABLE);


    /**
     * @brief Set main loop delay in milliseconds in function MTD2A_loop_execute(); 
     * @brief resets the time measurement and calls MTD2A_reset_stats ();
     * @name set_globalDelayTimeMS
     * @param ( {1 - 10} or { DELAY_10MS | DELAY_5MS | DELAY_2MS | DELAY_1MS } );
     * @return none
     */
    static void set_globalDelayTimeMS (uint8_t setGlobalDelayTimeMS = DELAY_10MS);


    // getters -------------------------------------------------------------


    /**
     * @brief Get main loop delay in milliseconds
     * @name get_globalDelayTimeMS();
     * @param none
     * @return uint8_t ( {1 - 10} or { DELAY_10MS | DELAY_5MS | DELAY_2MS | DELAY_1MS } );
     */
    static uint8_t get_globalDelayTimeMS ();

 
    /**
     * @brief Get current common reference time for all instantiated objects
     * @name get_globalSyncTimeMS();
     * @param none
     * @return uint32_t milliseconds
     */ 
    static uint32_t get_globalSyncTimeMS ();

    
    /**
     * @brief Get number of instantiated MTD2A objects 
     * @name get_globalObjectCount();
     * @param none
     * @return uint8_t count
     */
    static uint8_t get_globalObjectCount ();


    /**
     * @brief Get max MTD2A code, user code and other library loop execution time. Use reset_stats () to reset
     * @name get_maxElapsedTimeMS ();
     * @param none
     * @return uint32_t milliseconds
     */
    static uint32_t get_maxElapsedTimeMS ();


    /**
     * @brief Get number of times loop time exceeds globalDelayTimeMS. Use reset_stats () to reset
     * @name get_timeOverrunCount ();
     * @param none
     * @return uint32_t number
     */
    static uint32_t get_timeOverrunCount ();


    /**
     * @brief Get number of loop count since last reset (approx every 72 minutes) 
     * @name get_totalLoopCount ();
     * @param none
     * @return uint64_t number
     */
    static uint64_t get_totalLoopCount ();

  
    /**
     * @brief print configuration parameters, timers and state logic.
     * @name print_conf ();
     * @param none
     * @return none
     */
    static void print_conf ();


    /**
     * @brief clear statistical parameters: elapsedTimeMS, maxElapsedTimeMS, timeOverrunCount
     * @name reset_stats ();
     * @param none
     * @return none
     */
    static void reset_stats ();


  private:
    // Function pointer linked list -----------------------------------------------------------------------------------
    static void MTD2A_add_function_pointer_loop_fast (MTD2A* object);
    static void MTD2A_remove_function_pointer_loop_fast (MTD2A* object);
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
    // loop_execute
    // static void     print_debug_info            (uint8_t id); // Testing
    static void     loop_init_epoch_once         ();
    static void     loop_execute_MTD2A_objects   ();
    static void     loop_measure_execution_time  ();
    static void     loop_time_overrun_correction ();
    static void     loop_handle_micros_overflow  ();
    static void     loop_target_timed_delay      ();
    // Internal functions
    static uint32_t MTD2A_round_US_to_MS         (uint32_t    microSeconds);
    static void     MTD2A_print_uint64           (uint64_t    uint64Value);
    static char    *MTD2A_set_object_name        (const char *setObjectName);
    static void     MTD2A_print_object_name      (const char *printObjectName);
    static uint8_t  MTD2A_reserve_and_check_pin  (uint8_t     checkPinNumber,    uint8_t checkPinFlags);
    // Error and debug print
    static void     MTD2A_print_US_to_MS         (uint32_t    microSeconds);
    static void     MTD2A_print_error_text       (const char *printObjectName,   bool    debugOrErrorPrint, uint8_t printErrorNumber, uint8_t printPinNumber);
    static void     MTD2A_print_debug_error      (bool        printDebugPrint,   bool    printErrorPrint,   uint8_t printErrorNumber);
    static void     MTD2A_print_pin_number       (uint8_t     printPinNumber);
    static void     MTD2A_print_name_state       (const char *printObjectName,   bool    printProcessState);
    static void     MTD2A_print_value_binary     (bool        binaryOrP_W_M,     uint8_t PrintValue);
    static void     MTD2A_print_enable_disable   (bool        enableOrDisable);
    static void     MTD2A_print_normal_inverted  (bool        normalOrInverted);
    static void     MTD2A_print_pulse_fixed      (bool        pulseOrFixed);
    
};  // MTD2A


// User friendly alias and consistent naming ------------------------------------------------------------------------------------


/**
 * @brief Update state mashine for all instantiated classes
 * @name MTD2A_loop_execute();
 * @param none
 * @return none
 */
inline void MTD2A_loop_execute () {  MTD2A::loop_execute();  }

/**
 * @brief Enable print phase state number and phase state text for all instantiated classes
 * @name MTD2A_globalDebugPrint
 * @param ( {ENABLE | DISABLE} );
 * @return none
 */
inline void MTD2A_globalDebugPrint (bool setEnableOrDisable = MTD2A_const::ENABLE) {
    MTD2A::set_globalDebugPrint(setEnableOrDisable);
}


/**
 * @brief Enable print error and warning text for all instantiated classes
 * @name MTD2A_globalErrorPrint
 * @param ( {ENABLE | DISABLE} );
 * @return none
 */
inline void MTD2A_globalErrorPrint (bool setEnableOrDisable = MTD2A_const::ENABLE) {
  MTD2A::set_globalErrorPrint (setEnableOrDisable);
}


/**
 * @brief print configuration parameters, timers and state logic.
 * @name MTD2A_print_conf ();
 * @param none
 * @return none
 */
inline void MTD2A_print_conf () {  MTD2A::print_conf ();  }


/**
 * @brief clear statistical parameters: elapsedTimeMS, maxElapsedTimeMS, timeOverrunCount
 * @name reset_stats ();
 * @param none
 * @return none
 */
inline void MTD2A_reset_stats () {  MTD2A::reset_stats ();  }


#endif
