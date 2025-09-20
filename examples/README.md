# Arduino examples

### blink_LED.ino
Two blinking LEDs. One with symmetric interval and another with asymetric interval. https://youtu.be/eyGRazX9Bko
<br/> Diagram drawing: [Blink LED](/image/blink_LED.png)

### read_sensor.ino
Read sensor and write phase state information to Arduino IDE serial monitor. https://youtu.be/RDFgEbhYUzE
<br/> Diagram drawing: [Read sensor](/image/read_sensor.png)

### time_event_processes.ino
7 time and event processes examples, to inspire how to code state machine process flows.


### train_direction.ino
Determining the direction of travel of the train (left / right). https://youtu.be/BqwTf03yEwQ
<br/> Diagram drawing: [Blink LED](/image/blink_LED.png) and [Read sensor](/image/read_sensor.png)

### stoplight_sound.ino
Stop light and sound message: The train brakes and temporarily stops at a red light. https://youtu.be/I5TYpuBB0eI
<br/> Speaker announcement is broadcast to the passengers. If the train is passing by, no announcement is broadcast.
<br/> Diagram drawing: [Stoplight sound](/image/stoplight_sound.png)

### switch_direction.ino
Automatic switch to the direction the train is coming from (to avoid derailment if the switch direction is wrong).
<br/> Using standard LGB switch (turnout) with position switches for feedback. https://youtu.be/oMeD3e_oXEI
<br/> Diagram drawing: [Switch (turnout) direction](/image/switch_direction.png)

### railway_crossing.ino
Danish railway crossing. Read left and right sensor, activate red blinking warning light, start pulsating bell, lower boom slowly. 
<br/> Wait for the train to pass, raise boom slowly and stop red blink. https://youtu.be/VaXmki2oLrk
<br/> Diagram drawing: [Railway crossing](/image/railway_crossing.png)

### math_fade_LED.ino
Two LEDs fading in and fading out using a linear function and a math potence (power) function. https://youtu.be/8TV6nOdXBno
<br/> Overview of [PWM curves](/doc/PWM_curves.pdf)

### servo_math_curve.ino
Use math S-curve to move servo from 0 to 180 degrees and vice versa. https://youtu.be/rhQtu0iKFl8
<br/> Exampe shows: 1) No curve 2) linear curve and 3) [Sigmoid S-curve](https://en.wikipedia.org/wiki/Sigmoid_function)
<br/> Overview of [PWM curves](/doc/PWM_curves.pdf)

# To do

### pendulum_relay.ino
Automatic pendulum operation on model railway system using relays. 
When the train reaches its holding area, it is sent back in the opposite direction after the set waiting time.

### pendulum_H_bridge.ino
Automatic pendulum and smooth operation on model railway system using PWM controlled analog DC motor H-bridge driver. 
When the train reaches its holding area, it is sent back in the opposite direction after the set waiting time.

<br/>

### Activate examples directly from Arduino IDE

![](/examples/Arduino-examples.png)
