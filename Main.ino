// =============================================================
// ==                      Include Files                      ==
// =============================================================

#include <Arduino.h>
#include "Display.h"
#include "testDisplay.h"
#include "purgeDisplay.h"
#include "TempControl.h"
#include "PumpComms.h"
#include <Wire.h>
#include <Adafruit_INA219.h>
#include "SPI.h"
#include "GlobalVars.h"
#include "Button.h"
#include "readTemp.h"
#include "airFlow.h"

// =============================================================
// ==                    RTD Setup 
//             Using INA219 volatage/current sensor           ==
// =============================================================

const int thermoSDA = 19;  // Data
const int thermoSCL = 18; // Clock
const int tempInput = 34; // Pin to read temperature 

Adafruit_INA219 INA; // Declare INA object

// =============================================================
// ==                      Timer Values                       ==
// =============================================================
// ==  Timer0 controls the spray duration                     ==
// ==  Timer1 controls the update rate of the progress bar    ==
// ==  All times are set in microseconds (1e6 = 1 second)     ==
// =============================================================

// Pointers to timer hardware configurations
hw_timer_t *Timer0_Cfg = NULL; 
hw_timer_t *Timer1_Cfg = NULL;

float spraytimer = 10 * 1e6; // Spray duration (15 seconds)
float progresstimer = 0.1 * 1e6; //Progress bar update interval (0.15 sec)

float purgeTimer = 0; // Timer for purge duration
float flushDuration = 0.15 *60000; // Flush duration in ms
float dryDuration = 0.15 * 60000; // Dry duration in ms 
int flushTime; // Timer for flushing duration
int dryTime; // Timer for drying duration

// =============================================================
// ==                  Controller Variables                   ==
// =============================================================

int duty = 0; // start the system with heaters fully OFF for safety 

float last_time = 0; // Time of previous PID update
float error; // Difference between setpoint and actual temperature


// =============================================================
// ==          Steady-State Detection Variables               ==
// =============================================================

const float tempTolerance = 1.0;
float steadyTime = 0; // Variable to store the amount of time for which the temperature is within tolerance
float steadyDuration = 5000; // The desired time (ms) in which the temperature is within tolerance - Once this time is reached the timers and the testing will begin
bool steadyreached = false;

// =============================================================
// ==                     Logic Flags                         ==
// =============================================================
// ==  Logic flags ensure certain functions in the main loop  ==
// ==  are executed only ONCE. The system continuously polls  ==
// ==  for specific conditions, and when met, triggers a      ==
// ==  function. The flag is then cleared to avoid repeats.   ==
// =============================================================

// Blockers - these flags are set up to block purging during testing and vice versa 
bool purgeBlock = false;
bool testBlock = false;

// Spinning widget during idle mode
bool widgetOn = true;

// Flags to be raised throughout purging process
bool flushed = false; // Has the system been flushed
bool dried = false; // Has the system been dried
bool purgeComplete = false; //Has the purging process completed yet

static bool prePurgeStarted = false;
static unsigned long prePurgeTimer = 0;

static bool preTestStarted = false;
static unsigned long preTestTimer = 0;
static bool preTestTriggered = false; // has this pre-test started for this button press?

bool timersStarted = false; // Prevents multiple calls of timer functions

// Warning flags to shutoff system
static unsigned long warningIgnoreUntil = 0;  // Time until which to ignore warnings
float tempOB = setpoint + 10;  // Failsafe in case of unsafe temperatures 
bool warning = true; // Warning flag to be raised once temperature is unsafe 

static unsigned long completionStart = 0;  // remember when completion started
volatile bool completemessage = false; // Flag that is raised once spray duraton is complete

// ===========================================================
//                          widget                          ==
// ===========================================================

int widget = 0;   

String widgetChar[] = {
  "|",
  "/",
  "-",
  "\\"  // two backslashes required - special character 
};

// =============================================================
// ==                  Timer Interrupts                       ==
// =============================================================
// ==  Timer0_ISR: Tracks the main spray duration and         ==
// ==  singnals completion of test                            ==
// ==                                                         ==
// ==  Timer1_ISR: Updates the progress bar by incrementing   ==
// ==  'fill' until it reaches 100, indicating completion.    ==
// =============================================================

