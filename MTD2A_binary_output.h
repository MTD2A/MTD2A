/*
 ******************************************************************************
 * @file    MTD2A_binary_output.h
 * @author  Joergen Bo Madsen
 * @version V1.1.5
 * @date    7. july 2025
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
  private:
    // Specific global constants from MTD2A_binary_output.h (MTD2A_const.h)
    static constexpr bool    P_W_M          {MTD2A_const::P_W_M};
    static constexpr bool    BINARY         {MTD2A_const::BINARY};
    static constexpr bool    STOP_TIMER     {MTD2A_const::STOP_TIMER};
    // Phases 
    static constexpr uint8_t RESET_PHASE    {MTD2A_const::RESET_PHASE}; 
    static constexpr uint8_t BEGIN_PHASE    {MTD2A_const::BEGIN_PHASE};
    static constexpr uint8_t OUTPUT_PHASE   {MTD2A_const::OUTPUT_PHASE}; 
    static constexpr uint8_t END_PHASE      {MTD2A_const::END_PHASE};
    static constexpr uint8_t COMPLETE_PHASE {MTD2A_const::COMPLETE_PHASE};
    // PWM curves
    static constexpr uint8_t NO_CURVE       {MTD2A_const::NO_CURVE};
    // Rising
    static constexpr uint8_t RISING_XY      {MTD2A_const::RISING_XY};
    static constexpr uint8_t RISING_B05     {MTD2A_const::RISING_B05};
    static constexpr uint8_t RISING_B025    {MTD2A_const::RISING_B025};
    static constexpr uint8_t RISING_E05     {MTD2A_const::RISING_E05};
    static constexpr uint8_t RISING_E025    {MTD2A_const::RISING_E025};
    static constexpr uint8_t RISING_SM8     {MTD2A_const::RISING_SM8};
    static constexpr uint8_t RISING_SM5     {MTD2A_const::RISING_SM5};
    static constexpr uint8_t RISING_LED     {MTD2A_const::RISING_LED};
    // Falling
    static constexpr uint8_t FALLING_XY     {MTD2A_const::FALLING_XY};
    static constexpr uint8_t FALLING_B05    {MTD2A_const::FALLING_B05};
    static constexpr uint8_t FALLING_B025   {MTD2A_const::FALLING_B025};
    static constexpr uint8_t FALLING_E05    {MTD2A_const::FALLING_E05};
    static constexpr uint8_t FALLING_E025   {MTD2A_const::FALLING_E025};
    static constexpr uint8_t FALLING_SM8    {MTD2A_const::FALLING_SM8};
    static constexpr uint8_t FALLING_SM5    {MTD2A_const::FALLING_SM5};
    static constexpr uint8_t FALLING_LED    {MTD2A_const::FALLING_LED};
    // Base
    static constexpr uint8_t MAX_BYTE_VALUE {MTD2A::MAX_BYTE_VALUE};
    static constexpr uint8_t PIN_ERROR_NO   {MTD2A::PIN_ERROR_NO};
    static constexpr uint8_t MAX_PWM_CURVES {MTD2A::MAX_PWM_CURVES};

    // Arguments
    char    *objectName     {nullptr};      // Constructor default argument (User defined name to display identification)
    uint32_t outputTimeMS   {0};            // Constructor default argument (Milliseconds) 
    uint32_t beginDelayMS   {0};            // Constructor default argument (Milliseconds)  
    uint32_t endDelayMS     {0};            // Constructor default argument (Milliseconds)
    bool     pinOutputMode  {BINARY};       // Constructor default argument (BINARY/P_W_M)
    uint8_t  pinBeginValue  {HIGH};         // Constructor default argument BINARY {HIGH | LOW} / P_W_M {0-255} 
    uint8_t  pinEndValue    {LOW};          // Constructor default argument BINARY {HIGH | LOW} / P_W_M {0-255}
    // pin and input setup
    uint8_t  pinNumber      {PIN_ERROR_NO}; // initialize () default argument
    uint8_t  pinOutputValue {LOW};          // Current value to write to output. get_pinOuputValue ()
    bool     pinWriteToggl  {ENABLE};       // set_pinWriteToggl ()
    bool     pinWriteMode   {NORMAL};       // initialize () and set_pinWriteMode () default argument / INVERTED
    uint8_t  pinStartValue  {LOW};          // initialize () default argument BINARY {HIGH | LOW} / P_W_M {0-255}
    uint8_t  setPinValue    {LOW};          // set_pinValue () default argument BINARY {HIGH | LOW} / P_W_M {0-255}
    bool     processState   {COMPLETE};     // process state / ACTIVE
    // Timers
    uint32_t setOutputMS    {0};            // Milliseconds (output start time)
    uint32_t setBeginMS     {0};            // Milliseconds (begin start time)
    uint32_t setEndMS       {0};            // Milliseconds (end start time)
    // Change timing
    bool     stopOutputTM   {DISABLE};      // stop output timer process
    bool     stopBeginTM    {DISABLE};      // stop begin delay timer proces
    bool     stopEndTM      {DISABLE};      // stop end delay timer proces
    // Other
    bool     errorPrint     {DISABLE};      // set_errorPrint () default argument ENABLE
    bool     debugPrint     {DISABLE};      // set_debugPrint () print debug & error. default argument ENABLE
    uint8_t  errorNumber    {0};            // get_reset_error () Error {1-127} and Warning {128-255}
    // state control
    bool     phaseChange    {false};        // true = change in timing state
    uint8_t  phaseNumber    {RESET_PHASE};  // Initialize and reset= 0, Begin delay = 1, Output = 2, End delay = 3, Complete = 4
    // PWM curves
    uint8_t  PWMcurveType   {NO_CURVE};     // PWM curve selection   
    uint8_t  PWMoffstePoint {0};            // PWM start (rising) or end (falling) value
    uint8_t  PWMcurrPoint   {0};            // Current calculatede point {0-255}
    uint8_t  PWMlastPoint   {MAX_BYTE_VALUE}; // Last to tjek for dublets (don't write)
    uint8_t  PWMpinValue    {0};            // Final curve calculated pin vallue    
    double   PWMscaleFactor {0.0};          // PWM scaling factor {-1 - 1} Zero results in a straight line
    double   PWMtimeStep    {0.0};          // PWM curve time stepping factor
    // PWM power curve calculations
    static constexpr double  powPotens05  {0.50};
    static constexpr double  powFactor05  {15.96843885};
    static constexpr double  powPotens025 {0.25};
    static constexpr double  powFactor025 {63.81240830};
    // PWM sigmoid curve S-curve calculation
    static constexpr double  sigmoid5     {5.0};
    static constexpr double  sigmoid8     {8.0};
    static constexpr double  expoAlign5   {1.0/78};
    static constexpr double  expoScale5   {sigmoid5 * 2.0 / MAX_BYTE_VALUE};
    static constexpr double  expoScale8   {sigmoid8 * 2.0 / MAX_BYTE_VALUE};
    static constexpr double  LEDfactor    {(MAX_BYTE_VALUE * log10(2)) / log10(MAX_BYTE_VALUE + 1)};

  public:
    // Constructor inittializers
    /*
     * @brief Create object and set configuration parameters or use defaults
     * @name MTD2A_BINARY_output
     * @param ( "Object Name", outputTimeMS, beginDelayMS, endDelayMS, {BINARY | P_W_M}, pinBeginValueMS, pinEndValue );
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
      if (globalObjectCount > 0 )
        globalObjectCount--;
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
     * @brief If P_W_M is selected and pin does not support PWM, pin writing is disabled!
     * @name object_name.initialize 
     * @param ( {0 - NUM_DIGITAL_PINS | 255}, {NORMAL| INVERTED}, BINARY {HIGH | LOW} / P_W_M {0-255} );
     * @return none
     */
    void initialize (const uint8_t &setPinNumber = PIN_ERROR_NO, const bool &setPinNomalOrInverted = NORMAL, const uint8_t &setPinStartValue = LOW);
  

    /*
     * @brief Activate process. PWM curve selection activates PWM mode. Optional loop update.
     * @name object_name.activate 
     * @param ( setPinBeginValue, setPinEndValue, setPWMcurveType );
     * @return none
     */  
    void activate ();
    void activate (const uint8_t &setPinBeginValue);
    void activate (const uint8_t &setPinBeginValue, const uint8_t &setPinEndValue);
    void activate (const uint8_t &setPinBeginValue, const uint8_t &setPinEndValue, const uint8_t &setPWMcurveType);
    void activate (const uint8_t &setPinBeginValue, const uint8_t &setPinEndValue, const uint8_t &setPWMcurveType, const bool &loopFastOnce);


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
     * @brief Enable or disable write to pin (BINARY and analog P_W_M).Optional loop update.
     * @name object_name.set_pinWriteToggl
     * @param ( {ENABLE | DISABLE} );
     * @return none
     */ 
    void set_pinWriteToggl (const bool &setPinEnableOrDisable = ENABLE, const bool &loopFastOnce = DISABLE);
  

    /*
     * @brief Configure pin write mode = NORMAL or INVERTED. Optional loop update.
     * @name object_name.set_pinWriteMode
     * @param ( {NORMAL | INVERTED} );
     * @return non
     */
    void set_pinWriteMode (const bool &setPinNomalOrInverted = NORMAL, const bool &loopFastOnce = DISABLE);


    /*
     * @brief write binary or PWM value directly to pin.Optional loop update.
     * @name object_name.set_setPinValue
     * @param (BINARY {HIGH | LOW} / P_W_M {0-255} );
     * @return none
     */  
    void set_setPinValue (const uint8_t &setSetPinValue = LOW, const bool &loopFastOnce = DISABLE);
  

    /*
     * @brief stop output timer process immediately and continue to next phase, or restart timer process. Optional loop update.
     * @name object_name.set_outputTimer
     * @param ( STOP_TIMER | RESTART_TIMER );
     * @return none
     */
    void set_outputTimer (const bool &changeOutputTimer = STOP_TIMER, const bool &loopFastOnce = DISABLE);
  

    /*
     * @brief stop begin timer process immediately and continue to next phase, or restart timer process. Optional loop update.
     * @name object_name.set_beginTimer
     * @param ( STOP_TIMER | RESTART_TIMER );
     * @return none
     */
    void set_beginTimer (const bool &changeBeginTimer = STOP_TIMER, const bool &loopFastOnce = DISABLE);
  

    /*
     * @brief stop end timer process immediately and continue to next phase, or restart timer process. Optional loop update.
     * @name object_name.set_endTimer
     * @param ( STOP_TIMER | RESTART_TIMER );
     * @return none
     */
    void set_endTimer (const bool &changeEndTimer = STOP_TIMER, const bool &loopFastOnce = DISABLE);
    

    /*
     * @brief Enable print phase state number, phase state text and error text. Optional loop update.
     * @name object_name.set_debugPrint
     * @param ( {ENABLE | DISABLE} );
     * @return none
     */  
    void set_debugPrint (const bool &setEnableOrDisable = ENABLE, const bool &loopFastOnce = DISABLE);
    
    
    /*
     * @brief Enable error print text. Optional loop update.
     * @name object_name.set_debugPrint
     * @param ( {ENABLE | DISABLE} );
     * @return none
     */  
    void set_errorPrint (const bool &setEnableOrDisable = ENABLE, const bool &loopFastOnce = DISABLE);
    
    
    // Getters -----------------------------------------------

    
    /*
     * @brief Get pinWriteToggl status
     * @name object_name.get_pinWriteToggl (); 
     * @param none
     * @return bool {ENABLE | DISABLE} );
     */  
    bool const &get_pinWriteToggl () const;
  
    /*
     * @brief Get pinOutputValue value
     * @name object_name.get_pinOutputValue (); 
     * @param none
     * @return uint8_t {0 - 255);
     */      
    uint8_t const &get_pinOutputValue () const;


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
    uint32_t check_set_time      (const uint32_t &setCheckTimeMS);
    void     check_pin_init      (const uint8_t  &checkPinNumber);
    void     activate_process    ();
    uint8_t  check_pin_value     (const uint8_t  &checkPinValue);
    void     write_pin_value     (const uint8_t  &writePinValue);
    uint8_t  check_PWM_curve     (const uint8_t  &checkPWMcurveType);
    void     PWM_curve_begin_end ();
    uint8_t  PWM_scale_point     (const double   &curvePointY);
    double   PWM_sigmoid_5       (const uint8_t  &curvePointX5);
    double   PWM_sigmoid_8       (const uint8_t  &curvePointX8);
    uint8_t  PWM_curve_point     (const uint8_t  &curvePointX, const uint8_t &curveType);
    void     PWM_curve_step      ();
    void     loop_fast_begin     ();
    void     loop_fast_out_begin ();
    void     loop_fast_out_end   ();
    void     loop_fast_end       ();
    void     loop_fast_complete  ();
    void     print_phase_text    ();
    void     print_phase_line    (const bool     &printRestartTimer = false);
}; // class MTD2A_binary_output 


#endif
