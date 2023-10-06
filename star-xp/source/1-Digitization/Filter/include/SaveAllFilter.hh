#ifndef _SaveAllFilter_h_Digi_
#define _SaveAllFilter_h_Digi_

#include "BasicFilter.hh"

class SaveAllFilter : public BasicFilter
{
public:
    SaveAllFilter() { ; }
    ~SaveAllFilter() { ; }

    void Filter(TH2 *, int, double);
    void SetTopMetalID(UInt_t);
    void InitFilter(TH2*){};

private:
    void Save();

    int m_iTopMetal;
    TH2* m_InHist;
    TH2F* m_OutputHist;
    int m_nBinX;
    int m_nBinY;
    int m_nFrame;
    double m_FlashTime;
};

#endif
