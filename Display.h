#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>

extern Adafruit_ST7735 tft; 
extern volatile int fill; 
extern volatile int complete; 

void idleDisplay(bool purge);
void updateIdleValues(float set, float act, int dty);
void updatePumpInfo(String sp, String dir, String stat);
void purgeDisplay();
void testDisplay();
void progBar();
void setupDisplay2();
void completionDisplay();
void warningDisplay();
void warmUpdisplay();
void updateWidget(String widget);

#endif