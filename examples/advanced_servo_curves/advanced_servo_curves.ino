// Advanced PWM control of servo with mathematical functions, using two differencnt servos: SG92A and 9imod 2065 waterproof
// There are 5 different examples to choose from. The last example is using the Adafruit PCA9685 with I2C communication.
// https://docs.arduino.cc/libraries/servo/ 
// Short DEMO: 
// Jørgen Bo Madsen / July 2026 / https://github.com/jebmdk

#include <MTD2A.h>
#include <Servo.h>
#include <Adafruit_PWMServoDriver.h> // Adafruit PCA9685 PWM Servo Driver Library
using namespace MTD2A_const;

#define section 3

// 1: 180 degrees step resolution
// 2: 256 step PWM max resolution
// 3: PWM Sinus curve - max speed at 0 and 180 degrees
// 4: PWM Sinus curve - max speed at 90 degrees
// 5: PWM Sinus amplitude decreasing curve - max speed at 90 degrees

// SG92R micro analog servo. Nylon gear. Range: 0 - 180 degrees. 50 Hz. Deadband ~10 us
// 9IMOD 7kg 2065 Digital Servo. Stainless steel gear. Water resistant IP66. Range: 0 - 180 & 50 - 330 Hz. Deadband ~2 us
// DS3118 PRO 20kg Digitalservo. Stainless steel gear. Water resistant IP66. Range: 0 - 180 & 50 - 330 Hz. Deadband ~2 us

const unsigned int SERVO_MIN_US =  520;
const unsigned int SERVO_MAX_US = 2480;
const unsigned int PCA9685_MIN  =  140; // This is the 'minimum' pulse length count (out of 4096)
const unsigned int PCA9685_MAX  =  590; // This is the 'maximum' pulse length count (out of 4096)

// Arduino board pins
const byte NANO_PIN_1  = 5;  // Nano Servo output PIN
const byte NANO_PIN_2  = 6;  // Nano Servo output PIN
const byte PCA9685_PIN = 0;  // Adadfruit I2C PCA9685 board servo PIN

MTD2A_binary_output boom_angel ("Boom angel");
Servo Nano_servo_1;
Servo Nano_servo_2;
Adafruit_PWMServoDriver PCA9685_servo = Adafruit_PWMServoDriver(); // default address 0x40
// https://learn.adafruit.com/16-channel-pwm-servo-driver?view=all

// PWM curves
bool changeFlag = true;
bool loopStart  = true;
int  loopCount  = 1;
int  stepCount  = 0;
int  PWMcurveNo = 0;
//
const byte  MAX_CURVES = 5;
const byte  PWMcurveBegin [MAX_CURVES] = {   RISING_XY,  RISING_SM5,  RISING_SM8,  RISING_BZ1,  RISING_BZ2 };
const byte  PWMcurveEnd   [MAX_CURVES] = {  FALLING_XY, FALLING_SM5, FALLING_SM8, FALLING_BZ1, FALLING_BZ2 };
const char* PWMcureveText [MAX_CURVES] = { "XY linear", "Sigmoid 5", "Sigmoid 8",  "Bezier 1",  "Bezier 2" };


void setup() {
  Serial.begin(9600);
  while (!Serial) { delay(10); } // ESP32 Serial Monitor ready delay

  boom_angel.initialize ();  // pin not used. Only use mathematical curve PWM values
  Nano_servo_1.attach (NANO_PIN_1, SERVO_MIN_US, SERVO_MAX_US);  // Attach servo to board pin
  Nano_servo_2.attach (NANO_PIN_2, SERVO_MIN_US, SERVO_MAX_US);  // Attach servo to board pin
  
  for (byte loopIndex = 0; loopIndex < 2; loopIndex++) {
    Nano_servo_1.write  (180); // Right
    Nano_servo_2.write  (180); // Right
    delay(1000);
    Nano_servo_1.write  (0);   // Left
    Nano_servo_2.write  (0);   // Left
    delay(1000);
  }

  PCA9685_servo.begin();
  PCA9685_servo.setPWMFreq(50); // 50 Hz (LED: 1000 Hz & max 25 mA)
}


#if section == 1  // --------------------------------------------------------------------------------------------------


void loop() {
  if (loopStart) {
    Serial.println(F("(1) 180 degress step resolution"));
    boom_angel.set_timers (3000, 100, 0);  // Output, begin, end
    loopStart = false;
  }
  if (boom_angel.get_processState() == COMPLETE) {
    if (changeFlag == true) {
      Serial.print (loopCount); Serial.print(F(": PWM curve: ")); Serial.println(PWMcureveText[PWMcurveNo]);
      boom_angel.activate (0, 180, PWMcurveBegin[PWMcurveNo]);
      changeFlag = false;
    } 
    else {
      boom_angel.activate (180, 0, PWMcurveEnd[PWMcurveNo]);
      changeFlag = true;
      select_curve ();
    }
  }
  else { // 180 step
    // Move servo left-right-pause-right-left-pause ....
    Nano_servo_1.write(boom_angel.get_outputValue());
    Nano_servo_2.write(boom_angel.get_outputValue()); 
  }
 
  MTD2A_loop_execute();  // Update the state (event) system
} // section 1


