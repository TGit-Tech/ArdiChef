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
 * Author:
 *    SurferTim   11/2013         Original Posting
 * Modified:
 *    tgit23      11/2015         Simplified pre-processor debug
 *    tgit23      11/2015         Added optional DHCP support
 *    tgit23      12/2015         Added 8.3-SFN Conversion routine
 *    tgit23      12/2015         Parsed code into class structure
 * ****************************************************************/
#ifndef WEBSERVERSTPLUS_H
#define WEBSERVERSTPLUS_H

#include <Arduino.h>
#include <Ethernet.h>
#include <SPI.h>
#include <utility/w5100.h>
#include <utility/socket.h>
#include <SD.h>

#define DEBUG 1   // Comment to disable serial debug (saves ~1.6K)
#if DEBUG>0
  #define DBGL(x) Serial.println x
  #define DBG(x) Serial.print x
#else
  #define DBGL(x)
  #define DBG(x)
#endif


class WebServerSTPlus {
  public:
    WebServerSTPlus(byte mac[], IPAddress ip, IPAddress gateway, IPAddress subnet, bool UseDHCP = false, int Port = 80);
    void doloop();
    void ShowSockStatus();
    void checkSockStatus();
  private:
    EthernetServer *server;
    File root;
    byte socketStat[MAX_SOCK_NUM];
    unsigned int requestNumber = 0;
    unsigned long connectTime[MAX_SOCK_NUM];
    int loopCount = 0;
    void printDirectory(File dir, int numTabs);
    void sendBadRequest(EthernetClient thisClient);
    void strtoupper(char* aBuf);
    int freeRam();
    void sendFileNotFound(EthernetClient thisClient); 
};
#endif
