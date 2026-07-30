/**
 ******************************************************************************
 * @file    MTD2A_binary_input.h
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
 * Do not call MTD2A_binary_input from an interrupt handler (ISR)
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


#ifndef MTD2A_BINARY_INPUT_H_
#define MTD2A_BINARY_INPUT_H_

#include "MTD2A_base.h"


class MTD2A_binary_input: public MTD2A
{
  private:
    // Specific global constants from MTD2A_const.h
    static constexpr uint32_t MAX_TIME_MS      {MTD2A_const::MAX_TIME_MS};
    //
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
    char    *objectName      {nullptr};        // Constructor initialized (User defined name to display identification)
    uint32_t delayTimeUS     {0UL};            // Constructor default argument (Microseconds)
    bool     triggerMode     {LAST_TRIGGER};   // Constructor default argument (FIRST_TRIGGER / LAST_TRIGGER)
    bool     timerMode       {TIME_DELAY};     // Constructor default argument (MONO_STABLE / TIME_DELAY)
    uint32_t pinBlockUS      {0UL};            // Constructor default argument (Microseconds)
    // pin and input setup
    uint8_t  pinNumber       {NO_PIN};         // initialize () default argument
    uint8_t  pinType         {INPUT_PULLUP};   // initialize () default argument. Open collector or switch to ground (LOW)
    bool     pinReadToggl    {DISABLE};        // initialize () default argument / ENABLE
    bool     pinReadMode     {NORMAL};         // initialize () and set_PinReadMode () default argument / INVERTED
    bool     inputMode       {PULSE};          // set_InputState () default argument / FIXED
    // Debug and error
    bool     debugPrint      {DISABLE};        // set_debugPrint () default argument / ENABLE
    bool     errorPrint      {DISABLE};        // set_errorPrint () default argument / ENABLE
    uint8_t  errorNumber     {0};              // get_reset_error () Error {1-127} and Warning {128-255}
    bool     initializedOnce {false};          // Initialize () called (in Arduino void setup())
    bool     objInstantiated {false};          // Object instantiated
    uint8_t  objectError     {0};              // Object instantiation error / warning   
    // Timers    
    uint32_t firstTimeUS     {0UL};            // Microseconds (FIRST_TRIGGER)
    uint32_t lastTimeUS      {0UL};            // Microseconds (LAST_TRIGGER)
    uint32_t endTimeUS       {0UL};            // Microseconds (delay end time)
    uint32_t blockTimeUS     {0UL};            // Microseconds (pin input blocking time)
    // Timer process stop
    bool     stopDelayTM     {DISABLE};        // stop first or last timer process and go to next phase
    bool     stopBlockTM     {DISABLE};        // stop blocking timer process and go to next phase
    // State control
    bool     inputState      {HIGH};           // set_inputState () / LOW (manual program trigger)
    bool     pinState        {HIGH};           // get_pinState () / LOW (trigger)
    bool     processState    {COMPLETE};       // get_processState () / ACTIVE (input trigger activated)
    bool     pinBlockState   {DISABLE};        // End pin blocking time stateBLOCKING_PHASE
    bool     pinReadSaved    {DISABLE};        // pinReadToggl state saved before BLOCKING_PHASE
    bool     currentState    {HIGH};           // Loop timing states
    bool     lastState       {HIGH};           // Loop timing states
    // Phase control
    bool     phaseChange     {false};          // true = change in timing state
    uint8_t  phaseNumber     {COMPLETE_PHASE}; // Reset = 0, active = 1, Set last time = 2, Pin block = 3, Complete = 4 
    // Input logic
    bool     inputGoLow      {false};          // Falling edge
    bool     inputGoHigh     {false};          // Rising edge

  public:
    // Constructor initializers
    
    // The default / no-args constructor is implicit in the constructor below. Do not use MTD2A_binary_input ();
       
    /**
     * @class MTD2A_binary_input
     * @brief Create object and set configuration parameters or use defaults
     * @param ( "Object Name", delayTimeMS, {FIRST_TRIGGER | LAST_TRIGGER}, {TIME_DELAY | MONO_STABLE}, pinBlockTimeMS );
     * @param delayTimeMS & pinblockTimeMS {0 - MAX_TIME_MS} milliseconds
     * @return none
     */
    MTD2A_binary_input (
      const char    *setObjectName  = "Object name", 
      uint32_t setDelayTimeMS = 0, 
      bool     setFirstOrLast = LAST_TRIGGER, 
      bool     setTimeOrMono  = TIME_DELAY, 
      uint32_t setPinBlockMS  = 0
    );

    
    // Destructor
    ~MTD2A_binary_input () { 
      if (objectName != nullptr) {
        delete [] objectName; 
        objectName = nullptr;
      }
    }
  
    // Operator overloading
    explicit operator bool() const {
      return (phaseChange == true  &&  phaseNumber == COMPLETE_PHASE); 
    }
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


  public: // Functions
    /**
     * @brief Initialize and configure pin number, input mode and input type. If illegal pin configuration, pin reading is disabled! 
     * @name object_name.initialize
     * @param ( {0 - NUM_DIGITAL_PINS | NO_PIN}, {NORMAL | INVERTED}, {INPUT | INPUT_PULLUP} );
     * @return none
     */
    void initialize (uint8_t setPinNumber = NO_PIN, bool setPinNormalOrInverted = NORMAL, uint8_t setPinPullupOrInput = INPUT_PULLUP);


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
    void set_pinReadToggl (bool setPinEnableOrDisable = ENABLE);


    /**
     * @brief Configure pin input trigger mode = NORMAL or INVERTED.
     * @name object_name.set_pinReadMode
     * @param ( {NORMAL | INVERTED} );
     * @return none
     */
    void set_pinReadMode (bool setPinNormalOrInverted = NORMAL);


    /**
     * @brief Set input state and set input mode.
     * @name object_name.set_inputState
     * @param ( {HIGH | LOW}, {PULSE | FIXED} );
     * @return none
     */
    void set_inputState (bool setInputLowOrHigh = LOW, bool setPulseOrFixed = PULSE);

    
    /**
     * @brief Set new delay time in milliseconds.
     * @name object_name.set_delayTimeMS
     * @param ( {0 - MAX_TIME_MS} );
     * @return none
     */  
    void set_delayTimeMS (uint32_t setDelayTimeMS);

    
    /**
     * @brief Set new pin block time in milliseconds.
     * @name object_name.set_pinBlockMS
     * @param ( {0 - MAX_TIME_MS} );
     * @return none
     */  
    void set_pinBlockMS (uint32_t setPinBlockMS);

    
    /**
     * @brief stop first or last timer process immediately and continue to next phase.
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
    void set_debugPrint (bool setEnableOrDisable = ENABLE);


    /**
     * @brief Enable error print text.
     * @name object_name.set_errorPrint
     * @param ( {ENABLE | DISABLE} );
     * @return none
     */    
    void set_errorPrint (bool setEnableOrDisable = ENABLE);


    // Getters -----------------------------------------------


    /**
     * @brief Get processState  
     * @name object_name.get_processState ();
     * @param none
     * @return bool {ACTIVE | COMPLETE}
     */
    bool get_processState () const;


    /**
     * @brief Get pinState
     * @name object_name.get_pinState ();
     * @param none
     * @return bool {HIGH | LOW}
    */
    bool get_pinState () const;


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
     * @return uint8_t Reset = 0, first time = 1, last time = 2, Pin block = 3, Complete = 4
     */
    uint8_t get_phaseNumber () const;


    /**
     * @brief Get firstTimeMS
     * @name object_name.get_firstTimeMS ();
     * @param none
     * @return uint32_t Milliseconds 
     */
    uint32_t get_firstTimeMS () const;


    /**
     * @brief Get lastTimeMS
     * @name object_name.get_lastTimeMS ();
     * @param none
     * @return uint32_t Milliseconds
     */
    uint32_t get_lastTimeMS () const;


    /**
     * @brief  Get endTimeMS Milliseconds 
     * @name object_name.get_endTimeMS ();
     * @param none
     * @return uint32_t Milliseconds
     */
    uint32_t get_endTimeMS () const;


    /**
     * @brief get inputGoHigh
     * @name object_name.get_inputGoHigh ();
     * @param none
     * @return bool {true | false}
     */
    bool get_inputGoHigh () const;


    /**
     * @brief get inputGoLow
     * @name object_name.get_inputGoLow ();
     * @param none
     * @return bool {true | false} 
     */
    bool get_inputGoLow () const;


    /**
     * @brief Get and reset error number to zero 
     * @name object_name.get_reset_error ();
     * @param none
     * @return uint8_t Error number. Error {1-127} and Warning {128-255}
     */
    uint8_t get_reset_error ();


  private: // Functions
  
    void     loop_fast          ();
    void     loop_fast_input    ();
    void     loop_fast_binary   ();
    void     loop_fast_first    ();
    void     loop_fast_last     ();
    void     begin_state        ();
    void     end_state          ();
    void     complete_state     ();
    uint32_t check_set_MS_to_US (uint32_t setCheckTimeMS);
    void     check_pin_init     (uint8_t  checkPinNumber, uint8_t checkPinPullupOrInput);
    void     print_error_text   (uint8_t  setErrorNumber);
    void     print_phase_text   ();
    void     print_phase_line   ();
    
}; // class MTD2A_binary_input


#endif
