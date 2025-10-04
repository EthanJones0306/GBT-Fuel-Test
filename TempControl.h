#ifndef TEMPCONTROL_H
#define TEMPCONTROL_H

#include <Arduino.h>
#include "math.h"
#include "SPI.h"

constexpr int channel = 0;
constexpr int freq = 1;
constexpr int res = 16;
constexpr int maxDuty = (1 << res) - 1;
extern float dutyCycle;

void setupPWM();
//float controlSetup(float setpoint,float actual, float last_time);
//float pid(float error, float dt);

float pid(float sp,float act, float lt);
void relaySignal(float pidout, float sp, float act);


#endif