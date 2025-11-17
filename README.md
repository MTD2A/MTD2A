# <ins>M</ins>odel <ins>T</ins>rain <ins>D</ins>etection <ins>A</ins>nd <ins>A</ins>ction - $\color{Green}\large{\textsf{Arduino library}}$

![](https://img.shields.io/badge/C%2B%2B-00599C?style=flat&logo=c%2B%2B&logoColor=white)
![](https://img.shields.io/github/v/release/MTD2A/MTD2A)
![](https://www.ardu-badge.com/badge/MTD2A.svg)
![](https://img.shields.io/github/license/MTD2A/MTD2A)
![](https://img.shields.io/badge/YouTube-FF0000?style=flat&logo=youtube&logoColor=white)

MTD2A is a collection of user friendly advanced and functional C++ classes - building blocks - for time-controlled handling of input and output. The library is intended for Arduino enthusiasts without much programming experience, who are interested in electronics control and automation, and model trains as a hobby. The library support parallel processing and asynchronous execution.

### Common to all building blocks are:
• They support a wide range of input sensors and output devices <br/>
• Are simple to use to build complex solutions with few commands <br/>
• They operate non-blocking, process-oriented and state-driven <br/>
• Offers extensive control and troubleshooting information <br/>
• Thoroughly documented with examples <br/>

## Mode of operation
MTD2A is a so-called state machine. This means MTD2A objects are quickly traversed in an infinite loop, and each traversal is divided into two phases:
1.	Changing state variables (changing and/or reading and checking)
2.	Execute functionality and control the passage of time.

### Concept Diagram
![](/image/Concept-Diagram.png)
The top $\color{Green}\large{\textbf{green}}$ process is carried out together with user-defined code and other libraries. <br/>
The lower $\color{#1E90FF}\large{\textbf{blue}}$ process is carried out via `MTD2A_loop_execute();` and as the last in `void loop();` 

### Parallel processing
In this way, an approximate parallelization is achieved, where several functions can in practice be executed simultaneously. For example, two flashing LEDs, where one is synchronous and the other is asynchronous. Example of parallel processing https://youtu.be/eyGRazX9Bko <br/>

The MTD2A library can be mixed with custom code and other libraries without further ado, as long as the execution is done non-blocking. But it requires a slightly different mindset when developing code, as it must always be taken into account that the infinite and fast loop must not be delayed, but also that user code is not executed more times than what is intended. It is often necessary to use different types of logic control flags. 

### Non-blocking execution 
It is absolutely crucial that no delaying or blocking code is used with the MTD2A library. Do not use delaying functions such as `delay();` as well as bibliographies that prevent rapid passages of the infinite loop. However, by default, delays up to a maximum of 10 milliseconds are allowed per pass. In most cases, this is sufficient of time to execute custom code and different types of libraries simultaneously. See further explanation [MTD2A_englsh.pdf](https://github.com/MTD2A/MTD2A/blob/main/doc/MTD2A_english.pdf) <br/>

Further explanation: [Finite-state machine - Wikipedia](https://en.wikipedia.org/wiki/Finite-state_machine) and here: [Real-time operating system - Wikipedia](https://en.wikipedia.org/wiki/Real-time_operating_system)

### documentation 
[https://github.com/MTD2A/MTD2A/tree/main/doc](https://github.com/MTD2A/MTD2A/blob/main/doc/README.md)

### New to eletronics and Arduino?
https://makeabilitylab.github.io/physcomp/

# Installation for Arduino IDE
MTD2A is part of the official Arduino library, and can be downloaded directly from the Arduino IDE (Integrated Development Environment) via the Library Manager

![](/image/Arduino-IDE-MTD2A-library-examples.png)


