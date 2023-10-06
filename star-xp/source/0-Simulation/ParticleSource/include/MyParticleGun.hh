//*********************************************
//  This is Geant4 Template
//                                  author:Qian
//

#ifndef _MyParticleGun_H_
#define _MyParticleGun_H_

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ThreeVector.hh"
#include "G4ParticleDefinition.hh"
#include "CosmicRay.hh"
#include "CosmicRayList.hh"

#include "globals.hh"
/*
#include "../packages/cry_v1.7/src/CRYSetup.h"
#include "../packages/cry_v1.7/src/CRYGenerator.h"
#include "../packages/cry_v1.7/src/CRYParticle.h"
#include "../packages/cry_v1.7/src/CRYUtils.h"
*/
#include "RNGWrapper.hh"
#include <vector>
#include <string>
#include <map>

//#include "g4root.hh"
class TFile;
class TTree;
class TH1F;
class G4Event;
class G4ParticleTable;
class G4ParticleGun;
class MyParticleGunMessenger;
class MyDetectorConstruction;

class MyParticleGun;
typedef void (MyParticleGun::*GenFunc)();

class MyParticleGun : public G4VUserPrimaryGeneratorAction
{
public:
    MyParticleGun(MyDetectorConstruction *);
    ~MyParticleGun();

    virtual void GeneratePrimaries(G4Event *anEvent);

private:
    G4ParticleTable* particleTable;
    G4ParticleGun *fParticleGun;
    MyParticleGunMessenger *fGunMessenger;


    //#PartGun 1. 定义用户变量
public:
    void OpenRootFile(G4String fname);
    inline void UseSimpleGun() { GunType = 0; }

    //cry package
    /*
    void InputCRY();
    void UpdateCRY(std::string* MessInput);
    void CRYFromFile(G4String newValue);
    */

private:
    G4int GunType;
    MyDetectorConstruction *fDetector;
    PGPSConfig fConfigPS;

    CosmicRay* fCosmicRay;
    CosmicRayList* fCosmicRayList;

    std::map<G4String, G4ParticleDefinition*> mapGenParticlePtr;

    //cry package
    /*
    std::vector<CRYParticle*> *vect; // vector of generated particles
    CRYGenerator* gen;
    G4int InputState;
    */
};

#endif
