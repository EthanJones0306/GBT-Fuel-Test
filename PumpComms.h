#ifndef PumpComms_H
#define PumpComms_H

#include <Arduino.h>



void setupPumpComms();
void startPump();
void stopPump();
String pumpSpeed();
String extractSpeed(String message);
String extractDirection(String message);
String extractRunning(String message);






#endif