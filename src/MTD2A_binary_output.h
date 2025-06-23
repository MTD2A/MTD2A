/*
 ******************************************************************************
 * @file    MTD2A_binary_output.h
 * @author  Joergen Bo Madsen
 * @version V1.1.3
 * @date    30. maj 2025
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

 
#ifndef _MTD2A_binary_output_H_
#define _MTD2A_binary_output_H_


class MTD2A_binary_output: public MTD2A
{
  public:
    // Debug help text
    static constexpr const char* phaseText[5] = { "[0] Reset", "[1] Begin delay", "[2] Output", "[3] End delay", "[4] Complete" };

//    static const char *phaseText[5];

  private:
    // Global parameter constants
    static constexpr uint8_t RESET_PHASE    = MTD2A_const::RESET_PHASE; 
    static constexpr uint8_t BEGIN_PHASE    = MTD2A_const::BEGIN_PHASE;
    static constexpr uint8_t OUTPUT_PHASE   = MTD2A_const::OUTPUT_PHASE; 
    static constexpr uint8_t END_PHASE      = MTD2A_const::END_PHASE;
    static constexpr uint8_t COMPLETE_PHASE = MTD2A_const::COMPLETE_PHASE;
    // PWM curves
    static constexpr uint8_t NO_CURVE       = MTD2A_const::NO_CURVE;
    static constexpr uint8_t RISING_XY      = MTD2A_const::RISING_XY;
    static constexpr uint8_t FALLING_XY     = MTD2A_const::FALLING_XY;
    static constexpr uint8_t RISING_B05     = MTD2A_const::RISING_B05;
    static constexpr uint8_t RISING_B025    = MTD2A_const::RISING_B025;
    static constexpr uint8_t RISING_E05     = MTD2A_const::RISING_E05;
    static constexpr uint8_t RISING_E025    = MTD2A_const::RISING_E025;
    static constexpr uint8_t FALLING_B05    = MTD2A_const::FALLING_B05;
    static constexpr uint8_t FALLING_B025   = MTD2A_const::FALLING_E025;
    static constexpr uint8_t FALLING_E05    = MTD2A_const::FALLING_E05;
    static constexpr uint8_t FALLING_E025   = MTD2A_const::FALLING_E025;
    // Arguments
    char    *objectName    {nullptr};      // Constructor default argument (User defined name to display identification)
    uint32_t outputTimeMS  {0};            // Constructor default argument (Milliseconds) 
    uint32_t beginDelayMS  {0};            // Constructor default argument (Milliseconds)  
    uint32_t endDelayMS    {0};            // Constructor default argument (Milliseconds)
    bool     pinOutputMode {BINARY};       // Constructor default argument (BINARY/PWM)
    uint8_t  pinBeginValue {HIGH};         // Constructor default argument BINARY {HIGH | LOW} / PWM {0-255} 
    uint8_t  pinEndValue   {LOW};          // Constructor default argument BINARY {HIGH | LOW} / PWM {0-255}
    // pin and input setup
    uint8_t  pinNumber     {PIN_ERROR_NO}; // initialize () default argument
    bool     pinWrite      {DISABLE};      // initialize () MTD2A_reserve_and_check_pin ()
    bool     pinOutput     {NORMAL};       // initialize () and set_PinOutput () default argument / INVERTED
    uint8_t  startPinValue {LOW};          // initialize () default argument BINARY {HIGH | LOW} / PWM {0-255}
    uint8_t  setPinValue   {LOW};          // set_pinValue () default argument BINARY {HIGH | LOW} / PWM {0-255}
    bool     processState  {COMPLETE};     // process state / ACTIVE
    // Timers
    uint32_t setOutputMS   {0};            // Milliseconds (output start time)
    uint32_t setBeginMS    {0};            // Milliseconds (begin start time)
    uint32_t setEndMS      {0};            // Milliseconds (end start time)
    uint32_t currentTimeMS {0};            // Handle millis overflow
    // Change timing
    bool     stopOutputTM  {DISABLE};      // stop output timer process
    bool     stopBeginTM   {DISABLE};      // stop begin delay timer proces
    bool     stopEndTM     {DISABLE};      // stop end delay timer proces
    // Other
    bool     debugPrint    {DISABLE};      // set_debugPrint () default argument / on
    uint8_t  errorNumber   {0};            // get_reset_error () Error {1-127} and Warning {128-255}
    // state control
    bool     phaseChange   {false};        // true = change in timing state
    uint8_t  phaseNumber   {RESET_PHASE};  // Initialize and reset= 0, Begin delay = 1, Output = 2, End delay = 3, Complete = 4
    // PWMcurves
    uint8_t  PWMcurveType  {NO_CURVE};     // PWM curve selection   
    //const uint32_t PWM_ARRAY_SIZE {256};   // 255 steps
    //uint8_t  PWMcurveArray [255] {0};

  public:
    // Constructor inittializers
    /*
     * @brief Create object and set configuration parameters or use defaults
     * @name MTD2A_BINARY_output
     * @param ( "Object Name", outputTimeMS, beginDelayMS, endDelayMS, {BINARY | PWM}, pinBeginValueMS, pinEndValue );
     * @param outputTimeMS, beginDelayMS, endDelayMSd {0 - 4294967295} milliseconds. pinValue {0 - 255}
     * @return none
     */
    MTD2A_binary_output (
      const char    *setObjectName    = "Object name",
      const uint32_t setOutputTimeMS  = 0,
      const uint32_t setBeginDelayMS  = 0,
      const uint32_t setEndDelayMS    = 0,
      const bool     setPinOutputMode = BINARY,
      const uint8_t  setPinBeginValue = HIGH, 
      const uint8_t  setPinEndValue   = LOW
    );
    // Destructor
    ~MTD2A_binary_output () { 
      if (objectName != nullptr) {
        delete [] objectName; 
        objectName = nullptr;
      }
    };

    // Operator oveloading
    bool operator==(const MTD2A_binary_output &obj) const {
      return (processState == obj.processState);
    }
    bool operator!=(const MTD2A_binary_output &obj) const {
      return (processState != obj.processState);
    }
    bool operator>(const MTD2A_binary_output &obj) const {
      return (processState == ACTIVE && obj.processState == COMPLETE);
    }
    bool operator<(const MTD2A_binary_output &obj) const {
      return (processState == COMPLETE && obj.processState == ACTIVE);
    }
    bool operator>>(const MTD2A_binary_output &obj) const {
      return (setOutputMS > obj.setOutputMS);
    }
    bool operator<<(const MTD2A_binary_output &obj) const {
      return (setOutputMS < obj.setOutputMS);
    }

  public: // Functions


    /*
     * @brief Initalize and configure output pin. Optional loop update.
     * @brief If setPinNumber > NUM_DIGITAL_PINS, pin writing is disabled!
     * @brief If PWM is selected and pin does not support PWM, pin writing is disabled!
     * @name object_name.initialize 
     * @param ( {0 - NUM_DIGITAL_PINS | 255}, {NORMAL| INVERTED}, BINARY {HIGH | LOW} / PWM {0-255} );
     * @return none
     */
    void initialize (const uint8_t &setPinNumber = PIN_ERROR_NO, const bool &setPinNomalOrInverted = NORMAL, const uint8_t &setStartPinValue = LOW);
  

    /*
     * @brief Activate process. Optional process update.
     * @name object_name.activate
     * @param ( pinBeginValueMS, pinEndValue, PWMcurve ));
     * @return none
     */  
    void activate ();
    void activate (const uint8_t &setPinBeginValue);
    void activate (const uint8_t &setPinBeginValue, const uint8_t &setPinEndValue);
    void activate (const uint8_t &setPinBeginValue, const uint8_t &setPinEndValue, const uint8_t &setPWMcurveType);
    void activate (const uint8_t &setPinBeginValue, const uint8_t &setPinEndValue, const uint8_t &setPWMcurveType, const bool &LoopFastOnce);


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
     * @brief Enable or disable write to pin (BINARY and analog PWM)
     * @name object_name.set_pinWrite
     * @param ( {ENABLE | DISABLE} );
     * @return none
     */ 
    void set_pinWrite (const bool &setPinEnableOrDisable = ENABLE, const bool &LoopFastOnce = DISABLE);
  

    /*
     * @brief Configure pin output mode = NORMAL or INVERTED. Optional loop update.
     * @name object_name.set_pinOutput
     * @param ( {NORMAL | INVERTED} );
     * @return non
     */
    void set_pinOutput (const bool &setPinNomalOrInverted = NORMAL, const bool &LoopFastOnce = DISABLE);


    /*
     * @brief write binary or PWM value directly to pin
     * @name object_name.set_setPinValue
     * @param (BINARY {HIGH | LOW} / PWM {0-255} );
     * @return none
     */  
    void set_setPinValue (const uint8_t &setSetPinValue = LOW, const bool &LoopFastOnce = DISABLE);
  

    /*
     * @brief stop output timer process immediately and continue to next phase, or restart timer process
     * @name object_name.set_outputTimer
     * @param ( STOP_TIMER | RESTART_TIMER );
     * @return none
     */
    void set_outputTimer (const bool &changeOutputTimer = STOP_TIMER, const bool &LoopFastOnce = DISABLE);
  

    /*
     * @brief stop output timer process immediately and continue to next phase, or restart timer process
     * @name object_name.set_beginTimer
     * @param ( STOP_TIMER | RESTART_TIMER );
     * @return none
     */
    void set_beginTimer (const bool &changeBeginTimer = STOP_TIMER, const bool &LoopFastOnce = DISABLE);
  

    /*
     * @brief stop begin timer process immediately and continue to next phase, or restart timer process
     * @name object_name.set_endTimer
     * @param ( STOP_TIMER | RESTART_TIMER );
     * @return none
     */
    void set_endTimer (const bool &changeEndTimer = STOP_TIMER, const bool &LoopFastOnce = DISABLE);


    /*
     * @brief Enable print phase state number and phase state text
     * @name object_name.set_debugPrint
     * @param ( {ENABLE | DISABLE} );
     * @return none
     */  
    void set_debugPrint (const bool &setEnableOrDisable = ENABLE, const bool &LoopFastOnce = DISABLE);
    
    
    // Getters -----------------------------------------------

    
    /*
     * @brief Get pinWrite status
     * @name object_name.get_pinWrite (); 
     * @param none
     * @return bool {ENABLE | DISABLE} );
     */  
    bool const &get_pinWrite () const;
  

    /*
     * @brief Get processState  
     * @name object_name.get_processState ();
     * @param none
     * @return bool {ACTIVE | finnish}
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
     * @brief Get begin start time
     * @name object_name.get_setBeginMS ();
     * @param none
     * @return uint32_t Milliseconds 
     */
    uint32_t const &get_setBeginMS () const;


    /*
     * @brief Get output start time
     * @name object_name.get_setOutputMS ();
     * @param none
     * @return uint32_t Milliseconds
     */
    uint32_t const &get_setOutputMS () const;


    /*
     * @brief  Get end start time 
     * @name object_name.get_setEndMS ();
     * @param none
     * @return uint32_t Milliseconds
     */
    uint32_t const &get_setEndMS () const;


    /*
     * @brief  Get and clear error number 
     * @name object_name.get_reset_error ();
     * @param none
     * @return uint8_t Error number. Error {1-127} and Warning {128-255}
     */  
    uint8_t const &get_reset_error ();


  private: // Functions
    void    activate_process    ();
    uint8_t check_pin_value     (const uint8_t &checkPinValue);
    void    write_pin_value     (const uint8_t &setPinValue);
    void    loop_fast_begin     ();
    void    loop_fast_out_begin ();
    void    loop_fast_out_end   ();
    void    loop_fast_end       ();
    void    loop_fast_complete  ();
    
}; // class MTD2A_binary_output 


#endif
