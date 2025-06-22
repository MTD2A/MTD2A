# Arduino examples

### blink_LED.ino
Two blinking LEDs. One with symmetric interval and another with asymetric interval https://youtu.be/eyGRazX9Bko
<br/> Diagram drawing: [Blink LED](/image/blink_LED.png)

### read_sensor.ino
Read sensor and write phase state information to Arduino IDE serial monitor https://youtu.be/RDFgEbhYUzE
<br/> Diagram drawing: [Read sensor](/image/read_sensor.png)

### train_direction.ino
Determining the direction of travel of the train (left / right) https://youtu.be/BqwTf03yEwQ
<br/> Diagram drawing: [Blink LED](/image/blink_LED.png) and [Read sensor](/image/read_sensor.png)

### stoplight_sound.ino
Stop light and sound message: The train brakes and temporarily stops at a red light. https://youtu.be/I5TYpuBB0eI
<br/> Speaker announcement is broadcast to the passengers. If the train is passing by, no announcement is broadcast.
<br/> Diagram drawing: [Stoplight sound](/image/stoplight_sound.png)

### switch_direction.ino
Automatic switch to the direction the train is coming from (to avoid derailment if the switch direction is wrong).
Using standard LGB switch (turnout) with position switches for feedback. https://youtu.be/oMeD3e_oXEI
<br/> Diagram drawing: [Switch (turnout) direction](/image/switch_direction.png)

# To do

### fade_in_out_LED.ino
Show differense in fading an LED: 1) uneven: lineary PWM curve 2) even: mathematical power formula PWM curve.

### railway_crossing.ino
Activate red warning light, start bell, lower barriers. Wait for the train to pass and do everything in reverse order.

### pendulum_relay.ino
Automatic pendulum operation on model railway system using relays. 
When the train reaches its holding area, it is sent back in the opposite direction after the set waiting time.

### pendulum_H-bridge.ino
Automatic pendulum and smooth operation on model railway system using PWM controlled analog DC motor H-bridge driver. 
When the train reaches its holding area, it is sent back in the opposite direction after the set waiting time.

<br/>

### Activate examples directly from Arduino IDE

![](/examples/Arduino-examples.png)
