/*
 ******************************************************************************
 * @file    MTD2A_base.cpp
 * @author  Joergen Bo Madsen
 * @version V1.1.3
 * @date    25. maj 2025
 * @brief   functions for MTD2A_base.h base class (Model Train Detection And Action)
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

#include "Arduino.h"
#include "MTD2A_base.h"


// MTD2A initializers
bool MTD2A::allDebugPrint {disable}; 
// Funtion pointers linket list
MTD2A* MTD2A::begin {nullptr};
MTD2A* MTD2A::end   {nullptr};

/*
 * @brief Enable print phase state number and phase state text for all instantiated classes
 * @name object_name.set_allDebugPrint
 * @param ( {enable | disable} );
 * @return none
 */
void MTD2A::set_allDebugPrint (const bool &setEnableOrDisable = enable) {
  allDebugPrint = setEnableOrDisable;
}


// Function pointer linket list -----------------------------------------------------------------------------------
void MTD2A::MTD2A_add_funtion_pointer_loop_fast (MTD2A* object) {
  if (begin == nullptr)
    begin = object;
  if (end != nullptr)
    end->next = object;
  end = object;
}


void ::MTD2A::loop_execute() {
  MTD2A* object = begin;
  while (object != nullptr) {
    object->function_pointer(object);
    object = object->next;
  }
  delay(MTD2AdelayTime);
}
// Function pointer linket list -----------------------------------------------------------------------------------


char *MTD2A::MTD2A_set_object_name (const char *setObjectName) {
  char *newObjectName;
  if (setObjectName == nullptr) {
    newObjectName = new char[1];
    *newObjectName = '\0';
  } 
  else {
    newObjectName = new char[strlen(setObjectName) + 1];
      strcpy(newObjectName, setObjectName);
  }
  return newObjectName;
} // MTD2A_set_object_name


uint8_t MTD2A::MTD2A_reserve_and_check_pin (const uint8_t &checkPinNumber, const uint8_t &checkPinFlags, const bool &checkDebugPrint) {
  static uint8_t pinFlags[NUM_DIGITAL_PINS] {0};
  uint8_t checkErrorNumber {0};
  // errorNumber {1-127} Error {128-255} Warning
  // github/arduino pins_arduino.h
  // bit 0 [1]  : digital
  // bit 1 [2]  : analog
  // bit 2 [4]  : input
  // bit 3 [8]  : input_pullup
  // bit 4 [16] : output
  // bit 5 [32] : PWM (digital)
  // bit 6 [64] : tone()
  // bit 7 [128]: Interrupt
  if (checkPinNumber >= NUM_DIGITAL_PINS  &&  checkPinNumber != 255) {
    if (bitRead(checkPinFlags, 0)) 
      checkErrorNumber = 2;
    if (bitRead(checkPinFlags, 1)  &&  checkPinNumber < (NUM_DIGITAL_PINS - NUM_ANALOG_INPUTS))
        checkErrorNumber = 3;
  } 
  else {
    if (bitRead(pinFlags[checkPinNumber], 2)  &&  bitRead(checkPinFlags, 2))
      checkErrorNumber = 128;
    if (bitRead(pinFlags[checkPinNumber], 4)  &&  bitRead(checkPinFlags, 4))
      checkErrorNumber = 4;
  }
  // Use of the tone() function will interfere with PWM output on pins 3 and 11 (not MEGA)    
  if (bitRead(checkPinFlags, 5)) {
    if (bitRead(checkPinFlags, 6) && (checkPinNumber == 3  ||  checkPinNumber == 11)) // tone()
      checkErrorNumber = 5;
    else
      if (!digitalPinHasPWM(checkPinNumber))
        checkErrorNumber = 6;
  }
  if (bitRead(checkPinFlags, 7)  &&  !digitalPinToInterrupt(checkPinNumber))
    checkErrorNumber = 5;
  if (checkErrorNumber == 0) 
    pinFlags[checkPinNumber] = checkPinFlags;
  else
    if (checkDebugPrint == enable || MTD2A::allDebugPrint == enable)
      MTD2A_print_pin_error (checkErrorNumber, checkPinNumber);
  return checkErrorNumber;
} // MTD2A_reserve_and_check_pin


void MTD2A::MTD2A_print_phase_line (const bool &printDebugPrint, const char *printObjectName, const char *printPhaseText) {
  if (printDebugPrint == enable  ||  MTD2A::allDebugPrint == enable) {
    Serial.print(millis()); Serial.print(F("\t ")); Serial.print(printObjectName); 
    Serial.print(F(": ")); Serial.println(printPhaseText);
  }
} // MTD2A_print_phase_line 


