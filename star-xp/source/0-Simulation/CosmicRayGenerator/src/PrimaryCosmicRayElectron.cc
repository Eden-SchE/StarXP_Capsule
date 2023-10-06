#include "G4SystemOfUnits.hh"
#include "Randomize.hh"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"
#include "TF1.h"
#pragma GCC diagnostic pop
#include "PrimaryCosmicRayElectron.hh"

G4double function_PrimaryCosmicRayElectron(G4double *Eenergy,G4double *par){
    G4double energy=Eenergy[0];
    G4double mass =0.511;
    G4double p_max = sqrt(pow(energy+par[2], 2) + 2 * (energy+par[2]) * mass)/1000;
    G4double RCUT = 14.9 * pow(1 + par[3] / par[4], -2.0) * pow(cos(par[5]), 4);
    G4double myR= sqrt(pow(energy, 2) + 2 * (energy) * mass)/1000;
    G4double field_modulate = 1 / (1 + pow(myR / RCUT, -par[7]));
    return  field_modulate * par[0] * pow(p_max, -par[1]) * ((pow(energy + mass, 2) - pow(mass, 2)) / ((pow(energy + mass + par[2], 2) - pow(mass, 2))));
}



PrimaryCosmicRayElectron::PrimaryCosmicRayElectron()
{
    fPGType = "Primary Cosmic Ray - Electron";
    fClassName = "PrimaryCosmicRayElectron";
    fParticle = "e-";
    fParticleEnergy = 0. * MeV;
    fParticlePosition = G4ThreeVector(0., 0., 0.);
    fParticlePolarization = G4ThreeVector(0., 0., 0.);
    fParticleMomentumDirection = G4ThreeVector(0., 0., 0.);
    fTimeFlag = 0.;
    fTypeFlag = 4001;
    fTimeVector.clear();
    fEvtFlag = 0;

    bottom = 3; //bottom && top energy of the spectrum
    top = 200;
    A = 0.65;
    a = 3.30;
    Z = 1;
    mass = 0.511;
    r = 6;
    solar_modulation = 250;
    Rearth = 6379;
    orbit_hight = 500;
    R = 4.5;
}

PrimaryCosmicRayElectron::~PrimaryCosmicRayElectron(){}

void PrimaryCosmicRayElectron::GenTimeVector()
{
    fTimeVector.clear();
    fEvtFlag = 0;

    TF1 *PCReFunc = new TF1("PCReFunc",function_PrimaryCosmicRayElectron,5000,10000000,8);
    //A,a,solar_modulation,orbit_hight,Rearth,latitude,R,r
    PCReFunc->SetParameters(A,a,solar_modulation,orbit_hight,Rearth,latitude,R,r);
    G4double uni_mean=PCReFunc->Integral(bottom,top);//单位角度单位面积上的计数率

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

void PrimaryCosmicRayElectron::Initialize(G4double GenTime, std::vector<std::string> PGParameters)
{
    if (PGParameters.size() > 0)
    {
        if(PGParameters.size() != 5)
        {
            G4cout << "Warning!!! Length of Input PGParameters and number of Initialize parameters of PrimaryCosmicRayElectron not matched!!!" << G4endl;
            G4cout << "Use default Initialize parameters" << G4endl;
        }
        else
        {
            try
            {
                bottom = ConvertStrToDouble(PGParameters[0]);
                top = ConvertStrToDouble(PGParameters[1]);
                solar_modulation = ConvertStrToDouble(PGParameters[2]);
                orbit_hight = ConvertStrToDouble(PGParameters[3]);
                latitude = ConvertStrToDouble(PGParameters[4]);
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

void PrimaryCosmicRayElectron::Generate()
{
    G4int success = 0;
    G4double energy = 0.;
    TF1 *myPCReFunc = new TF1("PCReFunc",function_PrimaryCosmicRayElectron,bottom,top,8);
    //A,a,solar_modulation,orbit_hight,Rearth,latitude,R,r
    myPCReFunc->SetParameters(A,a,solar_modulation,orbit_hight,Rearth,latitude,R,r);
    energy = myPCReFunc->GetRandom();
    // G4double latitude = 0.;
    // G4double p_max = sqrt(pow(bottom+solar_modulation, 2) + 2 * (bottom+solar_modulation) * mass)/1000; 
    // while (success != 1)
    // {   
    //     G4double max_count = 0; //need optimize4
    //     G4double y_rand = G4UniformRand();
    //     G4double x_rand = G4UniformRand() * (top - bottom) + bottom;
    //     G4double RCUT = 14.9 * pow(1 + orbit_hight / Rearth, -2.0) * pow(cos(latitude), 4);
    //     R= sqrt(pow(bottom, 2) + 2 * (bottom) * mass)/1000;
    //     G4double field_modulate = 1 / (1 + pow(R / RCUT, -r));
    //     max_count = field_modulate*A * pow(p_max , -a) * ((pow(bottom + mass, 2) - pow(mass, 2)) / ((pow(bottom + mass + solar_modulation, 2) - pow(mass, 2))));
    //     G4double out_y = 0;
    //     G4double p_rand = sqrt(pow(x_rand+solar_modulation, 2) + 2 * (x_rand+solar_modulation) * mass)/1000;
    //     R= sqrt(pow(x_rand, 2) + 2 * (x_rand) * mass)/1000;
    //     field_modulate = 1 / (1 + pow(R / RCUT, -r));
    //     out_y = (field_modulate * A * pow(p_rand, -a) * ((pow(x_rand + mass, 2) - pow(mass, 2)) / ((pow(x_rand + mass + solar_modulation, 2) - pow(mass, 2))))) / max_count;

    //     if (y_rand <= out_y)
    //     {  
    //         energy = x_rand;
    //         success += 1;
    //     }
    // }
    success=0;
    std::pair<G4ThreeVector, G4ThreeVector> PosAndMomDir = this->GenPosAndMomDir();

    fParticleEnergy = energy * MeV;
    fParticlePosition = PosAndMomDir.first;
    fParticleMomentumDirection = PosAndMomDir.second;
    if(fGenTime > 0)
        fTimeFlag = fTimeVector[fEvtFlag];
    ++fEvtFlag;
}