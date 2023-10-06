#include "TH2F.h"
#include "TString.h"
#include "TSystem.h"
#include <unistd.h>

void SimplePlot()
{
    TString FileName = "digi.root";
    TChain* chain = new TChain("TopMetal_0");
    chain->Add(FileName);

    if(access("Plot", 00) == -1) gSystem->Exec("mkdir Plot");

    Int_t iEvent, iFrame;
    TH2F* TopMetalHist = NULL;
    chain->SetBranchAddress("iEvent", &iEvent);
    chain->SetBranchAddress("iFrame", &iFrame);
    chain->SetBranchAddress("TopMetalHist", &TopMetalHist);
    chain->GetBranch("TopMetalHist")->SetAutoDelete(kTRUE);

    TCanvas* can = new TCanvas("can", "", 800, 600);
    gStyle->SetOptStat(0);

    Long64_t NumOfEntries = chain->GetEntries();
    for (int iE = 0; iE < NumOfEntries; ++iE)
    {
        chain->GetEntry(iE);
        TopMetalHist->Draw("colz");
        can->Update();
        can->SaveAs(TString::Format("Plot/Event_%d_Frame_%d.png", iEvent, iFrame));
    }
}
