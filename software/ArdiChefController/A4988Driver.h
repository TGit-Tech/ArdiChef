/**************************************************************************//**
 *  @file   A4988.h
 *  @brief  Port Driver for the Allegro A4988 Stepper Motor Driver.
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

/**********************************************************************************************//**
 *  @class  A4988Driver
 *  @brief  ArdiChef Port Driver for Allegro A4988 Stepper Motor Driver.
 *************************************************************************************************/
class A4988Driver : public DriverBase {
  public:
  
    /** @brief A4988Driver Constructor
     *  @param[in] stepPin Arduino OUTPUT pin# connected to the A4988 'STEP' Pin
     *  @param[in] dirPin Arduino OUTPUT pin# connected to the A4988 'DIR' Pin
     *  @param[in] PulseWidth sets the motor speed; 'STEP' time length in uSec; smaller = faster
     *  
     *  <B>Example:</B> @code{.cpp}
     *  A4988Driver *driver;
     *  driver = new A4988Driver(6,7,1000); @endcode*/
    A4988Driver(int stepPin, int dirPin, unsigned long PulseWidth = 400);

    /** @brief Process a driver command
     *  @param[in] Command in form '\c <Port#> \c <#-of-Steps>'
     *  
     *  <B>Example:</B> @code{.cpp}
     *    if (true) Command("0 2300"); @endcode*/
    void Command(const char* Command);

    /// @brief Repetative call function to keep the output active
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
