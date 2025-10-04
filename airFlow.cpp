// =============================================================
// ==                      Include Files                      ==
// =============================================================

#include <Arduino.h>

// =============================================================
// ==                      Define Pins                        ==
// =============================================================

const uint8_t valvePin = 13;

// Turn solenoid valve on
void valveOn(){
  pinMode(valvePin, OUTPUT);
  digitalWrite(valvePin, HIGH);  // Ensure solenoid is off initially
}

// Turn solenoid valve off
void valveOff(){
  pinMode(valvePin, OUTPUT);
  digitalWrite(valvePin, LOW);
}