#elif section == 2  // --------------------------------------------------------------------------------------------------


float factorUS = 2000.0 / 255.0;
int   offsetUS = 500;
int   servoUS  = 0; 

void loop() {
  if (loopStart) {
    Serial.println(F("(2) 256 max PWM step resolution"));
    boom_angel.set_timers (3000, 100, 0);  // Output, begin, end
    loopStart = false;
  }
  if (boom_angel.get_processState() == COMPLETE) {
    if (changeFlag == true) {
      Serial.print(loopCount); Serial.print(F(": PWM curve: ")); Serial.println(PWMcureveText[PWMcurveNo]);
      boom_angel.activate (MIN_PWM_VALUE, MAX_PWM_VALUE, PWMcurveBegin[PWMcurveNo]);
      changeFlag = false;
    } 
    else {
      boom_angel.activate (MAX_PWM_VALUE, MIN_PWM_VALUE, PWMcurveEnd[PWMcurveNo]);
      changeFlag = true;
      select_curve ();
    }
  }
  else { // 256 step
    // Move servo left-right-pause-right-left-pause ....   
    if (boom_angel.get_outputState ()) {
      servoUS = offsetUS + static_cast<int>(roundf(boom_angel.get_outputValue() * factorUS));
      Nano_servo_1.writeMicroseconds(servoUS);
      Nano_servo_2.writeMicroseconds(servoUS);
    }
  }
 
  MTD2A_loop_execute();  // Update the state (event) system
} // section 2


#elif section == 3  // --------------------------------------------------------------------------------------------------


void loop() {
  if (loopCount == 1) {
    Serial.println(F("(4) PWM Sinus curve - max speed at 0 and 180 degrees"));
    boom_angel.set_timers (1000, 0, 0);  // Output, begin, end
    loopCount = 0;
  }
  switch (stepCount) {
    case 0:
      if (boom_angel.get_processState() == COMPLETE) {
        boom_angel.activate (0, 90, RISING_SIN0);
        Serial.println(F("RISING_SIN0"));
        stepCount = 1;
      }
      break;
    case 1:
      if (boom_angel.get_processState() == COMPLETE) {
        boom_angel.activate (90, 180, RISING_SIN270);
        Serial.println(F("RISING_SIN270"));
        stepCount = 2;
      }
      break;
    case 2:
      if (boom_angel.get_processState() == COMPLETE) {
        boom_angel.activate (180, 90, FALLING_SIN180);
        Serial.println(F("FALLING_SIN180"));
        stepCount = 3;
      }
      break;
    case 3:
      if (boom_angel.get_processState() == COMPLETE) {
        boom_angel.activate (90, 0, FALLING_SIN90);
        Serial.println(F("FALLING_SIN90"));
        stepCount = 0;
      }
      break;
  }
  // Move servo left-right-pause-right-left-pause ....
  if (boom_angel.get_outputState ()) {
    Nano_servo_1.write(boom_angel.get_outputValue()); 
    Nano_servo_2.write(boom_angel.get_outputValue()); 
  } 

  MTD2A_loop_execute();  // Update the state (event) system
}  // section 3


#elif section == 4  // --------------------------------------------------------------------------------------------------


void loop() {
  if (loopCount == 1) {
    Serial.println(F("(5) PWM Sinus curve - max speed at 90 degrees"));
    boom_angel.set_timers (1000, 0, 0);  // Output, begin, end
    loopCount = 0;
  }
  switch (stepCount) {
    case 0:
      if (boom_angel.get_processState() == COMPLETE) {
        boom_angel.activate (0, 90, RISING_SIN270);
        Serial.println(F("RISING_SIN270"));
        stepCount = 1;
      }
      break;
    case 1:
      if (boom_angel.get_processState() == COMPLETE) {
        boom_angel.activate (90, 180, RISING_SIN0);
        Serial.println(F("RISING_SIN0"));
        stepCount = 2;
      }
      break;
    case 2:
      if (boom_angel.get_processState() == COMPLETE) {
        boom_angel.activate (180, 90, FALLING_SIN90);
        Serial.println(F("FALLING_SIN90"));
        stepCount = 3;
      }
      break;
    case 3:
      if (boom_angel.get_processState() == COMPLETE) {
        boom_angel.activate (90, 0, FALLING_SIN180);
        Serial.println(F("FALLING_SIN180"));
        stepCount = 0;
      }
      break;
  } // switch

  // Move servo left-right-pause-right-left-pause ....
  if (boom_angel.get_outputState ()) {
    Nano_servo_1.write(boom_angel.get_outputValue());
    Nano_servo_2.write(boom_angel.get_outputValue());
  }
 
  MTD2A_loop_execute();  // Update the state (event) system
}  // section 4


