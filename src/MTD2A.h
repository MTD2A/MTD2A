/*
 ******************************************************************************
 * @file    MTD2A.h
 * @author  Joergen Bo Madsen
 * @version V1.1.1
 * @date    21. maj 2025
 * @brief   Abstract main header for MTD2A (Model Train Detection And Action)
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

#ifndef _MTD2A_H_
#define _MTD2A_H_


#ifndef SerialPort
#define SerialPort true
#endif

#if SerialPort == true
  #define PortPrint(x)   Serial.print(x)
  #define PortPrintln(x) Serial.println(x)
#else // Production
  #define PortPrint(x)
  #define PortPrintln(x)
#endif


#include "Arduino.h"
#include "MTD2A_const.h"
#include "MTD2A_base.h"
#include "MTD2A_binary_input.h"
#include "MTD2A_binary_output.h"


#endif