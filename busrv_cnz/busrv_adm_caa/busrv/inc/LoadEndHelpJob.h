#ifndef _LOADENDHELPJOB_H_
#define _LOADENDHELPJOB_H_
/*

COPYRIGHT
    COPYRIGHT Ericsson Utvecklings AB, Sweden 2001. All rights reserved.

    The Copyright to the computer program(s) herein is the property of
    Ericsson Utvecklings AB, Sweden. The program(s) may be used and/or
    copied only with the written permission from Ericsson Utvecklings AB
    or in accordance with the terms and conditions stipulated in the 
    agreement/contract under which the program(s) have been supplied.

DESCRIPTION 
   Help LoadEnd execution

ERROR HANDLING
   -

DOCUMENT NO
   190 89-CAA 

AUTHOR 
   

LINKAGE
    -

SEE ALSO 
   -

*/

#include "JobBase.h"

class LoadEndHelpJob : public JobBase {
//foos
public:
    LoadEndHelpJob();
    void virtual execute() {}
    virtual void executeSpecial(u_int16);

private:
    LoadEndHelpJob(const LoadEndHelpJob& );             // not impl.
    LoadEndHelpJob& operator=(const LoadEndHelpJob& );  // -:-
    

    //virtual void executeNonOp(u_int16 sbc) {}
    //virtual void resetState() {}
    //virtual JQS_STATE getState(u_int& sbc) { sbc = 0xFF; return JQS_NORMAL; }

};

#endif