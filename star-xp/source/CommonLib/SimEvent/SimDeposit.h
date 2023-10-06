//*********************************************
//  This is Geant4 Template
//                                  author:Qian
//

#ifndef SimDeposit_h
#define SimDeposit_h

#include "TObject.h"
#include "TVector3.h"
#include "TString.h"
#include <vector>
#include <iostream>

class SimDeposit : public TObject
{
  private:
    Int_t pdg_id;
    Int_t track_id;
    Int_t parent_id; // Parent ID
    Int_t charge;

    // == Init ==
    TVector3 PreMom;
    TVector3 PrePos;
    Double_t pre_t;
    TString pre_volume;

    // == Exit ==
    TVector3 PostMom;
    TVector3 PostPos;
    Double_t post_t;
    TString post_process;

    // == Visible or Deposit Energy Related ==
    Float_t edep;
    Float_t length; // step length
    bool isFirstDeposit;

  public:
    SimDeposit()
    {
      pdg_id = 0;
      track_id = -1;
      parent_id = -1;
      charge = -1;

      // == Init ==
      PreMom = TVector3(0., 0., 0.);
      PrePos = TVector3(0., 0., 0.);
      pre_t = 0;
      pre_volume = "";

      // == Exit ==
      PostMom = TVector3(0., 0., 0.);
      PostPos = TVector3(0., 0., 0.);
      post_t = 0;
      post_process = "";

      // == Visible or Deposit Energy Related ==
      edep = 0;
      length = 0;
      isFirstDeposit = false;
    }
    SimDeposit(const SimDeposit& aDeposit)
    {
      this->pdg_id = aDeposit.pdg_id;
      this->track_id = aDeposit.track_id;
      this->parent_id = aDeposit.parent_id; // Parent ID
      this->charge = aDeposit.charge;

      // == Init ==
      this->PreMom = aDeposit.PreMom;
      this->PrePos = aDeposit.PrePos;
      this->pre_t = aDeposit.pre_t;
      this->pre_volume = aDeposit.pre_volume;

      // == Exit ==
      this->PostMom = aDeposit.PostMom;
      this->PostPos = aDeposit.PostPos;
      this->post_t = aDeposit.post_t;
      this->post_process = aDeposit.post_process;

      // == Visible or Deposit Energy Related ==
      this->edep = aDeposit.edep;
      this->length = aDeposit.length;
      this->isFirstDeposit = aDeposit.isFirstDeposit;
    }
    SimDeposit &operator=(const SimDeposit &aDeposit)
    {
      this->pdg_id = aDeposit.pdg_id;
      this->track_id = aDeposit.track_id;
      this->parent_id = aDeposit.parent_id; // Parent ID
      this->charge = aDeposit.charge;

      // == Init ==
      this->PreMom = aDeposit.PreMom;
      this->PrePos = aDeposit.PrePos;
      this->pre_t = aDeposit.pre_t;
      this->pre_volume = aDeposit.pre_volume;

      // == Exit ==
      this->PostMom = aDeposit.PostMom;
      this->PostPos = aDeposit.PostPos;
      this->post_t = aDeposit.post_t;
      this->post_process = aDeposit.post_process;

      // == Visible or Deposit Energy Related ==
      this->edep = aDeposit.edep;
      this->length = aDeposit.length;
      this->isFirstDeposit = aDeposit.isFirstDeposit;
      return *this;
    }

    virtual ~SimDeposit() {}

    //_____________
    // == Getter ==
    int GetPDGID() const { return pdg_id; }
    int GetTrackID() const { return track_id; }
    int GetParentID() const { return parent_id; }
    int GetCharge() const { return charge; }

    // == Pre ==
    TVector3 GetPreMomentum() const { return PreMom; }
    TVector3 GetPrePosition() const { return PrePos; }
    double GetPreT() const { return pre_t; }
    TString GetVolumeName() const { return pre_volume; }
    
    // == Post ==
    TVector3 GetPostMomentum() const { return PostMom; }
    TVector3 GetPostPosition() const { return PostPos; }
    double GetPostT() const { return post_t; }
    TString GetProcessName() const { return post_process; }
    
    // == Visible or Deposit Energy Related ==
    float GetEdep() const { return edep; }
    float GetStepLength() const { return length; }
    bool IsFirstDeposit() const { return isFirstDeposit; }

    //_____________
    // == Setter ==
    void SetPDGID(int val) { pdg_id = val; }
    void SetTrackID(int val) { track_id = val; }
    void SetParentID(int val) { parent_id = val; }
    void SetCharge(int val) { charge = val; }

    // == Pre ==
    void SetPreMomentum(TVector3 val) {PreMom = val;}
    void SetPrePosition(TVector3 val) {PrePos = val;}
    void SetPreT(double val) { pre_t = val; }
    void SetVolumeName(TString val) { pre_volume = val; }
    
    // == Post ==
    void SetPostMomentum(TVector3 val) {PostMom = val;}
    void SetPostPosition(TVector3 val) {PostPos = val;}
    void SetPostT(double val) { post_t = val; }
    void SetProcessName(TString val) { post_process = val; }
    
    // == Visible or Deposit Energy Related ==
    void SetEdep(float val) { edep = val; }
    void SetStepLength(float val) { length = val; }
    void SetFirstDeposit(bool val) { isFirstDeposit = val; }

    ClassDef(SimDeposit, 1)
};

//SimDeposit::SimDeposit(const SimDeposit& aDeposit)
//{
//    this->pdg_id = aDeposit.pdg_id;
//    this->track_id = aDeposit.track_id;
//    this->parent_id = aDeposit.parent_id; // Parent ID
//    this->charge = aDeposit.charge;
//
//    // == Init ==
//    this->PreMom = aDeposit.PreMom;
//    this->PrePos = aDeposit.PrePos;
//    this->pre_t = aDeposit.pre_t;
//    this->pre_volume = aDeposit.pre_volume;
//
//    // == Exit ==
//    this->PostMom = aDeposit.PostMom;
//    this->PostPos = aDeposit.PostPos;
//    this->post_t = aDeposit.post_t;
//    this->post_process = aDeposit.post_process;
//
//    // == Visible or Deposit Energy Related ==
//    this->edep = aDeposit.edep;
//    this->length = aDeposit.length;
//    this->isFirstDeposit = aDeposit.isFirstDeposit;
//}

//SimDeposit & SimDeposit::operator=(const SimDeposit& aDeposit)
//{
//    this->pdg_id = aDeposit.pdg_id;
//    this->track_id = aDeposit.track_id;
//    this->parent_id = aDeposit.parent_id; // Parent ID
//    this->charge = aDeposit.charge;
//
//    // == Init ==
//    this->PreMom = aDeposit.PreMom;
//    this->PrePos = aDeposit.PrePos;
//    this->pre_t = aDeposit.pre_t;
//    this->pre_volume = aDeposit.pre_volume;
//
//    // == Exit ==
//    this->PostMom = aDeposit.PostMom;
//    this->PostPos = aDeposit.PostPos;
//    this->post_t = aDeposit.post_t;
//    this->post_process = aDeposit.post_process;
//
//    // == Visible or Deposit Energy Related ==
//    this->edep = aDeposit.edep;
//    this->length = aDeposit.length;
//    this->isFirstDeposit = aDeposit.isFirstDeposit;
//    return *this;
//}

#endif
