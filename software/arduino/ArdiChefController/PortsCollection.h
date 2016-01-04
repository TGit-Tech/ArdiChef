/************************************************************************//**
 * @file PortsCollection.h
 * @brief A 'Port' manager for connecting drivers to Arduino pin-groups
 *    The ports collection handles Arduino pin-groups and driver snd sends
 *    ArdiChef operation commands to their appropriate drivers.
 * @authors
 *    tgit23      11/2015       Original
 * @see http://www.ArdiChef.com
 ******************************************************************************/
#ifndef PORTSCOLLECTION_H
  #define PORTSCOLLECTION_H

  #include <ArduinoJson.h>
  
  #include "Settings.h"
  #include "DriverBase.h"
  #include "A4988Driver.h"
  #include "ULN2003Driver.h"
  
  #include <SD.h>
  #include <SPI.h>

  #define MAX_PINS 31
  #define MAX_DRIVER_SIZE 31
  #define MAX_PORTS 10            // Maximum number of definable ports
  #define JSON_BUFFER_SIZE 2200   // Larger allows more complex ingred.json files
  
  #define DEBUG 1   // Set to 0 for release
  #if DEBUG>0
    #define DB1L(x) Serial.println x
    #define DB1(x) Serial.print x
  #else
    #define DB1L(x)
    #define DB1(x)
  #endif

/**************************************************************************//**
 * @class  PortsCollection
 * @brief  A 'Port' manager for connecting drivers to Arduino pin-groups
 ******************************************************************************/
  class PortsCollection {
    public:
      PortsCollection();
      void SetupWithSDFile(char* SDjsonFile);
      void Setup(char* json);
      /// Send text commands to port drivers; format is driver specific.
      void Command(const char* DCommand);
      void doloop();
    private:
      DriverBase *mPortDriver[MAX_PORTS];
      void PrintDirectory(File dir, int numTabs);
  };
#endif
