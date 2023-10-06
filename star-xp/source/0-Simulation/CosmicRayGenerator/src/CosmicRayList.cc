#include "GammaRayBursts.hh"
#include "AtmosphericAlbedoPhoton.hh"
#include "CosmicXrayBackground.hh"
#include "PrimaryCosmicRayElectron.hh"
#include "PrimaryCosmicRayProton.hh"
#include "SouthAtlanticAnomaly.hh"
#include "SecondaryParticleElectron.hh"
#include "SecondaryParticleProton.hh"
#include "GPSFromMAC.hh"
#include "SimpleParticleGun.hh"

#include "CosmicRayList.hh"

CosmicRayList *CosmicRayList::fInstance = NULL;

CosmicRayList::CosmicRayList():fValid(false)
{
    fMapGeneratorPtr[G4String("GRB")]  = new GammaRayBursts();
    fMapGeneratorPtr[G4String("AAP")]  = new AtmosphericAlbedoPhoton();
    fMapGeneratorPtr[G4String("CXB")]  = new CosmicXrayBackground();
    fMapGeneratorPtr[G4String("PCRE")] = new PrimaryCosmicRayElectron();
    fMapGeneratorPtr[G4String("PCRP")] = new PrimaryCosmicRayProton();
    fMapGeneratorPtr[G4String("SAA")]  = new SouthAtlanticAnomaly();
    fMapGeneratorPtr[G4String("SPE")]  = new SecondaryParticleElectron();
    fMapGeneratorPtr[G4String("SPP")]  = new SecondaryParticleProton();
    fMapGeneratorPtr[G4String("GPS")]  = new GPSFromMAC();
    fMapGeneratorPtr[G4String("Simple")]  = new SimpleParticleGun();
}

CosmicRayList::~CosmicRayList(){
    for(const auto& GenPtr:fMapGeneratorPtr)
        delete GenPtr.second;
}

CosmicRay* CosmicRayList::GetGenerator()
{
    if (fMapGeneratorPtr.find(fConfigPS.ParticleGunType) != fMapGeneratorPtr.end())
        return fMapGeneratorPtr[fConfigPS.ParticleGunType];
    else
    {
        G4cerr << "Error!!! ParticleGun Type \" " << fConfigPS.ParticleGunType << " \" NOT FOUND!!! Please check your PGType config." << G4endl;
        G4cerr << "Avaiable Type:" << G4endl;
        for(const auto& GenPtr:fMapGeneratorPtr)
            G4cerr << "  " << GenPtr.first << ": " << (GenPtr.second)->GetPGType() << G4endl;
        
        exit(EXIT_FAILURE);
    }
}

CosmicRay* CosmicRayList::GetGenerator(G4String PGType)
{
    if (fMapGeneratorPtr.find(PGType) != fMapGeneratorPtr.end())
        return fMapGeneratorPtr[PGType];
    else
    {
        G4cerr << "Error!!! ParticleGun Type \" " << PGType << " \" NOT FOUND!!! Please check your PGType config." << G4endl;
        G4cerr << "Avaiable Type:" << G4endl;
        for(const auto& GenPtr:fMapGeneratorPtr)
            G4cerr << "  " << GenPtr.first << ": " << (GenPtr.second)->GetPGType() << G4endl;
        
        exit(EXIT_FAILURE);
    }
}