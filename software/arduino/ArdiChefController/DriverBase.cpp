/************************************************************************//**
 *  @brief  Base Class Definition for all ArdiChef output drivers
 *  @code
 *    exmaple code
 *  @endcode
 *  @authors 
 *    tgit23        11/2013       Original
 ******************************************************************************/
 #include "DriverBase.h"

void DriverBase::Command(const char* DCommand) {};
void DriverBase::doloop() {DB1L(("DriverBase::doloop()"));};
 
