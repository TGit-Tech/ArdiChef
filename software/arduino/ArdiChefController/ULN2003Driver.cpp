#include "ULN2003Driver.h"

/******************************************************************************************
 * @brief Initializer / Constructor
 * @param Pin1 - Arduino output digital pin number connected to 1st step motor wire
 * @param Pin2 - Arduino output digital pin number connected to 2nd step motor wire
 * @param Pin3 - Arduino output digital pin number connected to 3rd step motor wire
 * @param Pin4 - Arduino output digital pin number connected to 4th step motor wire
 * @param PulseWidth - Set the Step Pulse Width time (smaller=faster)
 *****************************************************************************************/
ULN2003Driver::ULN2003Driver(int Pin1, int Pin2, int Pin3, int Pin4, unsigned long PulseWidth) {
  DB1(("ULN2003Driver::Constructor( Pin1="));DB1((Pin1));
  DB1((",Pin2="));DB1((Pin2));
  DB1((",Pin3="));DB1((Pin3));
  DB1((",Pin4="));DB1((Pin4));
  DB1((",PulseWidth="));DB1((PulseWidth));DB1L((" )"));
  
  mPin1 = Pin1; pinMode(mPin1, OUTPUT); digitalWrite(mPin1, LOW);
  mPin2 = Pin2; pinMode(mPin2, OUTPUT); digitalWrite(mPin2, LOW);
  mPin3 = Pin3; pinMode(mPin3, OUTPUT); digitalWrite(mPin3, LOW);
  mPin4 = Pin4; pinMode(mPin4, OUTPUT); digitalWrite(mPin4, LOW);
  mPulseWidth = PulseWidth;
  return;
}

/******************************************************************************************
 * @brief Command(ULN2003-Command)
 *    The ULN2003Driver 'Commands' are in the form "<port>,<steps>,<dir>;"
 * @param[in] Command - The text command to process
 * @return void
 *****************************************************************************************/
void ULN2003Driver::Command(const char* DCommand) {
  DB1(("ULN2003Driver::Command( '"));DB1((DCommand));DB1((F("' ) -> ")));
  
  char pinsBuff[32]; int port = -1;
  sscanf(DCommand, "%d,%lu,%d;", &port, &msteps, &mdir);
  
  // Display parsing
  DB1((F("Port=")));DB1((port));
  DB1((F(",Steps=")));DB1((msteps));
  DB1((F(",Dir=")));DB1L((mdir));
  
  return;
}

/******************************************************************************************
 * @brief doloop() Keep active routine.  Processes text commands asynchronously
 * @return void
 *****************************************************************************************/
void ULN2003Driver::doloop() {

  if ( msteps < 1 ) return;                   // If no steps need action then exit
  
  unsigned long t = micros();                 // Record time
  if ( (t - mlast_uSec) > mPulseWidth ) {     // If time passed > PulseWidth
    mlast_uSec = t; msteps--;                 // Mark time; Preform a step
    DB2(("ULN2003Driver::doloop() -> "));
    DB2(("Steps:"));DB2((msteps));
    DB2((", uSecPast:"));DB2L((t - mlast_uSec));
    digitalWrite(mPin1, MotorSteps[mSubStep][0]); 
    digitalWrite(mPin2, MotorSteps[mSubStep][1]);
    digitalWrite(mPin3, MotorSteps[mSubStep][2]);
    digitalWrite(mPin4, MotorSteps[mSubStep][3]);
    if(mdir>0) mSubStep++; else mSubStep--;
    if(mSubStep>7) mSubStep=0;
    if(mSubStep<0) mSubStep=7;
    
    if (msteps < 1 ) {                        // Shut-off pins on last step
      digitalWrite(mPin1, LOW);
      digitalWrite(mPin2, LOW);
      digitalWrite(mPin3, LOW);
      digitalWrite(mPin4, LOW);
    }
  }

  return;
}
