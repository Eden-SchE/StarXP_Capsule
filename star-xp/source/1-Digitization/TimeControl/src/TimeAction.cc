#include "TimeAction.hh"

TimeAction::TimeAction()
{
    m_ActionType = "";
    m_ActionID = 0;
    m_MotherType = "";
    m_MotherID = 0;
    m_DaughterNumber = 0;
}
TimeAction::TimeAction(TString ActionType)
    :m_ActionType(ActionType){}

TimeAction::TimeAction(TString ActionType, Int_t ActionID)
    :m_ActionType(ActionType), m_ActionID(ActionID){}

TimeAction::TimeAction(TString ActionType, TString MotherType)
    :m_ActionType(ActionType), m_MotherType(MotherType){}

TimeAction::TimeAction(TString ActionType, TString MotherType, Int_t ActionID)
    :m_ActionType(ActionType), m_MotherType(MotherType), m_ActionID(ActionID){}

TimeAction::TimeAction(TString ActionType, TString MotherType, Int_t ActionID, Int_t MotherID)
    :m_ActionType(ActionType), m_MotherType(MotherType), m_ActionID(ActionID), m_MotherID(MotherID){}

TimeAction::~TimeAction(){}