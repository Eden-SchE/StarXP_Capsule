#include "TRandom.h"

#include "TimeControl.hh"
#include "SDBoxContainer.hh"
#include "EventAction.hh"
#include "FilterCreator.hh"
#include "YamlConfig.hh"

#include <iostream>
#include <set>

Int_t TimeControl::m_Verbose = 0;
bool TimeControl::m_GMCPOutput = false;
bool TimeControl::m_DropAllMCPPol = false;
bool TimeControl::m_RandomTime = true;

TimeControl::TimeControl()
{
    if(!YamlConfig::GetInstance().IfValid())
    {
        std::cerr << "Error!!! YamlConfig has not been initialized!" << std::endl;
        exit(0);
    }
    m_Verbose = YamlConfig::GetInstance().GetYaml()["Verbose"].as<int>(0);
    m_ExitTime = YamlConfig::GetInstance().GetYaml()["ExitTime"].as<double>(-1.);
    m_ExitEvents = YamlConfig::GetInstance().GetYaml()["ExitEvents"].as<int>(-1);
    m_SaveAllFrame = YamlConfig::GetInstance().GetYaml()["SaveAllFrame"].as<bool>(false);
    m_GMCPOutput = YamlConfig::GetInstance().GetYaml()["GMCPOutput"].as<bool>(false);
    m_DropAllMCPPol = YamlConfig::GetInstance().GetYaml()["DropAllMCPPol"].as<bool>(false);
    m_RandomTime = YamlConfig::GetInstance().GetYaml()["RandomTime"].as<bool>(true);

    YAML::Node inputNode = YamlConfig::GetInstance().GetYaml()["Input"];
    int numOfInputFiles = inputNode["Files"].size();
    m_InputFiles.resize(numOfInputFiles, "");
    m_InputFilesTimeShift.resize(numOfInputFiles, 0.);

    for(int i = 0; i < numOfInputFiles; ++i)
    {
        m_InputFiles[i] = inputNode["Files"][i].as<std::string>("sim.root");
    }
    if(inputNode["TimeShift"].IsDefined())
    {
        if(inputNode["TimeShift"].size() == numOfInputFiles)
        {
            for (int i = 0; i < numOfInputFiles; ++i)
            {
                m_InputFilesTimeShift[i] = inputNode["TimeShift"][i].as<double>(0.);
            }
        }
        else
        {
            std::cerr << "Error!!! Length of TimeShift not equal to length of input files!" << std::endl;
            exit(0);
        }
    }

    m_TimeLine = new TimeLine();
    m_TimeLine->Clear();

    m_ActionTypes.clear();
    m_ActionTypes.push_back("InputFile");
    m_ActionTypes.push_back("Event");
    m_ActionTypes.push_back("Step");
    m_ActionTypes.push_back("Multiplication");
    m_ActionTypes.push_back("TopMetal");
    m_ActionTypes.push_back("GMCP");

    for (auto ActionType:m_ActionTypes)
    {
        std::vector<TimeAction*>* ActionPtrVector = new std::vector<TimeAction*>();
        ActionPtrVector->clear();
        m_ActionPtrContainer[ActionType] = ActionPtrVector;
    }
}

TimeControl::~TimeControl()
{
    for(const auto& ActionPtrVector:m_ActionPtrContainer)
    {
        if (ActionPtrVector.first != "TopMetal" && ActionPtrVector.first != "GMCP")
            for (auto ActionPtr : *(ActionPtrVector.second))
            {
                if (ActionPtr != NULL)
                {
                    // std::clog << "Warning!!! Found a Action that didn't deleted before the destructor of TimeControl, Please check it!!!" << std::endl;
                    // std::clog << "Action Ptr:  " << ActionPtr << std::endl;
                    // std::clog << "Action Type: " << ActionPtr->GetActionType() << std::endl;
                    // std::clog << "Action ID:   " << ActionPtr->GetActionID() << std::endl;
                    delete ActionPtr;
                }
            }
        delete ActionPtrVector.second;
    }
}

void TimeControl::Process()
{
    std::string Mode = YamlConfig::GetInstance().GetYaml()["Mode"].as<std::string>();
    if (Mode == "SingleParticle")
        this->SingleParticleMode();
    else if (Mode == "TimeLine")
        this->TimeLineMode();
    else
    {
        std::cerr << "Error!!! Input Mode \" " << Mode << " \" NOT FOUND!" << std::endl;
        exit(0);
    }
}

