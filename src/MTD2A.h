/*
 ******************************************************************************
 * @file    MTD2A.h
 * @author  Joergen Bo Madsen
 * @version V1.1
 * @date    12. maj 2025
 * @brief   Abstract Class for MTD2A (Mobile Train Detection And Action)
 * 
 * Supporting a vast variety of input sensors and output devices 
 * Simple to use to build complex solutions 
 * Non blocking simple yet efficient state machine
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

#ifndef _MTD2A_H_
#define _MTD2A_H_

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



// MTD2A base classary ---------------------------------------------------------------------------------------------------



// Common phase
const uint8_t resetPhase {0};

class MTD2A
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



// MTD2A_binary_input ----------------------------------------------------------------------------------------------------



const uint8_t firstTimePhase {1}, lastTimePhase {2}, blockingPhase {3},  pendingPhase {4};

class MTD2A_binary_input: public MTD2A
{
  public:
    // Debug help text
    const char *phaseText[5] = {"[0] Reset", "[1] First time", "[2] Last time", "[3] Pin blocking", "[4] Pending"};

  private:
    // Arguments
    char    *objectName    {nullptr}; // Constructor initialized (User defined name to display identification)
    uint32_t delayTimeMS   {};  // Constructor default argument (Milliseconds)
    bool     triggerMode   {};  // Constructor default argument (firstTrigger / lastTrigger)
    bool     timerMode     {};  // Constructor default argument (monoStable / timeDelay)
    uint32_t pinBlockMS    {};  // Constructor default argument (Milliseconds)
    // pin and input setup
    uint8_t  pinNumber     {255};          // initialize () default argument
    uint8_t  pinType       {INPUT_PULLUP}; // initialize () default argument. Open collector or switch to ground (LOW)
    bool     pinRead       {Disable};      // initialize () default argument / enable
    bool     pinInput      {normal};       // set_PinInput () default argument / inverted
    bool     inputMode     {pulse};        // set_InputState () default argument / Fixed
    // Other
    bool     debugPrint    {disable};      // set_debugPrint () default argument / enable
    uint8_t  errorNumber   {0};            // get_reset_error () Error {1-127} and Warning {128-255}
    // Timers    
    uint32_t firstTimeMS   {0};            // Milliseconds (firstTrigger)
    uint32_t lastTimeMS    {0};            // Milliseconds (lastTrigger)
    uint32_t endTimeMS     {0};            // Milliseconds (delay end time)
    uint32_t BlockTimeMS   {0};            // Milliseconds (pin input bloscking time)
    // State control
    bool     inputState    {HIGH};         // set_inputState () / LOW (manual program trigger)
    bool     pinState      {HIGH};         // get_pinState () / LOW (trigger)
    bool     processState  {pending};      // get_processState () / active (input trigger activated)
    bool     pinBlockState {disable};      // End pin blocking time state
    bool     currentState  {HIGH};         // Loop timing states
    bool     lastState     {HIGH};         // Loop timing states
    bool     phaseChange   {false};        // true = change in timing state
    uint8_t  phaseNumber   {resetPhase};   // initialize and reset= 0, active = 1, Set last time = 2, Pin block = 3, Pending = 4 
    bool     inputGoLow    {false};        // Falling edge
    bool     inputGoHigh   {false};        // Rising edge

  public: 
    // Constructor inittializers
    /*
     * @brief Create object and set configuration parameters or use defaults
     * @name MTD2A_binary_input object_name
     * @param ( "Object Name", delayTimeMS, {firstTrigger | lastTrigger}, {timeDelay | monoStable}, pinBlocTimeMS );
     * @param delayTimeMS & pinBlockTimeMS {0 - 4294967295} milliseconds
     * @return none
     */
    MTD2A_binary_input (
      const char    *setObjectName  = "Object name", 
      const uint32_t setDelayTimeMS = 0, 
      const bool     setFirstOrLast = firstTrigger, 
      const bool     setTimeOrMono  = timeDelay, 
      const uint32_t setPinBlockMS  = 0 );

    // Destructor
    ~MTD2A_binary_input() { delete [] objectName; };
 
    // Operator oveloading
    bool operator==(const MTD2A_binary_input &obj) const {
      return (processState == obj.processState);
    }
    bool operator!=(const MTD2A_binary_input &obj) const {
      return (processState != obj.processState);
    }
    bool operator>(const MTD2A_binary_input &obj) const {
      return (processState == active && obj.processState == pending);
    }
    bool operator<(const MTD2A_binary_input &obj) const {
      return (processState == pending && obj.processState == active);
    }
    bool operator+(const MTD2A_binary_input &obj) const {
      return (phaseChange == true && phaseNumber == firstTimePhase);
    }
    bool operator-(const MTD2A_binary_input &obj) const {
      return (phaseChange == true && phaseNumber == pendingPhase);
    }
    bool operator>>(const MTD2A_binary_input &obj) const {
      return (lastTimeMS > obj.lastTimeMS);
    }
    bool operator<<(const MTD2A_binary_input &obj) const {
      return (lastTimeMS < obj.lastTimeMS);
    }

  public: // Functions
    /*
     * @brief Initalize and configure input pin. If illigal pin configuration, pin reading is disabled! 
     * @name object_name.initialize
     * @param ( {0 - NUM_DIGITAL_PINS | 255}, {normal | inverted}, {INPUT | INPUT_PULLUP} );
     * @return none
     */
    void initialize (const uint8_t &setPinNumber = 255, const bool &setPinNomalOrInverted = normal, const uint8_t &setPinPullupOrInput = INPUT_PULLUP);


    /*
     * @brief Reset all timers and state logic. Ready to start.
     * @name object_name.reset ();
     * @param none
     * @return none
     */
    void reset ();

    
    /*
     * @brief update functions and state logic every 1 - 10 millisseconds (delay(10);)
     * @name object_name.loop_fast ();
     * @param none
     * @return none
     */
    void loop_fast ();


    /*
     * @brief print configuration parameters, timers and state logic.
     * @name object_name.print_conf ();
     * @param none
     * @return none
     */
    void print_conf ();

    
    // Setters -----------------------------------------------


    /*
     * @brief Enable or disable pin reading. Optional loop update.
     * @name object_name.set_pinRead 
     * @param ( {enable | disable} );
     * @return none
     */
    void set_pinRead (const bool &setPinEnableOrDisable = enable, const bool &LoopFastOnce = disable);


    /*
     * @brief Configure pin input trigger mode = LOW or HIGH (inverted).Optional loop update.
     * @name object_name.set_pinInput
     * @param ( {normal | inverted} );
     * @return non
     */
    void set_pinInput (const bool &setPinNomalOrInverted = normal, const bool &LoopFastOnce = disable);


    /*
     * @brief ASet input state and set input mode. Optional loop update.
     * @name object_name.set_inputState
     * @param ( {HIGH | LOW}, {pulse | fixed} );
     * @return none
     */
    void set_inputState (const bool &setInputLowOrHigh = LOW, const bool &setPulseOrFixed = pulse, const bool &LoopFastOnce = disable);


    /*
     * @brief Enable print phase state number and phase state text. Optional loop update.
     * @name object_name.set_debugPrint
     * @param ( {enable | disable} );
     * @return none
     */    
    void set_debugPrint (const bool &setEnableOrDisable = disable, const bool &LoopFastOnce = disable);


    // Getters -----------------------------------------------


    /*
     * @brief Get processState  
     * @name object_name.get_processState ();
     * @param none
     * @return bool {active | pending}
     */
    bool const &get_processState () const;


    /*
     * @brief Get pinState
     * @name object_name.get_pinState ();
     * @param none
     * @return bool {HIGN | LOW}
    */
    bool const &get_pinState () const;


    /*
     * @brief get phaseChange 
     * @name object_name.get_phaseChange ();
     * @param none
     * @return bool {true | false}
     */
    bool const &get_phaseChange () const;


    /*
     * @brief get phase number. 
     * @name object_name.get_phaseNumber ();
     * @param none
     * @return uint8_t Initialize and reset = 0, first time = 1, last time = 2, Pin block = 3, Pending = 4
     */
    uint8_t const &get_phaseNumber () const;


    /*
     * @brief Get firstTimeMS
     * @name object_name.get_firstTimeMS ();
     * @param none
     * @return uint32_t Milliseconds 
     */
    uint32_t const &get_firstTimeMS () const;


    /*
     * @brief Get lastTimeMS
     * @name object_name.get_lastTimeMS ();
     * @param none
     * @return uint32_t Milliseconds
     */
    uint32_t const &get_lastTimeMS () const;


    /*
     * @brief  Get endTimeMS Milliseconds 
     * @name object_name.get_endTimeMS ();
     * @param none
     * @return uint32_t Milliseconds
     */
    uint32_t const &get_endTimeMS () const;


    /*
     * @brief get inputGoHigh
     * @name object_name.get_inputGoHigh ();
     * @param none
     * @return bool {true | false}
     */
    bool const &get_inputGoHigh () const;


    /*
     * @brief get inputGoLow
     * @name object_name.get_inputGoLow ();
     * @param none
     * @return bool {true | false} 
     */
    bool const &get_inputGoLow () const;


    /*
     * @brief  Get and clear error number 
     * @name object_name.get_reset_error ();
     * @param none
     * @return uint8_t Error number. Error {1-127} and Warning {128-255}
     */
    uint8_t const &get_reset_error ();


  private: // Functions
    void loop_fast_input  ();
    void loop_fast_binary ();
    void loop_fast_first  ();
    void loop_fast_last   ();
    void begin_state      ();
    void end_state        ();
    void pending_state    ();
    void loop_fast_ptr    ();
}; // class MTD2A_binary_input



