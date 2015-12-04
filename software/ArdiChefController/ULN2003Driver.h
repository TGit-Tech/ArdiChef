/******************************************************************************
 * ULN2003 ArdiChef Driver (Works with most 4-wire stepper motors)
 * 
 ******************************************************************************/
#ifndef ULN2003DRIVER_H
#define ULN2003DRIVER_H
#include "Arduino.h"
#include "DriverBase.h"

#define DEBUG 0   // Set to 0 for release
#if DEBUG>0
  #define DBGL(x) Serial.println x
  #define DBG(x) Serial.print x
#else
  #define DBGL(x)
  #define DBG(x)
#endif

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
class ULN2003Driver : public DriverBase {
  public:
    ULN2003Driver(int Pin1, int Pin2, int Pin3, int Pin4, unsigned long PulseWidth = 1200);
    void Command(const char* Command);
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
