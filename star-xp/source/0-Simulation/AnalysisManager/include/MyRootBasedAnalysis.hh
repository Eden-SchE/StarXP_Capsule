//*********************************************
//  This is Geant4 Template
//                                  author:Qian
//

#ifndef MyRootBasedAnalysis_h
#define MyRootBasedAnalysis_h 1

#include "G4RunManager.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4Event.hh"
#include "CosmicRayList.hh"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"
#include "TROOT.h"
#include "TString.h"
#pragma GCC diagnostic pop

#include <iomanip>

class SimEvent;
class TFile;
class TTree;
class TNtuple;
class MyDetectorConstruction;
class MyEvtAction;
class MyRunAction;

class MyRootBasedAnalysis
{
public:
    MyRootBasedAnalysis();
    ~MyRootBasedAnalysis();

    void BeginOfEventAction(const G4Event *evt);
    void EndOfEventAction(const G4Event *evt);

    void BeginOfRunAction();
    void EndOfRunAction();

    G4ClassificationOfNewTrack ClassifyNewTrack(const G4Track *aTrack);
    void SteppingAction(const G4Step *aStep);
    void PreTrackingAction(const G4Track *aTrack);
    void PostTrackingAction(const G4Track *aTrack);

    inline void SetFileName(G4String fname) { fFileName = fname; }
    inline void Activated() { active = true; }
    inline void Deactivated() { active = false; }
    inline bool IsActivated() { return active; }

    inline void SetDetectorConstruction(MyDetectorConstruction *det) { fDetector = det; }
    inline void SetRunAction(MyRunAction *run) { fRunAction = run; }
    inline void SetEvtAction(MyEvtAction *evt) { fEvtAction = evt; }
    
    SimEvent *GetSimEvent() { return fEvent; }

    //读取所有GasSD的大小和位置
    void GetSDInfo(G4String NameOfSD = "GasSD");

private:
    bool active = false;

    CosmicRay* fCosmicRay;
    CosmicRayList* fCosmicRayList;
    bool fIfValidEventGen;
    bool fIfValidEvent;

    //ROOT file
    G4String fFileName;
    TFile *fRootFp;
    //Tree for MC Truth
    TTree *fConfigTree;
    TString fGitVersion;
    TString fPGType;
    Double_t fGenTime;
    Int_t fGenEvents;
    Int_t fGenValidEvents;
    Int_t fTotalEventNumber;
    Int_t fValidEventNumber;
    TString fPGParameters;
    Double_t fSDSize[3];
    UInt_t fNumOfSD;
    Double_t fSDPosition[100][3];

    //Tree for Event
    TTree *fTree;
    SimEvent *fEvent;

    MyDetectorConstruction *fDetector;
    MyRunAction *fRunAction;
    MyEvtAction *fEvtAction;
};

#endif