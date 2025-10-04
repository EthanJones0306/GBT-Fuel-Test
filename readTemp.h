#ifndef TEMP_READER_H
#define TEMP_READER_H

#include <Arduino.h>
#include <Adafruit_INA219.h>

float temp(int inputPin, Adafruit_INA219 &INA); 

#endif 