TString TimeControl::ReadTruthInfo(Int_t iF, TString FilePath)
{
    TChain chain("MCConfig");
    chain.Add(FilePath);
    
    Double_t GenTime = 0.;
    Int_t GenEvents = 0;
    Int_t GenValidEvents = 0;

    chain.SetBranchAddress("GenTime", &GenTime);
    chain.SetBranchAddress("GenEvents", &GenEvents);
    chain.SetBranchAddress("GenValidEvents", &GenValidEvents);

    chain.GetEntry(0);

    chain.ResetBranchAddresses();

    if(iF == 0)
        SDBoxContainer::GetInstance().ReadSDConfig(&chain);    

    if(GenTime > 0) return TString("GenTime");
    else if(GenEvents > 0) return TString("GenEvents");
    else if(GenValidEvents > 0) return TString("GenValidEvents");
    else return TString("");

}

void TimeControl::SingleParticleMode()
{
    //Load input simulation files
    Int_t NumOfFiles = m_InputFiles.size();

    if (NumOfFiles > 1)
    {
        std::cerr << "Error!!! SingleParticleMode only suppot one file input!!!" << std::endl;
        exit(EXIT_FAILURE);
    }

    //Read truth info
    this->ReadTruthInfo(0, TString(m_InputFiles[0]));

    (m_ActionPtrContainer["InputFile"])->push_back(new InputFileReader(TString(m_InputFiles[0]), 0., 0));

    //Filter initialization
    FilterCreator::GetInstance().SetFilter(YamlConfig::GetInstance().GetYaml()["Filter"]);

    //TopMetal initialization
    m_TopMetalContainer = &TopMetalContainer::GetInstance();
    m_TopMetalContainer->Initialize();
    for(Int_t iT = 0; iT < SDBoxContainer::NumOfSD(); ++iT)
    {
        (m_ActionPtrContainer["TopMetal"])->push_back(m_TopMetalContainer->iTopMetal(iT));
        if(m_GMCPOutput)
            (m_ActionPtrContainer["GMCP"])->push_back(m_TopMetalContainer->iGMCP(iT));
    }

    //Run TimeLine
    for(Int_t iE = 0; ; ++iE)
    {
        std::pair<Double_t, TimeAction *> EvtAction = (*(m_ActionPtrContainer["InputFile"]))[0]->Action(0.);
        if(EvtAction.first < 0) break;
        m_TopMetalContainer->ClearAllCharge();

        Double_t RandomTime = (dynamic_cast<EventAction*>(EvtAction.second))->SetSingleParticleMode();
        m_TimeLine->InsertNode(RandomTime, TimeActionInfo("Event", m_ActionPtrContainer[EvtAction.second->GetActionType()]->size()));
        (m_ActionPtrContainer[EvtAction.second->GetActionType()])->push_back(EvtAction.second);

        while (!m_TimeLine->Empty())
        {
            TimeNode *Node = m_TimeLine->GetFirstNode();

            if (m_Verbose > 1)
                std::cout << std::endl
                          << "-->At time " << Node->m_TimeFlag << " in node " << Node->m_Action.m_ActionType << " " << Node->m_Action.m_ActionID << std::endl;
            if (Node->m_Action.m_ActionType == TString("Event"))
            {
                // Catch a new event, Init and insert topmetal action.
                EventAction *eventAction = dynamic_cast<EventAction *>((*(m_ActionPtrContainer[Node->m_Action.m_ActionType]))[Node->m_Action.m_ActionID]);
                if (eventAction->Start() == false && eventAction->NumOfValidStep() > 0)
                {
                    std::set<UInt_t> FiredSD = eventAction->GetFiredSD();
                    for (auto iSD : FiredSD)
                    {
                        m_TimeLine->InsertNode(Node->m_TimeFlag, TimeActionInfo(TString("TopMetal"), Int_t(iSD)));
                        if(m_GMCPOutput)
                            m_TimeLine->InsertNode(Node->m_TimeFlag, TimeActionInfo(TString("GMCP"), Int_t(iSD)));
                    }
                }
            }
            std::pair<Double_t, TimeAction *> Action = (*(m_ActionPtrContainer[Node->m_Action.m_ActionType]))[Node->m_Action.m_ActionID]->Action(Node->m_TimeFlag);
            if (Action.first < 0)
            {
                if (Node->m_Action.m_ActionType != TString("TopMetal") && Node->m_Action.m_ActionType != TString("GMCP"))
                {
                    if (m_Verbose > 1)
                        std::cout << "Delete Action" << std::endl;
                    delete (*(m_ActionPtrContainer[Node->m_Action.m_ActionType]))[Node->m_Action.m_ActionID];
                    (*(m_ActionPtrContainer[Node->m_Action.m_ActionType]))[Node->m_Action.m_ActionID] = NULL;
                }
            }
            else if (Action.second != NULL)
            {
                if (m_Verbose > 1)
                    std::cout << "Daughter Action " << Action.second->GetActionType();
                // Insert action
                Action.second->SetActionID((m_ActionPtrContainer[Action.second->GetActionType()])->size());
                if (m_Verbose > 1)
                    std::cout << " " << Action.second->GetActionID() << " time " << Action.first << std::endl;
                (m_ActionPtrContainer[Action.second->GetActionType()])->push_back(Action.second);
                Action.second->SetMotherID(Node->m_Action.m_ActionID);
                m_TimeLine->InsertNode(Action.first, TimeActionInfo(Action.second->GetActionType(), Action.second->GetActionID()));
                // Insert mother action
                m_TimeLine->InsertNode(Action.first, TimeActionInfo(Node->m_Action.m_ActionType, Node->m_Action.m_ActionID));
            }
            else
            {
                // Insert mother action
                m_TimeLine->InsertNode(Action.first, TimeActionInfo(Node->m_Action.m_ActionType, Node->m_Action.m_ActionID));
            }
            if (m_Verbose > 1)
                std::cout << "-->End of " << Node->m_Action.m_ActionType << " " << Node->m_Action.m_ActionID << " action" << std::endl;

            Node = NULL;
            m_TimeLine->DeleteFirstNode();
            if (m_Verbose > 1)
                m_TimeLine->Print();
        }
        m_TimeLine->Clear();
        if(m_Verbose)
            std::cout << "TimeControl::SingleParticleMode(): End of Event: " << iE << std::endl;
        if(m_ExitEvents > 0 && m_ExitEvents <= iE)
            break;
    }
    m_TimeLine->Clear();
    m_TopMetalContainer->GetOutputFilePtr()->SaveAll();
}

