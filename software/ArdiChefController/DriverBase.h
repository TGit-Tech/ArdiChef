/******************************************************************************
 * DriverBase - Base Class Definition for ArdiChef drivers
 * 
 ******************************************************************************/
#ifndef DRIVERBASE_H
#define DRIVERBASE_H

#define MAX_PINS 31
#define MAX_DRIVER_SIZE 31

#define DEBUG 0   // Set to 0 for release
#if DEBUG>0
  #define DBGL(x) Serial.println x
  #define DBG(x) Serial.print x
#else
  #define DBGL(x)
  #define DBG(x)
#endif

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
    virtual void Command(const char* Command);
    virtual void doloop();
};

#endif
