//*********************************************
//  This is Geant4 Template
//                                  author:Qian
//

#ifndef MyG4CoinSysAnalysis_h
#define MyG4CoinSysAnalysis_h 1

#include "G4RunManager.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
#include "MyG4BasedAnalysis.hh"
#include "G4Event.hh"
#include <iomanip>

class MyDetectorConstruction;
class MyEvtAction;
class MyRunAction;

class MyG4CoinSysAnalysis: public MyG4BasedAnalysis
{
public:
    MyG4CoinSysAnalysis();
    ~MyG4CoinSysAnalysis();

    void BeginOfEventAction(const G4Event *evt);
    void EndOfEventAction(const G4Event *evt);

    void BeginOfRunAction();
    void EndOfRunAction();

    G4ClassificationOfNewTrack ClassifyNewTrack(const G4Track *aTrack);
    void SteppingAction(const G4Step *aStep);
    void PreTrackingAction(const G4Track *aTrack);
    void PostTrackingAction(const G4Track *aTrack);

    inline void SetFileName(G4String fname) { fFileName = fname; }
    inline void Activated() { active = true; G4cout << "----> Enable MyG4CoinSysAnalysis framwork." << G4endl;}
    inline void Deactivated() { active = false; }
    inline bool IsActivated() { return active; }

    inline void SetDetectorConstruction(MyDetectorConstruction *det) { fDetector = det; }
    inline void SetRunAction(MyRunAction *run) { fRunAction = run; }
    inline void SetEvtAction(MyEvtAction *evt) { fEvtAction = evt; }

private:
    bool active = false;
    G4String fFileName;
    MyDetectorConstruction *fDetector;
    MyRunAction *fRunAction;
    MyEvtAction *fEvtAction;

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