void TimeControl::TimeLineMode()
{
    //Load input simulation files
    Int_t NumOfFiles = m_InputFiles.size();
    bool ifShift = (m_InputFilesTimeShift.size() != 0);

    if (ifShift && Int_t(m_InputFilesTimeShift.size()) != NumOfFiles)
    {
        std::cerr << "Error!!! Number of input files and time shift not matched!!!" << std::endl;
        exit(EXIT_FAILURE);
    }

    for(Int_t iF = 0; iF < NumOfFiles; ++iF)
    {
        //Check if generated with GenTime mode
        if(this->ReadTruthInfo(iF, TString(m_InputFiles[iF])) != TString("GenTime"))
        {
            std::cerr << "Error!!! TimeLineMode only accept MC samples generated with GenTime mode!!!" << std::endl;
            std::cerr << "Please check file: " << m_InputFiles[iF] << std::endl;
            exit(EXIT_FAILURE);
        }

        if(ifShift)
            (m_ActionPtrContainer["InputFile"])->push_back(new InputFileReader(TString(m_InputFiles[iF]), m_InputFilesTimeShift[iF], iF));
        else
            (m_ActionPtrContainer["InputFile"])->push_back(new InputFileReader(TString(m_InputFiles[iF]), 0., iF));

        //InputFile initialization
        m_TimeLine->InsertNode(0., TimeActionInfo("InputFile", iF));
    }

    //Filter initialization
    FilterCreator::GetInstance().SetFilter(YamlConfig::GetInstance().GetYaml()["Filter"]);

    //TopMetal initialization
    m_TopMetalContainer = &TopMetalContainer::GetInstance();
    m_TopMetalContainer->Initialize();
    for(Int_t iT = 0; iT < SDBoxContainer::NumOfSD(); ++iT)
    {
        (m_ActionPtrContainer["TopMetal"])->push_back(m_TopMetalContainer->iTopMetal(iT));
        if(m_GMCPOutput)
            (m_ActionPtrContainer["GMCP"])->push_back(m_TopMetalContainer->iGMCP(iT));
        if(m_SaveAllFrame)
        {
            m_TimeLine->InsertNode(0., TimeActionInfo(TString("TopMetal"), iT));
            m_TopMetalContainer->iTopMetal(iT)->SaveAllFrame();
        }
    }

    //Run TimeLine
    while(!m_TimeLine->Empty())
    {
        TimeNode* Node = m_TimeLine->GetFirstNode();

        if(m_ExitTime > 0 && Node->m_TimeFlag > m_ExitTime) break;

        if(m_Verbose > 1) std::cout << std::endl << "-->At time " << Node->m_TimeFlag << " in node " << Node->m_Action.m_ActionType << " " << Node->m_Action.m_ActionID << std::endl;
        if(Node->m_Action.m_ActionType == TString("Event") && !m_SaveAllFrame)
        {
            //Catch a new event, Init and insert topmetal action.
            EventAction *eventAction = dynamic_cast<EventAction *>((*(m_ActionPtrContainer[Node->m_Action.m_ActionType]))[Node->m_Action.m_ActionID]);
            if (eventAction->Start() == false && eventAction->NumOfValidStep() > 0)
            {
                std::set<UInt_t> FiredSD = eventAction->GetFiredSD();
                for (auto iSD : FiredSD)
                {
                    m_TopMetalContainer->iTopMetal(iSD)->InitTopMetal(Node->m_TimeFlag);
                    m_TimeLine->InsertNode(Node->m_TimeFlag, TimeActionInfo(TString("TopMetal"), Int_t(iSD)));
                    if(m_GMCPOutput)
                    {
                        m_TopMetalContainer->iGMCP(iSD)->InitGMCP(Node->m_TimeFlag);
                        m_TimeLine->InsertNode(Node->m_TimeFlag, TimeActionInfo(TString("GMCP"), Int_t(iSD)));
                    }
                }
            }
        }
        std::pair<Double_t, TimeAction*> Action = (*(m_ActionPtrContainer[Node->m_Action.m_ActionType]))[Node->m_Action.m_ActionID]->Action(Node->m_TimeFlag);
        if(Action.first < 0) 
        {
            if (Node->m_Action.m_ActionType != TString("TopMetal") && Node->m_Action.m_ActionType != TString("GMCP"))
            {
                if(m_Verbose > 1) std::cout << "Delete Action" << std::endl;
                delete (*(m_ActionPtrContainer[Node->m_Action.m_ActionType]))[Node->m_Action.m_ActionID];
                (*(m_ActionPtrContainer[Node->m_Action.m_ActionType]))[Node->m_Action.m_ActionID] = NULL;
            }
        }
        else if(Action.second != NULL)
        {
            if(m_Verbose > 1) std::cout << "Daughter Action " << Action.second->GetActionType();
            //Insert action
            Action.second->SetActionID((m_ActionPtrContainer[Action.second->GetActionType()])->size()); 
            if(m_Verbose > 1) std::cout << " " << Action.second->GetActionID() << " time " << Action.first << std::endl;
            (m_ActionPtrContainer[Action.second->GetActionType()])->push_back(Action.second);
            Action.second->SetMotherID(Node->m_Action.m_ActionID); 
            m_TimeLine->InsertNode(Action.first, TimeActionInfo(Action.second->GetActionType(), Action.second->GetActionID()));
            //Insert mother action
            m_TimeLine->InsertNode(Action.first, TimeActionInfo(Node->m_Action.m_ActionType, Node->m_Action.m_ActionID));
        }
        else
        {
            //Insert mother action
            m_TimeLine->InsertNode(Action.first, TimeActionInfo(Node->m_Action.m_ActionType, Node->m_Action.m_ActionID));
        }
        if(m_Verbose > 1) std::cout << "-->End of " << Node->m_Action.m_ActionType << " " << Node->m_Action.m_ActionID << " action" << std::endl;

        Node = NULL;
        m_TimeLine->DeleteFirstNode();
        if(m_Verbose > 1) m_TimeLine->Print();
    }
    m_TimeLine->Clear();
    m_TopMetalContainer->GetOutputFilePtr()->SaveAll();
}