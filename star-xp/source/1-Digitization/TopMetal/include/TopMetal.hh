#ifndef _TopMetal_h_Digi_
#define _TopMetal_h_Digi_

#include "TString.h"
#include "TH2F.h"

#include "Pixel.hh"
#include "TimeAction.hh"
#include "TopMetalContainer.hh"
#include "BasicFilter.hh"

#include <map>

class TopMetal: public TimeAction
{
public:
    TopMetal(UInt_t);
    ~TopMetal();

    void ClearAllCharge();
    void InitTopMetal(Double_t);
    void SaveAllFrame() {m_NumOfFramePerEvent = -1;}
    Pixel* iPixel(UInt_t, UInt_t);

    std::pair<Double_t, TimeAction*> Action(Double_t);

private:
    UInt_t m_NumOfPixelX;
    UInt_t m_NumOfPixelY;
    static Double_t m_PixelTime;
    static Double_t m_RowTime;
    static Double_t m_FrameTime;

    UInt_t m_iTopMetal;
    UInt_t m_iX;
    UInt_t m_iY;
    UInt_t m_nFrame;
    UInt_t m_NumOfFramePerEvent;

    std::map<std::pair<UInt_t, UInt_t>, Pixel*> m_MapPixels;
    TH2F* m_TH2TopMetal;
    BasicFilter* m_Filter;
};

#endif