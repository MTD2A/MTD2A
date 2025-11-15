/**
 ******************************************************************************
 * @file    MTD2A_binary_input.h
 * @author  Joergen Bo Madsen
 * @version 1.1.6
 * @date    31. august 2025
 * @brief   Abstract Class for MTD2A (Model Train Detection And Action)
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


#ifndef _MTD2A_binary_input_H_
#define _MTD2A_binary_input_H_


class MTD2A_binary_input: public MTD2A
{
  private:
    // Specific global constants from MTD2A_binary_output.h (MTD2A_const.h)
    static constexpr bool     FIRST_TRIGGER    {MTD2A_const::FIRST_TRIGGER};
    static constexpr bool     LAST_TRIGGER     {MTD2A_const::LAST_TRIGGER};
    static constexpr bool     TIME_DELAY       {MTD2A_const::TIME_DELAY};
    static constexpr bool     MONO_STABLE      {MTD2A_const::MONO_STABLE};
    // Timer
    static constexpr uint8_t  RESET_TIMER      {MTD2A_const::RESET_TIMER};
    // Phases 
    static constexpr uint8_t  RESET_PHASE      {MTD2A_const::RESET_PHASE}; 
    static constexpr uint8_t  FIRST_TIME_PHASE {MTD2A_const::FIRST_TIME_PHASE};
    static constexpr uint8_t  LAST_TIME_PHASE  {MTD2A_const::LAST_TIME_PHASE}; 
    static constexpr uint8_t  BLOCKING_PHASE   {MTD2A_const::BLOCKING_PHASE};
    static constexpr uint8_t  COMPLETE_PHASE   {MTD2A_const::COMPLETE_PHASE};
    
    // Arguments
    char    *objectName    {nullptr};        // Constructor initialized (User defined name to display identification)
    uint32_t delayTimeMS   {0};              // Constructor default argument (Milliseconds)
    bool     triggerMode   {LAST_TRIGGER};   // Constructor default argument (FIRST_TRIGGER / LAST_TRIGGER)
    bool     timerMode     {TIME_DELAY};     // Constructor default argument (MONO_STABLE / TIME_DELAY)
    uint32_t pinBlockMS    {0};              // Constructor default argument (Milliseconds)
    // pin and input setup
    uint8_t  pinNumber     {PIN_ERROR_NO};   // initialize () default argument
    uint8_t  pinType       {INPUT_PULLUP};   // initialize () default argument. Open collector or switch to ground (LOW)
    bool     pinReadToggl  {DISABLE};        // initialize () default argument / ENABLE
    bool     pinReadMode   {NORMAL};         // initialize () and set_PinReadMode () default argument / INVERTED
    bool     inputMode     {PULSE};          // set_InputState () default argument / FIXED
    // Debug and error
    bool     debugPrint    {DISABLE};        // set_debugPrint () default argument / ENABLE
    bool     errorPrint    {DISABLE};        // set_errorPrint () default argument / ENABLE
    uint8_t  errorNumber   {0};              // get_reset_error () Error {1-127} and Warning {128-255}
    // Timers    
    uint32_t firstTimeMS   {0};              // Milliseconds (FIRST_TRIGGER)
    uint32_t lastTimeMS    {0};              // Milliseconds (LAST_TRIGGER)
    uint32_t endTimeMS     {0};              // Milliseconds (delay end time)
    uint32_t blockTimeMS   {0};              // Milliseconds (pin input blocking time)
    // Timer process stop
    bool     stopDelayTM   {DISABLE};        // stop first or last timer proces and go to next phase
    bool     stopBlockTM   {DISABLE};        // stop blocking timer procesand go to next phase
    // State control
    bool     inputState    {HIGH};           // set_inputState () / LOW (manual program trigger)
    bool     pinState      {HIGH};           // get_pinState () / LOW (trigger)
    bool     processState  {COMPLETE};       // get_processState () / ACTIVE (input trigger activated)
    bool     pinBlockState {DISABLE};        // End pin blocking time state
    bool     currentState  {HIGH};           // Loop timing states
    bool     lastState     {HIGH};           // Loop timing states
    bool     phaseChange   {false};          // true = change in timing state
    uint8_t  phaseNumber   {COMPLETE_PHASE}; // Reset= 0, active = 1, Set last time = 2, Pin block = 3, Complete = 4 
    bool     inputGoLow    {false};          // Falling edge
    bool     inputGoHigh   {false};          // Rising edge

  public:
    // Constructor initializers
    
    /**
     * @class MTD2A_binary_input
     * @brief Create object and set configuration parameters or use defaults
     * @param ( "Object Name", delayTimeMS, {FIRST_TRIGGER | LAST_TRIGGER}, {TIME_DELAY | MONO_STABLE}, pinBlocTimeMS );
     * @param delayTimeMS & pinblockTimeMS {0 - 4294967295} milliseconds
     * @return none
     */
    MTD2A_binary_input (
      const char    *setObjectName  = "Object name", 
      const uint32_t setDelayTimeMS = 0, 
      const bool     setFirstOrLast = LAST_TRIGGER, 
      const bool     setTimeOrMono  = TIME_DELAY, 
      const uint32_t setPinBlockMS  = 0
    );

    // Destructor
    ~MTD2A_binary_input () { 
      if (objectName != nullptr) {
        delete [] objectName; 
        objectName = nullptr;
      }
      if (globalObjectCount > 0 )
        globalObjectCount--;
    };    
  
    // Operator oveloading
    bool operator==(const MTD2A_binary_input &obj) const {
      return (processState == obj.processState);
    }
    bool operator!=(const MTD2A_binary_input &obj) const {
      return (processState != obj.processState);
    }
    bool operator>(const MTD2A_binary_input &obj) const {
      return (processState == ACTIVE && obj.processState == COMPLETE);
    }
    bool operator<(const MTD2A_binary_input &obj) const {
      return (processState == COMPLETE && obj.processState == ACTIVE);
    }
    bool operator>>(const MTD2A_binary_input &obj) const {
      return (lastTimeMS > obj.lastTimeMS);
    }
    bool operator<<(const MTD2A_binary_input &obj) const {
      return (lastTimeMS < obj.lastTimeMS);
    }

  public: // Functions
    /**
     * @brief Initalize and configure pin number, input mode and input type. If illigal pin configuration, pin reading is disabled! 
     * @name object_name.initialize
     * @param ( {0 - NUM_DIGITAL_PINS | 255}, {NORMAL | INVERTED}, {INPUT | INPUT_PULLUP} );
     * @return none
     */
    void initialize (const uint8_t &setPinNumber = PIN_ERROR_NO, const bool &setPinNomalOrInverted = NORMAL, const uint8_t &setPinPullupOrInput = INPUT_PULLUP);


    /**
     * @brief Reset all timers and state logic. Ready to start.
     * @name object_name.reset ();
     * @param none
     * @return none
     */
    void reset ();


    /*
     * @brief print configuration parameters, timers and state logic.
     * @name object_name.print_conf ();
     * @param none
     * @return none
     */
    void print_conf ();

    
    // Setters -----------------------------------------------


    /**
     * @brief Enable or disable pin reading.
     * @name object_name.set_pinReadToggl
     * @param ( {ENABLE | DISABLE} );
     * @return none
     */
    void set_pinReadToggl (const bool &setPinEnableOrDisable = ENABLE);


    /**
     * @brief Configure pin input trigger mode = NORMAL og INVERTED.
     * @name object_name.set_pinReadMode
     * @param ( {NORMAL | INVERTED} );
     * @return non
     */
    void set_pinReadMode (const bool &setPinNomalOrInverted = NORMAL);


    /**
     * @brief Set input state and set input mode.
     * @name object_name.set_inputState
     * @param ( {HIGH | LOW}, {PULSE | FIXED} );
     * @return none
     */
    void set_inputState (const bool &setInputLowOrHigh = LOW, const bool &setPulseOrFixed = PULSE);

    
    /**
     * @brief Set new delay time in milliseconds.
     * @name object_name.set_delayTimeMS
     * @param ( {0 - 4294967295} );
     * @return none
     */  
    void set_delayTimeMS (const uint32_t &setDelayTimeMS);

    
    /**
     * @brief Set new pin block time in milliseconds.
     * @name object_name.set_pinBlockMS
     * @param ( {0 - 4294967295} );
     * @return none
     */  
    void set_pinBlockMS (const uint32_t &setPinBlockMS);

    
    /**
     * @brief stop first og last timer process immediately and continue to next phase.
     * @name object_name.set_stopDelayTimer
     * @param none
     * @return none
     */
    void set_stopDelayTimer ();

    /**
     * @brief stop blocking timer process immediately and continue to next phase.
     * @name object_name.set_stopBlockTimer
     * @param none
     * @return none
     */
    void set_stopBlockTimer ();


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


    // Getters -----------------------------------------------


    /**
     * @brief Get processState  
     * @name object_name.get_processState ();
     * @param none
     * @return bool {ACTIVE | COMPLETE}
     */
    bool const &get_processState () const;


    /**
     * @brief Get pinState
     * @name object_name.get_pinState ();
     * @param none
     * @return bool {HIGN | LOW}
    */
    bool const &get_pinState () const;


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
     * @return uint8_t Reset = 0, first time = 1, last time = 2, Pin block = 3, Complete = 4
     */
    uint8_t const &get_phaseNumber () const;


    /**
     * @brief Get firstTimeMS
     * @name object_name.get_firstTimeMS ();
     * @param none
     * @return uint32_t Milliseconds 
     */
    uint32_t const &get_firstTimeMS () const;


    /**
     * @brief Get lastTimeMS
     * @name object_name.get_lastTimeMS ();
     * @param none
     * @return uint32_t Milliseconds
     */
    uint32_t const &get_lastTimeMS () const;


    /**
     * @brief  Get endTimeMS Milliseconds 
     * @name object_name.get_endTimeMS ();
     * @param none
     * @return uint32_t Milliseconds
     */
    uint32_t const &get_endTimeMS () const;


    /**
     * @brief get inputGoHigh
     * @name object_name.get_inputGoHigh ();
     * @param none
     * @return bool {true | false}
     */
    bool const &get_inputGoHigh () const;


    /**
     * @brief get inputGoLow
     * @name object_name.get_inputGoLow ();
     * @param none
     * @return bool {true | false} 
     */
    bool const &get_inputGoLow () const;


    /**
     * @brief Get and reset error number to zero 
     * @name object_name.get_reset_error ();
     * @param none
     * @return uint8_t Error number. Error {1-127} and Warning {128-255}
     */
    uint8_t const get_reset_error ();


  private: // Functions
  
    void     loop_fast        ();
    void     loop_fast_input  ();
    void     loop_fast_binary ();
    void     loop_fast_first  ();
    void     loop_fast_last   ();
    void     begin_state      ();
    void     end_state        ();
    void     complete_state   ();
    uint32_t check_set_time   (const uint32_t &setCheckTimeMS);
    void     print_error_text (const uint8_t setErrorNumber);
    void     print_phase_text ();
    void     print_phase_line (const uint8_t &printRestartTimer = MAX_BYTE_VALUE);
    
}; // class MTD2A_binary_input


#endif
