// =============================================================
// ==                      Include Files                      ==
// =============================================================

#include "math.h"
#include <SPI.h>
#include <Arduino.h>
#include "GlobalVars.h"

// =============================================================
// ==                     Define IO Pins                      ==
// =============================================================

const int outputPin = 25; // PWM output to SSR
const int channel = 0; // PWM channel

// =============================================================
// ==                Define PWM Parameters                   ==
// =============================================================

const int freq = 1; // 1 Hz -- this can be adjusted 
const int res = 16; // 16-bit resolution
const int maxDuty = ( 1 << res) - 1; // Equivalent to 2^res -1

// =============================================================
// ==                    PID Variables                       ==
// =============================================================

float dt = 0.0; //Change in time
// volatile float last_time = 0.0; // Timestamp for previous loop iteration
float integral =0; // Accumulate derror for integral controller
float previous = 0; // Previous error for derivative controller
float controlOutput = 0; // PID controller output
float dutyCycle = 0; // for passing duty cycle to main for display


// =============================================================
// ==                    PID Constants                       ==
// =============================================================

// Increases reaction to error
float kp = 2.75; 

// Reacts to steady state error
float ki = 0;

// Reacts to rate of change
float kd = 0; 

// =============================================================
// ==                     Set Up PWM                          ==
// =============================================================

void setupPWM()
{
 // Serial.begin(115200); 
  
  // Setup PWM on selected pin & channel
  ledcSetup(channel, freq, res); // This function uses the slected channel, frequency, and resolution to initialise the PWM signal
  ledcAttachPin(outputPin, channel); // Selects the pin to output the PWM signal
  ledcWrite(channel, 0); // heater OFF initially

  // last_time = millis(); // Stores the time that the board has been running the current program in ms

}

// =============================================================
// ==                   PID Controller                        ==
// =============================================================

float pid(float sp,float act, float lt){ // setpoint, actual temp, last_time -- use different names as actual defined variables to avoid complier issues
  // Controller
  float now = millis(); // Variable to store the current time
  float dt = (now - lt) / 1000.0; // delta time in seconds
  if (dt<= 0){
    dt = 0.001;
  }
  lt = now; // Constantly update time
  float error = sp+1 - act; // Error signal for control system
  float proportional = error; // Proportional controller
  integral += error * dt; // integral controller
  float derivative = (error - previous) / dt; // derivative controller
  previous = error; // Calculate incremental change after each iteration to be used for derivative controller 

  float output = (kp * proportional) + (ki * integral) + (kd * derivative); // Output controlled signal - acts as a multiplier for the duty cycle as a response to adjust the heat
  return output; // Can be adjusted to return error if desired 
}

// =============================================================
// ==                    Relay Signal                         ==
// =============================================================
// ==  Send PWM signal to relay or output control hardware.   ==
// ==  Also prints key values for user feedback via Serial:   ==
// ==  Setpoint, Actual Temp, and Duty Cycle.                 ==
// =============================================================

void relaySignal(float pidout, float sp, float act){ // Controlled signal, setpoint, actual temp
  float scaled = constrain(pidout, 0, 100); // Number to constrain, Lower Bound, Upper Bound
  dutyCycle = scaled; //global variable for display
  int duty = (int)(scaled / 100.0 * maxDuty); // Duty cycle for PWM which is to be controlled by the PID and scaled to the resolution

  if (isnan(duty)){ // Safety precaution - duty = NAN output duty cycle is 100% -- This keeps heaters fully on 
    duty = 0;
  }

  ledcWrite(channel, duty); // Output PWM
  Serial.printf("Set: %.2f°C, Actual: %.2f°C, Duty: %.1f%%\n", sp, act, scaled);

}