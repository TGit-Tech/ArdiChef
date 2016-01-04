/************************************************************************//**
 * @file   ULN2003Driver.h
 * @brief  ArdiChef Step Motor Driver (Works with most 4-wire stepper motors)
 *    SD Web server for Arduino.  Specifically modified for the 
 *    ArdiChef project.  see http://www.ArdiChef.com
 * @details
 *    Arduino Mega2560
 *    Desloo Ethernet Shield w/5100 Wiznet
 * @authors
 *    tgit23      11/2015       Original Author
 * @todo
 * @see http://www.ArdiChef.com
 ******************************************************************************/
#ifndef ULN2003DRIVER_H
#define ULN2003DRIVER_H
#include "Settings.h"
#include "DriverBase.h"

const uint8_t MotorSteps[8][4] =      // [8-steps][4-Wire]
{
  { LOW, LOW, LOW, HIGH },      // Step 0
  { LOW, LOW, HIGH, HIGH },     // Step 1
  { LOW, LOW, HIGH, LOW },      // Step 2
  { LOW, HIGH, HIGH, LOW },     // Step 3
  { LOW, HIGH, LOW, LOW },      // Step 4
  { HIGH, HIGH, LOW, LOW },     // Step 5
  { HIGH, LOW, LOW, LOW },      // Step 6
  { HIGH, LOW, LOW, HIGH }      // Step 7
};

/**************************************************************************//**
 * @class  ULN2003Driver
 * @brief  Driver code for the ULN2003A step motor driver module
 ******************************************************************************/
class ULN2003Driver : public DriverBase {
  public:
    ULN2003Driver(int Pin1, int Pin2, int Pin3, int Pin4, unsigned long PulseWidth = 1200);
    void Command(const char* DCommand);
    void doloop();
  private:
    unsigned long msteps = 0;
    unsigned long mPulseWidth = 0;
    int mdir = 0;
    unsigned long mlast_uSec = 0;
    int mSubStep = 0;
    int mPin1 = 0;
    int mPin2 = 0;
    int mPin3 = 0;
    int mPin4 = 0;
    
};

#endif
