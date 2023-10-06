#include "TString.h"
#include "TSystem.h"
#include "TChain.h"
#include "../../../source/CommonLib/SimEvent/SimEvent.h"
#include <iostream>

void RootScan()
{
    gSystem->Load("../CommonLib/libSimEventDict.so");
    TString FileName = "sim.root";

    //Print MCConfig info
    TChain* MCConfig = new TChain("MCConfig");
    MCConfig->Add(FileName);
    MCConfig->Scan();

    //Scan Events
    TChain* chain = new TChain("Sim");
    chain->Add(FileName);
    Int_t NumOfEntries = chain->GetEntries();
    std::cout << "Entries: " << NumOfEntries << std::endl;

    SimEvent* aEvent = NULL;
    chain->SetBranchAddress("SimEvent", &aEvent);
    chain->GetBranch("SimEvent")->SetAutoDelete(kTRUE);

    for(int entry = 0; entry < NumOfEntries; ++entry)
    {
        chain->GetEntry(entry);

        TMap* TrackMap = aEvent->GetTrackMap();
        TMap* StepMap = aEvent->GetDepositMap();
        Int_t TrackNumber = TrackMap->GetSize();
        Int_t StepNumber = StepMap->GetSize();
        std::cout << "Event: " << entry << "\tTrack Number: " << TrackNumber << "\tStep Number:" << StepNumber
        << "\tTruthE: " << aEvent->GetTruthEnergy() << std::endl;
        for(int iT = 1; iT <= TrackNumber; ++iT)
        {
            SimTrack* aTrack = aEvent->GetTrack(iT);
            std::cout << "\t-->Track: " << iT << "\tPDGID: " << aTrack->GetPDGID() << "\tTrackID: " << aTrack->GetTrackID() << "\tParantID: " << aTrack->GetParentID()
            << "\tLenght: " << aTrack->GetTrackLength() << "\tEdep: " << aTrack->GetEdep() << std::endl;
            std::vector<Int_t> StepIdx = aTrack->GetStepIdx();
            Int_t NumOfStepPerTrack = StepIdx.size();
            for(int iS = 0; iS < NumOfStepPerTrack; ++iS)
            {
                SimDeposit* aStep = aEvent->GetDeposit(StepIdx[iS]);
                std::cout << "\t\t-->Step: " << iS << "\tVolume: " << aStep->GetVolumeName() << "\tProcess: " << aStep->GetProcessName() 
                << "\tLength: " << aStep->GetStepLength() << "\tEdep: " << aStep->GetEdep() << std::endl;
                delete aStep;
            }
            delete aTrack;
        }
        TrackMap->Delete();
        StepMap->Delete();
        aEvent->MyClear();
    }
}