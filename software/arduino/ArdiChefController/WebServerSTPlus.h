/************************************************************************//**
 * @file WebServerSTPlus.h
 * @brief  WebServerST - PLUS - v1.0
 *    SD Web server for Arduino.  Specifically modified for the 
 *    ArdiChef project.  see http://www.ArdiChef.com
 * @details
 *    Arduino Mega2560
 *    Desloo Ethernet Shield w/5100 Wiznet
 * @authors
 *    SurferTim   11/2013       Original Posting
 *    tgit23      11/2015       Simplified pre-processor debug
 *    tgit23      11/2015       Added optional DHCP support
 *    tgit23      11/2015       Added 8.3-SFN Filename conversion routine
 *    tgit23      12/2015       Parsed code into class structure
 * @todo
 * @see http://www.ArdiChef.com
 ******************************************************************************/
#ifndef WEBSERVERSTPLUS_H
#define WEBSERVERSTPLUS_H

#include <Arduino.h>
#include <Ethernet.h>
#include <SPI.h>
#include <utility/w5100.h>
#include <utility/socket.h>
#include <SD.h>
#include "Settings.h"

/**************************************************************************//**
 * @class  WebServerSTPlus
 * @brief  Class to activate EWS SD-Card web hosting on the Arduino with and ethernet shield
 ******************************************************************************/
class WebServerSTPlus {
  public:
    WebServerSTPlus(byte mac[], IPAddress ip, IPAddress gateway, IPAddress subnet, bool UseDHCP = false, int Port = 80);
    void doloop(char *postreq);
    void ShowSockStatus();
    void checkSockStatus();
  private:
    enum sdWriteType{WRITE,APPEND};
    EthernetServer *server;
    File root;
    File fw;
    byte socketStat[MAX_SOCK_NUM];
    unsigned int requestNumber = 0;
    unsigned long connectTime[MAX_SOCK_NUM];
    int loopCount = 0;
    void printDirectory(File dir, int numTabs);
    void sendBadRequest(EthernetClient thisClient);
    void strtoupper(char* aBuf);
    int freeRam();
    void sendFileNotFound(EthernetClient thisClient);
    void urldecode(char* urltext);
    void sdWrite(char* fileName, char* text, sdWriteType WriteType);
    void urldecode(char *dst, const char *src);
};
#endif