// MTD2A_binary_output ---------------------------------------------------------------------------------------------------



const uint8_t beginPhase {1}, outputPhase {2}, endPhase {3}, completePhase {4};

class MTD2A_binary_output: public MTD2A
{
  public:
    // Debug help text
    const char *phaseText[5] = {"[0] Reset", "[1] Begin delay", "[2] Output", "[3] End delay", "[4] Complete"};

  private:
    // Arguments
    char    *objectName    {nullptr};    // Constructor default argument (User defined name to display identification)
    uint32_t outputTimeMS  {};           // Constructor default argument (Milliseconds) 
    uint32_t beginDelayMS  {};           // Constructor default argument (Milliseconds)  
    uint32_t endDelayMS    {};           // Constructor default argument (Milliseconds)
    bool     pinOutputMode {};           // Constructor default argument (binary/PWM)
    uint8_t  pinBeginValue {};           // Constructor default argument binary {HIGH | LOW} / PWM {0-255} 
    uint8_t  pinEndValue   {};           // Constructor default argument binary {HIGH | LOW} / PWM {0-255}
    // pin and input setup
    uint8_t  pinNumber     {255};        // initialize () default argument
    bool     pinWrite      {disable};    // initialize () MTD2A_reserve_and_check_pin ()
    uint8_t  startPinValue {0};          // initialize () default argument binary {HIGH | LOW} / PWM {0-255}
    uint8_t  setPinValue   {0};          // set_pinValue () default argument binary {HIGH | LOW} / PWM {0-255}
    bool     processState  {complete};   // process state / active
    // Timers    
    uint32_t setBeginMS    {0};          // Milliseconds (begin start time)
    uint32_t setOutputMS   {0};          // Milliseconds (output start time)
    uint32_t setEndMS      {0};          // Milliseconds (end start time)
    // Other
    bool     debugPrint    {disable};    // set_debugPrint () default argument / on
    uint8_t  errorNumber   {0};          // get_reset_error () Error {1-127} and Warning {128-255}
    // state control
    bool     phaseChange   {false};      // true = change in timing state
    uint8_t  phaseNumber   {resetPhase}; // Initialize and reset= 0, Begin delay = 1, Output = 2, End delay = 3, Complete = 4 

