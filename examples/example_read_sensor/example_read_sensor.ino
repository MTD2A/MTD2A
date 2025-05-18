
#include <MTD2A.h>

// https://github.com/MTD2A/FC-51

MTD2A_binary_input FC_51_left ("FC-51 left", 5000, lastTrigger, timeDelay);
// "FC-51 left" = Sensor (object) navn, som vises sammen med tilstandsbeskeder.
// 5000 = Tidsforsinkelse i millisekunder (5 sekunder).
// lastTrigger = Tidspunkt for start på tidtagning. Her sidste impuls i tidsperioden (LOW->HIGH) 
// timeDelay = Benyt tidsforsinkelse (timer funktion)

void setup () {
  Serial.begin (9600);  // Nødvendigt og først, hvis der skal vises tilstandsbeskeder.
  FC_51_left.initialize (2); // Arduino board pin 2.
  FC_51_left.set_debugPrint ();  // Vis tilstandsbeskeder.
}

void loop () {
  // Hver gang sensoren aktiveres vil der udskrives status på Arduino Serial Monitor
  FC_51_left.loop_fast(); // Opdatere state (event) systemet
  delay (10); // Opdatere klasseobjektet. Typisk 1-10 millisekunder
}

