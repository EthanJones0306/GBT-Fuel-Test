// =============================================================
// ==                      Include Files                      ==
// =============================================================

#include <Arduino.h>
#include "Display.h"
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>
#include "GlobalVars.h"
#include "PumpComms.h"

// =============================================================
// ==                    Purge Display                         ==
// =============================================================

void purgeDisplay(){
  tft.fillScreen(ST77XX_BLACK);
  
  // ============= Top Left ============= //

  tft.drawRect(0,0,64,80,0x9d8189); // Outline block

  // === Top Left Block === //
  tft.drawRect(0, 0, 64, 80, 0x9d8189); 
  tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(20, 7);  tft.println("Pump");
  tft.setCursor(15, 17); tft.println("status:");
  tft.setCursor(5, 60);  tft.print("Spd:");
  tft.setCursor(5, 70);  tft.print("Dir:");


  // ============= Top Right ============= //

  tft.drawRect(64,0,64,80,0x9d8189); // Outline block

  // Heading
  tft.setTextColor(ST77XX_CYAN, ST77XX_BLACK); 
  tft.setCursor(80, 2);
  tft.setTextSize(0.5);     
  tft.println("Temp:"); // Abreviated due to spacing isues
  tft.drawLine(80, 9, 68 + 40, 9, ST77XX_CYAN); // adjust 35 to match text width

  tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
  tft.setCursor(66, 13);
  tft.println("Set:");    
  tft.setCursor(92, 13);
  
  tft.setCursor(66, 35);
  tft.println("Temp:");

  tft.setCursor(66, 56);
  tft.println("Duty:");
  
  // ============= Bottom ============= //
  tft.setTextColor(ST77XX_CYAN, ST77XX_BLACK); 
  tft.setTextSize(2);
  tft.setCursor(10, 90);
  tft.print("Purging");
}

void purgeFlush(int flushTime){

 // ============= Bottom ============= //
  tft.fillRect(120, 112, 50, 26, ST77XX_BLACK); // x, y, width, height

  // Calculate percentage
  int percent = map(fill, 0, 100, 0, 100); // Assuming fill goes 0-100

  // Print the label
  tft.setCursor(4, 125);
  tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
  tft.setTextSize(1);
  tft.print("Flush Time Remaining");

  // Convert ms to seconds
  float seconds = flushTime / 1000.0;

  // Print the remaining time with " s"
  tft.setCursor(50, 145);
  tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
  tft.setTextSize(1);
  tft.print(seconds, 1);  // one decimal place
  tft.print(" s");
}

void purgeDry(int dryTime){
  // ============= Bottom ============= //
  //tft.fillRect(130, 112, 50, 26, ST77XX_BLACK); // x, y, width, height

  // Calculate percentage
  int percent = map(fill, 0, 100, 0, 100); // Assuming fill goes 0-100

  // Print the label
  tft.setCursor(4, 115);
  tft.setTextColor(ST77XX_CYAN, ST77XX_BLACK); 
  tft.setTextSize(1);
  tft.print("Flush Complete");

  // Print the label
  tft.setCursor(4, 125);
  tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
  tft.setTextSize(1);
  tft.print("Dry Time Remaining");

  // Convert ms to seconds
  float seconds = dryTime / 1000.0;

  // Print the remaining time with " s"
  tft.setCursor(50, 145);
  tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
  tft.setTextSize(1);
  tft.print(seconds, 1);  // one decimal place
  tft.print(" s");
}
