/************************************************************************//**
 * @file ArdiChefController.ino
 * @brief  ArdiChef's 'Main' Sketch used for all Arduino board(s).
 *    Primary Functions:
 *    @li Web Server - |Optionally| host EWS through ethernet shield
 *    @li I2C Communications - Recieve, Parse, and Execute I2C Commands
 *    @li Manage Ports - Load JSON, Start/Stop Motors, Cancel in Motion
 * @details
 *    Arduino Mega2560
 *    Desloo Ethernet Shield w/5100 Wiznet
 *    (3) ULN2003 Step Motor Driver boards
 * @authors
 *    tgit23      11/2015       Original code.
 * @notes
 * 
 *    File Dependancies:
 *      A4988Driver(.cpp,.h)
 *      DriverBase(.cpp,.h)
 *      PortsCollection(.cpp,.h)
 *      Settings.h
 *      ULN2003Driver(.cpp,.h)
 *      WebServerSTPlus(.cpp,.h)
 *      
 *    Library Dependancies:
 *      Ethernet
 *      SD
 *      SPI
 *      ArduinoJson
 *      Wire
 * @todo
 * @see http://www.ArdiChef.com
 ******************************************************************************/
#include "Settings.h"

#if IS_WEBSERVER>0
  #include "WebServerSTPlus.h"        // Include WebServer Code
  #include <Ethernet.h>               // Include Ethernet Shield Library
  #include <utility/w5100.h>          // Include w5100 library
  #include <utility/socket.h>         // Include socket library
  #include <SD.h>                     // Include SD-Card library
  #include <SPI.h>                    // Include SPI library for SD-Card
  #include <ArduinoJson.h>            // Include a JSON parser for SD-Card files
  WebServerSTPlus *webserver;         // Initialize 'webserver' pointer
#endif


#define SERIAL_BUFFER_SIZE 64         // Largest I2C or Serial recievable text
#if SLAVE_I2C_ADDR>0                  // Include the Wire.h library for I2C communications
  #include <Wire.h>
#endif

#include "PortsCollection.h"          // Include I/O Ports code
PortsCollection Ports;                // Initialize the 'Ports' Object

char postreq[48] = {0};
bool once = false;                    // Used to trigger a one-time command in the loop()
/***************************************************************************************
 * setup()
 * @param void
 * @return void
 **************************************************************************************/
void setup() {
  #if DEBUG>0                               // Begin Serial Monitor for DEBUG Messages
    Serial.begin(SERIAL_BAUD);
  #endif
  #if SLAVE_I2C_ADDR==0                     // Setup Ports using JSON File if "Master" Arduino
    Ports.SetupWithSDFile(SD_SETUP_FILE);
  #else
    Wire.begin();                           // Activate I2C Communications if "Slave" Arduino
  #endif
  
  #if IS_WEBSERVER>0                        // Setup Arduino Web-Host Settings
    byte mac[] = { MAC };                   // See Settings.h
    IPAddress ip( IP_ADDRESS );
    IPAddress gateway( GATEWAY );
    IPAddress subnet( SUBNET );
    #if USE_DHCP>0
      webserver = new WebServerSTPlus(mac,ip,gateway,subnet,true,80);
    #else
      webserver = new WebServerSTPlus(mac,ip,gateway,subnet,false,80);
    #endif
  #endif
} 

/***************************************************************************************
 * loop()
 * @param void
 * @return void
 **************************************************************************************/
void loop() {

  #if IS_WEBSERVER>0                      // Call the 'webserver' doloop()
    for (int c=0;c<48;c++) postreq[c] = '\0';
    webserver->doloop(postreq);
    if ( strlen(postreq) > 0 ) { Ports.Command(postreq); }
    myStuff();
  #endif
  Ports.doloop();                         // Call the 'Ports' doloop()
  
  if ( !once ) { 
    once = true;
    Ports.Command("2,30000,0;");
    Ports.Command("1,30000,0;");
    Ports.Command("0,30000,0;");
  }
}

void myStuff() {
  if(Serial.available()) { if(Serial.read() == 'r') webserver->ShowSockStatus(); }
  webserver->checkSockStatus();
}
