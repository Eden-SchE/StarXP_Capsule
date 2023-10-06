#ifndef _InputFileReader_h_Digi_
#define _InputFileReader_h_Digi_

#include "TFile.h"
#include "TString.h"
#include "TChain.h"

#include "SimEvent.h"

#include "TimeAction.hh"

class TimeAction;
class InputFileReader : public TimeAction
{
public:
    InputFileReader();
    InputFileReader(TString FileName, Double_t TimeShift, Int_t ActionID);
    ~InputFileReader();

    void LoadSimFile(TString);

    std::pair<Double_t, TimeAction*> Action(Double_t);

    void NextEvent();

private:
    TChain* m_Chain;
    TString m_FileName;
    Int_t m_Entry;
    Int_t m_NumOfEntries;
    Double_t m_TimeShift;
    Double_t m_EventTimeFlag;

    SimEvent* m_Event;
    SimEvent m_ThisEvent;
    SimEvent m_NextEvent;


};

#endif