void IRAM_ATTR Timer0_ISR() // Function is called every time the timer goes off 
{
  complete++;
  completemessage = true;
  timersStarted = false;
  purgeBlock = false;
}

void IRAM_ATTR Timer1_ISR() // Function is called every time the timer goes off 
{
  if (fill < 100) { // Once fill = 100, the timer is complete
    fill++;
  }
}


// =============================================================
// ==                  Initialise Timers                      ==
// =============================================================

void setupTimers() {

  Timer0_Cfg = timerBegin(0, 80, true); // Timer 0, PSC = 80, Count up
  timerAttachInterrupt(Timer0_Cfg, &Timer0_ISR, true); // Attach ISR to interrupt handler
  timerAlarmWrite(Timer0_Cfg, spraytimer, true); // Set timer duration 
  timerAlarmEnable(Timer0_Cfg); // Enable Timer0 alarm

  Timer1_Cfg = timerBegin(1, 80, true); // Timer 1, PSC = 80, Count up
  timerAttachInterrupt(Timer1_Cfg, &Timer1_ISR, true); // Attach ISR to interrupt handler
  timerAlarmWrite(Timer1_Cfg, progresstimer, true); // Set timer duration 
  timerAlarmEnable(Timer1_Cfg); //Enable Timer0 alarm
  
}

// =============================================================
// ==                Handle the spinner widge                 ==
// =============================================================

void handleWidget() {
 
 updateWidget(widgetChar[widget]);
 widget++;
 if (widget >= 4){widget = 0;};  
}

// =============================================================
// ==                    Main Function                        ==
// =============================================================

void setup() {
  Serial.begin(115200);
  valveOff();
  String message = pumpSpeed();
  idleDisplay(purgeComplete); 
  setupPumpComms();  //Initialise pump comms
  stopPump();
  setupPWM(); // Initilaise PWM for controller
  analogReadResolution(12);  // ESP32 ADC resolution
  setupButtons(); // Initialise buttons for user control 
 
  Wire.begin(thermoSDA, thermoSCL);  //set non-default pins for INA219 for INA
  
  if (! INA.begin()) {
    Serial.println("Failed to find INA219 chip");
    while (1) { delay(10); }
  }

  last_time = millis(); // Set inital 'last time' to the time at which program starts
}

