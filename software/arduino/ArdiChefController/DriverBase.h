/************************************************************************//** 
 * @file DriverBase.h
 * @brief  Base Class Definition for all ArdiChef output drivers
 * @code
 *    exmaple code
 * @endcode
 * @authors 
 *    tgit23        11/2013       Original
 ******************************************************************************/
#ifndef DRIVERBASE_H
#define DRIVERBASE_H

#include "Settings.h"
#define MAX_PINS 31
#define MAX_DRIVER_SIZE 31

/**********************************************************************************************//**
 *  @class  DriverBase
 *  @brief  Base Class for all Drivers
 *************************************************************************************************/
class DriverBase {
  public:
    int dir = 0;
    int dirPin = 0;
    int stepPin = 0;
    int pinCnt = 0;
    int pins[MAX_PINS] = {};
    unsigned long steps = 0;
    unsigned long stepsperoz = 0;
    int pinIdx = 0;
    unsigned long last_uSec = 0;
    unsigned long uPWM = 400;
    const char *driver;
    virtual void Command(const char* DCommand);
    virtual void doloop();
};

#endif
