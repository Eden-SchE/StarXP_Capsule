//*********************************************
//  This is List of all the Cosmic Ray
//                                  author:Ruiting
//

#ifndef _CosmicRayList_H_
#define _CosmicRayList_H_

#include "G4String.hh"
#include "CosmicRay.hh"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"
#include "TVector3.h"
#pragma GCC diagnostic pop

#include <map>

class CosmicRay;
class GammaRayBursts;
class CosmicRayList;

struct PGPSConfig
{
    //Config for ParticleGun
    bool PGEnable;
    G4String ParticleGunType;
    G4double GenTime;
    G4int GenEvents;
    G4int GenValidEvents;
    G4long GenRandomSeed;
    bool OnlyValid;
    std::vector<std::string> ParticleGunParameters;
    G4String AdditionalMACCommand;
    //Simple Particle Gun
    G4String ParticleName;
    G4double ParticleEnergy;
    G4ThreeVector ParticlePosition;
    G4ThreeVector ParticlePolarization;
    G4double ParticlePolarizationFactor;
    G4String IsArea;
    G4String IsRandSphere;
    G4String IsStokes;
    G4ThreeVector RandomArea;
    G4ThreeVector ParticleMomentumDirection;
    G4ThreeVector StokesParameters;
    //Config for GPS
    bool PSEnable;
    G4String PSSignal;
};


class CosmicRayList
{
public:
    CosmicRayList();
    ~CosmicRayList();

    static CosmicRayList *GetInstance()
    {
        if (CosmicRayList::fInstance == NULL)
            CosmicRayList::fInstance = new CosmicRayList();
        return CosmicRayList::fInstance;
    }

    inline void SetPGPSConfig(PGPSConfig ConfigPS){fConfigPS = ConfigPS;}

    CosmicRay* GetGenerator();
    CosmicRay* GetGenerator(G4String);
    inline PGPSConfig GetPGPSConfig() const {return fConfigPS;}

    inline void SetValidEvent(G4bool Valid){fValid = Valid;}
    inline G4bool IfValidEvent(){return fValid;}

private:
    static CosmicRayList* fInstance;
    PGPSConfig fConfigPS;
    G4bool fValid;
    std::map<G4String, CosmicRay*> fMapGeneratorPtr;

};

#endif