# MTD2A_binary_input 
object_name ( "Object Name", delayTimeMS, {firstTrigger | lastTrigger}, {timeDelay | monoStable}, pinBlockTimeMS );

### Eksample
```
MTD2A_binary_input FC_51_left ("FC-51 left", 5000, lastTrigger, timeDelay);
// "FC-51 left" = Sensor (object) name, which is displayed together with status messages.
// 5000 = Time delay in milliseconds (5 seconds).
// lastTrigger = When to start (restart) timing. Here last impulse in the time period (LOW->HIGH)
// timeDelay = Use time delay (timer function)

void setup () {
Serial.begin (250000); // Necessary and first, if status messages are to be displayed.
FC_51_left.init_setup (DigitalPin_2);
// DigitalPin_2 = Arduino board pin number.
FC_51_left.set_debugPrint (on);
// on = Show status messages.
}

void loop () {
FC_51_left.loop_fast();
// Update the class object. Typically 10 milliseconds
delay (10);
}
```

### DANSK Eksempel
```
MTD2A_binary_input FC_51_left ("FC-51 left", 5000, lastTrigger, timeDelay);
// "FC-51 left" = Sensor (object) navn, som vises sammen med tilstandsbeskeder.
// 5000 = Tidsforsinkelse i milliseskunder (5 sekunder).
// lastTrigger = Tidspunkt for start på tidstagning. Her sidste impuls i tidsperioden (LOW->HIGH) 
// timeDelay = Benyt tidsforsinkelse (timer funktion)

void setup () {
  Serial.begin (250000);  // Nødvendigt og først, hvis der skal vises tilstandsbeskeder.
  FC_51_left.init_setup (DigitalPin_2); 
  // DigitalPin_2 = Arduino board ben nummer.
  FC_51_left.set_debugPrint (on); 
  // on = Vis tilstandsbeskeder.
}

void loop () {
  FC_51_left.loop_fast();
  // Opdatere klasseobjektet. Typisk 10 millisekunder
  delay (10);
}
```
## Chronological logic overview

![](/image/monostable_timing_and_delay.png)
