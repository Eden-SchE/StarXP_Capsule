//*********************************************
//  This is Geant4 Template
//                                  author:Qian
//

#include "MyRootBasedAnalysis.hh"
#include "SimEvent.h"
#include "Verbose.hh"

#include "G4SystemOfUnits.hh"
#include "G4Box.hh"
#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4PhysicalVolumeStore.hh"

#include "MyDetectorConstruction.hh"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"
#include "TObject.h"
#include "TFile.h"
#include "TTree.h"
#include "TSystem.h"
#include "TNtuple.h"
#include "TH1.h"
#include "TH2.h"
#include "TVector3.h"

#pragma GCC diagnostic pop

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

MyRootBasedAnalysis::MyRootBasedAnalysis()
{
    SetFileName("output.root");
    fIfValidEventGen = false;
    fIfValidEvent = false;
    fTotalEventNumber = 0;
    fValidEventNumber = 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

MyRootBasedAnalysis::~MyRootBasedAnalysis()
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void MyRootBasedAnalysis::BeginOfRunAction()
{
    if (!active)
        return;

    if (verbose)
        G4cout << "====>MyRootBasedAnalysis::BeginOfRunAction()" << G4endl;

    fRootFp = new TFile(fFileName, "recreate");
    if (!fRootFp)
    {
        G4cout << "\n====>MyRootBasedAnalysis::BeginOfRunAction(): "
               << "cannot open " << fFileName << G4endl;
        return;
    }

    fCosmicRayList = CosmicRayList::GetInstance();
    if((fCosmicRayList->GetPGPSConfig()).GenValidEvents > 0 || fCosmicRayList->GetPGPSConfig().OnlyValid)
        fIfValidEventGen = true;
    fCosmicRay = fCosmicRayList->GetGenerator();

    fEvent = new SimEvent();
    fConfigTree = new TTree("MCConfig", "Tree of ParticleGun Config");
    fConfigTree->Branch("GitVersion", &fGitVersion);
    fConfigTree->Branch("PGType", &fPGType);
    fConfigTree->Branch("GenTime", &fGenTime, "GenTime/D");
    fConfigTree->Branch("GenEvents", &fGenEvents, "GenEvents/I");
    fConfigTree->Branch("GenValidEvents", &fGenValidEvents, "GenValidEvents/I");
    fConfigTree->Branch("TotalEventNumber", &fTotalEventNumber, "TotalEventNumber/I");
    fConfigTree->Branch("PGParameters", &fPGParameters);
    fConfigTree->Branch("SDSize", &fSDSize, "SDSize[3]/D");
    fConfigTree->Branch("NumOfSD", &fNumOfSD, "NumOfSD/i");
    fConfigTree->Branch("SDPosition", &fSDPosition, "SDPosition[NumOfSD][3]/D");

    fTree = new TTree("Sim", "Tree of data events");
    TBranch* br_SimEvent = fTree->Branch("SimEvent", "SimEvent", fEvent, 320000, 0);
    br_SimEvent->SetAutoDelete(true);

    //------- add your codes down here
    //
    return;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void MyRootBasedAnalysis::EndOfRunAction()
{
    if (!active)
        return;

    if (verbose)
        G4cout << "====>MyRootBasedAnalysis::EndOfRunAction()" << G4endl;

    if (!fRootFp)
    {
        G4cout << "\n====>MyRootBasedAnalysis::EndOfRunAction(): "
               << "cannot open " << fFileName << G4endl;
        return;
    }

    //------- add your codes down here
    //

    PGPSConfig ConfigPS = fCosmicRayList->GetPGPSConfig();
    fGitVersion = TString(VERSION_GIT_HEAD_VERSION);
    fPGType = ConfigPS.ParticleGunType;
    fGenTime = ConfigPS.GenTime;
    fGenEvents = ConfigPS.GenEvents;
    fGenValidEvents = ConfigPS.GenValidEvents;
    std::string PGParameters;
    for (auto Str:ConfigPS.ParticleGunParameters)
    {
        PGParameters += Str;
        PGParameters += std::string(",");
    }
    fPGParameters = TString(PGParameters);
    this->GetSDInfo();
    fConfigTree->Fill();
    fRootFp->cd();
    fConfigTree->AutoSave();

    G4cout << "\n====>In total " << fTree->GetEntries() << " Events have been stored." << G4endl;
    fRootFp->cd();
    fTree->AutoSave();
    //fRootFp->Write();
    fRootFp->Close();
    return;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void MyRootBasedAnalysis::BeginOfEventAction(const G4Event *)
{
    if (!active)
        return;

    if (verbose > 1)
        G4cout << "====>MyRootBasedAnalysis::BeginOfEventAction()" << G4endl;

    fIfValidEvent = false;
    fEvent->MyClear();

    //------- add your codes down here
    //
    return;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void MyRootBasedAnalysis::EndOfEventAction(const G4Event * aEvent)
{
    if (!active)
        return;

    if (verbose > 1)
        G4cout << "====>MyRootBasedAnalysis::EndOfEventAction()" << G4endl;

    //------- add your codes down here
    //
    ++fTotalEventNumber;
    if(fIfValidEvent) ++fValidEventNumber;
    if (fTotalEventNumber % 1000 == 0)
        G4cout << "\n---> Generate " << fTotalEventNumber <<  " events, " << fValidEventNumber << " events valid." << G4endl;
    fCosmicRayList->SetValidEvent(fIfValidEvent);
    if(fIfValidEventGen && !(fIfValidEvent))
    {
        fEvent->MyClear();
        return;
    }

    fEvent->SetTruthParticle(fCosmicRay->GetParticle().data());
    fEvent->SetTruthEnergy(fCosmicRay->GetParticleEnergy() / MeV);
    G4ThreeVector TruthPos = fCosmicRay->GetParticlePosition();
    G4ThreeVector TruthPol = fCosmicRay->GetParticlePolarization();
    G4ThreeVector TruthMomDir = fCosmicRay->GetParticleMomentumDirection();
    fEvent->SetTruthPosition(TVector3(TruthPos[0] / mm, TruthPos[1] / mm, TruthPos[2] / mm));
    fEvent->SetTruthPolarization(TVector3(TruthPol[0], TruthPol[1], TruthPol[2]));
    fEvent->SetTruthMomentumDirection(TVector3(TruthMomDir[0], TruthMomDir[1], TruthMomDir[2]));
    fEvent->SetTimeFlag(fCosmicRay->GetTimeFlag());

    fTree->Fill();
    fEvent->MyClear();

    return;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
G4ClassificationOfNewTrack MyRootBasedAnalysis::ClassifyNewTrack(const G4Track *)
{
    if (!active)
        return fUrgent;

    //------- add your codes down here
    //
    return fUrgent;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void MyRootBasedAnalysis::PreTrackingAction(const G4Track *aTrack)
{
    if (!active)
        return;

    //------- add your codes down here
    //
    G4ParticleDefinition *particle = aTrack->GetDefinition();
    G4int trkID = aTrack->GetTrackID();
    SimTrack* aSimTrack = fEvent->GetTrack(trkID);
    aSimTrack->SetPDGID(particle->GetPDGEncoding());
    aSimTrack->SetTrackID(trkID);
    aSimTrack->SetParentID(aTrack->GetParentID());
    aSimTrack->SetInitMass(particle->GetAtomicMass() / MeV);
    aSimTrack->SetInitEk(aTrack->GetKineticEnergy() / MeV);
    aSimTrack->SetCreatorProcess(aTrack->GetCreatorModelName());

    G4ThreeVector InitMom = aTrack->GetMomentum();
    G4ThreeVector InitPos = aTrack->GetPosition();
    aSimTrack->SetInitMom(TVector3(InitMom[0] / MeV, InitMom[1] / MeV, InitMom[2] / MeV));
    aSimTrack->SetInitPos(TVector3(InitPos[0] / mm, InitPos[1] / mm, InitPos[2] / mm));
    aSimTrack->SetInitT(aTrack->GetGlobalTime() / s);

    //delete aSimTrack;
    return;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void MyRootBasedAnalysis::PostTrackingAction(const G4Track *aTrack)
{
    if (!active)
        return;

    //------- add your codes down here
    //
    SimTrack* aSimTrack = fEvent->GetTrack(aTrack->GetTrackID());
    
    G4ThreeVector ExitMom = aTrack->GetMomentum();
    G4ThreeVector ExitPos = aTrack->GetPosition();
    aSimTrack->SetExitMom(TVector3(ExitMom[0] / MeV, ExitMom[1] / MeV, ExitMom[2] / MeV));
    aSimTrack->SetExitPos(TVector3(ExitPos[0] / mm, ExitPos[1] / mm, ExitPos[2] / mm));
    aSimTrack->SetExitT(aTrack->GetGlobalTime() / s);

    aSimTrack->SetTrackLength(aTrack->GetTrackLength() / mm);
    aSimTrack->SetEdep((aSimTrack->GetInitEk() - aTrack->GetKineticEnergy()) / MeV);

    //delete aSimTrack;
    return;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void MyRootBasedAnalysis::SteppingAction(const G4Step * aStep)
{
    if (!active)
        return;

    //------- add your codes down here
    //
    const G4Track *aTrack = aStep->GetTrack();
    const G4ParticleDefinition *particle = aTrack->GetParticleDefinition();
    G4int particleID = particle->GetPDGEncoding();

    G4StepPoint *preStepPoint = aStep->GetPreStepPoint();
    G4StepPoint *postStepPoint = aStep->GetPostStepPoint();

    G4ThreeVector prePos = preStepPoint->GetPosition();
    G4ThreeVector preMomtum = preStepPoint->GetMomentum();

    G4ThreeVector postPos = postStepPoint->GetPosition();
    G4ThreeVector postMomtum = postStepPoint->GetMomentum();
    G4String postVolumeName = preStepPoint->GetPhysicalVolume()->GetName();

    G4String proName = postStepPoint->GetProcessDefinedStep()->GetProcessName();
    G4ProcessType proType = postStepPoint->GetProcessDefinedStep()->GetProcessType();
    G4int proSubType = postStepPoint->GetProcessDefinedStep()->GetProcessSubType();

    G4double StepEdep = aStep->GetTotalEnergyDeposit();

    if (particleID != 22 && postVolumeName == G4String("GasSDPhys") && StepEdep > 0.) fIfValidEvent = true; //标记有效事例
    //if (proName == "pol-phot") fIfValidEvent = true; //标记光电效应

    //Save a Deposit
    SimDeposit* aSimDeposit = new SimDeposit();

    aSimDeposit->SetPDGID(particle->GetPDGEncoding());
    aSimDeposit->SetTrackID(aTrack->GetTrackID());
    aSimDeposit->SetParentID(aTrack->GetParentID());
    aSimDeposit->SetCharge(particle->GetPDGCharge());

    aSimDeposit->SetPreMomentum(TVector3(preMomtum[0] / MeV, preMomtum[1] / MeV, preMomtum[2] / MeV));
    aSimDeposit->SetPrePosition(TVector3(prePos[0] / mm, prePos[1] / mm, prePos[2] / mm));
    aSimDeposit->SetPreT(preStepPoint->GetGlobalTime() / s);
    aSimDeposit->SetVolumeName(postVolumeName.data());


    aSimDeposit->SetPostMomentum(TVector3(postMomtum[0] / MeV, postMomtum[1] / MeV, postMomtum[2] / MeV));
    aSimDeposit->SetPostPosition(TVector3(postPos[0] / mm, postPos[1] / mm, postPos[2] / mm));
    aSimDeposit->SetPostT(postStepPoint->GetGlobalTime() / s);
    aSimDeposit->SetProcessName(TString(proName.data()));

    aSimDeposit->SetEdep(StepEdep / MeV);
    aSimDeposit->SetStepLength(aStep->GetStepLength() / mm);
    aSimDeposit->SetFirstDeposit(aStep->IsFirstStepInVolume());

    fEvent->AddDeposit(aSimDeposit->GetTrackID(), aSimDeposit);

    //delete aSimDeposit;

    return;
}

void MyRootBasedAnalysis::GetSDInfo(G4String NameOfSD)
{
    G4LogicalVolumeStore* LogVolStore = G4LogicalVolumeStore::GetInstance();
    G4LogicalVolume* SDLogVol = LogVolStore->GetVolume(NameOfSD);

    //Get SD Size
    G4Box* SDSolVol = dynamic_cast<G4Box*>(SDLogVol->GetSolid());
    fSDSize[0] = SDSolVol->GetXHalfLength() * 2. / mm;
    fSDSize[1] = SDSolVol->GetYHalfLength() * 2. / mm;
    fSDSize[2] = SDSolVol->GetZHalfLength() * 2. / mm;
    std::cout << "SD size: X: " << fSDSize[0] << "\tY: " << fSDSize[1] << "\tZ: " << fSDSize[2] << std::endl;

    //fNumOfSD = 2;
    //fSDPosition[0][0] = 0.;     fSDPosition[0][1] = 0.;     fSDPosition[0][2] = 0.2;
    //fSDPosition[1][0] = 44.;    fSDPosition[1][1] = 0.;     fSDPosition[1][2] = 0.2;

    //Get SD Size
    //G4VSolid* fSDVolPtr = SDLogVol->GetSolid();
    //G4ThreeVector volLoLimit, volHiLimit;
    //fSDVolPtr->BoundingLimits(volLoLimit, volHiLimit);

    //std::cout << "Catch CsI logic volume:" << std::endl;
    //std::cout << "Lo Limit: X: " << volLoLimit[0] << "\tY: " << volLoLimit[1] << "\tZ: " << volLoLimit[2] << std::endl;
    //std::cout << "Hi Limit: X: " << volHiLimit[0] << "\tY: " << volHiLimit[1] << "\tZ: " << volHiLimit[2] << std::endl;
    //fSDSize[0] = volHiLimit[0] - volLoLimit[0];
    //fSDSize[1] = volHiLimit[1] - volLoLimit[1];
    //fSDSize[2] = volHiLimit[2] - volLoLimit[2];

    fNumOfSD = 0;
    G4PhysicalVolumeStore* PhyVolStore = G4PhysicalVolumeStore::GetInstance();
    G4ThreeVector GasSDPos;
    for (auto iPhyVol = PhyVolStore->begin(); iPhyVol != PhyVolStore->end(); ++iPhyVol)
    {
        G4String Name = (*iPhyVol)->GetName();
        if(Name.find(G4String("GasSDPhys")) != G4String::npos)
        {
            GasSDPos = (*iPhyVol)->GetTranslation() / mm;
            if(verbose) std::cout << "GasSDPos: X: " << fSDPosition[fNumOfSD][0] << "\tY: " << fSDPosition[fNumOfSD][1] << "\tZ: " << fSDPosition[fNumOfSD][2] << std::endl;
        }
    }
    for (auto iPhyVol = PhyVolStore->begin(); iPhyVol != PhyVolStore->end(); ++iPhyVol)
    {
        G4String Name = (*iPhyVol)->GetName();
        if(Name.find(G4String("DetectorPhys")) != G4String::npos)
        {
            G4ThreeVector Pos = (*iPhyVol)->GetTranslation() / mm;
            fSDPosition[fNumOfSD][0] = Pos[0] + GasSDPos[0];
            fSDPosition[fNumOfSD][1] = Pos[1] + GasSDPos[1];
            fSDPosition[fNumOfSD][2] = Pos[2] + GasSDPos[2];
            if(verbose) std::cout << "Catch SD, Pos: X: " << fSDPosition[fNumOfSD][0] << "\tY: " << fSDPosition[fNumOfSD][1] << "\tZ: " << fSDPosition[fNumOfSD][2] << std::endl;
            ++fNumOfSD;
        }
    }
    if(verbose) std::cout << "Catch totally " << fNumOfSD << " SD." << std::endl;


    //fNumOfSD = UInt_t(SDLogVol->GetNoDaughters());
    //for (UInt_t iP = 0; iP < fNumOfSD; ++iP)
    //{
    //    G4VPhysicalVolume* SDPhyVol = SDLogVol->GetDaughter(iP);
    //    G4ThreeVector VSDPosition = SDPhyVol->GetTranslation();
    //    for(int i = 0; i < 3; ++i)
    //        fSDPosition[iP][i] = VSDPosition[i];
    //}

    //G4cout << G4endl;
    //G4cout << "List of LogicalVolume:" << G4endl;
    //for(G4LogicalVolumeStore::iterator iter = LogVolStore->begin(); iter != LogVolStore->end(); ++iter)
    //    G4cout << (*iter)->GetName() << G4endl;

    //G4cout << G4endl;
    //G4cout << "List of PhysicalVolume:" << G4endl;
    //G4PhysicalVolumeStore* PhyVolStore = G4PhysicalVolumeStore::GetInstance();
    //for(G4PhysicalVolumeStore::iterator iter = PhyVolStore->begin(); iter != PhyVolStore->end(); ++iter)
    //    G4cout << (*iter)->GetName() << G4endl;
}