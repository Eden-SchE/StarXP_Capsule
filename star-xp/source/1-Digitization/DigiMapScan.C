#include "TH2F.h"
#include "TString.h"
#include "TSystem.h"
#include <unistd.h>

Double_t GetHistValue(TH2F* th2, UInt_t iX, UInt_t iY)
{
    //读取th2中对应iX, iY bin中的值
    Double_t Value = th2->GetBinContent(th2->GetBin(iX+1, iY+1));
    if (Value != 0)
        return Value;

    //如果值为0，则取周围值做平均
    Int_t ValidBin = 0;
    Double_t Average = 0.;
    Int_t it;
    for(it = 1; ; ++it)
    {
        for(Int_t ix = -it; ix <= it; ++ix)
        {
            for(Int_t iy = -it; iy <= it; ++iy)
            {
                Double_t val = th2->GetBinContent(th2->GetBin(iX+ix+1, iY+iy+1));
                if(val != 0.)
                {
                    ++ValidBin;
                    Average+=val;
                }
            }
        }
        if(ValidBin > 0) break;
    }
    return Average/Double_t(ValidBin);
}

void DigiMapScan()
{
    TFile* CalibFile = new TFile("CalibFiles/pededata.root");
    TH2F* th2_InCalib = (TH2F*)CalibFile->Get("pede_mean");
    TH2F* th2_Calib = new TH2F("th2_Calib", "", 72, 0, 73, 72, 0, 73);
    for(int iX = 1; iX <= 72; ++iX)
    {
        for (int iY = 1; iY <= 72; ++iY)
        {
            th2_Calib->SetBinContent(th2_Calib->GetBin(iX, iY), GetHistValue(th2_InCalib, iX, iY) + 20.);
        }
    }

    const Int_t FramePerEvent = 10;

    TString FileName = "digi.root";
    TChain* chain = new TChain("TopMetal_0");
    chain->Add(FileName);

    if(access("Plot", 00) == -1) gSystem->Exec("mkdir Plot");

    Double_t FrameWriteTime;
    TH2F* TopMetalHist = NULL;
    TH2F* EventFrames[FramePerEvent] = {NULL};
    chain->SetBranchAddress("FrameWriteTime", &FrameWriteTime);
    chain->SetBranchAddress("TopMetalHist", &TopMetalHist);
    chain->GetBranch("TopMetalHist")->SetAutoDelete(kTRUE);

    TCanvas* can = new TCanvas("can", "", 4000, 1200);
    can->Divide(5,2);
    gStyle->SetOptStat(0);

    Long64_t NumOfEntries = chain->GetEntries();
    for (int iE = 0; iE < NumOfEntries; ++iE)
    {
        chain->GetEntry(iE);
        Int_t EventNum = ceil(iE/FramePerEvent) + 1;
        Int_t FrameNum = iE - (EventNum - 1) * FramePerEvent;
        can->cd(FrameNum+1);
        EventFrames[FrameNum] = new TH2F(*TopMetalHist);
        EventFrames[FrameNum]->Add(th2_Calib, -1.);
        for (int iX = 1; iX <= 72; ++iX)
        {
            for (int iY = 1; iY <= 72; ++iY)
            {
                if(EventFrames[FrameNum]->GetBinContent(EventFrames[FrameNum]->GetBin(iX, iY)) < 0.)
                {
                    EventFrames[FrameNum]->SetBinContent(EventFrames[FrameNum]->GetBin(iX, iY), 0.);
                }
            }
        }
        EventFrames[FrameNum]->Draw("colz");
        can->Update();
        if((iE + 1) % FramePerEvent == 0)
        {
            can->SaveAs(TString::Format("Plot/Event_%d.png", EventNum));
            for(int i = 0; i < FramePerEvent; ++i)
            {
                delete EventFrames[i];
            }
        }
    }
}
