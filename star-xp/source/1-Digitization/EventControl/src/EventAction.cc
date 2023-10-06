#include "TMap.h"
#include "TRandom.h"

#include "EventAction.hh"
#include "StepAction.hh"
#include "SDBoxContainer.hh"
#include "TopMetalContainer.hh"
#include "TimeControl.hh"

EventAction::EventAction(SimEvent *aEvent, Double_t TimeShift)
    : m_Event(*aEvent), m_TimeShift(TimeShift), m_iStep(0), m_NumOfValidStep(0), TimeAction("Event", "InputFile")
{
    m_ValidStep.clear();

    m_TruthInfo.FiredSD.clear();
    m_TruthInfo.FirstSDStepTime = 0.;
    m_TruthInfo.IfPolPhot = 0;
    m_TruthInfo.IfPolPhotOnMCP = 0;
    m_TruthInfo.PolPhotElectronPos = TVector3(0., 0., 0.);
    m_TruthInfo.PolPhotElectronMom = TVector3(0., 0., 0.);

    TMap *TrackMap = m_Event.GetTrackMap();
    TMap *StepMap = m_Event.GetDepositMap();
    Int_t TrackNumber = TrackMap->GetSize();

    for (int iT = 1; iT <= TrackNumber; ++iT)
    {
        SimTrack *aTrack = m_Event.GetTrack(iT);
        std::vector<Int_t> StepIdx = aTrack->GetStepIdx();
        if (aTrack->GetPDGID() == 11 && aTrack->GetParentID() == 1 && m_Event.GetTrack(aTrack->GetParentID())->GetPDGID() == 22)
        {
            Int_t iParentLastStep = (m_Event.GetTrack(aTrack->GetParentID())->GetStepIdx()).back();
            SimDeposit* ParentLastStep = m_Event.GetDeposit(iParentLastStep);
            if (ParentLastStep->GetProcessName() == TString("pol-phot") && (ParentLastStep->GetVolumeName() == TString("GasSDPhys") || ParentLastStep->GetVolumeName() == TString("copper_glassFramePhys")))
            {
                TVector3 PolPos = aTrack->GetInitPos();
                Int_t iSD = SDBoxContainer::GetInstance().WhichSD(PolPos, true);
                m_TruthInfo.IfPolPhot = 1;
                m_TruthInfo.PolPhotElectronPos = SDBoxContainer::GetInstance()[iSD].GetRelativePoint(PolPos);
                m_TruthInfo.PolPhotElectronMom = aTrack->GetInitMom();
                if (ParentLastStep->GetVolumeName() == TString("copper_glassFramePhys"))
                {
                    m_TruthInfo.IfPolPhotOnMCP = 1;
                    if(TimeControl::DropAllMCPPol() || gRandom->Uniform(0., 1.) > SDBoxContainer::GetInstance()[iSD].PolPhotOnMCPScale()) 
                    {
                        m_ValidStep.clear();
                        break;
                    }
                }
            }
        }
        Int_t NumOfStepPerTrack = StepIdx.size();
        for (int iS = 0; iS < NumOfStepPerTrack; ++iS)
        {
            SimDeposit *aStep = m_Event.GetDeposit(StepIdx[iS]);
            if (aStep->GetVolumeName() == TString("GasSDPhys") && aStep->GetEdep() > 0 && aStep->GetPDGID() != 22) // Valid step: 在GasSD内有沉积能量的带电粒子step
            {
                m_ValidStep.push_back(*aStep);
                Int_t FiredSD = SDBoxContainer::GetInstance().WhichSD(aStep->GetPostPosition());
                if (FiredSD != -1)
                    m_TruthInfo.FiredSD.insert(FiredSD);
                else
                {
                    std::clog << "Warning!!! Got point ( " << aStep->GetPostPosition()[0] << ", " << aStep->GetPostPosition()[1] << ", " << aStep->GetPostPosition()[2] << " ) dosn't belong to any GasSD!!!" << std::endl;
                }
            }
        }
    }
    m_NumOfValidStep = m_ValidStep.size();
    if(TimeControl::Verbose()) std::cout << "EventAction::EventAction(): Info Find " << m_NumOfValidStep << " valid steps." << std::endl;
    if (m_NumOfValidStep > 0)
    {
        std::sort(m_ValidStep.begin(), m_ValidStep.end(), EventAction::myStepSort);

        m_EventStartTime = m_Event.GetTimeFlag() - m_TimeShift;
        m_EventTruthEnergy = m_Event.GetTruthEnergy();
        m_TruthInfo.FirstSDStepTime = m_ValidStep[0].GetPreT() + m_EventStartTime;
    }

    m_StartEvent = false;
}

EventAction::~EventAction() {}

std::pair<Double_t, TimeAction *> EventAction::Action(Double_t)
{
    if (m_NumOfValidStep <= m_iStep)
        return std::pair<Double_t, TimeAction *>(-1, NULL);

    if (m_StartEvent == false)
    {
        TopMetalContainer::GetInstance().GetOutputFilePtr()->WriteTruthInfo(&m_Event, &m_TruthInfo);
        m_StartEvent = true;
    }
    if (TimeControl::Verbose())
        std::cout << "NumOfValidStep: " << m_NumOfValidStep << "\tiStep: " << m_iStep << std::endl;

    StepAction *stepAction = new StepAction(m_ValidStep[m_iStep], m_EventStartTime);
    Double_t StepStartTime = m_ValidStep[m_iStep].GetPreT() + m_EventStartTime;
    ++m_iStep;
    return std::pair<Double_t, TimeAction *>(StepStartTime, stepAction);
}

Double_t EventAction::SetSingleParticleMode()
{
    SDBox* theBox = nullptr;
    if(m_TruthInfo.FiredSD.size() == 0)
    {
        theBox = &SDBoxContainer::GetInstance()[0];
    }
    else
    {
        theBox = &SDBoxContainer::GetInstance()[*m_TruthInfo.FiredSD.begin()];
    }
    //SDBox* theBox = &SDBoxContainer::GetInstance()[0];
    Double_t FrameTime = theBox->NumOfPixel().first * theBox->NumOfPixel().second * theBox->PixelTime();
    Double_t RandomTime = 0.;
    if(TimeControl::RandomTime())
    {
        RandomTime = gRandom->Uniform(FrameTime);
    }
    m_TruthInfo.FirstSDStepTime = m_TruthInfo.FirstSDStepTime - m_EventStartTime + RandomTime;
    m_EventStartTime = RandomTime;

    return RandomTime;
}