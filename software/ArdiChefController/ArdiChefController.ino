/******************************************************************************
 * ArdiChef Controller is the 'main' sketch used on the Arduino board(s) that
 * controls the dispensers.  
 * 
 * Primary Function:
 *  1) Recieve I2C commands
 *  2) Parse I2C commands
 *  3) Manage motor driver libraries
 *      a) Load the data 
 *      b) Start/Stop motors
 *      c) Cancel commands already in motion
 *      
 * Optional Functions:
 *  1) WebServer - When used w/o a Rasberry Pi (set #define INDEPENDANT)
 *****************************************************************************/
#include "PortsCollection.h"
#include <ArduinoJson.h>

#include <SD.h>
#include <SPI.h>

#define DEBUG 1   // Set to 0 for release
#if DEBUG>0
  #define DBGL(x) Serial.println x
  #define DBG(x) Serial.print x
#else
  #define DBGL(x)
  #define DBG(x)
#endif

#if BOARD_I2C_ADDR>0            // Include I2C support if not the 'main' board
#include <Wire.h>
#endif

#define SERIAL_BUFFER_SIZE 64   // Largest I2C or Serial recievable text


PortsCollection Ports;
int SerMicroT = 0;int SerSize = 0;
boolean once = false;
//----------------------------------------------------------------------------
void setup() {
  #if DEBUG>0  
    Serial.begin(115200);       // Enable DBG
  #endif
  #if BOARD_I2C_ADDR==0  
    Ports.SetupWithSDFile("/JSON/INGRED~1.JSO");   // Setup Ports using JSON File
  #else
    Wire.begin();             // Not finished!!!
  #endif
} 
//----------------------------------------------------------------------------
void loop() {

  // EXAMPLE;  This show how Commands are sent to activate Ports#0 and Ports#1
  //if ( once==false ) {
  //  DBGL(("once/n"));
  //  Ports.Command("1 50000");
  //  Ports.Command("0 50000");
  //  once = true;
  //}

  // Check for Serial Commands (We still get funny stuff when executing fast commands!!!!)
  // The Serial Buffer takes time - looping causes 'A', 'ABC', 'DE' etc.. to appear per loop
  // Therefore we had to make a timer of 100-millis to ensure a full string recieved.
  SerSize = Serial.available();
  if (SerSize > 0) {
    if ( SerMicroT == 0 ) {
      SerMicroT = millis();                         // Record recieve start time
    } else if ( (millis() - SerMicroT) > 400) {     // After 1-Sec process
      char Ser[SerSize + 1];int SerIdx = 0;         // Initialize memory
      while (Serial.available()) {
        Ser[SerIdx] = Serial.read();                // Read a character
        SerIdx++;                                   // Set for next character
      }
      Ser[SerIdx] = '\0';
      SerIdx = 0; SerMicroT = 0;                    // Reset for next request
      Ports.Command(Ser);
    }
  }

  Ports.doloop();
}

