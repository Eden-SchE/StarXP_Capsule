#ifndef _Diff3FrameFilter_h_Digi_
#define _Diff3FrameFilter_h_Digi_

#include "BasicFilter.hh"
#include "TH2.h"
#include <vector>

struct FrameCache
{
    int m_nFrame;
    double m_FlashTime;
    int m_nBinX;
    int m_nBinY;
    std::vector<std::vector<double> >* m_thDiff;
    FrameCache(int nFrame, double FlashTime, double ThresholdV, int nBinX, int nBinY, std::vector<std::vector<double> >* thPre, std::vector<std::vector<double> >* thThis):
        m_nFrame(nFrame), m_FlashTime(FlashTime), m_nBinX(nBinX), m_nBinY(nBinY)
    {
        for (int iX = 0; iX < m_nBinX; ++iX)
        {
            for (int iY = 0; iY < m_nBinY; ++iY)
            {
                (*m_thDiff)[iX][iY] = (*thThis)[iX][iY] - (*thPre)[iX][iY] > ThresholdV ? (*thThis)[iX][iY] - (*thPre)[iX][iY] : 0;
                //(*m_thDiff)[iX][iY] = (*m_thDiff)[iX][iY] > ThresholdV ? (*m_thDiff)[iX][iY] : 0;
            }
        }
    }
    ~FrameCache()
    {
        delete m_thDiff;
    }

};

class Diff3FrameFilter : public BasicFilter
{
public:
    Diff3FrameFilter();
    ~Diff3FrameFilter();
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
