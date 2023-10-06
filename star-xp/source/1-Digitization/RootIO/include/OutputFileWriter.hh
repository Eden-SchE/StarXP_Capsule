#ifndef _OutputFileWriter_h_Digi_
#define _OutputFileWriter_h_Digi_

#include "TString.h"
#include "TH2F.h"
#include "TFile.h"
#include "TTree.h"
#include "TVector3.h"

#include "EventAction.hh"
#include "SimEvent.h"

#include <vector>
#include <map>
#include <set>

#define MAXSD 50

class TimeAction;
class OutputFileWriter
{
public:
    OutputFileWriter();
    ~OutputFileWriter();

    TH2F* CreateTopMetalTree(UInt_t iTopMetal);

    void WriteTruthInfo(SimEvent*, TruthInfo*);
    void WriteFrame(UInt_t, Double_t, UInt_t);
    void WriteGMCP(UInt_t, Double_t, Double_t);

    void SaveAll();

    void WriteFile();

private:
    //void WriteHistToMap(UInt_t);
    bool m_TotalTruth;

    TString m_FileName;
    TFile* m_File;
    SimEvent* m_SimEvent;

    //Branch for DifiConfig
    //TTree* m_ConfigTree;
    //DigitizationConfig m_Config;

    //Branchs for Truth Tree
    TTree* m_TruthTree;
    Double_t m_TruthEnergy;
    TString  m_TruthParticle;
    TVector3 m_TruthPosition;
    TVector3 m_TruthPolarization;
    TVector3 m_TruthMomentumDirection;
    Double_t m_TimeFlag;
    Double_t m_FirstSDStepTime;
    Int_t m_NumOfFiredSD;
    Int_t m_FiredSD[MAXSD];
    Int_t m_iEvent;
    TVector3 m_PolPhotElectronPos;
    TVector3 m_PolPhotElectronMom;
    Int_t m_IfPolPhot;
    Int_t m_IfPolPhotOnMCP;

    //Branchs for TopMetal Tree
    std::vector<TTree*> m_TopMetalTree;
    Double_t m_FrameWriteTime[MAXSD];
    std::vector<TH2F*> m_TopMetalHist;
    Int_t m_iFrame[MAXSD];
    //std::vector<std::map<std::pair<Int_t, Int_t>, Double_t>*> m_MapTopMetalHist;

    //Branchs for GMCP Tree
    std::vector<TTree*> m_GMCPTree;
    Int_t m_iGMCP[MAXSD];
    Double_t m_GMCPTime[MAXSD];
    Double_t m_GMCPAmp[MAXSD];
};

#endif