#include "TFile.h"
#include "TTree.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TH1F.h"

void check(){
    TFile* f0 = new TFile("out8keV.root");
    TFile* fsave = new TFile("./Event8keV/ana8keV.root","recreate");
    TH2F* ADCDis = new TH2F("ADCDis","ADCDis",300,500,1600,300,1000,11000);
    TH1F* TimeDif = new TH1F("TimeDif","TimeDif",20,0,6);
    TCanvas* c1 = new TCanvas("GMCP_Topmetal_ADC","GMCP_Topmetal_ADC",800,600);
    TTree* tr = (TTree*)f0->Get("tree");
    double topmetaltime;
    double gmcpTime;
    int SumTopmetalADC;
    int gmcpADC;
    int num = tr->GetEntries();
    tr->SetBranchAddress("topmetaltime",&topmetaltime);
    tr->SetBranchAddress("gmcpTime",&gmcpTime);
    tr->SetBranchAddress("SumTopmetalADC",&SumTopmetalADC);
    tr->SetBranchAddress("gmcpADC",&gmcpADC);
    for(int i =0; i<num;i++){
        tr->GetEntry(i);
        ADCDis->Fill(gmcpADC,SumTopmetalADC);
        TimeDif->Fill(double(topmetaltime-gmcpTime));
    }
    ADCDis->Draw("colz");
    fsave->cd();
    TimeDif->Write();
    c1->Write();
    fsave->Close();


}

