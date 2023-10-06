//*********************************************
//  This is Geant4 Template
//                                  author:Qian
//

#ifndef SimTrack_h
#define SimTrack_h

#include "TObject.h"
#include "TVector3.h"
#include <vector>
#include <iostream>

#include "SimDeposit.h"

class SimTrack : public TObject
{

  public:
    SimTrack() { MyClear(); }
    SimTrack(const SimTrack& aTrack)
    {
        this->MyClear();

        this->pdg_id = aTrack.pdg_id;
        this->track_id = aTrack.track_id;
        this->parent_id = aTrack.parent_id;

        // == Init ==
        this->init_mass = aTrack.init_mass;
        this->init_Ek = aTrack.init_Ek;
        this->InitMom = aTrack.InitMom;
        this->InitPos = aTrack.InitPos;
        this->init_t = aTrack.init_t;
        this->creator_process = aTrack.creator_process;

        // == Exit ==
        this->ExitMom = aTrack.ExitMom;
        this->ExitPos = aTrack.ExitPos;
        this->exit_t = aTrack.exit_t;

        this->track_length = aTrack.track_length;

        this->tEdep = aTrack.tEdep;

        this->stepIdx = aTrack.stepIdx;
    }
    SimTrack &operator=(const SimTrack &aTrack)
    {
        this->MyClear();

        this->pdg_id = aTrack.pdg_id;
        this->track_id = aTrack.track_id;
        this->parent_id = aTrack.parent_id;

        // == Init ==
        this->init_mass = aTrack.init_mass;
        this->init_Ek = aTrack.init_Ek;
        this->InitMom = aTrack.InitMom;
        this->InitPos = aTrack.InitPos;
        this->init_t = aTrack.init_t;
        this->creator_process = aTrack.creator_process;

        // == Exit ==
        this->ExitMom = aTrack.ExitMom;
        this->ExitPos = aTrack.ExitPos;
        this->exit_t = aTrack.exit_t;

        this->track_length = aTrack.track_length;

        this->tEdep = aTrack.tEdep;

        this->stepIdx = aTrack.stepIdx;

        return *this;
    }
    virtual ~SimTrack() {}

    void MyClear();

    //______________
    // === Getter ==
    int GetPDGID() { return pdg_id; }
    int GetTrackID() { return track_id; }
    int GetParentID() { return parent_id; }
    float GetInitMass() { return init_mass; }
    float GetInitEk() { return init_Ek; }

    // == Init ==
    TVector3 GetInitMom() { return InitMom; }
    TVector3 GetInitPos() { return InitPos; }
    double GetInitT() { return init_t; }
    TString GetCreatorProcess() { return creator_process; }

    // == Exit ==
    TVector3 GetExitMom() { return ExitMom; }
    TVector3 GetExitPos() { return ExitPos; }
    double GetExitT() { return exit_t; }
    float GetTrackLength() { return track_length; }

    float GetEdep() { return tEdep; }

    const std::vector<Int_t> GetStepIdx() const { return stepIdx; }

    //_____________
    // == Setter ==
    void SetPDGID(int val) { pdg_id = val; }
    void SetTrackID(int val) { track_id = val; }
    void SetParentID(int val) { parent_id = val; }
    void SetInitMass(float val) { init_mass = val; }
    void SetInitEk(float val) { init_Ek = val; }

    // == Init ==
    void SetInitMom(TVector3 val) {InitMom = val;}
    void SetInitPos(TVector3 val) {InitPos = val;}
    void SetInitT(double val) { init_t = val; }
    void SetCreatorProcess(TString val) { creator_process = val; }

    // == Exit ==
    void SetExitMom(TVector3 val) {ExitMom = val;}
    void SetExitPos(TVector3 val) {ExitPos = val;}
    void SetExitT(double val) { exit_t = val; }
    void SetTrackLength(float val) { track_length = val; }

    void SetEdep(float val) { tEdep = val; }

    void SetStepIdx(const std::vector<Int_t> &val) { stepIdx = val; }

    // == other functions ==
    void addDeposit(int idx, SimDeposit *aDep)
    {
        stepIdx.push_back(idx);
        tEdep += aDep->GetEdep();
		//std::cout<<"thr energy is "<<tEdep<<std::endl;
        track_length += aDep->GetStepLength();
    }

  private:
    Int_t pdg_id;
    Int_t track_id;
    Int_t parent_id;

    // == Init ==
    Float_t init_mass;
    Float_t init_Ek;
    TVector3 InitMom;
    TVector3 InitPos;
    Double_t init_t;
    TString creator_process;

    // == Exit ==
    TVector3 ExitMom;
    TVector3 ExitPos;
    Double_t exit_t;

    Float_t track_length;

    Float_t tEdep;

    std::vector<Int_t> stepIdx;

    ClassDef(SimTrack, 1)
};

inline void SimTrack::MyClear()
{
    pdg_id = 0;
    track_id = -1;
    parent_id = 0;

    // == Init ==
    init_mass = 0;
    init_Ek = 0;
    InitMom = TVector3(0., 0., 0.);
    InitPos = TVector3(0., 0., 0.);
    init_t = 0;
    creator_process = "";

    // == Exit ==
    ExitMom = TVector3(0., 0., 0.);
    ExitPos = TVector3(0., 0., 0.);
    exit_t = 0;
    track_length = 0.;

    // == Visible or Deposit Energy Related ==
    tEdep = 0;

    stepIdx.clear();
    std::vector<Int_t>().swap(stepIdx);
}

//SimTrack::SimTrack(const SimTrack& aTrack)
//{
//    this->MyClear();
//
//    this->pdg_id = aTrack.pdg_id;
//    this->track_id = aTrack.track_id;
//    this->parent_id = aTrack.parent_id;
//
//    // == Init ==
//    this->init_mass = aTrack.init_mass;
//    this->init_Ek = aTrack.init_Ek;
//    this->InitMom = aTrack.InitMom;
//    this->InitPos = aTrack.InitPos;
//    this->init_t = aTrack.init_t;
//
//    // == Exit ==
//    this->ExitMom = aTrack.ExitMom;
//    this->ExitPos = aTrack.ExitPos;
//    this->exit_t = aTrack.exit_t;
//
//    this->track_length = aTrack.track_length;
//
//    this->tEdep = aTrack.tEdep;
//
//    this->stepIdx = aTrack.stepIdx;
//
//}

//SimTrack & SimTrack::operator=(const SimTrack& aTrack)
//{
//    this->MyClear();
//
//    this->pdg_id = aTrack.pdg_id;
//    this->track_id = aTrack.track_id;
//    this->parent_id = aTrack.parent_id;
//
//    // == Init ==
//    this->init_mass = aTrack.init_mass;
//    this->init_Ek = aTrack.init_Ek;
//    this->InitMom = aTrack.InitMom;
//    this->InitPos = aTrack.InitPos;
//    this->init_t = aTrack.init_t;
//
//    // == Exit ==
//    this->ExitMom = aTrack.ExitMom;
//    this->ExitPos = aTrack.ExitPos;
//    this->exit_t = aTrack.exit_t;
//
//    this->track_length = aTrack.track_length;
//
//    this->tEdep = aTrack.tEdep;
//
//    this->stepIdx = aTrack.stepIdx;
//
//    return *this;
//}

#endif