// =============================================================
// ==                      Main Loop                          ==
// =============================================================
void loop() {

  String message = pumpSpeed();
  String speed = extractSpeed(message);
  String direction = extractDirection(message);
  String status = extractRunning(message);

  float actual = temp(tempInput, INA);

  // --- Steady-state detection ---
  if (fabs(setpoint - actual) <= tempTolerance) {
      // Temp within tolerance: accumulate time
      steadyTime += (millis() - last_time);
  } else {
      // Temp outside tolerance: reset timer
      steadyTime = 0;
      steadyReached = false;
  }
  last_time = millis();

  // If held steady for required duration, mark as ready
  if (steadyTime >= steadyDuration && !steadyReached) {
      steadyReached = true;        // Signal readiness
      warmUpdisplay();             // Show standby-for-heating message
      Serial.println("Steady state achieved – ready for test");
  }


  static bool ignoreTestTempSpike = false;



  // Run Test Mode 
  // Checks for 3 conditions before starting test:
  // 1. Button pressed
  // 2. Timers not yet started 
  // 3. Testing is not being blocked

  // Trigger pre-test when button is first pressed
  if (readTestButton() && !timersStarted && !testBlock && !preTestStarted && steadyReached) {
    ignoreTestTempSpike = true;               // Ignore temp spike
    warningIgnoreUntil = millis() + 3000;
    valveOn(); // Start solenoid
    preTestTimer = millis(); // Start pre-test timer
    preTestStarted = true; // Raise flag to indicate pre-test is active
    valveBuffer(); // Priming display
    widgetOn = false;
    purgeBlock = true;
  }

  // ==== Check for temperature failsafe OR stop button ====

  bool ignoreWarning = millis() < warningIgnoreUntil;

if ((!ignoreWarning && actual > tempOB && warning && !ignoreTestTempSpike) || readStopButton()) {    // Stop all operations immediately
    ledcWrite(channel, 0); // Heaters off
    stopPump(); // Pump stopped
    timersStarted = false; // Timers off
    purgeBlock = false; // Stop purging from occurring
    testBlock = false; // Stop testing from occurring
    purgeComplete = false; // Reset purge completion
    completemessage = false; // Reset test completion
    fill = 0; // Reset progress
    purgeTimer = 0; 
    complete = 0;
    widgetOn = false;

    if (!readStopButton()) {  // Only show warning if it's over-temp (not STOP)
      warningDisplay();
      warning = false;
    }

    idleDisplay(false);
    return;
  }

  // Once valve priming is completed
  if (preTestStarted && millis() - preTestTimer >= 3000) {
    testDisplay();
    startPump(); // Start pump
    setupTimers();             
    timersStarted = true;     
    preTestStarted = false; // Reset pre-test flag
  }

  //  Run Purge Mode 
  // Checks for 2 conditions before starting purge:
  // 1. Button pressed
  // 2. Purging is not being blocked
  if (readPurgeButton() && !purgeBlock && !prePurgeStarted) {
    valveOn(); // Start solenoid
    prePurgeTimer = millis(); // Start pre-purge timer
    prePurgeStarted = true;
    valveBuffer(); // Priming display
    widgetOn = false;
    testBlock = true; // Block test during purge
  }

  // Check if pre-purge timer has completed
  if (prePurgeStarted && millis() - prePurgeTimer >= 3000) {
    purgeDisplay(); // Show purge active display
    startPump(); // Start pump
    purgeTimer = millis(); // Start purge timer
    flushed = false;
    dried = false;
    dryTime = 0;
    prePurgeStarted = false; // Reset pre-purge flag
    purgeBlock = true; // Keep purge block active
  }

  // Display during flusing stage of purge mode
  if (purgeTimer > 0 && millis() - purgeTimer <= flushDuration) {
    unsigned long elapsed = millis() - purgeTimer;
    unsigned long remaining = flushDuration - elapsed;
    purgeFlush(remaining);
  }

 if (purgeTimer > 0 && millis() - purgeTimer > flushDuration && !flushed) {
    flushed = true;
    purgeTimer = 0;
    stopPump();
    dryTime = millis();
 }

 if (dryTime > 0 && millis() - dryTime <= dryDuration) {
    unsigned long elapsedDry = millis() - dryTime;
    unsigned long remainingDry = dryDuration - elapsedDry;
    stopPump();
    purgeDry(remainingDry);
 }

 if (dryTime > 0 && millis() - dryTime > dryDuration && !dried) {
    dried = true;
    purgeComplete = true;
    valveOff();
    idleDisplay(purgeComplete); 
    widgetOn = true;
    testBlock = false;
    purgeBlock = false;
 }

  // Progress Bar 
  if (timersStarted){
      progBar();
  }

  // Completion Message 
  static unsigned long completionStart = 0;  // remember when completion started
  if (complete == 1 && completemessage) {
    if (completionStart == 0) {
      completionStart = millis();  // first time entering completion
      stopPump();
      completionDisplay(); // show completion
      valveOff();
    }
    else if (millis() - completionStart >= 1500) {  // 2 seconds elapsed
      idleDisplay(false); // return to idle 
      completionStart = 0; // reset for next completion
      completemessage = false; // reset flag so completion isn't repeated
    }
  }

  // Controller 
  float pid_output =  0;

  if(timersStarted){
    pid_output = pid(setpoint, actual, last_time);
  }

  relaySignal(pid_output, setpoint, actual);

  if (!steadyReached) {
    warmUpdisplay();  // Show "Standby for Heating" until steady state is achieved
  }


  if (!completemessage){
    updateIdleValues(setpoint, actual, constrain(pid_output, 0, 100));
    updatePumpInfo(speed, direction, status);  
    

  }


  // Widget
  if (widgetOn){
      handleWidget();
  }

  delay(250);
}
