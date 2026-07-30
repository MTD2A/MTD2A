/**
 ******************************************************************************
 * @file    MTD2A_base.cpp
 * @author  Joergen Bo Madsen
 * @version 1.2.1
 * @date    15. july 2026
 * @brief   Base class for common functions (Model Train Detection And Action)
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


#include "MTD2A_base.h"

// Global constants from MTD2A_base.h (MTD2A_const.h) 
using namespace MTD2A_const;

constexpr uint32_t MTD2A::MAX_TIME_MS;    // definition, no initializer here
constexpr uint8_t  MTD2A::MAX_PWM_CURVES; // definition, no initializer here

// Function pointer linked list
MTD2A   *MTD2A::begin {nullptr};
MTD2A   *MTD2A::end   {nullptr};

// MTD2A static initializers (c++11 thus not class inline)
// Globals - all MTD2A instantiations
bool     MTD2A::globalDebugPrint  {DISABLE};
bool     MTD2A::globalErrorPrint  {ENABLE};
uint8_t  MTD2A::globalObjectCount {0};
//
uint32_t MTD2A::globalSyncTimeUS  {0UL};
uint8_t  MTD2A::globalDelayTimeMS {DELAY_10MS};

// statistics
uint32_t MTD2A::elapsedLoopTimeUS {0UL};
uint32_t MTD2A::maxElapsedTimeUS  {0UL};
uint32_t MTD2A::timeOverrunCount  {0UL};
// control logic
bool     MTD2A::epochInitialized  {false};
// Timers
 int64_t MTD2A::epochTimeUS       {0LL};
uint32_t MTD2A::currentTimeUS     {0UL};
uint32_t MTD2A::delayLoopTimeUS   {DELAY_10MS * MS_to_US};
uint32_t MTD2A::beginLoopTimeUS   {0UL};
uint32_t MTD2A::endLoopTimeUS     {0UL};
uint32_t MTD2A::lastEndLoopTimeUS {0UL};
// Long
uint64_t MTD2A::targetLoopTimeUS  {0ULL};
uint64_t MTD2A::totalLoopCount    {0ULL};


 int32_t MTD2A::remainingTimeUS   {0L};
uint16_t MTD2A::mainDelayTimeUS   {0U};
 int32_t MTD2A::correctionTimeUS  {0L};


// Setters -------------------------------------------------------------


void MTD2A::set_globalDebugPrint (bool setEnableOrDisable) {
  globalDebugPrint = setEnableOrDisable;
}


void MTD2A::set_globalErrorPrint (bool setEnableOrDisable) {
  globalErrorPrint = setEnableOrDisable;
}


void MTD2A::set_globalDelayTimeMS (uint8_t setGlobalDelayTimeMS) {
  if (setGlobalDelayTimeMS >= DELAY_1MS  &&  setGlobalDelayTimeMS <= DELAY_10MS) {
    globalDelayTimeMS = setGlobalDelayTimeMS;
    delayLoopTimeUS   = globalDelayTimeMS * MS_to_US;
    epochInitialized  = false;
    reset_stats ();
  }
  else {
      MTD2A_print_error_text (baseName, (globalDebugPrint == ENABLE ||  globalErrorPrint == ENABLE), 15, NO_PIN);
  }
} // set_globalDelayTimeMS


// getters -------------------------------------------------------------


uint8_t MTD2A::get_globalDelayTimeMS () {
  return globalDelayTimeMS;
}


uint32_t MTD2A::get_globalSyncTimeMS () {
  return MTD2A_round_US_to_MS (globalSyncTimeUS);
}


uint32_t MTD2A::get_maxElapsedTimeMS () {
  return MTD2A_round_US_to_MS(maxElapsedTimeUS);
}


uint32_t MTD2A::get_timeOverrunCount () {
  return timeOverrunCount;
}


uint8_t MTD2A::get_globalObjectCount () {
  return globalObjectCount;
}


uint64_t MTD2A::get_totalLoopCount () {
  return totalLoopCount;  
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
} // MTD2A_add_function_pointer_loop_fast


void MTD2A::MTD2A_remove_function_pointer_loop_fast (MTD2A* object) {
  MTD2A* previous = nullptr;
  MTD2A* current  = begin;
  while (current != nullptr  &&  current != object) {  // Find object and its predecessor
    previous = current;
    current  = current->next;
  }
  if (current == nullptr) {  // Not in the list - nothing to do
    return;
  }
  if (previous == nullptr)
    begin = current->next;      // Removing the first object
  else
    previous->next = current->next;
  if (end == current)           // Removing the last object
    end = previous;
  current->next = nullptr;
  if (globalObjectCount > 0)
    globalObjectCount--;
} // MTD2A_remove_function_pointer_loop_fast


MTD2A::~MTD2A () {
  MTD2A_remove_function_pointer_loop_fast (this);
} // ~MTD2A


// ========== Main loop: Function pointer linked list of the function "loop_fast" instantiated objects


void MTD2A::loop_execute () {
  currentTimeUS = micros();
  // Synchronized reference time for all child objects
  globalSyncTimeUS = currentTimeUS;

  loop_init_epoch_once         ();
  loop_execute_MTD2A_objects   ();
  loop_measure_execution_time  ();
  loop_time_overrun_correction ();
  loop_handle_micros_overflow  ();
  loop_target_timed_delay      ();

  // Advance to next loop
  beginLoopTimeUS = micros();
  totalLoopCount++;
} // loop_execute


void MTD2A::loop_init_epoch_once () {
  // First call or initialize from changing set_globalDelayTimeMS(): establish the epoch
  if (!epochInitialized) {
    epochInitialized = true;
    epochTimeUS      = currentTimeUS;
    beginLoopTimeUS  = currentTimeUS;
    totalLoopCount   = 1;
  }
} // loop_init_epoch_once


void MTD2A::loop_execute_MTD2A_objects () {
  // Execute all instantiated MTD2A objects
  MTD2A* object = begin;
  while (object != nullptr) {
    object->function_pointer(object);
    object = object->next;
  }
} // loop_execute_MTD2A_objects


void MTD2A::loop_measure_execution_time () {
  // Measure code execution time (user code, libraries and MTD2A instantiated opbects)
  lastEndLoopTimeUS = endLoopTimeUS;
  endLoopTimeUS     = micros();
  elapsedLoopTimeUS = endLoopTimeUS - beginLoopTimeUS;
  maxElapsedTimeUS  = max(elapsedLoopTimeUS, maxElapsedTimeUS);
} // loop_measure_execution_time


void MTD2A::loop_time_overrun_correction () {
  // Time overrun detection: User code and MTD2A instantiated objects took longer than globalDelayTimeMS
  // Ensure synchronization of millis() and reference target timet, increasing totalLoopCount
  // ensure maximum time frame for user code and MTD2A instantiated objects
  if (elapsedLoopTimeUS > (delayLoopTimeUS)) {
    timeOverrunCount++;
    // Subtract 1 because loop_execute() unconditionally does totalLoopCount++ at the end
    // of every call, which already accounts for one period.
    totalLoopCount += ((elapsedLoopTimeUS + (delayLoopTimeUS / 2)) / delayLoopTimeUS) - 1;
  }
} // time_overrun_correction


void MTD2A::loop_handle_micros_overflow () {
  // Handle micros () overflow conditions
  if (lastEndLoopTimeUS > endLoopTimeUS) {
    if (beginLoopTimeUS > endLoopTimeUS) {  // Phase 1: User code or MTD2A instantiated objects
      // Calculate time from micros() overflow time back to beginTimeMS = epoch time.
      epochTimeUS = -(static_cast<int64_t>(0x100000000LL - beginLoopTimeUS));
    }
    else {  // Phase 2: Main delay and correction delay process
      epochTimeUS  = static_cast<int64_t>(beginLoopTimeUS);  // Precise
    }
    totalLoopCount = 1;
  }
} // loop_handle_micros_overflow


void MTD2A::loop_target_timed_delay () {
  targetLoopTimeUS = static_cast<int64_t>(totalLoopCount * delayLoopTimeUS) + epochTimeUS;
  // Epoch-anchored delay
  remainingTimeUS = static_cast<int32_t>(targetLoopTimeUS - endLoopTimeUS);
  // Normal: we have time left — delay until the target
  if (remainingTimeUS > 0) {
    // Main delay: ~87% of remaining to leave room for correction
    mainDelayTimeUS = static_cast<uint16_t>(remainingTimeUS - (remainingTimeUS >> 3));
    if (mainDelayTimeUS > 0) {
      delayMicroseconds(mainDelayTimeUS);
    }
    // Correction: re-measure and delay any remainder
    currentTimeUS = micros();
    if (currentTimeUS >= endLoopTimeUS) {
      correctionTimeUS = static_cast<int32_t>(targetLoopTimeUS - currentTimeUS);
    }
    else {
      // micros() overflowed. TargetTimeUS is already in int64_t space above 2^32,
      // so lift currentTimeUS into the same space.
      int64_t overrunTimeUS = static_cast<int64_t>(currentTimeUS) + 0x100000000LL;
      correctionTimeUS = static_cast<int32_t>(targetLoopTimeUS - overrunTimeUS);
    }
    if (correctionTimeUS > 0 && correctionTimeUS < static_cast<int32_t>(delayLoopTimeUS)) {
      delayMicroseconds(static_cast<uint16_t>(correctionTimeUS));
    }
  }
} // loop_target_timed_delay

/* DEBUGGING
void MTD2A::print_debug_info (uint8_t id) {
  PortPrint (id); PortPrintln (" > ------------");
  PortPrint (F("totalLoopCount   : ")); MTD2A_print_uint64 (totalLoopCount);
  PortPrint (F("elapsedLoopTimeUS: ")); PortPrintln        (elapsedLoopTimeUS);
  PortPrint (F("targetLoopTimeUS : ")); MTD2A_print_uint64 (targetLoopTimeUS);
  PortPrint (F("beginLoopTimeUS  : ")); PortPrintln        (beginLoopTimeUS);
  PortPrint (F("endLoopTimeUS    : ")); PortPrintln        (endLoopTimeUS);
  PortPrint (F("remainingTimeUS  : ")); PortPrintln        (remainingTimeUS);
  PortPrint (F("currentTimeUS    : ")); PortPrintln        (currentTimeUS);
  PortPrint (F("epochTimeUS      : ")); MTD2A_print_uint64 (epochTimeUS);  // negative numbers will show garbage
  PortPrintln ();
}
*/