void MTD2A::MTD2A_print_pin_error (const uint8_t &printErrorNumber, const uint8_t &printPinNumber ) {
  Serial.print (F("MTD2A_reserve_and_check_pin: "));
  if (printErrorNumber < 128) 
    Serial.print (F("ERROR")); 
  else 
    Serial.print (F("Warning"));
  Serial.print (F(" ["));  Serial.print (printErrorNumber); Serial.print (F("]: "));
  //
  switch (printErrorNumber) {
    case   2: Serial.println (F("Digital pin out of range"));       break;
    case   3: Serial.println (F("Analog pin out of range"));        break;
    case   4: Serial.println (F("Output pin already in use"));      break;
    case   5: Serial.println (F("tone() occupies PWM pin"));        break;
    case   6: Serial.println (F("Pin does not support PWM"));       break;
    case   7: Serial.println (F("Pin does not support interrupt")); break;
    case 128: Serial.println (F("Pin used more than once"));        break;
    default:
      Serial.println(F("Unknown error"));
  }
} // MTD2A_print_pin_error


void MTD2A::MTD2A_print_error_text (const uint8_t &errorNumber) {
  if (errorNumber < 128) 
    Serial.print (F("ERROR")); 
  else 
    Serial.print (F("Warning"));
  Serial.print (F(" ["));  Serial.print (errorNumber); Serial.print (F("]: "));
  switch (errorNumber) {
    case   1: Serial.println (F("")); break;
    case   2: Serial.println (F("")); break;
    case   3: Serial.println (F("")); break;
    case   4: Serial.println (F("")); break;
    case   5: Serial.println (F("")); break;
    case   6: Serial.println (F("")); break;
    case   7: Serial.println (F("")); break;
    case   8: Serial.println (F("Must INPUT or INPUT_PULLUP"));   break;
    case   9: Serial.println (F("Pin number not set (255)"));     break;
    case  10: Serial.println (F("")); break;
    case  11: Serial.println (F("Pin write is disabled"));        break;
    case  12: Serial.println (F("")); break;
    case 130: Serial.println (F("Timer value is zero"));          break;
    case 140: Serial.println (F("Begin and end value is zero"));  break;
    case 141: Serial.println (F("All three timers are zero"));    break;
    default:
      Serial.println(F("Unknown error"));
  }
} // MTD2A_print_error_text


void MTD2A::MTD2A_print_generic_info (const char *printObjectName, const bool &printProcessState, const char *printPhaseText) {
  for (size_t i {1}; i < 20; i++) Serial.print(F("-")); Serial.println();
  Serial.print  (F("  objectName   : ")); Serial.println(printObjectName);
  Serial.print  (F("  processState : ")); if (printProcessState == active) Serial.println(F("Active")); else Serial.println(F("Pending"));
  Serial.print  (F("  phaseText    : ")); Serial.println(printPhaseText);
}; // MTD2A_print_phase_info


void MTD2A::MTD2A_print_debug_error (const uint8_t &printErrorNumber, const bool &printDebugPrint) {
  Serial.print  (F("  debugPrint   : ")); if (printDebugPrint == enable) Serial.println(F("Enable"));     else Serial.println(F("Disable"));
  Serial.print  (F("  errorNumber  : "));
  if (printErrorNumber == 0) {
     Serial.print(printErrorNumber); Serial.println (F(" OK")); 
  }
  else
    MTD2A_print_error_text (printErrorNumber);
}; // MTD2A_print_debug_error 


void MTD2A::MTD2A_print_pin_number (const uint8_t &printPinNumber) {
  Serial.print  (F("  pinNumber    : ")); Serial.print(printPinNumber); 
  if (printPinNumber == 255) 
    Serial.println(F(" (NOT defined)")); 
  else 
    Serial.println();
}
// MTD2A_print_pin_number


void MTD2A::MTD2A_print_value_binary (const bool &binaryOrPWM, const uint8_t &PrintValue) {
  if (binaryOrPWM == PWM)
    Serial.println(PrintValue);
  else {
    if (PrintValue == HIGH) 
      Serial.println(F("High"));
    else 
      Serial.println(F("Low"));
  }
} // MTD2A_print_value_binary


void MTD2A::MTD2A_print_enable_disable (const bool &enableOrDisable) {
  if (enableOrDisable == enable)
    Serial.println(F("Enable"));
  else
    Serial.println(F("Disable"));
} // MTD2A_print_enable_disable


void MTD2A::MTD2A_print_pulse_fixed (const bool &pulseOrFixed) {
  if (pulseOrFixed == pulse)
    Serial.println(F("Pulse"));
  else
    Serial.println(F("Fixed"));
} // MTD2A_print_puls_fixed

