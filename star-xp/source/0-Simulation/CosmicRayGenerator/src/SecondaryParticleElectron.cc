#include "G4SystemOfUnits.hh"
#include "Randomize.hh"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"
#include "TF1.h"
#pragma GCC diagnostic pop
#include "SecondaryParticleElectron.hh"

SecondaryParticleElectron::SecondaryParticleElectron()
{
    fPGType = "Secondary Particle - Electron";
    fClassName = "SecondaryParticleElectron";
    fParticle = "e-";
    fParticleEnergy = 0. * MeV;
    fParticlePosition = G4ThreeVector(0., 0., 0.);
    fParticlePolarization = G4ThreeVector(0., 0., 0.);
    fParticleMomentumDirection = G4ThreeVector(0., 0., 0.);
    fTimeFlag = 0.;
    fTypeFlag = 6001;
    fTimeVector.clear();
    fEvtFlag = 0;

    bottom = 30; //bottom && top energy of the spectrum
    top = 20000;
}

SecondaryParticleElectron::~SecondaryParticleElectron(){}

double SCP_spectrum_function(G4double *Eenergy, G4double *Ppart)
{
    G4double part =Ppart[0];
    G4double energy=Eenergy[0];
    G4double F0 = 0.136;
    G4double F1 = 0.1233;
    G4double a = 0.155;
    G4double b = 0.1;
    G4double Ec = 510;
    G4double Ebk = 600000;
    if (part >= 0. && part<0.3)
    {
        F0 = 0.3;
        a = 2.2;
        Ebk = 3000;
        b = 4.0;
        if (energy < 100)
        {
            return 4.33*F0 * pow(energy / 100, -1);
        }
        else if (energy >= 100 && energy <= Ebk)
        {
            return 4.33*F0 * pow(energy / 100, -a);
        }
        else
        {
            return 4.33*F0 * pow(Ebk / 100, -a) * pow(energy / Ebk, -b);
        }
    }
    else if (part >= 0.3 && part<0.6)
    {
        F0 = 0.3;
        a = 2.7;
        if (energy < 100)
        {
            return 1.66*F0 * pow(energy / 100, -1);
        }
        else
        {
            return 1.66*F0 * pow(energy / 100, -a);
        }
    }
    else if (part >= 0.6 && part<0.8)
    {
        F0 = 0.3;
        a = 3.3;
        F1 = 0.0002;
        b = 1.5;
        Ec = 2300;
        Ebk = 300000;
        if (energy < 100)
        {
            return 2*F0 * pow(energy / 100, -1);
        }
        else
        {
            return 2*(F0 * pow(Ebk / 100, -a) + F1 * pow(energy / 1000, -b) * exp(-pow(energy / Ec, b + 1)));
        }
    }

    else if (part >= 0.8 && part<0.9)
    {
        F0 = 0.3;
        a = 3.5;
        F1 = 0.0016;
        b = 2.0;
        Ec = 1600;
        Ebk = 400000;
        if (energy < 100)
        {
           return 2*F0 * pow(energy / 100, -1);
        }
        else
        {
            return 2*(F0 * pow(Ebk / 100, -a) + F1 * pow(energy / 1000, -b) * exp(-pow(energy / Ec, b + 1)));
        }
    }
    else if (part >= 0.9)
    {
        F0 = 0.3;
        a = 2.5;
        if (energy < 100)
        {
            return 2*F0 * pow(energy / 100, -1);
        }
        else
        {
            return 2*F0 * pow(energy / 100, -a);
        }
    }
    else
    {
        printf("no mach area!\n");
        exit(1);
    }
}


void SecondaryParticleElectron::GenTimeVector()
{
    fTimeVector.clear();
    fEvtFlag = 0;
    TF1 *SCPeFunc=new TF1("SCPeFunc",SCP_spectrum_function,10,10000000,1);
    SCPeFunc->SetParameters(latitude,0);

    G4double uni_mean=SCPeFunc->Integral(bottom,top);//单位角度单位面积上的计数率
    SampleSquare square =SampleSquare();
    //事例率
    G4double mean = 4*3.14*square.GetLength()*square.GetLength()*uni_mean/10000; //mean 个/s
    G4double total_number = fGenTime*mean;
    for(G4int i=0;i<total_number;i++){
        fTimeVector.push_back(G4UniformRand()*fGenTime);
    }
    std::sort(fTimeVector.begin(), fTimeVector.end(), CosmicRay::mySort);

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

void SecondaryParticleElectron::Initialize(G4double GenTime, std::vector<std::string> PGParameters)
{
    if (PGParameters.size() > 0)
    {
        if(PGParameters.size() != 3)
        {
            G4cout << "Warning!!! Length of Input PGParameters and number of Initialize parameters of SecondaryParticleElectron not matched!!!" << G4endl;
            G4cout << "Use default Initialize parameters" << G4endl;
        }
        else
        {
            try
            {
                bottom = ConvertStrToDouble(PGParameters[0]);
                top = ConvertStrToDouble(PGParameters[1]);
                latitude = ConvertStrToDouble(PGParameters[2]);
            }
            catch (const char *msg)
            {
                G4cerr << msg << G4endl;
                G4cerr << fClassName << ": Please check your PGParameter config" << G4endl;
                exit(EXIT_FAILURE);
            }
        }
    }
    //Generate Time list
    fGenTime = GenTime;
    if(fGenTime > 0)
        this->GenTimeVector();
}

void SecondaryParticleElectron::Generate()
{
    G4int success = 0;
    G4int charm = 0;
    G4double energy = 0.;
    while (success != 1)
    {
        G4double y_rand = G4UniformRand();
        G4double x_rand = G4UniformRand() * (top - bottom) + bottom;
        G4double max_count;
        G4double out_y;
        G4double *Latitude;
        G4double *Bottom;
        Latitude=&latitude;
        Bottom=&bottom;
        max_count = SCP_spectrum_function(Bottom, Latitude);
        G4double *X_rand;
        X_rand=&x_rand;
        out_y = SCP_spectrum_function(X_rand, Latitude) / max_count;
        if(y_rand<=out_y){
            energy = x_rand;
            if(latitude<=0.3){
                if (G4UniformRand() > 0.2309468822)
                {
                    fParticle="e+";
                }
                else
                {
                    fParticle="e-";
                }
                success += 1;
            }
            else if(latitude<=0.6){
                if (G4UniformRand() > 0.3759394)
                {
                    fParticle="e+";
                }
                else
                {
                    fParticle="e-";
                }
                success += 1;
            }
            else{
                if (G4UniformRand() > 0.5)
                {
                    fParticle="e+";
                }
                else
                {
                    fParticle="e-";
                }
                success += 1;
            }
        }
    }
    std::pair<G4ThreeVector, G4ThreeVector> PosAndMomDir = this->GenPosAndMomDir();

    fParticleEnergy = energy * MeV;
    fParticlePosition = PosAndMomDir.first;
    fParticleMomentumDirection = PosAndMomDir.second;
    if(fGenTime > 0)
        fTimeFlag = fTimeVector[fEvtFlag];
    ++fEvtFlag;
}