#ifndef _StepAction_h_Digi_
#define _StepAction_h_Digi_

#include "Rtypes.h"
#include "TVector3.h"

#include "SimEvent.h"
#include "SimDeposit.h"

#include "TimeAction.hh"

#include <vector>

struct IonizationInfo
{
    Int_t SDNum;
    Double_t MCPTime;
    Double_t DriftDistance;
    TVector3 InitElectronPosition;
};

class StepAction : public TimeAction
{

public:
    StepAction(SimDeposit, Double_t);
    ~StepAction();

    std::pair<Double_t, TimeAction*> Action(Double_t);

    inline static bool mySort(double i, double j){return (i < j);}
    inline static bool myIonSort(IonizationInfo i, IonizationInfo j){return (i.MCPTime < j.MCPTime);}

private:
    SimDeposit m_Step;

    Double_t m_EventStartTime;
    Int_t m_NumOfInitElectron;
    Int_t m_iInitElectron;
    Int_t m_SDNum;
    std::vector<IonizationInfo> m_IonizationInfo;

};

#endif