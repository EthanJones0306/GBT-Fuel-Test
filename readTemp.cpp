// =============================================================
// ==                      Include Files                      ==
// =============================================================

#include <SPI.h>
#include <Arduino.h>
#include "GlobalVars.h"
#include <Adafruit_INA219.h>

// =============================================================
// ==                  Conversion Constants                   ==
// =============================================================

const float SHUNT_OHMS = 150.0;   // external shunt resistor value (Ω)
const float TEMP_SCALE = 28.125;  // °C per mA above 4 mA 
const float tempOffset = 0.014;

// =============================================================
// ==             Signal Conditioning Constants               ==
// =============================================================


float tempFiltered = 0;    // EMA filtered temperature
// Smoothing
const float alpha = 0.7;   // smoothing factor (0 < alpha < 1 where 0 = hihgly smooth but unresponsive and 1 is not smooth but highly responsive


float temp(int inputPin, Adafruit_INA219 &INA){
  float busV = INA.getBusVoltage_V()-tempOffset; // Measure voltage across shunt resistor
  float bus_mV = busV * 1000.0; // convert to mV

  float current_mA = bus_mV / SHUNT_OHMS; // Ohm's law

  float temperatureC = (current_mA - 4.0) * TEMP_SCALE;
  if (tempFiltered == 0) {
    tempFiltered = temperatureC; // first reading sets initial EMA
  } 
  else {
    tempFiltered = alpha * temperatureC + (1 - alpha) * tempFiltered;
  }
  return tempFiltered;
}