  public:
    // Constructor inittializers
    /*
     * @brief Create object and set configuration parameters or use defaults
     * @name MTD2A_binary_output object_name
     * @param ( "Object Name", outputTimeMS, beginDelayMS, endDelayMS, {binary | PWM}, pinBeginValueMS, pinEndValue );
     * @param outputTimeMS, beginDelayMS, endDelayMSd {0 - 4294967295} milliseconds. pinValue {0 - 255}
     * @return none
     */
    MTD2A_binary_output (
      const char    *setObjectName    = "Object name",
      const uint32_t setOutputTimeMS  = 0,
      const uint32_t setBeginDelayMS  = 0,
      const uint32_t setEndDelayMS    = 0,
      const bool     setPinOutputMode = binary,
      const uint8_t  setPinBeginValue = 255, 
      const uint8_t  setPinEndValue   = 0 );
    // Destructor
    ~MTD2A_binary_output () { delete [] objectName; };

    // Operator oveloading
    bool operator==(const MTD2A_binary_output &obj) const {
      return (processState == obj.processState);
    }
    bool operator!=(const MTD2A_binary_output &obj) const {
      return (processState != obj.processState);
    }
    bool operator>(const MTD2A_binary_output &obj) const {
      return (processState == active && obj.processState == pending);
    }
    bool operator<(const MTD2A_binary_output &obj) const {
      return (processState == pending && obj.processState == active);
    }
    bool operator+(const MTD2A_binary_output &obj) const {
      return (phaseChange == true && phaseNumber == completePhase);
    }
    bool operator-(const MTD2A_binary_output &obj) const {
      return (phaseChange == true && phaseNumber == completePhase);
    }

