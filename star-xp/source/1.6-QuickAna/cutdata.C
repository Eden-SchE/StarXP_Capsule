#include "TFile.h"
#include "TTree.h"
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include "iostream"
#include "fstream"
#include "iterator"
#include "vector"
using namespace std;



void cutdata(){
    TFile* f0= new TFile("Feature.root");
    TTree* tree = (TTree*)f0->Get("t_data");
    ofstream outfileFrame("aftercut.data",ios::binary);
    Short_t data[72][72];
    short outsigdata[72*72] = {0};
    Int_t numEvent;         
    Int_t numFrame;         
    Int_t timeYear;         
    Int_t timeMon;          
    Int_t timeMday;         
    Int_t timeHour;         
    Int_t timeMin;          
    Int_t timeSec;          
    Int_t timeNsec;         
    Int_t peakADC;          
    Double_t peakX;   
    Double_t peakY;         
    Int_t sumADC;           
    Int_t HitADC;          
    Int_t sumHit;          
    Double_t lenXY;        
    Double_t ratioXY;      
    Double_t BcenterX;      
    Double_t BcenterY;      
    Double_t lenBtoP;      
    Int_t length;          
    Double_t width;         
    Int_t outline;          
    Int_t edgeHit;          
    Double_t ellip;
    Double_t circ;
    tree->SetBranchAddress("data",&data);
    tree->SetBranchAddress("numEvent",&numEvent);
    tree->SetBranchAddress("numFrame",&numFrame);
    tree->SetBranchAddress("timeYear",&timeYear);
    tree->SetBranchAddress("timeMon",&timeMon);
    tree->SetBranchAddress("timeMday",&timeMday);
    tree->SetBranchAddress("timeHour",&timeHour);
    tree->SetBranchAddress("timeMin",&timeMin);
    tree->SetBranchAddress("timeSec",&timeSec);
    tree->SetBranchAddress("timeNsec",&timeNsec);
    tree->SetBranchAddress("peakADC",&peakADC);
    tree->SetBranchAddress("peakX",&peakX);
    tree->SetBranchAddress("peakY",&peakY);
    tree->SetBranchAddress("sumADC",&sumADC);
    tree->SetBranchAddress("HitADC",&HitADC);
    tree->SetBranchAddress("sumHit",&sumHit);
    tree->SetBranchAddress("lenXY",&lenXY);
    tree->SetBranchAddress("ratioXY",&ratioXY);
    tree->SetBranchAddress("BcenterX",&BcenterX);
    tree->SetBranchAddress("BcenterY",&BcenterY);
    tree->SetBranchAddress("lenBtoP",&lenBtoP);
    tree->SetBranchAddress("length",&length);
    tree->SetBranchAddress("width",&width);
    tree->SetBranchAddress("outline",&outline);
    tree->SetBranchAddress("edgeHit",&edgeHit);
    tree->SetBranchAddress("ellip",&ellip);
    tree->SetBranchAddress("circ",&circ);
    int EventNum = tree->GetEntries();
    for(int i =0; i<EventNum;i++){
        tree->GetEntry(i);
        if(/*put your cut condition here, eg. peakADC>40 && HitADC>30*/){
            for(int ix=0;ix<72;ix++){
                for(int iy=0;iy<72;iy++){
                    outfileFrame.write((char*)&(data[ix][iy]),sizeof(data[ix][iy]));
                }
            }
        }
    }
    outfileFrame.close();
}