#include "G4SystemOfUnits.hh"
#include "Randomize.hh"

#include "GPSFromMAC.hh"

GPSFromMAC::GPSFromMAC()
{
    fPGType = "GPS from MAC file";
    fClassName = "GPSFromMAC";
    fParticle = "gamma";
    fParticleEnergy = 0. * MeV;
    fParticlePosition = G4ThreeVector(0., 0., 0.);
    fParticlePolarization = G4ThreeVector(0., 0., 0.);
    fParticleMomentumDirection = G4ThreeVector(0., 0., 0.);
    fTimeFlag = 0.;
    fTypeFlag = 10001;
    fTimeVector.clear();
    fEvtFlag = 0;

}

GPSFromMAC::~GPSFromMAC(){}

void GPSFromMAC::GenTimeVector()
{
    fTimeVector.clear();
}

void GPSFromMAC::Initialize(G4double GenTime, std::vector<std::string> PGParameters)
{
    fGenTime = GenTime;
    if(fGenTime > 0)
        this->GenTimeVector();
}

void GPSFromMAC::Generate()
{
    fTimeFlag = 0;
    ++fEvtFlag;
}