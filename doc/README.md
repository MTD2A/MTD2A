# Documentation

### Mobile Train Detection And Action - Arduino library
 * Supporting a vast variety of input sensors and output devices 
 * Simple to use to build complex solutions 
 * Non blocking, simple, yet efficient event-driven state machine


## MTD2A_binary_input 
object_name ( "Object Name", delayTimeMS, {firstTrigger | lastTrigger}, {timeDelay | monoStable}, pinBlockTimeMS );

### Detailed danish documentation
https://github.com/MTD2A/MTD2A/blob/main/doc/MTD2A%20binary%20input.pdf

### Chronological logic overview

![](/image/monostable_timing_and_delay.png)


# MTD2A_binary_output
object_name ( "Object name", outputTimeMS, beginDelayMS, endDelayMS, pinOutputMode, pinBeginValue, pinEndValue );
