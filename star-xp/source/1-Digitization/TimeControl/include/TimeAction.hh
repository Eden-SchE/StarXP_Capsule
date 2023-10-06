#ifndef _TimeAction_h_Digi_
#define _TimeAction_h_Digi_

#include "Rtypes.h"
#include "TString.h"

#include <vector>

class TimeAction
{
public:
    TimeAction();
    TimeAction(TString);
    TimeAction(TString, Int_t);
    TimeAction(TString, TString);
    TimeAction(TString, TString, Int_t);
    TimeAction(TString, TString, Int_t, Int_t);
    ~TimeAction();

    virtual std::pair<Double_t, TimeAction*> Action(Double_t){return std::pair<Double_t, TimeAction*>(-1., NULL);};

    void SetActionType(TString val) {m_ActionType = val;}
    void SetActionID(Int_t val) {m_ActionID = val;}
    void SetMotherType(TString val) {m_MotherType = val;}
    void SetMotherID(Int_t val) {m_MotherID = val;}
    void SetDaughterNumber(Int_t val) {m_DaughterNumber = val;}

    TString GetActionType() const {return m_ActionType;}
    Int_t GetActionID() const {return m_ActionID;}
    TString GetMotherType() const {return m_MotherType;}
    Int_t GetMotherID() const {return m_MotherID;}
    Int_t GetDaughterNumber() const {return m_DaughterNumber;}


private:
    Double_t m_Time;
    Double_t m_NextTime;
    TString m_ActionType;
    Int_t m_ActionID;
    TString m_MotherType;
    Int_t m_MotherID;
    Int_t m_DaughterNumber;

};

#endif