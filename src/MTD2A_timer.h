/*
 ******************************************************************************
 * @file    MTD2A_timer.h
 * @author  Joergen Bo Madsen
 * @version 1.3.1
 * @date    15. july 2026
 * @brief   Abstract Class for MTD2A (Model Train Detection And Action)
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
 * Do not call MTD2A_timer from an interrupt handler (ISR)
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


#ifndef MTD2A_TIMER_H_
#define MTD2A_TIMER_H_

#include "MTD2A_base.h"

class MTD2A_timer: public MTD2A
{ 
  private:
  
    // Specific global constants from MTD2A_const.h
    static constexpr uint32_t MAX_TIME_MS  {MTD2A_const::MAX_TIME_MS};
    // Timer phases
    static constexpr uint8_t  RESET_TIMER  {MTD2A_const::RESET_TIMER};
    static constexpr uint8_t  START_TIMER  {MTD2A_const::START_TIMER};
    static constexpr uint8_t  PAUSE_TIMER  {MTD2A_const::PAUSE_TIMER};
    static constexpr uint8_t  STOP_TIMER   {MTD2A_const::STOP_TIMER};
    // Error and warning handling
    static constexpr uint8_t  ERR_OBJECT_INSTANT     {MTD2A_const::ERR_OBJECT_INSTANT};
    static constexpr uint8_t  ERR_TIME_BELOW_DELAY   {MTD2A_const::ERR_TIME_BELOW_DELAY};
    static constexpr uint8_t  ERR_TIME_ABOVE_MAX     {MTD2A_const::ERR_TIME_ABOVE_MAX};
    static constexpr uint8_t  ERR_NOT_COMPLETE       {MTD2A_const::ERR_NOT_COMPLETE};
    static constexpr uint8_t  ERR_TIMER_ARGUMENT     {MTD2A_const::ERR_TIMER_ARGUMENT};
    static constexpr uint8_t  ERR_NOT_ACTIVE         {MTD2A_const::ERR_NOT_ACTIVE};
    static constexpr uint8_t  WARN_TIMER_ZERO        {MTD2A_const::WARN_TIMER_ZERO};
    static constexpr uint8_t  WARN_TIME_PAUSE_MAX    {MTD2A_const::WARN_TIME_PAUSE_MAX};
    static constexpr uint8_t  WARN_COUNTDOWN_IGNORED {MTD2A_const::WARN_COUNTDOWN_IGNORED};
    static constexpr uint8_t  WARN_PAUSE_ACTIVE      {MTD2A_const::WARN_PAUSE_ACTIVE};

    // Arguments
    char    *objectName      {nullptr};     // Constructor initialized (User defined name to display identification)
    uint32_t countDownUS     {0UL};         // Constructor default argument. setCountDownMS [Milliseconds] * MS_to_US
      // Timers
    uint32_t startTimeUS     {0UL};         // get_startTimeUS   () Microseconds
    uint32_t stopTimeUS      {0UL};         // get_stopTimeUS    () Microseconds
    uint32_t remaingTimeUS   {0UL};         // get_remaingTimeUS () Microseconds
    uint32_t elapsedTimeUS   {0UL};         // get_elapsedTimeUS () Microseconds
    uint32_t pauseTimeUS     {0UL};         // get_pauseTimeUS   () Microseconds
    // Debug and error
    bool     debugPrint      {DISABLE};     // set_debugPrint  () default argument / ENABLE
    bool     errorPrint      {DISABLE};     // set_errorPrint  () default argument / ENABLE
    uint8_t  errorNumber     {0};           // get_reset_error () Error {1-127} and Warning {128-255}
    bool     objInstantiated {false};       // Object instantiated
    uint8_t  objectError     {0};           // Object instantiation error / warning       
    // State control
    bool     processState    {COMPLETE};    // get_processState () / ACTIVE 
    bool     phaseChange     {false};       // true = change in timing state (one loop)
    uint8_t  phaseNumber     {STOP_TIMER}; // RESET_TIMER = 0, START_TIMER = 1, PAUSE_TIMER = 2, STOP_TIMER = 3
    uint8_t  setPhaseNumber  {STOP_TIMER}; // Set phase number to execute (loop_fast ())
    // Pause timers
    uint32_t pauseBeginUS    {0UL};         // Timer begin pause microseconds
    uint32_t pauseEndUS      {0UL};         // Timer end pause microseconds
    // Time control
    bool     startProcess    {false};       // Control executing flags
    bool     calcProcess     {false};       // Control executing flags
    bool     stopProcess     {false};       // Control executing flags
    bool     beginPause      {false};       // Control executing flags
    bool     endPause        {false};       // Control executing flags

 public:
    // Constructor initializers

    // The default / no-arags constructor is implicit in the constructor below. Do not use MTD2A_timer ();
    
    /**
     * @class MTD2A_timer
     * @brief Create object and set configuration parameter or use defaults
     * @param ( "Object Name", delayTimeMS {0 - MAX_TIME_MS} ); // approx 72 minutes
     * NOTE: countdown time + accumulated pause time must not exceed MAX_TIME_MS
     * @return none
     */
    MTD2A_timer (
      const char *setObjectName  = "Object name", 
      uint32_t    setCountDownMS = 0
    );

    // Destructor
    ~MTD2A_timer () { 
      if (objectName != nullptr) {
        delete [] objectName; 
        objectName = nullptr;
      }
    }  
  
    // Operator overloading
    explicit operator bool() const {
      return (phaseChange == true  &&  phaseNumber == STOP_TIMER); 
    }
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


    public: // Functions

    // Function overloading
    
    /**
     * @brief Count down to zero and change process state to COMPLETE
     * @name timer
     * @param ( {START_TIMER | PAUSE_TIMER | STOP_TIMER | RESET_TIMER} );
     * @param ( {START_TIMER | PAUSE_TIMER | STOP_TIMER | RESET_TIMER}, setCountDownMS );
     * @return none
     */
    void timer (uint8_t setStartStopPauseReset = START_TIMER);
    void timer (uint8_t setStartStopPauseReset, uint32_t setCountDownMS);


    /**
     * @brief print configuration parameters, timers and state logic.
     * @name object_name.print_conf ();
     * @param none
     * @return none
     */  
    void print_conf ();

    // setters -------------------------------------------------------------


    /**
     * @brief set countdown time in milliseconds
     * @name set_countDownMS
     * @param ( {0 - MAX_TIME_MS } ); // 72 minutes
     * NOTE: countdown time + accumulated pause time must not exceed MAX_TIME_MS 
     * @return none
     */    
    void set_countDownMS (uint32_t setCountDownMS);


    /**
     * @brief Enable print phase state number, phase state text and error text.
     * @name object_name.set_debugPrint
     * @param ( {ENABLE | DISABLE} );
     * @return none
     */  
    void set_debugPrint (bool setEnableOrDisable = ENABLE);
    
    
    /**
     * @brief Enable error print text.
     * @name object_name.set_errorPrint
     * @param ( {ENABLE | DISABLE} );
     * @return none
     */  
    void set_errorPrint (bool setEnableOrDisable = ENABLE);


    // getters -------------------------------------------------------------


    /**
     * @brief get start time in milliseconds (first if no pause was initiated)
     * @name object_name.get_startTimeMS ();
     * @param none
     * @return uint32_t milliseconds
     */    
    uint32_t get_startTimeMS () const;


    /**
     * @brief get stop time in milliseconds
     * @name object_name.get_stopTimeMS ();
     * @param none
     * @return uint32_t milliseconds
     */    
    uint32_t get_stopTimeMS () const;


    /**
     * @brief get accumulated pause time in milliseconds (sum of all pause periods)
     * @name object_name.get_pauseTimeMS ();
     * @param none
     * @return uint32_t milliseconds
     */    
    uint32_t get_pauseTimeMS () const;

    /**
     * @brief get remaining time in milliseconds
     * @name object_name.get_remaingTimeMS ();
     * @param none
     * @return uint32_t milliseconds
     */
    uint32_t get_remaingTimeMS () const;


    /**
     * @brief get elapsed time in milliseconds
     * @name object_name.get_elapsedTimeMS ();
     * @param none
     * @return uint32_t milliseconds
     */
    uint32_t get_elapsedTimeMS () const;


    /**
     * @brief Get processState  
     * @name object_name.get_processState ();
     * @param none
     * @return bool {ACTIVE | COMPLETE}
     */
    bool get_processState () const;


    /**
     * @brief get phaseChange 
     * @name object_name.get_phaseChange ();
     * @param none
     * @return bool {true | false}
     */
    bool get_phaseChange () const;
  

    /**
     * @brief get phase number. 
     * @name object_name.get_phaseNumber ();
     * @param none
     * @return uint8_t RESET_TIMER = 0, START_TIMER = 1, PAUSE_TIMER = 2, STOP_TIMER = 3
     */  
    uint8_t get_phaseNumber () const;


    /**
     * @brief Get and reset error number to zero 
     * @name object_name.get_reset_error ();
     * @param none
     * @return uint8_t Error number. Error {1-127} and Warning {128-255}
     */
    uint8_t get_reset_error ();

    
  private: // Internal functions

    bool     pending_state      () const;   
    void     start_timer        ();
    void     reset_timer        ();
    void     pause_timer        ();    
    void     stop_timer         ();
    void     set_timer_state    (uint8_t  setStartResetPauseStop);
    uint32_t check_set_MS_to_US (uint32_t setCheckTimeMS);
    // Execute
    void     loop_fast             ();
    void     loop_fast_start_reset ();
    void     loop_fast_pause_begin ();
    void     loop_fast_pause_guard (); 
    void     loop_fast_pause_end   ();
    void     loop_fast_calc_time   ();
    void     loop_fast_timer_stop  ();
    // print
    void     print_phase_line ();
    void     print_phase_text ();
    void     print_error_text (uint8_t setErrorNumber);

};  // MTD2A_timer


#endif
