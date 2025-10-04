#ifndef BUTTON_H
#define BUTTON_H

#include <Arduino.h>

const int buttonPin = 26;

void setupButtons();
bool readPurgeButton();
bool readTestButton();
bool readStopButton();

#endif
