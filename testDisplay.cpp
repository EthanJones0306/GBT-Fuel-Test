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
// ==                 Initialize Timer Flags                  ==
// =============================================================

// These need to be volatile to be used and changed in the main file 
volatile int fill = 0; // The main variable to be used to track progress
volatile int complete = 0; // The main variable to track the completion of the spray duration

// =============================================================
// ==                Wait for valve to start                  ==
// =============================================================

void valveBuffer(){
  tft.fillScreen(ST77XX_BLACK);
  
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
  tft.setTextSize(1);
  tft.setCursor(10, 90);
  tft.print("Starting Airflow");
}

// =============================================================
// ==                  Progress Display                       ==
// =============================================================
// Display during testing 
void testDisplay(){

  // mySPI.begin(TFT_SCK,// SCK pin for display (CLK)
  //             -1, // MISO not used
  //             TFT_MOSI, // MOSI pin for display (DIN)
  //             TFT_CS); // optional SS pin
  // mySPI.setFrequency(27000000); 

  tft.initR(INITR_BLACKTAB); // Use INITR_BLACKTAB for this display

  tft.fillScreen(ST77XX_BLACK);

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

  tft.setCursor(25, 90);
  tft.setTextSize(1);
  tft.print("Test Progress");
  tft.drawRect(14,104,100,28,ST77XX_RED); //bar
  
}

// =============================================================
// ==                  Progress Bar                           ==
// =============================================================
// Increments progress bar using filler variable
void progBar(){
  tft.fillRect(15,105,fill,26,ST77XX_GREEN);// Fill bar

  tft.fillRect(120, 112, 50, 26, ST77XX_BLACK); // x, y, width, height

  // Calculate percentage
  int percent = map(fill, 0, 100, 0, 100); // Assuming fill goes 0-100

  // Print the percentage
  tft.setCursor(60, 145); // Adjust x, y to position text nicely
  tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK); // White text on black background
  tft.setTextSize(1);
  tft.print(percent);
  tft.print("%");
}

// =============================================================
// ==                  Completion Display                     ==
// =============================================================

void completionDisplay(){
  tft.fillScreen(ST77XX_BLACK);  
  tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
  tft.setTextSize(2);  
  tft.setCursor(40, 10);
  tft.println("Test");
  tft.setCursor(20, 30);
  tft.println("Complete");

  tft.drawRect(10,70,100,30,0x9d8189); // X position for top left corner, Y position for top left corner, width, height, colour

  tft.fillRect(11,71,98,28,ST77XX_GREEN); // X position for top left corner, Y position for top left corner, width, height, colour

  tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
  tft.setTextSize(2);
  tft.setCursor(40, 105);
  tft.print("100");

    // Print the percent sign
  tft.setTextSize(2);
  tft.setCursor(80, 105);
  tft.print("%");

}
