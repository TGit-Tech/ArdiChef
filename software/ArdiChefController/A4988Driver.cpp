/******************************************************************************
 * A4988 ArdiChef Driver
 * 
 ******************************************************************************/
#include "A4988Driver.h"


A4988Driver::A4988Driver(int stepPin, int dirPin, unsigned long PulseWidth) {
  //DBG(("A4988Driver::Constructor( stepPin = "));DBG((stepPin));DBG((" , dirPin = "));DBG((dirPin));
  //DBG((" , PulseWidth = "));DBG((PulseWidth));DBGL((" )"));
  mstepPin = stepPin;
  mdirPin = dirPin;
  mPulseWidth = PulseWidth;
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  return;
}

// Parse an I2C command <port> <steps> <dir>
void A4988Driver::Command(const char* Command) {
  DBG((F("A4988Driver::Command( '")));DBG((Command));DBGL((F("' )")));
  
  char pinsBuff[32]; int port = 0;
  sscanf(Command, "%31d %31lu %31d", port, &msteps, &mdir);
  if ( mdir>0 ) {
    digitalWrite(mdirPin, LOW);
  } else {
    digitalWrite(mdirPin, HIGH);
  }
  // Display parsing
  DBG((F("\tmsteps = ")));DBGL((msteps));
  DBG((F("\tmdir = ")));DBGL((mdir));
  return;
}

void A4988Driver::doloop() {
  //DBGL((F("A4988Driver::doloop()")));
  if ( msteps < 1 ) return;
  unsigned long t = micros();
  if ( (t - mlast_uSec) > mPulseWidth ) {
    mlast_uSec = t; msteps--;
    //DBG(("PulsePin:"));DBG((mstepPin));DBG((", Steps:"));DBG((msteps));DBG((", uSecPast:"));DBGL((t - mlast_uSec));
    if ( digitalRead(mstepPin) == HIGH ) {
      digitalWrite(mstepPin, LOW);
    } else {
      digitalWrite(mstepPin, HIGH);
    }
  }
  return;
}
