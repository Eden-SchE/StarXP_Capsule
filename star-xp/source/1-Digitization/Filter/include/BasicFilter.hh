#ifndef _BasicFilter_h_Digi_
#define _BasicFilter_h_Digi_

#include "TH2F.h"
#include "TString.h"
#include <string>

class BasicFilter
{
public:
    BasicFilter(){};
    ~BasicFilter(){};

    virtual void Filter(TH2*, int, double)=0;
    virtual void SetTopMetalID(UInt_t)=0;
    virtual void InitFilter(TH2*)=0;

protected:
    virtual void Save()=0;
};

#endif