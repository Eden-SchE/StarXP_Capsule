#ifndef _DiffFilter_h_Digi_
#define _DiffFilter_h_Digi_

#include "BasicFilter.hh"
#include "TH2.h"
#include <vector>

class DiffFilter : public BasicFilter
{
public:
    DiffFilter();
    ~DiffFilter();
    void Filter(TH2 *, int, double);
    void InitFilter(TH2*);
    void SetTopMetalID(UInt_t);

private:
    void Save();
    void VectorToVector(std::vector<std::vector<double> >*, std::vector<std::vector<double> >*);
    void HistToVector(TH2*, std::vector<std::vector<double> >*);
    void VectorToHist(std::vector<std::vector<double> >*, TH2*);

    int m_iTopMetal;
    TH2F* m_OutputHist;
    int m_nBinX;
    int m_nBinY;
    int m_nFrame;
    double m_FlashTime;

    bool m_Start;
    double m_ThresholdV;
    double m_ThresholdN;

    std::vector<std::vector<double> >* m_thPre;
    std::vector<std::vector<double> >* m_thThis;
    std::vector<std::vector<double> >* m_thDiff;
};

#endif
