/*
 ******************************************************************************
 * @file    MTD2A_timers.h
 * @author  Joergen Bo Madsen
 * @version V1.1.1
 * @date    7. September 2025
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


#ifndef _MTD2A_timers_H_
#define _MTD2A_timers_H_


#include "Arduino.h"
#include "MTD2A_base.h"


class MTD2A_timer: public MTD2A
{ 
  private:
    // Specific global constants from MTD2A_binary_output.h (MTD2A_const.h)
    static constexpr uint8_t  RESET_TIMER  {MTD2A_const::RESET_TIMER};
    static constexpr uint8_t  START_TIMER  {MTD2A_const::START_TIMER};
    static constexpr uint8_t  PAUSE_TIMER  {MTD2A_const::PAUSE_TIMER};
    static constexpr uint8_t  STOP_TIMER   {MTD2A_const::STOP_TIMER};
    // MTD2A_base
    static constexpr uint8_t  NO_PRINT_PIN {MTD2A::NO_PRINT_PIN};

    // Arguments
    char    *objectName    {nullptr};     // Constructor initialized (User defined name to display identification)
    uint32_t countDownMS   {0};           // Constructor default argument. Milliseconds
    // Timers
    uint32_t startTimeMS   {0};           // get_startTimeMS   () Milliseconds
    uint32_t stopTimeMS    {0};           // get_stopTimeMS    () Milliseconds
    uint32_t remainTimeMS  {0};           // get_remainTimeMS  () Milliseconds
    uint32_t elapsedTimeMS {0};           // get_elapsedTimeMS () Milliseconds
    uint32_t pauseTimeMS   {0};           // get_pauseTimeMS   () Milliseconds
    // Debug and error
    bool     debugPrint    {DISABLE};     // set_debugPrint  () default argument / ENABLE
    bool     errorPrint    {DISABLE};     // set_errorPrint  () default argument / ENABLE
    uint8_t  errorNumber   {0};           // get_reset_error () Error {1-127} and Warning {128-255}    
    // State control
    bool     processState  {COMPLETE};    // get_processState () / ACTIVE 
    bool     phaseChange   {false};       // true = change in timing state (one loop)
    uint8_t  phaseNumber   {STOP_TIMER};  // RESET_TIMER = 0, START_TIMER = 1, PAUSE_TIMER = 2, STOP_TIMER = 3
    uint8_t  setPhaseNumber{STOP_TIMER};  // Set phase number to execute (loop_fast ();)
    // Pause timers
    uint32_t pauseBeginMS  {0};           // Timer begin pause milliseconds
    uint32_t pauseEndMS    {0};           // Timer end pause milliseconds
    // Time control
    bool     startProcess  {false};       // Control executing flags
    bool     stopProcess   {false};       // Control executing flags
    bool     beginPause    {false};       // Control executing flags
    bool     endPause      {false};       // Control executing flags

 public:
    // Constructor inittializers
    /**
     * @class MTD2A_timer
     * @brief Create object and set configuration parameter or use defaults
     * @param ( "Object Name", delayTimeMS {0 - 4294967295} );
     * @return none
     */
    MTD2A_timer (
      const char    *setObjectName  = "Object name", 
      const uint32_t setCountDownMS = 0
    );

    // Destructor
    ~MTD2A_timer () { 
      if (objectName != nullptr) {
        delete [] objectName; 
        objectName = nullptr;
      }
      if (globalObjectCount > 0 )
        globalObjectCount--;
    };    
  
    // Operator oveloading
    bool operator==(const MTD2A_timer &obj) const {
      return (processState == obj.processState);
    }
    bool operator!=(const MTD2A_timer &obj) const {
      return (processState != obj.processState);
    }
    bool operator>(const MTD2A_timer &obj) const {
      return (processState == ACTIVE && obj.processState == COMPLETE);
    }
    bool operator<(const MTD2A_timer &obj) const {
      return (processState == COMPLETE && obj.processState == ACTIVE);
    }
    bool operator>>(const MTD2A_timer &obj) const {
      return (stopTimeMS > obj.stopTimeMS);
    }
    bool operator<<(const MTD2A_timer &obj) const {
      return (stopTimeMS < obj.stopTimeMS);
    }

    public: // Functions

    // Function overloading
    
    /**
     * @brief countdown to zero and change process state to COMPLETE
     * @name timer
     * @param ( {START_TIMER | PAUSE_TIMER | STOP_TIMER | RESET_TIMER} );
     * @param ( {START_TIMER | PAUSE_TIMER | STOP_TIMER | RESET_TIMER}, setCountDownMS );
     * @return none
     */
    void timer (const uint8_t &setStartStopPauseReset = START_TIMER);
    void timer (const uint8_t &setStartStopPauseReset, const uint32_t &SetCountDownMS);


    /**
     * @brief print configuration parameters, timers and state logic.
     * @name object_name.print_conf ();
     * @param none
     * @return none
     */  
    void print_conf ();

    // setters -------------------------------------------------------------


    /**
     * @brief set countdown time i milliseconds
     * @name set_countDownMS
     * @param ( {0 - 4294967295} );
     * @return none
     */    
    void set_countDownMS (const uint32_t &setCountDownMS);


    /**
     * @brief Enable print phase state number, phase state text and error text.
     * @name object_name.set_debugPrint
     * @param ( {ENABLE | DISABLE} );
     * @return none
     */  
    void set_debugPrint (const bool &setEnableOrDisable = ENABLE);
    
    
    /**
     * @brief Enable error print text.
     * @name object_name.set_errorPrint
     * @param ( {ENABLE | DISABLE} );
     * @return none
     */  
    void set_errorPrint (const bool &setEnableOrDisable = ENABLE);


    // getters -------------------------------------------------------------


    /**
     * @brief get start time in milliseconds (first if no pause was initated)
     * @name object_name.startTimeMS ();
     * @param none
     * @return unit32_t milliseconds
     */    
    uint32_t const &get_startTimeMS () const;


    /**
     * @brief get stop time in milliseconds
     * @name object_name.get_stopTimeMS ();
     * @param none
     * @return unit32_t milliseconds
     */    
    uint32_t const &get_stopTimeMS () const;


    /**
     * @brief get acuumulated pause time in milliseconds (sum of all pause periods)
     * @name object_name.get_pauseTimeMS ();
     * @param none
     * @return unit32_t milliseconds
     */    
    uint32_t const &get_pauseTimeMS () const;

    /**
     * @brief get remaning time in milliseconds
     * @name object_name.get_remainTimeMS ();
     * @param none
     * @return unit32_t milliseconds
     */
    uint32_t const &get_remainTimeMS () const;


    /**
     * @brief get elapsed time in milliseconds
     * @name object_name.get_elapsedTimeMS ();
     * @param none
     * @return unit32_t milliseconds
     */
    uint32_t const &get_elapsedTimeMS () const;


    /**
     * @brief Get processState  
     * @name object_name.get_processState ();
     * @param none
     * @return bool {ACTIVE | COMPLETE}
     */
    bool const &get_processState () const;


    /**
     * @brief get phaseChange 
     * @name object_name.get_phaseChange ();
     * @param none
     * @return bool {true | false}
     */
    bool const &get_phaseChange () const;
  

    /**
     * @brief get phase number. 
     * @name object_name.get_phaseNumber ();
     * @param none
     * @return uint8_t RESET_TIMER = 0, START_TIMER = 1, PAUSE_TIMER = 2, STOP_TIMER = 3
     */  
    uint8_t const &get_phaseNumber () const;


    /**
     * @brief Get and reset error number to zero 
     * @name object_name.get_reset_error ();
     * @param none
     * @return uint8_t Error number. Error {1-127} and Warning {128-255}
     */
    uint8_t const &get_reset_error ();

    
  private: // Internal functions
  
    void     start_timer       ();
    void     reset_timer       ();
    void     pause_timer       ();    
    void     stop_timer        ();
    bool     check_timer_arg   (const uint8_t  &argStartStopPauseReset);
    void     set_timer_state   (const uint8_t  &setStartStopPauseReset);
    uint32_t check_set_time    (const uint32_t &setCheckTimeMS);
    // Execute
    void     loop_fast             ();
    void     loop_fast_start_reset ();
    void     loop_fast_pause_begin ();
    void     loop_fast_pause_end   ();
    void     loop_fast_calc_time   ();
    void     loop_fast_timer_stop  ();
    // print
    void     print_phase_line ();
    void     print_phase_text ();
    void     print_error_text (const uint8_t setErrorNumber);

};  // MTD2A_timer


#endif
