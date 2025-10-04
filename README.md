# Fuel Testing Automation – ESP32

This project automates a gasoline intake valve deposit (IVD) testing process based on the Chinese standard **GB/T 37322-2019**.  
The automation is built on the **ESP32 microcontroller**, integrating embedded systems, electronics, and temperature control to replace manual operations with a reliable automated workflow.  

---

## Features
- Heating control: Custom-designed heat controller to regulate test temperatures with precision.  
- Pump and solenoid automation: Automated control for purging, priming, and airflow cycles.  
- Timing and sequencing: State machine logic ensures correct test order and delays.  
- TFT user interface: Real-time display of pump status, airflow direction, speed, and heater status.  
- Safety features: Includes purge-before-test cycles and timing safeguards.  

---

## Hardware
- ESP32 microcontroller  
- TFT display (ST7735 / ST77XX family)  
- Solenoid valves and pump control  
- Temperature sensors and heating element  

---

## Project Structure
- `src/` → ESP32 Arduino code (C++ / Arduino framework)  
- `airFlow.*` → Control logic for airflow and solenoid operations  
- `heatControl.*` → Heater control and temperature management  
- `display.*` → TFT display functions for test states (idle, warmup, purge, test)  

---

## Process Flow
1. **Warm-Up Stage** – Heaters turn on and system waits until steady temperature is reached.  
2. **Pre-Test Stage** – Pump/solenoid runs briefly before testing to prime the system.  
3. **Test Stage** – Automated timing and flow control based on the standard.  
4. **Purge Stage** – System flushes to prepare for the next test.  

---

## Future Improvements
- Data logging (SD card or cloud integration)  
- PID control for more accurate heating  
- Web dashboard for remote monitoring  

---

## Author
Developed by **Ethan Jones** during an engineering internship to automate a lab testing process.  
Focus areas included **embedded systems, microcontrollers, and control design** to bridge laboratory standards with real-world automation.  