// ========== Main loop: Function pointer linked list of the function "loop_fast" instantiated objects


uint32_t MTD2A::MTD2A_round_US_to_MS (uint32_t microSeconds) { // Rounded integer division
  return (microSeconds / MS_to_US) + ((microSeconds % MS_to_US) >= (MS_to_US / 2) ? 1 : 0);
}


char *MTD2A::MTD2A_set_object_name (const char *setObjectName) {
  if (setObjectName == nullptr) {
    setObjectName = "";  // Copy empty string, so delete [] is always safe
  }
  size_t nameLength   = strlen (setObjectName) + 1;  // Include terminating '\0'
  char  *objectNameCopy = new char[nameLength];
  if (objectNameCopy == nullptr) {  // AVR: new returns nullptr on out-of-memory (no exceptions)
    return nullptr;                 // MTD2A_print_object_name () already handles nullptr -> "Unnamed"
  }
  strcpy (objectNameCopy, setObjectName);
  return objectNameCopy;
} // MTD2A_set_object_name


void MTD2A::MTD2A_print_object_name (const char *printObjectName) {
  if (printObjectName != nullptr)
    PortPrint(printObjectName);
  else
    PortPrint(F("Unnamed"));
} // MTD2A_print_object_name


uint8_t MTD2A::MTD2A_reserve_and_check_pin (uint8_t checkPinNumber, uint8_t checkPinFlags) {
  // https://github.com/arduino/ArduinoCore-avr/blob/master/variants/standard/pins_arduino.h
  // https://github.com/espressif/arduino-esp32/blob/master/variants/nano32/pins_arduino.h
  uint8_t checkErrorNumber = 0;
  // errorNumber {1-127} Error {128-255} Warning
  #ifndef NUM_DIGITAL_PINS
    #define NUM_DIGITAL_PINS 100  // Safe default for most Arduino boards
    checkErrorNumber = 128;  // Warning, but continue processing
  #endif
  static uint8_t pinFlags[NUM_DIGITAL_PINS] = {0};

  // bit 0 [1]  : digital
  // bit 1 [2]  : analog
  // bit 2 [4]  : input
  // bit 3 [8]  : input_pullup
  // bit 4 [16] : output
  // bit 5 [32] : PWM (digital)
  // bit 6 [64] : tone()
  // bit 7 [128]: Interrupt

  // Digital 
  if ((checkPinFlags & DIGITAL_FLAG_0)  &&  (checkPinNumber >= NUM_DIGITAL_PINS)) {
    checkErrorNumber = 2;
    return checkErrorNumber;
  }

  // Analog
  if (checkPinFlags & ANALOG_FLAG_1) {
    #if defined(ARDUINO_ARCH_ESP32)
      // ESP32 family: ADC-capable GPIOs are scattered, not a contiguous block.
      // digitalPinToAnalogChannel() (esp32-hal-gpio.h) returns -1 for non-ADC pins.
      // NOTE: ADC2 channels are unavailable while WiFi is active (hardware limitation).
      if (digitalPinToAnalogChannel(checkPinNumber) < 0) {
        checkErrorNumber = 3;
        return checkErrorNumber;
      }
    #elif defined(NUM_ANALOG_INPUTS) && defined(analogInputToDigitalPin)
      // Classic AVR: analog pins map to a contiguous block of digital pin numbers.
      if (checkPinNumber <  analogInputToDigitalPin(0)  ||
          checkPinNumber >  analogInputToDigitalPin(NUM_ANALOG_INPUTS - 1)) {
        checkErrorNumber = 3;
        return checkErrorNumber;
      }
    #elif defined(NUM_ANALOG_INPUTS)
      // Fallback: original block-at-the-end assumption.
      if (checkPinNumber < (NUM_DIGITAL_PINS - NUM_ANALOG_INPUTS)) {
        checkErrorNumber = 3;
        return checkErrorNumber;
      }
    #else
      checkErrorNumber = 129;  // Warning, but continue processing
    #endif
  }

  // Double pin binding
  if ((pinFlags[checkPinNumber] & INPUT_FLAG_2)  &&  (checkPinFlags & INPUT_FLAG_2)) {
    checkErrorNumber = 130;  // Warning, but continue processing
  }
  if ((pinFlags[checkPinNumber] & OUTPUT_FLAG_4)  &&  (checkPinFlags & OUTPUT_FLAG_4)) {
    checkErrorNumber = 4;
    return checkErrorNumber;
  }

  // Use of the tone() function will interfere with PWM output on pins 3 and 11 on NANO and 9 and 10 on MEGA.
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
    if (checkPinFlags & PWM_FLAG_5) {
      checkErrorNumber = 131;  // WARN_PWM_NO_CHECK - only if PWM was requested
    }
  #endif

  #if defined(digitalPinToInterrupt)
    #ifndef NOT_AN_INTERRUPT // Interrupt capability check
      #define NOT_AN_INTERRUPT 255 // Fallback for older Arduino versions
      if (checkPinFlags & INTERRUPT_FLAG_7) {
        checkErrorNumber = 132;  // WARN_INT_NO_CHECK
      }
    #endif
    if ((checkPinFlags & INTERRUPT_FLAG_7)  &&  (digitalPinToInterrupt(checkPinNumber) == NOT_AN_INTERRUPT)) {
      checkErrorNumber = 7;
      return checkErrorNumber;
    }
  #else
    if (checkPinFlags & INTERRUPT_FLAG_7) {
      checkErrorNumber = 132;  // WARN_INT_NO_CHECK
    }
  #endif

  if (checkErrorNumber == 0  ||  checkErrorNumber >= WARNING_START) 
    pinFlags[checkPinNumber] |= checkPinFlags;
  return checkErrorNumber;
} // MTD2A_reserve_and_check_pin


