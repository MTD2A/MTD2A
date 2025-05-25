/*
 ******************************************************************************
 * @file    MTD2A_binary_output.h
 * @author  Joergen Bo Madsen
 * @version V1.1.2
 * @date    25. maj 2025
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
     * @name MTD2A_binary_output
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
     * @param ( {0 - NUM_DIGITAL_PINS | 255}, binary {HIGH | LOW} / PWM {0-255} );
     * @return none
     */
    void initialize (const uint8_t &setPinNumber = 255, const uint8_t &setStartPinValue = LOW);
  

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
    uint8_t check_pin_value     (const uint8_t &checkPinValue);
    void    write_pin_value     (const uint8_t &setPinValue);
    void    loop_fast_begin     ();
    void    loop_fast_out_begin ();
    void    loop_fast_out_end   ();
    void    loop_fast_end       ();
    void    loop_fast_complete  ();
    
}; // class MTD2A_binary_output 


#endif
