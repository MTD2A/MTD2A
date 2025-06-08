/*
 ******************************************************************************
 * @file    MTD2A_base.cpp
 * @author  Joergen Bo Madsen
 * @version V1.1.4
 * @date    30. maj 2025
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


#ifndef _MTD2A_base_CPP_
#define _MTD2A_base_CPP_


#include "Arduino.h"
#include "MTD2A_const.h"
#include "MTD2A_base.h"

// MTD2A static initializers (c++11 thus not inline)
bool     MTD2A::globalDebugPrint = DISABLE;
uint32_t MTD2A::delayTimeMS      = DELAY_10MS;
uint8_t  MTD2A::objectCount      = 0;
uint32_t MTD2A::currentTimeMS    = 0;
uint32_t MTD2A::lastTimeMS       = 0;
uint32_t MTD2A::execTimeMS       = 0;
uint32_t MTD2A::maxExecTimeMS    = 0;
// Funtion pointer linked list
MTD2A   *MTD2A::begin = nullptr;
MTD2A   *MTD2A::end   = nullptr;


void MTD2A::set_globalDebugPrint (const bool &setEnableOrDisable) {
  globalDebugPrint = setEnableOrDisable;
}


void MTD2A::set_delayTimeMS (const bool &setDelayTimeMS) {
  if (setDelayTimeMS == DELAY_10MS) 
    delayTimeMS = DELAY_10MS;
  else 
    delayTimeMS = DELAY_1MS;
}


// ========== Function pointer linked list of the function "loop_fast" instantiated object
void MTD2A::MTD2A_add_function_pointer_loop_fast (MTD2A* object) {
  if (begin == nullptr)
    begin = object;
  if (end != nullptr) {
    end->next = object;
  }
  end = object;
  objectCount++;
}


void MTD2A::loop_execute() {
  MTD2A* object = begin;
  while (object != nullptr) {
    object->function_pointer(object);
    object = object->next;
  }
  // Cadence precision correction
  currentTimeMS = millis();
  execTimeMS = currentTimeMS - lastTimeMS;
  if (execTimeMS > maxExecTimeMS)
    maxExecTimeMS = execTimeMS;
  if (delayTimeMS == DELAY_10MS) {
    if (execTimeMS > DELAY_10MS)
      ; // Serial.println(F("Warning: User coding executing delay is above threshold"));
    else 
      delay(DELAY_10MS - execTimeMS);
  }
  else
    delay(delayTimeMS);
  lastTimeMS = millis();
}
// ========== Function pointer linked list of the function "loop_fast" instantiated object


uint32_t MTD2A::get_maxExecTimeMS () {
   return maxExecTimeMS;
}


uint8_t MTD2A::get_objectCount () {
  return objectCount;
}


char *MTD2A::MTD2A_set_object_name (const char *setObjectName) {
  if (setObjectName == nullptr) {
    char *emptyObjectName = new char[1];
    emptyObjectName[0] = '\0';
    return emptyObjectName;
  } 
  char *newObjectName = new char[strlen(setObjectName) + 1];
  strcpy(newObjectName, setObjectName);
  return newObjectName;
} // MTD2A_set_object_name


uint8_t MTD2A::MTD2A_reserve_and_check_pin (const uint8_t &checkPinNumber, const uint8_t &checkPinFlags) {
  static uint8_t pinFlags[NUM_DIGITAL_PINS] = {0};
  uint8_t checkErrorNumber = 0;
  // errorNumber {1-127} Error {128-255} Warning
  if (checkPinNumber == PIN_ERROR_NO) {
    checkErrorNumber = 1;
    return checkErrorNumber;
  }
  // github/arduino pins_arduino.h
  // bit 0 [1]  : digital
  // bit 1 [2]  : analog
  // bit 2 [4]  : input
  // bit 3 [8]  : input_pullup
  // bit 4 [16] : output
  // bit 5 [32] : PWM (digital)
  // bit 6 [64] : tone()
  // bit 7 [128]: Interrupt

  // https://github.com/arduino/ArduinoCore-avr/blob/master/variants/standard/pins_arduino.h
  if ((checkPinFlags & DIGITAL_FLAG_0)  &&  (checkPinNumber >= NUM_DIGITAL_PINS)) {
    checkErrorNumber = 2;
    return checkErrorNumber;
  }
  if ((checkPinFlags & ANALOG_FLAG_1)  &&  (analogInputToDigitalPin(NUM_DIGITAL_PINS - NUM_ANALOG_INPUTS - checkPinNumber))) {
    checkErrorNumber = 3;
    return checkErrorNumber;
  } 
  // Double pin binding
  if ((pinFlags[checkPinNumber] & INPUT_FLAG_2)  &&  (checkPinFlags & INPUT_FLAG_2)) {
    checkErrorNumber = 128;  // Warning, but continue processing
  }
  if ((pinFlags[checkPinNumber] & OUTPUT_FLAG_4)  &&  (checkPinFlags & OUTPUT_FLAG_4)) {
    checkErrorNumber = 4;
    return checkErrorNumber;
  }

  // Use of the tone() function will interfere with PWM output on pins 3 and 11 on NANO and 0 and 10 on MEGA.
  if (checkPinFlags & PWM_FLAG_5) {
    if (!digitalPinHasPWM(checkPinNumber)) {
      checkErrorNumber = 5;
      return checkErrorNumber;
    }
    #if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega168__) // Arduino Uno/Nano    
      if ((checkPinFlags & TONE_FLAG_6) && (checkPinNumber == 3  ||  checkPinNumber == 11)) {  // tone()
        checkErrorNumber = 6;
        return checkErrorNumber;
      }
    #elif defined(__AVR_ATmega2560__) // Arduino Mega
      if ((checkPinFlags & TONE_FLAG_6) && (checkPinNumber == 9  ||  checkPinNumber == 10)) {  // tone()
        checkErrorNumber = 6;
        return checkErrorNumber;
      }
    #endif
  } // checkPinFlags & PWM_FLAG_5

  #ifndef NOT_AN_INTERRUPT // Interrupt capability check
  #define NOT_AN_INTERRUPT MAX_BYTE_VALUE; // Fallback for older Arduino versions
  #endif

  if ((checkPinFlags & INTERRUPT_FLAG_7)  &&  (digitalPinToInterrupt(checkPinNumber)  == NOT_AN_INTERRUPT)) {
    checkErrorNumber = 7;
    return checkErrorNumber;
  }

  if (checkErrorNumber == 0  ||  checkErrorNumber >= WARNING_START) 
    pinFlags[checkPinNumber] |= checkPinFlags;
  return checkErrorNumber;
} // MTD2A_reserve_and_check_pin


void MTD2A::MTD2A_print_phase_line (const bool &checkDebugPrint, const char *printObjectName, const char *printPhaseText, const bool &printRestartTimer) {
  if (checkDebugPrint == ENABLE  ||  globalDebugPrint == ENABLE) {
    Serial.print(millis()); Serial.print(F("\t ")); 
    if (printObjectName != nullptr)
      Serial.print(printObjectName);
    else
      Serial.print("Unnamed");
    if (printRestartTimer == RESTART_TIMER)
      Serial.print(": Restart timer ");
    Serial.print(F(": ")); Serial.println(printPhaseText);
  }
} // MTD2A_print_phase_line 


void MTD2A::MTD2A_print_error_text (const bool &checkDebugPrint, const uint8_t &printErrorNumber, const uint8_t &printPinNumber) {
  if (checkDebugPrint == ENABLE || globalDebugPrint == ENABLE) {
    if (printErrorNumber < WARNING_START) 
      Serial.print (F("ERROR")); 
    else 
      Serial.print (F("Warning"));
    Serial.print (F(" ["));     Serial.print (printErrorNumber); Serial.print (F("]: "));
    Serial.print (F(" Pin: ")); Serial.print (printPinNumber);   Serial.print (F(" - "));
    switch (printErrorNumber) {
      case   1: Serial.println (F("Pin not defined"));                 break;
      case   2: Serial.println (F("Digital pin number out of range")); break;
      case   3: Serial.println (F("Analog pin number out of range"));  break;
      case   4: Serial.println (F("Output pin already in use"));       break;
      case   5: Serial.println (F("Pin does not support PWM"));        break;
      case   6: Serial.println (F("tone() conflicts with PWM pin"));   break;
      case   7: Serial.println (F("Pin does not support interrupt"));  break;    
      case   8: Serial.println (F("Must be INPUT or INPUT_PULLUP"));   break;
      case   9: Serial.println (F("Pin number not set (255)"));        break;
      case  10: Serial.println (F("")); break;
      case  11: Serial.println (F("Pin write is disabled"));           break;
      case  12: Serial.println (F("")); break;
      case 128: Serial.println (F("Pin used more than once"));         break;
      case 130: Serial.println (F("Timer value is zero"));             break;
      case 140: Serial.println (F("Output value is zero"));            break;
      case 141: Serial.println (F("All three timers are zero"));       break;
      default:
        Serial.println(F("Unknown error"));
    }
  }
} // MTD2A_print_error_text


void MTD2A::MTD2A_print_generic_info (const char *printObjectName, const bool &printProcessState, const char *printPhaseText) {
  for (size_t i {1}; i < 20; i++) Serial.print(F("-")); Serial.println();
  Serial.print  (F("  objectName   : ")); Serial.println(printObjectName);
  Serial.print  (F("  processState : ")); if (printProcessState == ACTIVE) Serial.println(F("ACTIVE")); else Serial.println(F("COMPLETE"));
  Serial.print  (F("  phaseText    : ")); Serial.println(printPhaseText);
}; // MTD2A_print_phase_info


void MTD2A::MTD2A_print_debug_error (const bool &printDebugPrint, const uint8_t &printErrorNumber) {
  Serial.print  (F("  debugPrint   : "));  MTD2A_print_enable_disable(printDebugPrint);
  Serial.print  (F("  errorNumber  : "));
  if (printErrorNumber == 0) {
     Serial.print(printErrorNumber); Serial.println (F(" OK")); 
  }
}; // MTD2A_print_debug_error 


void MTD2A::MTD2A_print_pin_number (const uint8_t &printPinNumber) {
  Serial.print  (F("  pinNumber    : ")); Serial.print(printPinNumber); 
  if (printPinNumber == PIN_ERROR_NO) 
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
      Serial.println(F("HIGH"));
    else 
      Serial.println(F("LOW"));
  }
} // MTD2A_print_value_binary


void MTD2A::MTD2A_print_enable_disable (const bool &enableOrDisable) {
  if (enableOrDisable == ENABLE)
    Serial.println(F("ENABLE"));
  else
    Serial.println(F("DISABLE"));
} // MTD2A_print_enable_disable


void MTD2A::MTD2A_print_pulse_fixed (const bool &pulseOrFixed) {
  if (pulseOrFixed == PULSE)
    Serial.println(F("PULSE"));
  else
    Serial.println(F("FIXED"));
} // MTD2A_print_puls_fixed


void MTD2A::MTD2A_print_normal_inverted (const bool &normalOrInverted) {
  if (normalOrInverted == NORMAL) 
    Serial.println(F("NORMAL"));
  else 
    Serial.println(F("INVERTED"));
}

#endif
