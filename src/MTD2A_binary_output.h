/**
 ******************************************************************************
 * @file    MTD2A_binary_output.h
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
 * *
 * Do not call MTD2A_binary_output from an interrupt handler (ISR)
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

 
#ifndef MTD2A_BINARY_OUTPUT_H_
#define MTD2A_BINARY_OUTPUT_H_

#include "MTD2A_base.h"


class MTD2A_binary_output: public MTD2A
{
  private:
    // Specific global constants from MTD2A_const.h
    static constexpr uint32_t MAX_TIME_MS     {MTD2A_const::MAX_TIME_MS};
    //
    static constexpr bool     P_W_M           {MTD2A_const::P_W_M};
    static constexpr bool     BINARY          {MTD2A_const::BINARY};
    // Timers
    static constexpr uint8_t  STOP_TIMER      {MTD2A_const::STOP_TIMER};
    static constexpr uint8_t  RESET_TIMER     {MTD2A_const::RESET_TIMER};
    // Phases 
    static constexpr uint8_t  RESET_PHASE     {MTD2A_const::RESET_PHASE}; 
    static constexpr uint8_t  BEGIN_PHASE     {MTD2A_const::BEGIN_PHASE};
    static constexpr uint8_t  OUTPUT_PHASE    {MTD2A_const::OUTPUT_PHASE}; 
    static constexpr uint8_t  END_PHASE       {MTD2A_const::END_PHASE};
    static constexpr uint8_t  COMPLETE_PHASE  {MTD2A_const::COMPLETE_PHASE};
    // PWM curves
    static constexpr uint8_t  NO_CURVE        {MTD2A_const::NO_CURVE};
    // Rising
    static constexpr uint8_t  RISING_XY       {MTD2A_const::RISING_XY};
    static constexpr uint8_t  RISING_B05      {MTD2A_const::RISING_B05};
    static constexpr uint8_t  RISING_B025     {MTD2A_const::RISING_B025};
    static constexpr uint8_t  RISING_E05      {MTD2A_const::RISING_E05};
    static constexpr uint8_t  RISING_E025     {MTD2A_const::RISING_E025};
    static constexpr uint8_t  RISING_SM8      {MTD2A_const::RISING_SM8};
    static constexpr uint8_t  RISING_SM5      {MTD2A_const::RISING_SM5};
    static constexpr uint8_t  RISING_BZ1      {MTD2A_const::RISING_BZ1};
    static constexpr uint8_t  RISING_BZ2      {MTD2A_const::RISING_BZ2};
    static constexpr uint8_t  RISING_SIN0     {MTD2A_const::RISING_SIN0};
    static constexpr uint8_t  RISING_SIN270   {MTD2A_const::RISING_SIN270};
    static constexpr uint8_t  RISING_LED      {MTD2A_const::RISING_LED};
    // Falling
    static constexpr uint8_t  FALLING_XY      {MTD2A_const::FALLING_XY};
    static constexpr uint8_t  FALLING_B05     {MTD2A_const::FALLING_B05};
    static constexpr uint8_t  FALLING_B025    {MTD2A_const::FALLING_B025};
    static constexpr uint8_t  FALLING_E05     {MTD2A_const::FALLING_E05};
    static constexpr uint8_t  FALLING_E025    {MTD2A_const::FALLING_E025};
    static constexpr uint8_t  FALLING_SM8     {MTD2A_const::FALLING_SM8};
    static constexpr uint8_t  FALLING_SM5     {MTD2A_const::FALLING_SM5};
    static constexpr uint8_t  FALLING_BZ1     {MTD2A_const::FALLING_BZ1};
    static constexpr uint8_t  FALLING_BZ2     {MTD2A_const::FALLING_BZ2};
    static constexpr uint8_t  FALLING_SIN90   {MTD2A_const::FALLING_SIN90};
    static constexpr uint8_t  FALLING_SIN180  {MTD2A_const::FALLING_SIN180};
    static constexpr uint8_t  FALLING_LED     {MTD2A_const::FALLING_LED};
    // Base
    static constexpr uint8_t  MAX_PWM_CURVES  {MTD2A::MAX_PWM_CURVES};

    // Arguments
    char    *objectName       {nullptr};        // Constructor default argument (User defined name to display identification)
    uint32_t outputTimeUS     {0UL};            // Constructor default argument (Microseconds) 
    uint32_t beginDelayUS     {0UL};            // Constructor default argument (Microseconds)  
    uint32_t endDelayUS       {0UL};            // Constructor default argument (Microseconds)
    bool     outputMode       {BINARY};         // Constructor default argument (BINARY/P_W_M)
    uint8_t  beginValue       {HIGH};           // Constructor default argument BINARY {HIGH | LOW} / P_W_M {0-255} 
    uint8_t  endValue         {LOW};            // Constructor default argument BINARY {HIGH | LOW} / P_W_M {0-255}
    bool     loopActivate     {DISABLE};        // Constructor default argument {ENABLE | DISABLE}
        // pin and input setup
    uint8_t  pinNumber        {NO_PIN};         // initialize () default argument
    bool     pinWriteToggl    {DISABLE};        // set_pinWriteToggl ()
    bool     pinWriteMode     {NORMAL};         // initialize () and set_pinWriteMode () default argument / INVERTED
    uint8_t  pinStartValue    {LOW};            // initialize () default argument BINARY {HIGH | LOW} / P_W_M {0-255}
    uint8_t  pinWriteValue    {LOW};            // set_pinWriteValue () default argument BINARY {HIGH | LOW} / P_W_M {0-255}
    bool     processState     {COMPLETE};       // Total process state period get_processState (); { ACTIVE | COMPLETE }
    // Output value
    uint8_t  outputValue      {LOW};            // Current value to write to output. get_outputValue (); {HIGH | LOW} / P_W_M {0-255}
    bool     outputOptimize   {ENABLE};         // If the same value occurs several times in a row, only output the first time
    bool     outputState      {false};          // Data ready. Indicates when to call get_outputValue (); { true | false }
    bool     outputProcess    {COMPLETE};       // Output process state period get_outputProcess { ACTIVE | COMPLETE }
    // Timers
    uint32_t setOutputUS      {0UL};            // Microseconds (output start time)
    uint32_t setBeginUS       {0UL};            // Microseconds (begin start time)
    uint32_t setEndUS         {0UL};            // Microseconds (end start time)
    // Stop timer
    bool     stopOutputTimer  {DISABLE};        // stop output timer process when executing MTD2A_loop_execute ();
    bool     stopBeginTimer   {DISABLE};        // stop begin delay timer process when executing MTD2A_loop_execute ();
    bool     stopEndTimer     {DISABLE};        // stop end delay timer process when executing MTD2A_loop_execute ();
    // sync time control
    bool     resetOutputTimer {DISABLE};        // Reset outputTimeUS when executing MTD2A_loop_execute ();
    bool     resetBeginTimer  {DISABLE};        // Reset beginDelayUS when executing MTD2A_loop_execute ();
    bool     resetEndTimer    {DISABLE};        // Reset endDelayUS when executing MTD2A_loop_execute ();
    // Debug and error
    bool     errorPrint       {DISABLE};        // set_errorPrint () default argument ENABLE
    bool     debugPrint       {DISABLE};        // set_debugPrint () print debug & error. default argument ENABLE
    uint8_t  errorNumber      {0};              // get_reset_error () Error {1-127} and Warning {128-255}
    bool     initializedOnce  {false};          // Initialize () called (in Arduino void setup())
    bool     objInstantiated  {false};          // Object instantiated
    uint8_t  objectError      {0};              // Object instantiation error / warning   
    // state control
    bool     startPhase       {false};          // Start phase procedure execution
    bool     setStartPhase    {false};          // Activate start phase procedure execution
    bool     phaseChange      {false};          // true = change in timing state
    uint8_t  phaseNumber      {COMPLETE_PHASE}; // reset = 0, Begin delay = 1, Output = 2, End delay = 3, Complete = 4
    uint8_t  setPhaseNumber   {COMPLETE_PHASE}; // Set phase number to execute (loop_fast ();)
    // PWM curves
    uint32_t PWMloopCount     {0UL};            // PWM Step calculation
    uint8_t  PWMcurveType     {NO_CURVE};       // PWM curve selection   
    uint8_t  PWMoffsetPoint   {0};              // PWM start (rising) or end (falling) value
    uint8_t  PWMCurrPinValue  {0};              // Final curve calculated pin value    
    uint8_t  PWMlastPinValue  {0};              // Last to check for duplicates (don't write)
    float    PWMtimedPoint_F  {0.0F};           // Current calculated timed point {0-255}
    float    PWMscaleFactor_F {0.0F};           // PWM scaling factor {-1 - 1} Zero results in a straight line

    // PWM power curve calculations
    static constexpr float   MAX_BYTE_VALUE_F {MAX_BYTE_VALUE};
    static constexpr float   power05_F        {0.50F};
    static constexpr float   powFactor05_F    {15.96844F}; // {15.96843885};
    static constexpr float   power025_F       {0.25F};
    static constexpr float   powFactor025_F   {63.81241F}; // {63.81240830};
    // PWM sigmoid curve S-curve calculation
    static constexpr float   sigmoidConst5_F  {5.0F};
    static constexpr float   sigmoidConst8_F  {8.0F};
    static constexpr float   expoAlign5_F     {1.0F/78.0F};
    static constexpr float   expoScale5_F     {sigmoidConst5_F * 2.0F / MAX_BYTE_VALUE_F};
    static constexpr float   expoScale8_F     {sigmoidConst8_F * 2.0F / MAX_BYTE_VALUE_F};
    static constexpr float   halfPI_F         {1.570796F};
    static constexpr float   bezierConst1_F   {0.667F};
    static constexpr float   bezierConst2_F   {6.75F};
    static constexpr float   LEDfactor_F      {31.87500F};  // {(MAX_BYTE_VALUE * log10(2)) / log10(MAX_BYTE_VALUE_F + 1)};

  public:
    // Constructor initializers
    
    // The default / no-args constructor is implicit in the constructor below. Do not use MTD2A_binary_output ();
    
    /**
     * @class MTD2A_binary_output
     * @brief Create object and set configuration parameters or use defaults
     * @param ( "Object Name", setOutputTimeMS, setBeginDelayMS, setEndDelayMS,
     * @param setOutputMode {BINARY | P_W_M}, setBeginValue, setEndValue, setLoopActivate {ENABLE | DISABLE});
     * @param Timers {0 - MAX_TIME_MS} and Values {0 - 255}
     * @return none
     */
    MTD2A_binary_output (
      const char *setObjectName   = "Object name",
      uint32_t    setOutputTimeMS = 0,
      uint32_t    setBeginDelayMS = 0,
      uint32_t    setEndDelayMS   = 0,
      bool        setOutputMode   = BINARY,
      uint8_t     setBeginValue   = HIGH, 
      uint8_t     setEndValue     = LOW,
      bool        setLoopActivate = DISABLE
    );


    // Destructor
    ~MTD2A_binary_output () { 
      if (objectName != nullptr) {
        delete [] objectName; 
        objectName = nullptr;
      }
    }


    // Operator overloading
    explicit operator bool() const {
      return (phaseChange == true  &&  phaseNumber == COMPLETE_PHASE);
    }
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


  public: // Functions

  
    /**
     * @brief Initialize and configure output pin.
     * @brief If setPinNumber is invalid, pin writing is disabled! (pinWriteToggl = DISABLE)
     * @brief If P_W_M is selected and pin does not support PWM, pin writing is disabled! (pinWriteToggl = DISABLE)
     * @name object_name.initialize 
     * @param ( {0 - NUM_DIGITAL_PINS | NO_PIN}, {NORMAL| INVERTED}, setPinStartValue );
     * @return none
     */
    void initialize (uint8_t setPinNumber = NO_PIN, bool setPinNormalOrInverted = NORMAL, uint8_t setPinStartValue = LOW);
  

    /**
     * @brief Activate process. {HIGH | LOW} / P_W_M {0 - 255} PWM curve selection activates PWM mode.
     * @name object_name.activate - overloaded
     * @param ( );
     * @param ( setBeginValue );
     * @param ( setBeginValue, setEndValue );
     * @param ( setBeginValue, setEndValue, setPWMcurveType {NO_CURVE / Curve number});
     * @param ( setBeginValue, setEndValue, setPWMcurveType, setOutputTimeMS {0 - MAX_TIME_MS} Milliseconds);
     * @return none
     */  
    void activate ();
    void activate (uint8_t setBeginValue);
    void activate (uint8_t setBeginValue, uint8_t setEndValue);
    void activate (uint8_t setBeginValue, uint8_t setEndValue, uint8_t setPWMcurveType);
    void activate (uint8_t setBeginValue, uint8_t setEndValue, uint8_t setPWMcurveType, uint32_t setOutputTimeMS);
    // Timers


    /**
     * @brief Reset all timers and state logic. Ready to start.
     * @name object_name.reset ();
     * @param none
     * @return none
     */ 
    void reset ();
  

    /**
     * @brief print configuration parameters, timers and state logic.
     * @name object_name.print_conf ();
     * @param none
     * @return none
     */  
    void print_conf ();
  

    // Setters -----------------------------------------------


    /**
     * @brief Select to write binary or PWM value to pin.
     * @name object_name.set_outputMode
     * @param ( {BINARY | P_W_M} );
     * @return none
     */  
    void set_outputMode (bool setBinaryOrPWM = BINARY);

    /**
     * @brief Select PMW curve type to write binary or PWM value to pin.
     * @name object_name.set_PWMcurveType
     * @param ( {CURVE NO | NO_CURVE} );
     * @return none
     */
    void set_PWMcurveType (uint8_t setPWMcurveType = NO_CURVE);


    /**
     * @brief write binary or PWM value directly to pin.
     * @name object_name.set_pinWriteValue
     * @param (BINARY {HIGH | LOW} / P_W_M {0-255} );
     * @param (BINARY {HIGH | LOW} / P_W_M {0-255}, {BINARY | P_W_M} );
     * @return none
     */  
    void set_pinWriteValue (uint8_t setPinWriteValue = LOW);
    void set_pinWriteValue (uint8_t setPinWriteValue, bool setPinBinaryOrPWM);

  
    /**
     * @brief Enable or disable write to pin (BINARY and analog P_W_M).
     * @name object_name.set_pinWriteToggl
     * @param ( {ENABLE | DISABLE} );
     * @return none
     */ 
    void set_pinWriteToggl (bool setPinEnableOrDisable = ENABLE);
  

    /**
     * @brief Configure pin write mode = NORMAL or INVERTED
     * @name object_name.set_pinWriteMode
     * @param ( {NORMAL | INVERTED} );
     * @return none
     */
    void set_pinWriteMode (bool setPinNormalOrInverted = NORMAL);

    
    /**
     * @brief Set new output delay time in milliseconds.
     * @name object_name.set_outputTimeMS
     * @param ( {0 - MAX_TIME_MS} );
     * @return none
     */  
    void set_outputTimeMS (uint32_t setOutputTimeMS);


    /**
     * @brief Set new begin delay time in milliseconds.
     * @name object_name.set_beginDelayMS
     * @param ( {0 - MAX_TIME_MS} );
     * @return none
     */  
    void set_beginDelayMS (uint32_t setBeginDelayMS);

    
    /**
     * @brief Set new end delay time in milliseconds.
     * @name object_name.set_endDelayMS
     * @param ( {0 - MAX_TIME_MS} );
     * @return none
     */  
    void set_endDelayMS (uint32_t setEndDelayMS);

    
    /**
     * @brief set one, two or three timers in milliseconds {0 - MAX_TIME_MS}
     * @name object_name.set_timers - overloaded
     * @param ( setOutputTimeMS );
     * @param ( setOutputTimeMS, setBeginDelayMS );
     * @param ( setOutputTimeMS, setBeginDelayMS, setEndDelayMS );
     * @return none
     */
    void set_timers (uint32_t setOutputTimeMS);
    void set_timers (uint32_t setOutputTimeMS, uint32_t setBeginDelayMS);
    void set_timers (uint32_t setOutputTimeMS, uint32_t setBeginDelayMS, uint32_t setEndDelayMS);


    /**
     * @brief stop output timer process immediately and continue to next phase, or restart timer process.
     * @name object_name.set_outputTimer
     * @param ( STOP_TIMER | RESET_TIMER );
     * @return none
     */
    void set_outputTimer (uint8_t setStopOrReset = STOP_TIMER);
  

    /**
     * @brief stop begin timer process immediately and continue to next phase, or restart timer process.
     * @name object_name.set_beginTimer
     * @param ( STOP_TIMER | RESET_TIMER );
     * @return none
     */
    void set_beginTimer (uint8_t setStopOrReset = STOP_TIMER);
  

    /**
     * @brief stop end timer process immediately and continue to next phase, or restart timer process.
     * @name object_name.set_endTimer
     * @param ( STOP_TIMER | RESET_TIMER );
     * @return none
     */
    void set_endTimer (uint8_t setStopOrReset = STOP_TIMER);
  

    /**
     * @brief Repeats a process started with activate(); enabling it does not start the process.
     * @name object_name.set_loopActivate
     * @param ( ENABLE | DISABLE );
     * @return none
     */    
    void set_loopActivate (bool setEnableOrDisable = ENABLE); 


    /**
     * @brief If the same value occurs several times in a row, only output the first time
     * @name object_name.set_outputOptimize
     * @param ( ENABLE | DISABLE );
     * @return none
     */    
    void set_outputOptimize (bool setEnableOrDisable = ENABLE);


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
     * @brief Get pinWriteToggl status
     * @name object_name.get_pinWriteToggl (); 
     * @param none
     * @return bool {ENABLE | DISABLE} );
     */  
    bool get_pinWriteToggl () const;


    /**
     * @brief Get pinWriteMode
     * @name object_name.get_pinWriteMode (); 
     * @param none
     * @return ( {NORMAL | INVERTED} );
     */
    bool get_pinWriteMode () const;


    /**
     * @brief Get outputMode
     * @name object_name.get_outputMode
     * @param none
     * @return ( {BINARY | P_W_M} );
     */  
    bool get_outputMode () const;

    
    /**
     * @brief Get PMW curve type number
     * @name object_name.get_PWMcurveType
     * @param ( {CURVE NO | NO_CURVE} );
     * @return none
     */
    uint8_t get_PWMcurveType () const;

  
    /**
     * @brief Get outputValue value
     * @name object_name.get_outputValue (); 
     * @param none
     * @return uint8_t {0 - 255);
     */      
    uint8_t get_outputValue () const;


    /**
     * @brief Get outputState indicates when to call get_outputValue ()
     * @name object_name.get_outputState (); 
     * @param none
     * @return bool {true | false}
     */      
    bool get_outputState () const;


    /**
     * @brief Get outputProcess
     * @name object_name.get_outputProcess ();
     * @param none
     * @return bool {ACTIVE | COMPLETE}
     */  
    bool get_outputProcess () const;


    /**
     * @brief get output value optimize setting (all values or only changed values) 
     * @name object_name.get_outputOptimize
     * @param none
     * @return bool {ENABLE | DISABLE} );
     */    
    bool get_outputOptimize () const;


    /**
     * @brief get repeating activation setting 
     * @name object_name.get_loopActivate
     * @param none
     * @return bool {ENABLE | DISABLE} );
     */    
    bool get_loopActivate () const;


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
     * @return uint8_t Reset = 0, Begin delay = 1, Output = 2, End delay = 3, Complete = 4
     */  
    uint8_t get_phaseNumber () const;
  

    /**
     * @brief Get begin start time
     * @name object_name.get_setBeginMS ();
     * @param none
     * @return uint32_t Milliseconds 
     */
    uint32_t get_setBeginMS () const;


    /**
     * @brief Get output start time
     * @name object_name.get_setOutputMS ();
     * @param none
     * @return uint32_t Milliseconds
     */
    uint32_t get_setOutputMS () const;


    /**
     * @brief  Get end start time 
     * @name object_name.get_setEndMS ();
     * @param none
     * @return uint32_t Milliseconds
     */
    uint32_t get_setEndMS () const;


    /**
     * @brief Get and reset error number to zero 
     * @name object_name.get_reset_error ();
     * @param none
     * @return uint8_t Error number. Error {1-127} and Warning {128-255}
     */
    uint8_t get_reset_error ();

    // -- Backward compatibility aliases - will be removed over timer -------------------------------------------
    uint8_t get_pinOutputValue     () const { return get_outputValue (); }
    bool    get_pinOutputMode      () const { return get_outputMode  (); }
    void    set_pinOutputMode      (bool setBinaryOrPWM = BINARY) { set_outputMode (setBinaryOrPWM); }


  private: // Internal functions

    void     loop_fast             ();
    uint32_t check_set_MS_to_US    (uint32_t setCheckTimeMS);
    void     check_pin_init        (uint8_t  checkPinNumber, uint8_t checkpinStartValue);
    void     activate_check        ();
    void     activate_process      (bool userActivate = true);
    bool     check_PWM_pin_mode    ();
    uint8_t  check_pin_value       (uint8_t  checkPinValue);
    void     write_pin_value       (uint8_t  writePinValue);
    uint8_t  check_PWM_curve       (uint8_t  checkPWMcurveType);
    void     PWM_curve_begin_end   ();
    uint8_t  PWM_scale_point       (const float    &curvePointY);
    float    PWM_sigmoid_5         (const float    &curvePointX5_F);
    float    PWM_sigmoid_8         (const float    &curvePointX8_F);
    float    PWM_rising_BZ1        (const float    &curvePointX1_F);
    float    PWM_rising_BZ2        (const float    &curvePointX1_F);
    float    PWM_falling_BZ1       (const float    &curvePointX2_F);
    float    PWM_falling_BZ2       (const float    &curvePointX2_F);
    uint8_t  PWM_curve_point       (const float    &curvePointX_F, uint8_t curveType);
    void     PWM_curve_step        ();
    void     loop_fast_begin_start ();
    void     loop_fast_begin_timer ();
    void     loop_fast_out_start   ();
    void     loop_fast_out_timer   ();
    void     loop_fast_end_start   ();
    void     loop_fast_end_timer   ();
    void     loop_fast_complete    ();
    void     print_error_text      (uint8_t setErrorNumber);
    void     print_phase_text      ();
    void     print_phase_line      (uint8_t printRestartTimer = MAX_BYTE_VALUE); // Dummy value

}; // class MTD2A_binary_output 

#endif
