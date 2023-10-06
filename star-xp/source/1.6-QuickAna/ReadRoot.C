#include "TFile.h"
#include "TTree.h"
#include "TH2F.h"

void ReadRoot(){
    TFile* f = new TFile("out.root");
    TTree* tree = (TTree*)f->Get("tree");
    double gmcptime;
    TH2F* topmetalpic= NULL;

    tree->SetBranchAddress("gmcptime",&gmcptime);
    tree->SetBranchAddress("topmetalpic",&topmetalpic);

    int num = tree->GetEntries();
    for(int i = 0; i<num; i++){
        tree->GetEntry(i);
        cout<<"gmcp time is: "<<gmcptime<<endl;
        topmetalpic->Draw("colz");
    }
}