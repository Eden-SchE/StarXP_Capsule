#ifndef _Multiplication_h_Digi_
#define _Multiplication_h_Digi_

#include "Rtypes.h"
#include "TVector3.h"
#include "TH2F.h"

#include "TimeAction.hh"
#include "StepAction.hh"

#include <vector>
#include <map>

struct TouchedPixel
{
    UInt_t iX;
    UInt_t iY;
    UInt_t NumOfElectron;
};

class Multiplication : public TimeAction
{
public:
    Multiplication(IonizationInfo);
    ~Multiplication();

    std::pair<Double_t, TimeAction*> Action(Double_t);
    inline Double_t GetMCPTimeFlag() {return m_IonInfo.MCPTime;}


private:
    IonizationInfo m_IonInfo;
    TH2F* m_TH2ElectronOnTopMetal;
    std::vector<TouchedPixel> m_TouchedPixelVector;
    Double_t m_TimeOntoTopMetal;

    Int_t m_iPixel;
    Int_t m_NumOfTouchedPixel;
};
#endif