//*********************************************
//  This is Geant4 Template
//                                  author:Qian
//

#ifndef MyAnalysisManager_h
#define MyAnalysisManager_h 1

#include "G4RunManager.hh"
#include "G4Event.hh"
#include "G4Track.hh"
#include "G4Step.hh"
#include <iomanip>

class MyG4BasedAnalysis;
class MyG4CoinSysAnalysis;
class MyRootBasedAnalysis;
class MyDetectorConstruction;
class MyEvtAction;
class MyRunAction;

struct OutputControl
{
    G4String DetectorName;
    G4String OutputFile;
    G4int OutputLevel;
};

class MyAnalysisManager
{
public:
    static MyAnalysisManager *GetInstance()
    {
        if (MyAnalysisManager::fInstance == NULL)
            MyAnalysisManager::fInstance = new MyAnalysisManager();
        return MyAnalysisManager::fInstance;
    }

    MyAnalysisManager();
    ~MyAnalysisManager();

    void BeginOfEventAction(const G4Event *evt);
    void EndOfEventAction(const G4Event *evt);

    void BeginOfRunAction();
    void EndOfRunAction();

    G4ClassificationOfNewTrack ClassifyNewTrack(const G4Track *aTrack);
    void SteppingAction(const G4Step *aStep);
    void PreTrackingAction(const G4Track *aTrack);
    void PostTrackingAction(const G4Track *aTrack);

    void SetG4BasedFileName(G4String fname);
    void G4BasedActivated();
    void G4BasedDeactivated();

    void SetOutputInfo(OutputControl ControlOutput);
    void SetRootBasedFileName(G4String fname);
    void RootBasedActivated();
    void RootBasedDeactivated();

    void SetDetectorConstruction(MyDetectorConstruction *det);
    void SetRunAction(MyRunAction *run);
    void SetEvtAction(MyEvtAction *evt);

private:
    OutputControl fControlOutput;
    MyG4BasedAnalysis *fMyG4BasedAnalysis;
    MyRootBasedAnalysis *fMyRootBasedAnalysis;

    MyDetectorConstruction *fDetector;
    MyRunAction *fRunAction;
    MyEvtAction *fEvtAction;
    static MyAnalysisManager *fInstance;
};

#endif
