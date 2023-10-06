//*********************************************
//  This is Geant4 Template
//                                  author:Qian
//

#ifndef MyG4basedAnalysis_h
#define MyG4basedAnalysis_h 1

#include "G4RunManager.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4Event.hh"
#include "CosmicRayList.hh"
#include <iomanip>

class MyDetectorConstruction;
class MyEvtAction;
class MyRunAction;

class MyG4BasedAnalysis
{
public:
    MyG4BasedAnalysis();
    ~MyG4BasedAnalysis();

    virtual void BeginOfEventAction(const G4Event *evt);
    virtual void EndOfEventAction(const G4Event *evt);

    virtual void BeginOfRunAction();
    virtual void EndOfRunAction();

    G4ClassificationOfNewTrack ClassifyNewTrack(const G4Track *aTrack);
    virtual void SteppingAction(const G4Step *aStep);
    virtual void PreTrackingAction(const G4Track *aTrack);
    virtual void PostTrackingAction(const G4Track *aTrack);

    virtual inline void SetFileName(G4String fname) { fFileName = fname; }
    virtual inline void Activated() { active = true; }
    virtual inline void Deactivated() { active = false; }
    virtual inline bool IsActivated() { return active; }

    virtual inline void SetDetectorConstruction(MyDetectorConstruction *det) { fDetector = det; }
    virtual inline void SetRunAction(MyRunAction *run) { fRunAction = run; }
    virtual inline void SetEvtAction(MyEvtAction *evt) { fEvtAction = evt; }

private:
    bool active = false;
    G4String fFileName;
    MyDetectorConstruction *fDetector;
    MyRunAction *fRunAction;
    MyEvtAction *fEvtAction;
    CosmicRay* fCosmicRay;
    CosmicRayList* fCosmicRayList;

    const G4Event *fEvent;

    //#ANALYSIS 0. 定义用户变量
private:
    double fEng;
    double fTrkLen;
    std::vector<double> fEdeps;
    std::vector<double> fHitsX;
    std::vector<double> fHitsY;
    std::vector<double> fHitsZ;
};

#endif