#elif section == 5  // --------------------------------------------------------------------------------------------------


int  offsetAngel = 0;
int  offsetStep  = 0;

void loop() {
  if (loopStart) {
    boom_angel.set_timers (500, 0, 0);  // Output, begin, end
    Serial.println(F("(6) PWM Sinus amplitude decreasing curve (pendulum) - max speed at 90 degrees"));
    loopStart  = false;
    offsetStep = 0;
  }
  //
  switch (stepCount) {
    case 0:
      if (boom_angel.get_processState() == COMPLETE) {
        offsetAngel += offsetStep;
        boom_angel.activate (offsetAngel - offsetStep, 90, RISING_SIN270);
        Serial.print(F("RISING_SIN270 : ")); Serial.print(offsetAngel); Serial.println (F(" - 90 degrees"));
        stepCount = 1;
      }
      break;
    case 1:
      if (boom_angel.get_processState() == COMPLETE) {
        offsetAngel += offsetStep;
        boom_angel.activate (90, 180 - offsetAngel, RISING_SIN0);
        Serial.print(F("RISING_SIN0:    90 - ")); Serial.print(offsetAngel); Serial.println (F(" degrees"));
        stepCount = 2;
      }
      break;
    case 2:
      if (boom_angel.get_processState() == COMPLETE) {
        offsetAngel += offsetStep;
        boom_angel.activate (180 - (offsetAngel - offsetStep), 90, FALLING_SIN90);
        Serial.print(F("FALLING_SIN90:  ")); Serial.print(offsetAngel); Serial.println (F(" - 90 degrees"));
        stepCount = 3;
      }
      break;
    case 3:
      if (boom_angel.get_processState() == COMPLETE) {
        offsetAngel += offsetStep;
        boom_angel.activate (90, 0 + offsetAngel , FALLING_SIN180);
        Serial.print(F("FALLING_SIN180: 90 - ")); Serial.print(offsetAngel); Serial.println (F(" degrees"));
        offsetStep = 4;
        stepCount = 0;
      }
      break;
  } // switch

  // Move servo left-right-pause-right-left-pause ....
  if (boom_angel.get_outputState ()) {
    Nano_servo_1.write(boom_angel.get_outputValue());
    Nano_servo_2.write(boom_angel.get_outputValue());
  }

  if (offsetAngel >= 86) {
    if (boom_angel.get_processState() == COMPLETE) {
      boom_angel.activate (90, 90, NO_CURVE);
      Serial.println (F("90 degrees"));
      offsetAngel = 0;
      loopCount   = 0;
    }
  }
  loopCount++;
 
  MTD2A_loop_execute();  // Update the state (event) system
}  // section 5


#elif section == 6  // --------------------------------------------------------------------------------------------------


float factorUS = (PCA9685_MAX - PCA9685_MIN) / 255.0F;
int   offsetUS = SERVO_MIN_US;
int   pulseLen = 0;

// 512 max step resolution
void loop() {
  if (loopStart) {
    Serial.println(F("(7) PC9685 PWM 256 max step resolution - setPWM() - 50 Hz"));
    boom_angel.set_timers (3000, 100, 0);  // Output, begin, end
    loopStart = false;
  }
  if (boom_angel.get_processState() == COMPLETE) {
    if (changeFlag == true) {
      Serial.print(loopCount); Serial.print(F(": PWM curve: ")); Serial.println(PWMcureveText[PWMcurveNo]);
      boom_angel.activate (MIN_PWM_VALUE, MAX_PWM_VALUE, PWMcurveBegin[PWMcurveNo]);
      changeFlag = false;
    } 
    else {
      boom_angel.activate (MAX_PWM_VALUE, MIN_PWM_VALUE, PWMcurveEnd[PWMcurveNo]);
      changeFlag = true;
      select_curve ();
    }
  }
  else { // 256 step
    if (boom_angel.get_outputState ()) {
      pulseLen = PCA9685_MIN + static_cast<int>(roundf(boom_angel.get_outputValue() * factorUS));
      PCA9685_servo.setPWM(PCA9685_PIN, 0, pulseLen);

    }
  }

  MTD2A_loop_execute();  // Update the state (event) system
} // section 6


# endif


// helper funtion -------------------------------------------------------------------------------------------


void select_curve () {
  if (loopCount < MAX_CURVES * 2) {
    if ((loopCount) %2 == 0) {
      PWMcurveNo++;
    }
    loopCount++;
  }
  else {
    loopCount  = 1;
    PWMcurveNo = 0;
  }     
} // select_curve
