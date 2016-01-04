/************************************************************************//**
 * @file Settings.h
 * @brief File for ArdiChef configuration settings
 * @details
 * @authors
 *    tgit23      11/2015       Original Author
 * @todo
 * @see http://www.ArdiChef.com
 ******************************************************************************/
#ifndef SETTINGS_H
#define SETTINGS_H

#include <Arduino.h>
/** @brief Activates the Arduino WebServer code to host the EWS through
 *  an attached ethernet shield */
#define IS_WEBSERVER 1
/** @brief Set to '0' if the Arduino is the "Master" Arduino board;  Otherwise
 *  assign an I2C decimal address for the "Slave" Arduino Board. */
#define SLAVE_I2C_ADDR 0
/** @brief Set to '1' to show debug messages in the 'Serial Monitor' */

#define USE_DHCP 0
#define IP_ADDRESS 192,168,0,25
#define GATEWAY 192,168,0,1
#define SUBNET 255,255,255,0
#define MAC 0xDE,0xAD,0xBE,0xEF,0xFE,0xEC

#define DEBUG 1
#define SD_SETUP_FILE "/JSON/INGRED~1.JSO"
#define SERIAL_BAUD 250000
#define SD_CSPIN 4
#define W5100_CSPIN 10


/******************************************************************************/
// Setup - No custom settings below this line
#if DEBUG>0
    #define DB1L(x) Serial.println x
    #define DB1(x) Serial.print x
#else
  #define DB1L(x)
  #define DB1(x)
#endif
#if DEBUG>1
  #define DB2L(x) Serial.println x
  #define DB2(x) Serial.print x
#else
  #define DB2L(x)
  #define DB2(x)
#endif
  
#endif
