### Model Train Detection And Action - NEW Arduino library

Work in progress

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
The top $\color{Green}{\textsf{green}}$ process is carried out together with user-defined code and other libraries. <br/>
The lower $\color{Blue}{\textsf{blue}}$ process is carried out via `MTD2A_loop_execute();` and as the last in `void loop();` 

### Example of parallel processing
https://youtu.be/eyGRazX9Bko

### To do
* class MTD2A_delay
* class MTD2A_astable
* class MTD2A_flip_flop
* class MTD2A_tone
* class MTD2A_sound
* class MTD2A_servo
* class MTD2A_stepper
* class MTD2A_display
* class MTD2A_ultrasonic
* class MTD2A_laser
* class MTD2A_IR_ranging
* class MTD2A_DCC_input

### documentation 
[https://github.com/MTD2A/MTD2A/tree/main/doc](https://github.com/MTD2A/MTD2A/blob/main/doc/README.md)

### New to eletronics and Arduino?
https://makeabilitylab.github.io/physcomp/

# Installation for Arduino IDE
Download the entire MTD2A repository as a ZIP file.

![](/image/MTD2A-download.png)

Unzip the ZIP file and rename the directory name to MTD2A. <br/>
Move everything to the Arduino home directory under libraries: **Arduino/libraries/MTD2A** <br/>
The examples can now be selected from the list of examples in the Arduino IDE **Examples** <br/>

![](/image/Arduino-examples.png)
