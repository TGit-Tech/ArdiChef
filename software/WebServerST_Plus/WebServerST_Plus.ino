/******************************************************************
 * Program:     WebServerST - PLUS - v1.0
 * 
 * Description: 
 *    SD Web server for Arduino.  Specifically modified for the 
 *    ArdiChef project.  see http://www.ArdiChef.com
 *              
 * Setup:       
 *    Arduino Mega2560
 *    Desloo Ethernet Shield w/5100 Wiznet
 * 
 * Authors:
 *    SurferTim   11/2013         Original Posting
 *    ThomasG     11/2015         Simplified pre-processor debug
 *    ThomasG     11/2015         Large file name and DHCP support
 * ****************************************************************/
#include <SPI.h>
#include <Ethernet.h>
#include <SD.h>
#include <utility/w5100.h>
#include <utility/socket.h>
#include "WebServerSTPlus.h"

#define DEBUG 1   // Comment to disable serial debug (saves ~1.6K)
#if DEBUG>0
  #define DBGL(x) Serial.println x
  #define DBG(x) Serial.print x
#else
  #define DBGL(x)
  #define DBG(x)
#endif

//--------------------- NETWORK SETTINGS --------------------------
bool UseDHCP = true;                // Attempt to use DHCP setup
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xEC };  // this must be unique
IPAddress ip( 192,168,0,25 );
IPAddress gateway( 192,168,0,1 );
IPAddress subnet( 255,255,255,0 );
//EthernetServer server(80);
//-----------------------------------------------------------------

unsigned long connectTime[MAX_SOCK_NUM];
WebServerSTPlus *webserver;

//=================================================================================================
void setup()
{
  Serial.begin(115200);
  webserver = new WebServerSTPlus(mac,ip,gateway,subnet,UseDHCP,80);

}

//=================================================================================================
void loop() {
  webserver->doloop();
  myStuff();
}

void myStuff() {
  if(Serial.available()) { if(Serial.read() == 'r') webserver->ShowSockStatus(); }
  webserver->checkSockStatus();
}

