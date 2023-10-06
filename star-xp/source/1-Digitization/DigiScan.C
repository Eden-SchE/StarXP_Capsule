#include "TH2F.h"
#include "TString.h"
#include "TSystem.h"
#include <unistd.h>
#include "TChain.h"
#include "TCanvas.h"
#include <iostream>
#include <fstream>
#include <string>
#include "TVector3.h"
#include "TFile.h"
using namespace std;
#include "TString.h"
#include "TSystem.h"
#include "TChain.h"
#include "../../../source/CommonLib/SimEvent/SimEvent.h"
#include <iostream>

void DigiScan()
{
    gSystem->Load("../CommonLib/libSimEventDict.so");
    const Int_t FramePerEvent = 10;

    TFile* CalibFile = new TFile("/mnt/e/star-xp2/source/1-Digitization/CalibFiles/pededata.root");
    TFile* AngleFile = new TFile("Angle.root","recreate");

    ofstream outfile("/mnt/e/StarXPdata/test.data",ios::binary);
    ofstream txtFile;		
	txtFile.open("/mnt/e/StarXPdata/test.txt");
    if (!txtFile.is_open())
    {
        cout << "文件打开失败" << endl;
        exit(-1);
    }

    TString FileName = "/mnt/e/star-xp/build/source/1-Digitization/digi.root";
    TFile *f =new TFile("/mnt/e/star-xp/build/source/1-Digitization/digi.root");
    TTree* Truthchain = (TTree*)f->Get("TruthTree");
    TChain* chain = new TChain("TopMetal_0");

    TH1F* myAngle = new TH1F("myAngle","myAngle",100,-M_PI,M_PI);
    
    chain->Add(FileName);

    //if(access("Plot", 00) == -1) gSystem->Exec("mkdir Plot");

    TVector3* PolPhotElectronPos = new TVector3();
    TVector3* PolPhotElectronMom = new TVector3();
    TH2F* CalibHist = (TH2F*)CalibFile->Get("pede_mean");

    Truthchain->SetBranchAddress("PolPhotElectronPos", &PolPhotElectronPos);
    Truthchain->SetBranchAddress("PolPhotElectronMom", &PolPhotElectronMom);

    Double_t FrameWriteTime;
    TH2F* TopMetalHist = NULL;
    TH2F* EventFrames[FramePerEvent] = {NULL};
    chain->SetBranchAddress("FrameWriteTime", &FrameWriteTime);
    chain->SetBranchAddress("TopMetalHist", &TopMetalHist);
    chain->GetBranch("TopMetalHist")->SetAutoDelete(kTRUE);

    //TCanvas* can = new TCanvas("can", "", 4000, 1200);
    //can->Divide(5,2);
    //gStyle->SetOptStat(0);

    long long NumTruth = Truthchain->GetEntries();
    cout<<"Truth number is:"<< NumTruth<<endl;
    for(int iTru=0; iTru<NumTruth; iTru++ ){
        Truthchain->GetEntry(iTru);
        TVector3 xAxis = TVector3(1,0,0);
        TVector3 projMom = TVector3(PolPhotElectronMom->X(),PolPhotElectronMom->Y(),0);
        float angle = xAxis.Angle(projMom);
        if(PolPhotElectronMom->Y()<0){
            angle*=-1.;
        }
        myAngle->Fill(angle);
        txtFile<<(iTru+1)<<setprecision(3)<<"\t"<<angle<<"\t\t"<<PolPhotElectronPos->X()<<"\t"<<PolPhotElectronPos->Y()<<endl;
    }
    txtFile.close();

    Long64_t NumOfEntries = chain->GetEntries();
    printf("Number is: %lld\n",NumOfEntries);
    for (int iE = 0; iE < NumOfEntries; ++iE)
    {
        chain->GetEntry(iE);
        
        Int_t EventNum = ceil(iE/FramePerEvent) + 1;
        Int_t FrameNum = iE - (EventNum - 1) * FramePerEvent;
        //can->cd(FrameNum+1);
        
        if(iE%FramePerEvent == 0){
            //cout<<"loop: "<<iE<<endl;
            double pixelSum = 0.;
            for(int ix=1; ix<=72; ix++){
                for(int iy=1; iy<=72; iy++){
                    pixelSum = (double(TopMetalHist->GetBinContent(ix,iy))-CalibHist->GetBinContent(ix,iy));
                    if(ix==1 || iy ==1){
                        pixelSum =0;
                    }
		    if(pixelSum<20){
		     pixelSum =0;
		    }
                    //cout<<pixelSum<<endl;
		    // if(pixelSum>1300){
		    //  printf("pixel is %lf\n",pixelSum);
		    //  short atmp = (short)(pixelSum);
		    //  printf("change is %u\n",atmp);
		    // }
            short tmp = (short)(pixelSum);
		    //printf("change is %u\n",tmp);
                    outfile.write((char*)&tmp, sizeof(short));
                }
            }
            //std::cout<<"sum is: "<<pixelSum<<std::endl;
        }
        
        // EventFrames[FrameNum] = new TH2F(*TopMetalHist);
        // //EventFrames[FrameNum]->Draw("colz");
        // //can->Update();
        // if((iE + 1) % FramePerEvent == 0)
        // {
        //     //can->SaveAs(TString::Format("Plot/Event_%d.png", EventNum));
        //     for(int i = 0; i < FramePerEvent; ++i)
        //     {
        //         delete EventFrames[i];
        //     }
        // }
    }
    outfile.close();
    AngleFile->cd();
    myAngle->Write();
    AngleFile->Close();
}
