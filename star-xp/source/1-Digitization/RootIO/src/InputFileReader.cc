#include "InputFileReader.hh"
#include "EventAction.hh"
#include "TimeControl.hh"

#include <iostream>

InputFileReader::InputFileReader()
    :TimeAction("InputFile", "", 0, 0)
{
    m_FileName = "";
    m_Chain = new TChain("Sim");
    m_Event = new SimEvent();
    //m_Event = NULL;
    m_Entry = 0;
    m_TimeShift = 0.;
    m_NumOfEntries = 0;
    m_EventTimeFlag = 0.;
}

InputFileReader::InputFileReader(TString FileName, Double_t TimeShift, Int_t ActionID)
    :TimeAction("InputFile", "", ActionID, 0)
{
    m_FileName = FileName;
    m_TimeShift = TimeShift;
    m_Chain = new TChain("Sim");
    m_Event = new SimEvent();
    m_Entry = 0;
    m_NumOfEntries = 0;
    m_EventTimeFlag = 0.;
    this->LoadSimFile(m_FileName);
}

void InputFileReader::LoadSimFile(TString FileName)
{
    m_Chain->Add(FileName);
    m_NumOfEntries = m_Chain->GetEntries();
    if(m_NumOfEntries == 0)
    {
        std::cerr << "Error!!! Entries number of file: " << FileName << " is ZERO!!!" << std::endl;
        exit(EXIT_FAILURE);
    }
    m_Chain->SetBranchAddress("SimEvent", &m_Event);
    m_Chain->GetBranch("SimEvent")->SetAutoDelete(true);

    //Shift time if necessary
    if(m_TimeShift > 0)
    {
        while(m_EventTimeFlag < m_TimeShift)
        {
            this->NextEvent();
            if(m_Event != NULL)
                m_EventTimeFlag = m_Event->GetTimeFlag();
            else
            {
                std::cerr << "Error!!! File " << m_FileName << " have no event after input time shift " << m_TimeShift << "s" << std::endl;
                exit(EXIT_FAILURE);
            }
        }
    }
    else
    {
        this->NextEvent();
    }
}

void InputFileReader::NextEvent()
{
    if(m_Entry >= m_NumOfEntries)
    {
        m_Event->MyClear();
        m_Event = NULL;
    }
    else
    {
        m_Event->MyClear();
        m_Chain->GetEntry(m_Entry);
        ++m_Entry;
        if(TimeControl::Verbose())
        {
            std::cout << "InputFileReader::LoadSimFile(): Reading Event " << m_Entry << std::endl;
            std::cout << "Truth Energy: " << m_Event->GetTruthEnergy() << std::endl;
        }
    }
}

std::pair<Double_t, TimeAction*> InputFileReader::Action(Double_t)
{

    if(m_Event == NULL)
        return std::pair<Double_t, TimeAction*>(-1., NULL);

    EventAction* evtAction = new EventAction(m_Event, m_TimeShift);
    m_EventTimeFlag = m_Event->GetTimeFlag();

    this->NextEvent();
    return std::pair<Double_t, TimeAction *>(m_EventTimeFlag-m_TimeShift, evtAction);
}

InputFileReader::~InputFileReader()
{
    m_Chain->Delete();
}