void MTD2A::MTD2A_print_error_text 
  (const char *printObjectName, bool debugOrErrorPrint, uint8_t printErrorNumber, uint8_t printPinNumber) {
  if (debugOrErrorPrint == ENABLE  ||  globalDebugPrint == ENABLE  ||  globalErrorPrint == ENABLE) {
    MTD2A_print_object_name (printObjectName);
    PortPrint (F(": ")); 
    if (printErrorNumber != 0) {
      if (printErrorNumber < WARNING_START) {
        PortPrint (F("ERROR"));
      } 
      else {
        PortPrint (F("Warning"));
      }
      PortPrint (F(" ["));    PortPrint (printErrorNumber); PortPrint (F("] > "));
      if (printPinNumber != NO_PIN) {
        PortPrint (F("Pin: ")); PortPrint (printPinNumber); PortPrint (F(" > "));
      }
    }
    // Named constants: central registry in MTD2A_const.h ties number, name and text together
    switch (printErrorNumber) {
      // Errors {1-127}
      case ERR_OBJECT_INSTANT:     PortPrintln (F("Object instantiation error / warning"));  break;
      case ERR_PIN_NOT_DEFINED:    PortPrintln (F("Pin number not defined (255)"));          break;
      case ERR_DIGITAL_PIN_RANGE:  PortPrintln (F("Digital pin number out of range"));       break;
      case ERR_ANALOG_PIN_RANGE:   PortPrintln (F("Analog pin number out of range"));        break;
      case ERR_OUTPUT_PIN_IN_USE:  PortPrintln (F("Output pin already in use"));             break;
      case ERR_NO_PWM_SUPPORT:     PortPrintln (F("Pin does not support PWM"));              break;
      case ERR_TONE_PWM_CONFLICT:  PortPrintln (F("tone() conflicts with PWM pin"));         break;
      case ERR_NO_INT_SUPPORT:     PortPrintln (F("Pin does not support interrupt"));        break;
      case ERR_INPUT_TYPE:         PortPrintln (F("Must be INPUT or INPUT_PULLUP"));         break;
      case ERR_TIME_BELOW_DELAY:   PortPrint   (F("Timer set to globalDelayTimeMS = ")); PortPrintln (globalDelayTimeMS); break;
      case ERR_TIME_ABOVE_MAX:     PortPrint   (F("Timer set to MAX_TIME_MS = "));       PortPrintln (MAX_TIME_MS); break;
      case ERR_PIN_WRITE_DISABLED: PortPrintln (F("Pin write is disabled"));                 break;
      case ERR_NOT_COMPLETE:       PortPrintln (F("Process state must be COMPLETE"));        break;
      case ERR_STOP_OR_RESET:      PortPrintln (F("Select STOP_TIMER or RESET_TIMER"));      break;
      case ERR_TIMER_ARGUMENT:     PortPrintln (F("Unknown TIMER argument"));                break;
      case ERR_DELAY_RANGE:        PortPrintln (F("globalDelayTimeMS must be: 1 - 10 MS"));  break;
      case ERR_NOT_ACTIVE:         PortPrintln (F("Process state must be ACTIVE"));          break;
      case ERR_UNFORESEEN_ERROR:   PortPrintln (F("Unforeseen system error"));               break;
      case ERR_TIMER_NOT_IN_USE:   PortPrintln (F("Must be ACTIVE and timer configured"));   break;
      case ERR_ALREADY_INIT:       PortPrintln (F("Already initialized"));                   break;

      // Warnings {128-255}
      case WARN_DIGITAL_NO_CHECK:  PortPrintln (F("Digital Pin check not possible"));        break;
      case WARN_ANALOG_NO_CHECK:   PortPrintln (F("Analog Pin check not possible"));         break;
      case WARN_PIN_REUSED:        PortPrintln (F("Pin used more than once"));               break;
      case WARN_PWM_NO_CHECK:      PortPrintln (F("PWM Pin check not possible"));            break;
      case WARN_INT_NO_CHECK:      PortPrintln (F("Interrupt Pin check not possible"));      break;
      case WARN_TIMER_ZERO:        PortPrintln (F("Timer value is zero"));                   break;
      case WARN_TIME_PAUSE_MAX:    PortPrintln (F("Time + pause exceeds MAX_TIME_MS"));      break;
      case WARN_COUNTDOWN_IGNORED: PortPrintln (F("setCountDownMS argument is ignored"));    break;
      case WARN_OUT_TIMER_ZERO:    PortPrintln (F("Output timer value is zero"));            break;
      case WARN_ALL_TIMERS_ZERO:   PortPrintln (F("All three timers are zero"));             break;
      case WARN_BINARY_VALUE:      PortPrintln (F("Binary pin value > 1. Set to HIGH"));     break;
      case WARN_UNDEF_PWM_CURVE:   PortPrint   (F("Undefined PWM curve. Must be <= "));  PortPrintln (MAX_PWM_CURVES); break;
      case WARN_USE_RISING:        PortPrintln (F("Use RISING curve instead of FALLING"));   break;
      case WARN_USE_FALLING:       PortPrintln (F("Use FALLING curve instead of RISING"));   break;
      case WARN_NO_CURVE_VALUE:    PortPrintln (F("BeginValue = endValue => NO_CURVE"));     break;
      case WARN_NO_CURVE_TIME:     PortPrintln (F("Time = globalDelayTime => NO_CURVE"));    break;
      case WARN_PAUSE_ACTIVE:      PortPrintln (F("PAUSE already active"));                  break;
      default:
        PortPrint(F("Unknown error: ")); PortPrint(printErrorNumber); PortPrintln(F(" Please report"));
    }
  }
} // MTD2A_print_error_text


