// =============================================================
// ==                      Include Files                      ==
// =============================================================

#include <Arduino.h>
#include "SPI.h"
#include "GlobalVars.h"

// =============================================================
// ==                      GPIO Pins                          ==
// =============================================================

int rx = 5; // Receive pin
int tx = 4; // Transmit pin



// =============================================================
// ==               Initialize Communication                  ==
// =============================================================

void setupPumpComms() {
 // Serial.begin(115200);  
  delay(1000);
  Serial1.begin(9600, SERIAL_8N2, rx, tx);  // UART1 initalization

}

//===============================================================
//==            pump control                                   ==
//===============================================================

void startPump() {

  Serial1.print("1SP015\r"); 
  //Serial1.print("1RC\R");
  Serial1.print("1GO\r");
}

void stopPump() {
  Serial1.print("1ST\r");
  Serial1.print("1ST\r");
  Serial1.print("1ST\r");
  Serial1.print("1ST\r");


}

//===============================================================
//==            pump status queries                            ==
//===============================================================
String pumpSpeed() {
  String message = "";

  Serial1.print("1RS\r");

  if (Serial1.available() > 0) {
    String receivedMessage = Serial1.readStringUntil('!');
    receivedMessage.trim();

  

    // Remove "Message: " prefix if it exists
    if (receivedMessage.startsWith("Message: ")) {
      message = receivedMessage.substring(8);  // Remove "Message: "
    } else {
      message = receivedMessage;
    }
  }

  return message;  // Return the full message string (e.g. "323Du 15.0 CW 1")
}

String extractSpeed(String message) {
  int firstSpace = message.indexOf(' ');
  int secondSpace = message.indexOf(' ', firstSpace + 1);
  return message.substring(firstSpace + 1, secondSpace);
}

String extractDirection(String message) {
  int secondSpace = message.indexOf(' ', message.indexOf(' ') + 1);
  int thirdSpace = message.indexOf(' ', secondSpace + 1);
  return message.substring(secondSpace + 1, thirdSpace);
}

String extractRunning(String message) {
  int lastSpace = message.lastIndexOf(' ');
  return message.substring(lastSpace + 1);
}

