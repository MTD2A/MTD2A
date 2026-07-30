/**
 ******************************************************************************
 * @file    MTD2A_print.h
 * @author  Joergen Bo Madsen
 * @version 1.3.1
 * @date    15. july 2026
 * @brief   Development / Production setup for MTD2A (Model Train Detection And Action)
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


#ifndef MTD2A_PRINT_H_
#define MTD2A_PRINT_H_


// Development and debug
#define PortPrint(x)   Serial.print(x)
#define PortPrintln(x) Serial.println(x)
// Optimized production
// #define PortPrint(x)
// #define PortPrintln(x)


#endif
