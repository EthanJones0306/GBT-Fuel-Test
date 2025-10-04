#ifndef PURGEDISPLAY_H
#define PURGEDISPLAY_H

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>

void purgeDisplay();
void purgeFlush(int flushTime);
void purgeDry(int dryTime);

#endif