/******************************************************************************
 * A4988 ArdiChef Driver
 * 
 ******************************************************************************/
#ifndef A4988DRIVER_H
#define A4988DRIVER_H
#include "Arduino.h"
#include "DriverBase.h"

#define DEBUG 1   // Set to 0 for release
#if DEBUG>0
  #define DBGL(x) Serial.println x
  #define DBG(x) Serial.print x
#else
  #define DBGL(x)
  #define DBG(x)
#endif

class A4988Driver : public DriverBase {
  public:
    A4988Driver(int stepPin, int dirPin, unsigned long PulseWidth = 400);
    void Command(const char* Command);
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
