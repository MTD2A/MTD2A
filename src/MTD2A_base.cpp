/*
 ******************************************************************************
 * @file    MTD2A_base.cpp
 * @author  Joergen Bo Madsen
 * @version V1.1.6
 * @date    7. july 2025
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


// MTD2A static initializers (c++11 thus not class inline)
bool     MTD2A::globalDebugPrint  {DISABLE};
bool     MTD2A::globalErrorPrint  {ENABLE};
uint32_t MTD2A::globalDelayTimeMS {DELAY_10MS};
uint32_t MTD2A::globalSyncTimeMS  {0};
uint32_t MTD2A::globalLastTimeMS  {0};
uint32_t MTD2A::globalLoopTimeMS  {0};
uint32_t MTD2A::globalMaxLoopMS   {0};
uint8_t  MTD2A::globalObjectCount {0};
// Funtion pointer linked list
MTD2A   *MTD2A::begin {nullptr};
MTD2A   *MTD2A::end   {nullptr};


// Global constants from MTD2A_bas.h (MTD2A_const.h)
constexpr bool MTD2A::ENABLE;
constexpr bool MTD2A::DISABLE;
constexpr bool MTD2A::ACTIVE;
constexpr bool MTD2A::COMPLETE;
constexpr bool MTD2A::BINARY;
constexpr bool MTD2A::NORMAL;
constexpr bool MTD2A::INVERTED;
constexpr bool MTD2A::PULSE;
constexpr bool MTD2A::MONO_STABLE;
constexpr bool MTD2A::LAST_TRIGGER;
constexpr bool MTD2A::TIME_DELAY;
constexpr bool MTD2A::P_W_M;
constexpr bool MTD2A::STOP_TIMER;
constexpr bool MTD2A::RESTART_TIMER;
constexpr uint32_t MTD2A::DELAY_10MS;
constexpr uint32_t MTD2A::DELAY_1MS;


void MTD2A::set_globalDebugPrint (const bool &setEnableOrDisable) {
  globalDebugPrint = setEnableOrDisable;
}


void MTD2A::set_globalErrorPrint (const bool &setEnableOrDisable) {
  globalErrorPrint = setEnableOrDisable;
}


void MTD2A::set_globalDelayTimeMS (const bool &setGlobalDelayTimeMS) {
  if (setGlobalDelayTimeMS == DELAY_10MS) 
    globalDelayTimeMS = DELAY_10MS;
  else 
    globalDelayTimeMS = DELAY_1MS;
}


uint32_t MTD2A::get_globalSyncTimeMS () {
  return globalSyncTimeMS;
}


uint32_t MTD2A::get_reset_maxLoopMS () {
  uint32_t tempLoopMS;
  tempLoopMS = globalMaxLoopMS;
  globalMaxLoopMS = 0;
  return tempLoopMS;
}


uint8_t MTD2A::get_objectCount () {
  return globalObjectCount;
}


// ========== Function pointer linked list of the function "loop_fast" instantiated object
void MTD2A::MTD2A_add_function_pointer_loop_fast (MTD2A* object) {
  if (begin == nullptr)
    begin = object;
  if (end != nullptr) {
    end->next = object;
  }
  end = object;
  globalObjectCount++;
}


void MTD2A::loop_execute() {
  // Correct the time if delay() is used before loop ()
  if (globalSyncTimeMS == 0);
    globalSyncTimeMS = millis();
  // Execute function pointers
  MTD2A* object = begin;
  while (object != nullptr) {
    object->function_pointer(object);
    object = object->next;
  }
  // Cadence precision correction
  globalSyncTimeMS = millis();
  globalLoopTimeMS = globalSyncTimeMS - globalLastTimeMS;
  globalMaxLoopMS = max(globalLoopTimeMS, globalMaxLoopMS);
  if (globalDelayTimeMS == DELAY_10MS) {
    if (globalLoopTimeMS > DELAY_10MS)
      ; // PortPrintln(F("Warning: User code executing delay is above threshold"));
    else 
      delay(DELAY_10MS - globalLoopTimeMS);
  }
  else
    delay(globalDelayTimeMS);
  globalLastTimeMS = millis();
}
// ========== Function pointer linked list of the function "loop_fast" instantiated object


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


void MTD2A::MTD2A_print_object_name (const char *printObjectName) {
  PortPrint(millis()); PortPrint(F("\t ")); 
  if (printObjectName != nullptr)
    PortPrint(printObjectName);
  else
    PortPrint(F("Unnamed"));
} // MTD2A_print_object_name


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
  // https://github.com/espressif/arduino-esp32/blob/master/variants/nano32/pins_arduino.h
  #if defined(NUM_DIGITAL_PINS)
    if ((checkPinFlags & DIGITAL_FLAG_0)  &&  (checkPinNumber >= NUM_DIGITAL_PINS)) {
      checkErrorNumber = 2;
      return checkErrorNumber;
    }
  #else
    checkErrorNumber = 128;  // Warning, but continue processing
  #endif
    
  // Analog
  #if defined(NUM_ANALOG_INPUTS)
    if ((checkPinFlags & ANALOG_FLAG_1)  &&  (NUM_DIGITAL_PINS - checkPinNumber >= NUM_ANALOG_INPUTS)) {
      checkErrorNumber = 3;
      return checkErrorNumber;
    }
  #else
    checkErrorNumber = 129;  // Warning, but continue processing
  #endif

  // Double pin binding
  if ((pinFlags[checkPinNumber] & INPUT_FLAG_2)  &&  (checkPinFlags & INPUT_FLAG_2)) {
    checkErrorNumber = 130;  // Warning, but continue processing
  }
  if ((pinFlags[checkPinNumber] & OUTPUT_FLAG_4)  &&  (checkPinFlags & OUTPUT_FLAG_4)) {
    checkErrorNumber = 4;
    return checkErrorNumber;
  }

  // Use of the tone() function will interfere with PWM output on pins 3 and 11 on NANO and 0 and 10 on MEGA.
  # if defined(digitalPinHasPWM)
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
  #else
    checkErrorNumber = 131;  // Warning, but continue processing
  #endif

  #if defined(digitalPinToInterrupt)
    #ifndef NOT_AN_INTERRUPT // Interrupt capability check
      #define NOT_AN_INTERRUPT MAX_BYTE_VALUE; // Fallback for older Arduino versions
    #endif
    if ((checkPinFlags & INTERRUPT_FLAG_7)  &&  (digitalPinToInterrupt(checkPinNumber) == NOT_AN_INTERRUPT)) {
      checkErrorNumber = 7;
      return checkErrorNumber;
    }
  #else
    checkErrorNumber = 132;  // Warning, but continue processing
  #endif

  if (checkErrorNumber == 0  ||  checkErrorNumber >= WARNING_START) 
    pinFlags[checkPinNumber] |= checkPinFlags;
  return checkErrorNumber;
} // MTD2A_reserve_and_check_pin


void MTD2A::MTD2A_print_error_text (const bool &DebugOrErrorPrint, const uint8_t &printErrorNumber, const uint8_t &printPinNumber) {
  if (DebugOrErrorPrint == ENABLE || globalDebugPrint == ENABLE ||  globalErrorPrint == ENABLE) {
    if (printErrorNumber < WARNING_START) 
      PortPrint (F("ERROR")); 
    else 
      PortPrint (F("Warning"));
    PortPrint (F(" ["));     PortPrint (printErrorNumber); PortPrint (F("]: "));
    PortPrint (F(" Pin: ")); PortPrint (printPinNumber);   PortPrint (F(" - "));
    switch (printErrorNumber) {
      case   1: PortPrintln (F("Pin number not set (255)"));          break;
      case   2: PortPrintln (F("Digital pin number out of range"));   break;
      case   3: PortPrintln (F("Analog pin number out of range"));    break;
      case   4: PortPrintln (F("Output pin already in use"));         break;
      case   5: PortPrintln (F("Pin does not support PWM"));          break;
      case   6: PortPrintln (F("tone() conflicts with PWM pin"));     break;
      case   7: PortPrintln (F("Pin does not support interrupt"));    break;
      case   8: PortPrintln (F("Must be INPUT or INPUT_PULLUP"));     break;
      case   9: PortPrint   (F("Time must be >= ")); PortPrintln (globalDelayTimeMS); break;
      case  11: PortPrintln (F("Pin write is disabled"));             break;
      case 128: PortPrintln (F("Digital Pin check not possible"));    break;
      case 129: PortPrintln (F("Anaog Pin check not possible"));      break;
      case 130: PortPrintln (F("Pin used more than once"));           break;
      case 131: PortPrintln (F("PWM Pin check not possible"));        break;
      case 132: PortPrintln (F("Interupt Pin check not possible"));   break;
      case 140: PortPrintln (F("Timer value is zero"));               break;
      case 150: PortPrintln (F("Output timer value is zero"));        break;
      case 151: PortPrintln (F("All three timers are zero"));         break;
      case 152: PortPrintln (F("Binary pin value > 1. Set to HIGH")); break;
      case 153: PortPrint   (F("Undefined PWM curve. Must be <= ")); PortPrintln (MAX_PWM_CURVES); break;
      case 154: PortPrintln (F("Use RISING curve instead of FALLING"));break;
      case 155: PortPrintln (F("Use FALLING curve instead of RISING"));break;
      default:
        PortPrint(F("Unknown error: ")); PortPrint(printErrorNumber); PortPrintln(F(" Please report"));
    }
  }
} // MTD2A_print_error_text


void MTD2A::MTD2A_print_name_state (const char *printObjectName, const bool &printProcessState) {
  for (size_t i {1}; i < 20; i++) PortPrint(F("-")); PortPrintln();
  PortPrint  (F("  objectName   : ")); PortPrintln(printObjectName);
  PortPrint  (F("  processState : ")); if (printProcessState == ACTIVE) PortPrintln(F("ACTIVE")); else PortPrintln(F("COMPLETE"));
}; // MTD2A_print_phase_info


void MTD2A::MTD2A_print_debug_error (const bool &printDebugPrint, const bool &printErrorPrint, const uint8_t &printErrorNumber) {
  PortPrint  (F("  debugPrint   : "));  MTD2A_print_enable_disable(printDebugPrint);
  PortPrint  (F("  globalDebugPr: "));  MTD2A_print_enable_disable(globalDebugPrint);
  PortPrint  (F("  errorPrint   : "));  MTD2A_print_enable_disable(printErrorPrint);
  PortPrint  (F("  globalErrorPr: "));  MTD2A_print_enable_disable(globalErrorPrint);
  PortPrint  (F("  errorNumber  : "));
  if (printErrorNumber == 0) {
     PortPrint(printErrorNumber); PortPrintln (F(" OK")); 
  }
  else
    PortPrintln(printErrorNumber);
}; // MTD2A_print_debug_error 


void MTD2A::MTD2A_print_pin_number (const uint8_t &printPinNumber) {
  PortPrint  (F("  pinNumber    : ")); PortPrint(printPinNumber); 
  if (printPinNumber == PIN_ERROR_NO) 
    PortPrintln(F(" (NOT defined)")); 
  else 
    PortPrintln();
}
// MTD2A_print_pin_number


void MTD2A::MTD2A_print_value_binary (const bool &binaryOrP_W_M, const uint8_t &PrintValue) {
  if (binaryOrP_W_M == P_W_M)
    PortPrintln(PrintValue);
  else {
    if (PrintValue == HIGH) 
      PortPrintln(F("HIGH"));
    else 
      PortPrintln(F("LOW"));
  }
} // MTD2A_print_value_binary


void MTD2A::MTD2A_print_enable_disable (const bool &enableOrDisable) {
  if (enableOrDisable == ENABLE)
    PortPrintln(F("ENABLE"));
  else
    PortPrintln(F("DISABLE"));
} // MTD2A_print_enable_disable


void MTD2A::MTD2A_print_pulse_fixed (const bool &pulseOrFixed) {
  if (pulseOrFixed == PULSE)
    PortPrintln(F("PULSE"));
  else
    PortPrintln(F("FIXED"));
} // MTD2A_print_puls_fixed


void MTD2A::MTD2A_print_normal_inverted (const bool &normalOrInverted) {
  if (normalOrInverted == NORMAL) 
    PortPrintln(F("NORMAL"));
  else 
    PortPrintln(F("INVERTED"));
}


void MTD2A::print_conf () {
  PortPrintln(F("MTD2A_base:"));
  for (size_t i {1}; i < 20; i++) PortPrint(F("-")); PortPrintln();
  PortPrint(F(" globalDebugPrint : ")); MTD2A_print_enable_disable(globalDebugPrint);
  PortPrint(F(" globalErrorPrint : ")); MTD2A_print_enable_disable(globalErrorPrint);
  PortPrint(F(" globalSyncTimeMS : ")); PortPrintln(globalSyncTimeMS);
  PortPrint(F(" globalDelayTimeMS: ")); if (globalDelayTimeMS == DELAY_1MS) PortPrintln(F("DELAY_1MS")); 
                                        else PortPrintln(F("DELAY_10MS"));
  PortPrint(F(" globalLastTimeMS : ")); PortPrintln(globalLastTimeMS);
  PortPrint(F(" globalLoopTimeMS : ")); PortPrintln(globalLoopTimeMS);
  PortPrint(F(" globalMaxLoopMS  : ")); PortPrintln(globalMaxLoopMS);
  PortPrint(F(" globalObjectCount: ")); PortPrintln(globalObjectCount);
}


#endif
