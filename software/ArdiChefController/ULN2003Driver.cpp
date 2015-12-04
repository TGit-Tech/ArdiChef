/******************************************************************************
 * A4988 ArdiChef Driver
 * 
 ******************************************************************************/
#include "ULN2003Driver.h"


ULN2003Driver::ULN2003Driver(int Pin1, int Pin2, int Pin3, int Pin4, unsigned long PulseWidth) {
  DBG(("ULN2003Driver::Constructor( Pin1="));DBG((Pin1));DBG((", Pin2="));DBG((Pin2));
  DBG((", Pin3="));DBG((Pin3));DBG((", Pin4="));DBG((Pin4));DBG((", PulseWidth="));DBG((PulseWidth));DBGL((" )"));
  mPin1 = Pin1; pinMode(mPin1, OUTPUT);
  mPin2 = Pin2; pinMode(mPin2, OUTPUT);
  mPin3 = Pin3; pinMode(mPin3, OUTPUT);
  mPin4 = Pin4; pinMode(mPin4, OUTPUT);
  mPulseWidth = PulseWidth;
  return;
}

// Parse an I2C command <port> <steps> <dir>
void ULN2003Driver::Command(const char* Command) {
 DBG(("A4988Driver::Command( "));DBGL((Command));
  
  char pinsBuff[32]; int port = 0;
  sscanf(Command, "%31d %31lu %31d", port, &msteps, &mdir);
  
  // Display parsing
  DBG(("\tmsteps = "));DBGL((msteps));
  DBG(("\tmdir = "));DBGL((mdir));
  return;
}

void ULN2003Driver::doloop() {
  //DBGL(("A4988Driver::doloop()"));
  if ( msteps < 1 ) return;
  unsigned long t = micros();
  if ( (t - mlast_uSec) > mPulseWidth ) {
    mlast_uSec = t; msteps--;
    //DBG(("PulsePin:"));DBG((mstepPin));DBG((", Steps:"));DBG((msteps));DBG((", uSecPast:"));DBGL((t - mlast_uSec));
    digitalWrite(mPin1, MotorSteps[mSubStep][0]); 
    digitalWrite(mPin2, MotorSteps[mSubStep][1]);
    digitalWrite(mPin3, MotorSteps[mSubStep][2]);
    digitalWrite(mPin4, MotorSteps[mSubStep][3]);
    if(mdir>0) mSubStep++; else mSubStep--;
    if(mSubStep>7) mSubStep=0;
    if(mSubStep<0) mSubStep=7;
  }
  return;
}
