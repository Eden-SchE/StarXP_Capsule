#include "G4SystemOfUnits.hh"
#include "Randomize.hh"
#include "CosmicRayList.hh"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"
#include "TF1.h"
#pragma GCC diagnostic pop

#include "SimpleParticleGun.hh"
#include "StokesVector.hh"

// G4double PolarAngDistribution(G4double par){
//     G4double digree = par;
//     G4double Imin = (1-digree)/(1+digree);
//     return Imin;
// }

G4ThreeVector CacuPolar(G4ThreeVector MomDir){
    G4ThreeVector tmp = G4ThreeVector(0.567,0.678,0.654);
    G4ThreeVector ProjectMax = tmp.cross(MomDir).unit();//偏振最大的方向
    return ProjectMax;
}

SimpleParticleGun::SimpleParticleGun()
{
    fPGType = "Simple Particle Gun";
    fClassName = "SimpleParticleGun";
    fParticle = "gamma";
    fParticleEnergy = 0. * MeV;
    fParticlePosition = G4ThreeVector(0., 0., 0.);
    fParticlePolarization = G4ThreeVector(0., 0., 0.);
    fParticlePolarizationFactor = -1.;
    fParticleMomentumDirection = G4ThreeVector(0., 0., 0.);
    fTimeFlag = 0.;
    fTypeFlag = 10001;
    fTimeVector.clear();
    fEvtFlag = 0;
    fmyPol = NULL;
    fIsArea ="no";
    fIsRandSphere = "no";

}

SimpleParticleGun::~SimpleParticleGun(){
    if(fmyPol != NULL)
        delete fmyPol;
}

void SimpleParticleGun::GenTimeVector()
{
    fTimeVector.clear();
    G4double mean = 100.; //mean 个/s
    G4double total_number = fGenTime*mean;
    for(G4int i=0;i<total_number;i++){
        G4double atime = i*mean + G4UniformRand()*mean;
        fTimeVector.push_back(atime);
    }
    //std::sort(fTimeVector.begin(), fTimeVector.end(), CosmicRay::mySort);

    fGenNumber = fTimeVector.size();
    if(fGenNumber == 0)
    {
        G4cout << fClassName <<": Length of fTimeVector == 0" << G4endl;
        G4cout << "No event will be generate. Maybe input GenTime is too short?" << G4endl;
        G4cout << "GenTime: " << fGenTime << " s" << G4endl;
        exit(EXIT_SUCCESS);
    }
    G4cout << fClassName <<": " << fGenNumber << " events will be generated." << G4endl;
}

void SimpleParticleGun::Initialize(G4double GenTime, std::vector<std::string> PGParameters)
{
    CosmicRayList* CRList = CosmicRayList::GetInstance();
    PGPSConfig Config =  CRList->GetPGPSConfig();
    fParticle = Config.ParticleName;
    fParticleEnergy = Config.ParticleEnergy * MeV;
    fParticlePosition = Config.ParticlePosition * mm;
    fParticlePolarization = Config.ParticlePolarization;
    fParticlePolarizationFactor = Config.ParticlePolarizationFactor;
    fParticleMomentumDirection = Config.ParticleMomentumDirection;
    fIsArea = Config.IsArea;
    fIsStokes = Config.IsStokes;
    fStokesParameters = Config.StokesParameters;
    fRandArea = Config.RandomArea;
    fOrigiParticlePosition = fParticlePosition;
    fOrigiPaticlePol = fParticlePolarization;
    if(fParticle="gamma"){
        if(abs(fOrigiPaticlePol.dot(fParticleMomentumDirection))>0.){
            fOrigiPaticlePol = fParticleMomentumDirection.cross(fOrigiPaticlePol).unit();
        }
    }
    fIsRandSphere = Config.IsRandSphere;

    fGenTime = GenTime;
    if(fGenTime > 0)
        this->GenTimeVector();
}

void SimpleParticleGun::Generate()
{
    if(fIsArea=="yes" && fIsRandSphere=="no"){
        // G4double RandLength = 8;//目前测试设置的随机光子范围4mm
        G4double Randx = fRandArea.x()*(G4UniformRand()-0.5);
        G4double Randy = fRandArea.y()*(G4UniformRand()-0.5);
        G4double Randz = fRandArea.z()*(G4UniformRand()-0.5);
        G4ThreeVector Areatmp = G4ThreeVector(fOrigiParticlePosition.x()+Randx,fOrigiParticlePosition.y()+Randy,fOrigiParticlePosition.z()+Randz);
        fParticlePosition = Areatmp;
    }
    else if (fIsArea=="no" && fIsRandSphere=="yes"){
        std::pair<G4ThreeVector, G4ThreeVector> PosAndMomDir = this->GenPosAndMomDir();
        fParticlePosition = PosAndMomDir.first;
        fParticleMomentumDirection = PosAndMomDir.second;
    }
    if(fIsStokes=="yes"){//采用斯托克斯参量描述光子的状态
        TVector3 standarvector = TVector3(0,1,0);
        TVector3 ParticleMom = TVector3(fParticleMomentumDirection.x(),fParticleMomentumDirection.y(),fParticleMomentumDirection.z());
        TVector3 setXVec = ParticleMom.Cross(standarvector);
        double rotAngle = Selectangle(fStokesParameters);
        TVector3 aParticleMomentumDirection = TVector3(fParticleMomentumDirection.x(),fParticleMomentumDirection.y(),fParticleMomentumDirection.z());
        TVector3 aParticlePolarization = setXVec;
        aParticlePolarization.Rotate(rotAngle,aParticleMomentumDirection);
        fParticlePolarization = G4ThreeVector(aParticlePolarization.x(),aParticlePolarization.y(),aParticlePolarization.z());
        TVector3 tmp2 = aParticlePolarization.Cross(aParticleMomentumDirection);
        TVector3 tmp3 = tmp2.Cross(aParticleMomentumDirection);
        fParticlePolarization = G4ThreeVector(tmp3.x(),tmp3.y(),tmp3.z());
    }
    if(fParticlePolarizationFactor >= 0)
    {
        // double aImin = PolarAngDistribution(fParticlePolarizationFactor);
        double aPolarrize = 2*fParticlePolarizationFactor;
        double aUnpolarize = 1-fParticlePolarizationFactor;
        if(fParticlePolarizationFactor<0 || fParticlePolarizationFactor>1){
            std::cout<<"PolarizationFactor should be less than 1 and greater than 0! Please check your paramesters!"<<std::endl;
            exit(-1);
        }
        double ITotal = aPolarrize+aUnpolarize;
        if((G4UniformRand()*(ITotal))>aPolarrize){
            fParticlePolarization = G4ThreeVector(0,0,0);
        }
        else{
            if(fIsStokes=="no"){
                fParticlePolarization = fOrigiPaticlePol;
            }
        }
    }
    if(fGenTime > 0)
        fTimeFlag = fTimeVector[fEvtFlag];
    ++fEvtFlag;
}