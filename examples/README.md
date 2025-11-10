# Arduino examples

### blink_LED.ino
Two blinking LEDs. One with symmetric interval and another with asymetric interval. 
<br/> Short DEMO: https://youtu.be/eyGRazX9Bko
<br/> Diagram drawing: [Blink LED](/image/blink_LED.png)

### read_sensor.ino
Read sensor and write phase state information to Arduino IDE serial monitor. 
<br/> Short DEMO: https://youtu.be/RDFgEbhYUzE
<br/> Diagram drawing: [Read sensor](/image/read_sensor.png)

### time_event_processes.ino
7 time and event processes examples, to inspire how to code state machine process flows. 
<br/> Short DEMO: https://youtu.be/UU4k4_8GWfM

### train_direction.ino
Determining the direction of travel of the train (left / right). 
<br/> Short DEMO: https://youtu.be/BqwTf03yEwQ
<br/> Diagram drawing: [Blink LED](/image/blink_LED.png) and [Read sensor](/image/read_sensor.png)

### stoplight_sound.ino
Stop light and sound message: The train brakes and temporarily stops at a red light. 
<br/> Speaker announcement is broadcast to the passengers. If the train is passing by, no announcement is broadcast.
<br/> Short DEMO: https://youtu.be/I5TYpuBB0eI
<br/> Diagram drawing: [Stoplight sound](/image/stoplight_sound.png)

### switch_direction.ino
Automatic switch to the direction the train is coming from (to avoid derailment if the track switch direction is wrong).
<br/> Using standard LGB track switch (turnout) with position switches for feedback. 
<br/> Short DEMO: https://youtu.be/oMeD3e_oXEI
<br/> Diagram drawing: [Track switch (turnout) direction](/image/switch_direction.png)

### railway_crossing.ino
Danish railway crossing. Read left and right sensor, activate red blinking warning light, start pulsating bell, lower boom slowly. 
<br/> Wait for the train to pass, raise boom slowly and stop red blink. 
<br/> Short DEMO: https://youtu.be/VaXmki2oLrk
<br/> Diagram drawing: [Railway crossing](/image/railway_crossing.png)

### math_fade_LED.ino
Two LEDs fading in and fading out using a linear function and a math potence (power) function. 
<br/> Short DEMO: https://youtu.be/8TV6nOdXBno
<br/> Diagram drawing: [Math fad LED](/image/math_fade_LED.png)
<br/> Overview of [PWM curves](/doc/PWM_curves.pdf)
<br/> Article on various fading lights: [Professor Tom Igoe](https://tigoe.github.io/LightProjects/fading.html)

### servo_math_curve.ino
Use math S-curve to move servo from 0 to 180 degrees and vice versa. 
<br/> Short DEMO: https://youtu.be/rhQtu0iKFl8
<br/> Diagram drawing: [Servo math curves](/image/servo_math_curve.png)
<br/> Demo shows: 1) No curve 2) linear curve and 3) [Sigmoid S-curve](https://en.wikipedia.org/wiki/Sigmoid_function)
<br/> Overview of [PWM curves](/doc/PWM_curves.pdf)

### PWM_power_curves.ino
Test and compare lineary and power PWM curves with heavy train load and light train load
https://youtu.be/Fi9D1hrzT9M
<br/> Diagram drawing: [PMW pwer cirves](/PWM_power_curves.png)
<br/> Overview of [PWM curves](/doc/PWM_curves.pdf)

### pendulum_relays.ino
Automatic pendulum operation on model railway system using relays to control track switch, train direction, train power, and one IR sensor to calibrate time and distance offset. When the train reaches its holding area, it is sent back in the opposite direction after the set waiting time. 
<br/> Short DEMO: https://youtu.be/mffAFvGHrFw
<br/> Diagram drawing: [Pendulum realys](/image/pendulum_relays.png)
<br/> H-bridge relays: 4 relays instead of 6 relays: https://www.talkingelectronics.com/projects/H-Bridge/H-Bridge-1.html

### pendulum_H_bridge.ino
Advanced automatic pendulum operation on model railway system using PWM controlled H-bridge
to power train engine and direction. A simple H-bridge is used to control track switch and direction. 
One IR sensor is used to calibrate time and distance offset. Finaly red and green train station signal.
The train route is controlled by advanced mathematical PWM curves that compensate for inertia and 
friction, and thus ensure smooth acceleration and deceleration.
<br/> Short DEMO: https://youtu.be/1i-cGc6Dk4E
<br/> Diagram drawing: [Pendulum H-brige](/image/advanced_pendulum_H_bridge.png)
<br/> Overview of [PWM curves](/doc/PWM_curves.pdf)
<br/> Driving DC Motors with Microcontrollers https://dronebotworkshop.com/dc-motor-drivers/
<br/>
<br/>

### Activate examples directly from Arduino IDE

![](/examples/Arduino-examples.png)
