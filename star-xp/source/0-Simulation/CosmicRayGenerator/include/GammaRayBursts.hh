
//*********************************************
//  This is Template of Cosmic Ray Generator
//                                  author:Ruiting
//

#ifndef _GammaRayBursts_H_
#define _GammaRayBursts_H_

#include "G4ThreeVector.hh"
#include "G4Types.hh"
#include "G4String.hh"
#include "CosmicRay.hh"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"
#include "TFile.h"
#pragma GCC diagnostic pop

#include <vector>
#include <string>

class GammaRayBursts: public CosmicRay
{
public:
    GammaRayBursts();
    ~GammaRayBursts();

    void Initialize(G4double, std::vector<std::string>);
    void GenTimeVector();
    void Generate();

    inline void SetParticle(G4String Particle){fParticle = Particle;}
    inline void SetParticleEnergy(G4double ParticleEnergy){fParticleEnergy = ParticleEnergy;}
    inline void SetParticlePosition(G4ThreeVector ParticlePosition){fParticlePosition = ParticlePosition;}
    inline void SetParticlePolarization(G4ThreeVector ParticlePolarization){fParticlePolarization = ParticlePolarization;}
    inline void SetParticleMomentumDirection(G4ThreeVector ParticleMomentumDirection){fParticleMomentumDirection = ParticleMomentumDirection;}
    inline void SetTimeFlag(G4double TimeFlag){fTimeFlag = TimeFlag;}
    inline void SetTypeFlag(G4int TypeFlag){fTypeFlag = TypeFlag;}

    inline G4String GetPGType(){return fPGType;}
    inline G4String GetParticle(){return fParticle;}
    inline G4double GetParticleEnergy(){return fParticleEnergy;}
    inline G4ThreeVector GetParticlePosition(){return fParticlePosition;}
    inline G4ThreeVector GetParticlePolarization(){return fParticlePolarization;}
    inline G4ThreeVector GetParticleMomentumDirection(){return fParticleMomentumDirection;}
    inline G4double GetTimeFlag(){return fTimeFlag;}
    inline G4int GetTypeFlag(){return fTypeFlag;}
    inline G4int GetGenerateNumber(){return fGenNumber;}

private:
    G4String fPGType;
    G4String fClassName;
    G4String fParticle;
    G4double fParticleEnergy; //Unit: MeV
    G4ThreeVector fParticlePosition;
    G4ThreeVector fParticlePolarization;
    G4ThreeVector fOrigiPaticlePol;
    G4ThreeVector fParticleMomentumDirection;
    G4ThreeVector fStokesParameters;
    G4double fTimeFlag;
    G4int fTypeFlag;
    G4int fGenNumber;
    G4double fGenTime;
    G4double fParticlePolarizationFactor;
    G4int fEvtFlag;
    std::vector<G4double> fTimeVector;

    G4String GRBname;
    G4double theta;
    G4double phi;

};

#endif