void MTD2A::MTD2A_print_name_state (const char *printObjectName, bool printProcessState) {
  for (size_t i {1}; i < 20; i++) PortPrint(F("-")); 
  PortPrintln();
  PortPrint  (F("  objectName   : ")); PortPrintln(printObjectName);
  PortPrint  (F("  processState : ")); if (printProcessState == ACTIVE) PortPrintln(F("ACTIVE")); else PortPrintln(F("COMPLETE"));
} // MTD2A_print_name_state


void MTD2A::MTD2A_print_debug_error (bool printDebugPrint, bool printErrorPrint, uint8_t printErrorNumber) {
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


void MTD2A::MTD2A_print_pin_number (uint8_t printPinNumber) {
  PortPrint  (F("  pinNumber    : ")); PortPrint(printPinNumber); 
  if (printPinNumber == NO_PIN) 
    PortPrintln(F(" (NOT defined)")); 
  else 
    PortPrintln();
}
// MTD2A_print_pin_number


void MTD2A::MTD2A_print_value_binary (bool binaryOrP_W_M, uint8_t PrintValue) {
  if (binaryOrP_W_M == P_W_M)
    PortPrintln(PrintValue);
  else {
    if (PrintValue == HIGH) 
      PortPrintln(F("HIGH"));
    else 
      PortPrintln(F("LOW"));
  }
} // MTD2A_print_value_binary


void MTD2A::MTD2A_print_enable_disable (bool enableOrDisable) {
  if (enableOrDisable == ENABLE)
    PortPrintln(F("ENABLE"));
  else
    PortPrintln(F("DISABLE"));
} // MTD2A_print_enable_disable


void MTD2A::MTD2A_print_pulse_fixed (bool pulseOrFixed) {
  if (pulseOrFixed == PULSE)
    PortPrintln(F("PULSE"));
  else
    PortPrintln(F("FIXED"));
} // MTD2A_print_pulse_fixed


void MTD2A::MTD2A_print_normal_inverted (bool normalOrInverted) {
  if (normalOrInverted == NORMAL) 
    PortPrintln(F("NORMAL"));
  else 
    PortPrintln(F("INVERTED"));
}


void MTD2A::MTD2A_print_US_to_MS (uint32_t microSeconds) {
  PortPrintln (static_cast<float>(microSeconds) / MS_to_US);
}


void MTD2A::MTD2A_print_uint64 (uint64_t uint64Value) {
  char buffer[21];                    // Max uint64_t = 20 digits + '\0'
  char *uint64ptr = &buffer[20];
  *uint64ptr = '\0';
  do {                                // Build digits right-to-left
    *--uint64ptr = '0' + (uint64Value % 10);
    uint64Value /= 10;
  } while (uint64Value > 0);
  PortPrintln (uint64ptr);
} // MTD2A_print_uint64


void MTD2A::print_conf () {
  PortPrintln(F("MTD2A_base:"));
  for (size_t i {1}; i < 21; i++) PortPrint(F("-")); 
  PortPrintln();
  PortPrint  (F("  globalDebugPrint : ")); MTD2A_print_enable_disable (globalDebugPrint);
  PortPrint  (F("  globalErrorPrint : ")); MTD2A_print_enable_disable (globalErrorPrint);
  PortPrint  (F("  globalObjectCount: ")); PortPrintln (globalObjectCount);
  PortPrint  (F("  globalDelayTimeMS: ")); PortPrintln (globalDelayTimeMS);
  PortPrint  (F("  globalSyncTimeMS : ")); PortPrintln (MTD2A_round_US_to_MS (globalSyncTimeUS));
  PortPrint  (F("  lastElapsedTimeMS: ")); MTD2A_print_US_to_MS (elapsedLoopTimeUS);
  PortPrint  (F("  maxElapsedTimeMS : ")); MTD2A_print_US_to_MS (maxElapsedTimeUS);
  PortPrint  (F("  timeOverrunCount : ")); PortPrintln (timeOverrunCount);
  PortPrint  (F("  totalLoopCount   : ")); MTD2A_print_uint64 (totalLoopCount);
}


void MTD2A::reset_stats () {
  elapsedLoopTimeUS = 0;
  maxElapsedTimeUS  = 0;
  timeOverrunCount  = 0;
}
