/**************************************************************************//**
 * @file   PortsCollection.h
 * @brief  'Port' manager for mapping & handling drivers and pin-groups. 
 * @author tgit23
 ******************************************************************************/
#ifndef PORTSCOLLECTION_H
  #define PORTSCOLLECTION_H

  #include <Arduino.h>
  #include <ArduinoJson.h>
  
  #include "DriverBase.h"
  #include "A4988Driver.h"
  #include "ULN2003Driver.h"
  
  #include <SD.h>
  #include <SPI.h>

  #define MAX_PINS 31
  #define MAX_DRIVER_SIZE 31
  #define MAX_PORTS 10            // Maximum number of definable ports
  #define JSON_BUFFER_SIZE 2200   // Larger allows more complex ingred.json files
  #define BOARD_I2C_ADDR 0        // I2C Address of Arduino Board ( Use 0 for 'main' )
  
  #define DEBUG 1   // Set to 0 for release
  #if DEBUG>0
    #define DBGL(x) Serial.println x
    #define DBG(x) Serial.print x
  #else
    #define DBGL(x)
    #define DBG(x)
  #endif

/**************************************************************************//**
 * @class  PortsCollection
 * @brief  'Port' manager for mapping & handling drivers and pin-groups. 
 * @author tgit23
 ******************************************************************************/
  class PortsCollection {
    public:
      PortsCollection();
      void SetupWithSDFile(char* SDjsonFile);
      void Setup(char* json);
      /// Send text commands to port drivers; format is driver specific.
      void Command(const char* DriverCommand);
      void doloop();
    private:
      DriverBase *mPortDriver[MAX_PORTS];
      void PrintDirectory(File dir, int numTabs);
  };
#endif
