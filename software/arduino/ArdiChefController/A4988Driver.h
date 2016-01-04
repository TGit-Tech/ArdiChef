/************************************************************************//**
 * @file A4988Driver.h
 * @brief  Port Driver for the Allegro A4988 Stepper Motor Driver.
 * @authors 
 *    tgit23        11/2013       Original
 ******************************************************************************/
#ifndef A4988DRIVER_H
#define A4988DRIVER_H

#include "Settings.h"
#include "DriverBase.h"

/**********************************************************************************************//**
 *  @class  A4988Driver
 *  @brief  ArdiChef Port Driver for Allegro A4988 Stepper Motor Driver.
 *************************************************************************************************/
class A4988Driver : public DriverBase {
  public:
    A4988Driver(int stepPin, int dirPin, unsigned long PulseWidth = 400);
    void Command(const char* DCommand);
    void doloop();
    
  private:
    unsigned long msteps = 0;
    int mdir = 0;
    unsigned long mlast_uSec = 0;
    int mstepPin = 0;
    int mdirPin = 0;
    unsigned long mPulseWidth = 0;
};

#endif
