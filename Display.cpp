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
// ==                    Define GPIO Pins                     ==
// =============================================================

#define TFT_CS 22 // CS
#define TFT_RST 27 // RESET
#define TFT_DC 2 // DC

#define TFT_MOSI 23 
#define TFT_SCK 14

SPIClass mySPI(VSPI);

//==============================================================
//== char and int for correct justification using sprintf     ==
//==============================================================

int iset;
int iact;
int idty;
char csetpoint[3];
char cactual[3];
char cduty[3];

// =============================================================
// ==                 Initialize Timer Flags                  ==
// =============================================================

Adafruit_ST7735 tft(&mySPI,TFT_CS, TFT_DC, TFT_RST);

// =============================================================
// ==                    Idle Display                         ==
// =============================================================
// This function indicates to the user that the system is in an idle state
void idleDisplay(bool purge) {
  mySPI.begin(TFT_SCK, -1, TFT_MOSI, TFT_CS); 
  mySPI.setFrequency(27000000); 

  tft.initR(INITR_BLACKTAB); 
  tft.fillScreen(ST77XX_BLACK);

  // === Top Left Block === //
  tft.drawRect(0, 0, 64, 80, 0x9d8189); 
  tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(20, 7);  tft.println("Pump");
  tft.setCursor(15, 17); tft.println("status:");
  tft.setCursor(5, 60);  tft.print("Spd:");
  tft.setCursor(5, 70);  tft.print("Dir:");

  // === Top Right Block === //
  tft.drawRect(64,0,64,80,0x9d8189);
  tft.setTextColor(ST77XX_CYAN, ST77XX_BLACK);
  tft.setCursor(80, 2); tft.println("Temp:");
  tft.drawLine(80, 9, 108, 9, ST77XX_CYAN);
  tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
  tft.setCursor(65, 13); tft.println("Set:");
  tft.setCursor(65, 35); tft.println("Tmp:");
  tft.setCursor(65, 56); tft.println("Duty:");

  // === Purge Indicator === //
  tft.setCursor(5,115);
  tft.print("Purge Since Startup?");
  if (purge) {
    tft.fillCircle(50, 140, 10, ST77XX_GREEN);
    tft.setCursor(70, 135);
    tft.setTextColor(ST77XX_GREEN);
    tft.print("YES");
  } else {
    tft.fillCircle(50, 140, 10, ST77XX_RED);
    tft.setCursor(70, 135);
    tft.setTextColor(ST77XX_RED);
    tft.print("NO");
  }
  tft.setCursor(5, 90);
  tft.setTextSize(2);
  tft.setTextColor(ST77XX_CYAN, ST77XX_BLACK);
  tft.print("IDLE");
}


void updateIdleValues(float set, float act, int dty) {
  tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
  tft.setTextSize(1);
  // Update setpoint
  tft.setCursor(90, 13);
  sprintf(csetpoint,"%3d", (int)set);
  tft.print(csetpoint);
  tft.write(247);  
  tft.print("C");

  // Update actual
  tft.setCursor(90, 35);
  sprintf(cactual,"%3d", (int)act);
  tft.print(cactual);
  tft.write(247);  
  tft.print("C");

  // Update duty
  tft.setCursor(90, 56);
  sprintf(cduty,"%3d", dty);
  tft.print(cduty);
  tft.print("%");
}

void updatePumpInfo(String sp, String dir, String stat) {
  // Pump ON/OFF indicator
  if (stat == "1") {
    tft.fillCircle(30, 40, 8, ST77XX_GREEN);
  } else {
    tft.fillCircle(30, 40, 8, ST77XX_RED);
  }

  // Speed and direction
  tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
  tft.fillRect(30, 60, 15, 5, ST77XX_BLACK); // Clear old speed
  tft.setCursor(30, 60);
  tft.print(sp);

  tft.fillRect(30, 70, 15, 5, ST77XX_BLACK); // Clear old direction
  tft.setCursor(30, 70);
  tft.print(dir);
}

// =============================================================
// ==           spinner widget to show comms                  ==
// =============================================================

void updateWidget(String widget){
  tft.setTextColor(ST77XX_GREEN, ST77XX_BLACK);
  tft.setTextSize(2);
  tft.setCursor(90, 95);
  tft.print(widget);

}

void warmUpdisplay(){
  tft.setCursor(5, 90);
  tft.setTextSize(2);
  tft.setTextColor(ST77XX_CYAN, ST77XX_BLACK);
  tft.print("Standby");
  tft.setCursor(5, 105);
  tft.print("for");
  tft.setCursor(5 120);
  tft.print("Heating")
}
// =============================================================
// ==                   Warning Display                       ==
// =============================================================

void warningDisplay(){
  // Clear entire screen with background color
    tft.fillScreen(ST77XX_RED);  
    tft.setTextColor(ST77XX_WHITE);
    tft.setTextSize(2); // 
    tft.setCursor(10, 10);
    tft.println("Unsafe Temperature");
}