#include <Arduino.h>

//=============================================================
//==              temperature control                        ==
//=============================================================

float setpoint = 110.00; // Target temp in °C

//=============================================================
//==    Stauts control, int and array                        ==
//=============================================================

int Status = 0;  //Sets status to "standby" on startup

String StatusDescr[] = {
  "Standby",
  "Heating",
  "Running",
  "Complete",
  "Purge"
};
