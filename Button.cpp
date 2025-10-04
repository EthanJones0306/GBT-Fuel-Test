// =============================================================
// ==                      Include Files                      ==
// =============================================================
#include <Arduino.h>

// =============================================================
// ==                    Define GPIO Pins                     ==
// =============================================================
const int purgeButtonPin = 26;
const int testButtonPin  = 19;
const int stopButtonPin  = 32;

// =============================================================
// ==                Button Logic Variables                   ==
// =============================================================

// Purge button
int purgeState = HIGH;
int purgeLastState = HIGH;
unsigned long purgeLastDebounce = 0;
unsigned long debounceDelay = 20;

// Test button
int testState = HIGH;
int testLastState = HIGH;
unsigned long testLastDebounce = 0;

// Stop button
int stopState = HIGH;
int stopLastState = HIGH;
unsigned long stopLastDebounce = 0;

// =============================================================
// ==                  Setup Buttons                          ==
// =============================================================
void setupButtons() {
  pinMode(purgeButtonPin, INPUT_PULLUP);
  pinMode(testButtonPin, INPUT_PULLUP);
  pinMode(stopButtonPin, INPUT_PULLUP);
}

// =============================================================
// ==                Read Button Functions                    ==
// =============================================================

bool readPurgeButton() {
  int reading = digitalRead(purgeButtonPin);

  if (reading != purgeLastState) {
    purgeLastDebounce = millis();
  }

  if ((millis() - purgeLastDebounce) > debounceDelay) {
    if (reading != purgeState) {
      purgeState = reading;
      if (purgeState == LOW) {
        purgeLastState = reading;
        return true;
      }
    }
  }

  purgeLastState = reading;
  return false;
}

bool readTestButton() {
  int reading = digitalRead(testButtonPin);

  if (reading != testLastState) {
    testLastDebounce = millis();
  }

  if ((millis() - testLastDebounce) > debounceDelay) {
    if (reading != testState) {
      testState = reading;
      if (testState == LOW) {
        testLastState = reading;
        return true;
      }
    }
  }

  testLastState = reading;
  return false;
}

bool readStopButton() {
  int reading = digitalRead(stopButtonPin);

  if (reading != stopLastState) {
    stopLastDebounce = millis();
  }

  if ((millis() - stopLastDebounce) > debounceDelay) {
    if (reading != stopState) {
      stopState = reading;
      if (stopState == LOW) {
        stopLastState = reading;
        return true;
      }
    }
  }

  stopLastState = reading;
  return false;
}
