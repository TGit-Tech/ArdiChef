/************************************************************************//**
 * @file A4988Driver.cpp
 * @brief  Port Driver for the Allegro A4988 Stepper Motor Driver.
 * @code
 *    exmaple code
 * @endcode
 * @authors 
 *    tgit23        11/2013       Original
 ******************************************************************************/
#include "A4988Driver.h"

/***********************************************************************************//**
 * @brief A4988Driver() Initializer
 *    Drivers are initialized by the 'PortsCollection' object.
 * @param[in] stepPin - Arduino digital pin number connected to the A4988 'step' pin
 * @param[in] dirPin - Arduino digital pin number connected to the A4988 'dir' pin
 * @param[in] PulseWidth - Essentially the speed; low=fast, high=slow
 * 
 * <B>Example:</B>@code{.cpp}
 *    A4988Driver *driver;
 *  driver = new A4988Driver(6,7,1000); @endcode
 **************************************************************************************/
A4988Driver::A4988Driver(int stepPin, int dirPin, unsigned long PulseWidth) {
  DB1(("A4988Driver::Constructor( stepPin = "));DB1((stepPin));DB1((" , dirPin = "));DB1((dirPin));
  DB1((" , PulseWidth = "));DB1((PulseWidth));DB1L((" )"));
  
  mstepPin = stepPin;
  mdirPin = dirPin;
  mPulseWidth = PulseWidth;
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  return;
}

/***********************************************************************************//**
 * @brief Command("port,steps,dir;") - Execute an A4988Driver command.
 * @param[in] DCommand - The Driver Command text to be executed
 *
 * <B>Example:</B>@code{.cpp}
 *    Port.Command("0,30000,1;"); @endcode
 **************************************************************************************/
void A4988Driver::Command(const char* DCommand) {
  DB1((F("A4988Driver::Command( '")));DB1((DCommand));DB1((F("' ) -> ")));
  
  int port = 0; //char pinsBuff[32]
  sscanf(DCommand, "%31d %31lu %31d", port, &msteps, &mdir);
  if ( mdir>0 ) {
    digitalWrite(mdirPin, LOW);
  } else {
    digitalWrite(mdirPin, HIGH);
  }
  
  // Display parsing
  DB1((F("Steps=")));DB1((msteps));DB1((F(", Dir=")));DB1L((mdir));
  return;
}

/***********************************************************************************//**
 * @brief doloop()
 *    Trigger function - needs to be called inside the main sketch loop()
 **************************************************************************************/
void A4988Driver::doloop() {
  
  
  if ( msteps < 1 ) return;
  unsigned long t = micros();
  if ( (t - mlast_uSec) > mPulseWidth ) {
    mlast_uSec = t; msteps--;
    DB2((F("A4988Driver::doloop() -> ")));
    DB2((F("PulsePin:")));DB2((mstepPin));
    DB2((F(", Steps:")));DB2((msteps));
    DB2((F(", uSecPast:")));DB2L((t - mlast_uSec));
    if ( digitalRead(mstepPin) == HIGH ) {
      digitalWrite(mstepPin, LOW);
    } else {
      digitalWrite(mstepPin, HIGH);
    }
  }
  return;
}
