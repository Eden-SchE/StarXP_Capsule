#ifndef _EventAction_h_Digi_
#define _EventAction_h_Digi_

#include "Rtypes.h"
#include "TVector3.h"

#include "SimEvent.h"
#include "SimDeposit.h"

#include "TimeAction.hh"

#include <set>

struct TruthInfo
{
    Double_t FirstSDStepTime;
    std::set<UInt_t> FiredSD;
    TVector3 PolPhotElectronPos;
    TVector3 PolPhotElectronMom;
    Int_t IfPolPhot;
    Int_t IfPolPhotOnMCP;
};

class EventAction : public TimeAction
{

public:
    EventAction(SimEvent*, Double_t);
    ~EventAction();

    std::pair<Double_t, TimeAction*> Action(Double_t);
    inline std::set<UInt_t> GetFiredSD() {return m_TruthInfo.FiredSD;}
    inline SimEvent* GetSimEventPtr() {return &m_Event;}
    inline Double_t GetFirstSDStepTime() {return m_TruthInfo.FirstSDStepTime;}
    inline bool Start() {return m_StartEvent;}
    inline bool NumOfValidStep() {return m_NumOfValidStep;}
    Double_t SetSingleParticleMode();

    inline static bool myStepSort(SimDeposit i, SimDeposit j){return (i.GetPreT() < j.GetPreT());}

private:
    SimEvent m_Event;
    Int_t m_NumOfValidStep;
    Int_t m_iStep;
    std::vector<SimDeposit> m_ValidStep;
    bool m_StartEvent;

    Double_t m_TimeShift;
    Double_t m_EventStartTime;
    Double_t m_EventTruthEnergy;

    TruthInfo m_TruthInfo;
    

};

#endif