  public: // Functions


    /*
     * @brief Initalize and configure output pin. Optional loop update.
     * @brief If setPinNumber > NUM_DIGITAL_PINS, pin writing is disabled!
     * @brief If PWM is selected and pin does not support PWM, pin writing is disabled!
     * @name object_name.initialize 
     * @param ( {0 - NUM_DIGITAL_PINS | 255} );
     * @return none
     */
    void initialize (const uint8_t &setPinNumber = 255, const uint8_t &setStartPinValue = 0);
  

    /*
     * @brief Activate process. Optional process update.
     * @name object_name.activate
     * @param ( {pulse | fixed} );
     * @return none
     */  
    void activate (const bool &LoopFastOnce = disable);
  


    /*
     * @brief Reset all timers and state logic. Ready to start.
     * @name object_name.reset ();
     * @param none
     * @return none
     */ 
    void reset ();
  

    /*
     * @brief update functions and state logic every 1 - 10 millisseconds (delay(10);)
     * @name object_name.loop_fast ();
     * @param none
     * @return none
     */  
    void loop_fast ();
  

    /*
     * @brief print configuration parameters, timers and state logic.
     * @name object_name.print_conf ();
     * @param none
     * @return none
     */  
    void print_conf ();
  
  
    // Setters -----------------------------------------------
  
  
    /*
     * @brief Enable or disable write to pin (binary and analog PWM)
     * @name object_name.set_pinWrite
     * @param ( {enable | disable} );
     * @return none
     */ 
    void set_pinWrite (const bool &setPinEnableOrDisable = disable, const bool &LoopFastOnce = disable);
  

    /*
     * @brief write binary or PWM value directly to pin
     * @name object_name.set_setPinValue
     * @param (binary {HIGH | LOW} / PWM {0-255} );
     * @return none
     */  
    void set_setPinValue (const uint8_t &setSetPinValue = 0, const bool &LoopFastOnce = disable);
  

    /*
     * @brief Enable print phase state number and phase state text
     * @name object_name.set_debugPrint
     * @param ( {enable | disable} );
     * @return none
     */  
    void set_debugPrint (const bool &setEnableOrDisable = disable, const bool &LoopFastOnce = disable);
    
    
    // Getters -----------------------------------------------

    
    /*
     * @brief Get pinWrite status
     * @name object_name.get_pinWrite (); 
     * @param none
     * @return bool {enable | disable} );
     */  
    bool const &get_pinWrite () const;
  

    /*
     * @brief Get processState  
     * @name object_name.get_processState ();
     * @param none
     * @return bool {active | finnish}
     */  
    bool const &get_processState () const;
  

    /*
     * @brief get phaseChange 
     * @name object_name.get_phaseChange ();
     * @param none
     * @return bool {true | false}
     */  
    bool const &get_phaseChange () const;
  

    /*
     * @brief get phase number. 
     * @name object_name.get_phaseNumber ();
     * @param none
     * @return uint8_t Initialize and reset = 0, Begin delay = 1, Output = 2, End delay = 3, Complete = 4
     */  
    uint8_t const &get_phaseNumber () const;
  

    /*
     * @brief  Get and clear error number 
     * @name object_name.get_reset_error ();
     * @param none
     * @return uint8_t Error number. Error {1-127} and Warning {128-255}
     */  
    uint8_t const &get_reset_error ();


  private: // Functions
    void loop_fast_begin     ();
    void loop_fast_out_begin ();
    void loop_fast_out_end   ();
    void loop_fast_end       ();
    void loop_fast_complete  ();
    
}; // class MTD2A_binary_output 



// MTD2A planned classes ---------------------------------------------------------------------------------------------------



// planned classes
class MTD2A_delay     {}; // milliseconds (delayTmeMS, {normal | pulse} )
class MTD2A_astable   {}; // highMS, LowMS, [(highFreq, lowFfreq) if != 0 -> PWM], numCycles, NumSubCycles, {pulse | fixed}
class MTD2A_sound     {}; // ptr frq list, ptr timer list}
class MTD2A_servo     {};
class MTD2A_DCC_input {};
// More to come

#endif