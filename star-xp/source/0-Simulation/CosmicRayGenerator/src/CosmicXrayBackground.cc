#include "G4SystemOfUnits.hh"
#include "Randomize.hh"
#include "CosmicXrayBackground.hh"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"
#include "TF1.h"
#include "TFile.h"
#pragma GCC diagnostic pop

Double_t CXBFunc(Double_t *Energy, Double_t *par){
    double energy =Energy[0];
    if(energy<0.02){
        return 0.54*pow(energy,-1.4);
    }
    else if(energy<0.1){
        return 0.0117*pow(energy,-2.38);
    }
    else{
        return 0.014*pow(energy,-2.3);
    }
    
    // if(0.002<=energy && energy<0.06){
    //     return 7.877*pow(energy*1000,-0.29)*exp(-energy/0.04113)*1000;
    // }
    // else if (energy>0.06){
    //     return (0.0259*pow(energy/0.06,-5.5)+0.504*pow(energy/0.06,-1.58)+0.0288*pow(energy/0.06,-1.05))*1000;
    // }
    // else{
    //     return 0.;
    // }
}


CosmicXrayBackground::CosmicXrayBackground()
{
    fPGType = "Cosmic X-ray Background";
    fClassName = "CosmicXrayBackground";
    fParticle = "gamma";
    fParticleEnergy = 0. * MeV;
    fParticlePosition = G4ThreeVector(0., 0., 0.);
    fParticlePolarization = G4ThreeVector(0., 0., 0.);
    fParticleMomentumDirection = G4ThreeVector(0., 0., 0.);
    fTimeFlag = 0.;
    fTypeFlag = 3001;
    fTimeVector.clear();
    fEvtFlag = 0;

    bottom = 0.003; //bottom && top energy of the spectrum
    top = 0.2;
}

CosmicXrayBackground::~CosmicXrayBackground(){}

void CosmicXrayBackground::GenTimeVector()
{
    fTimeVector.clear();
    fEvtFlag = 0;
    TF1 *fCXB = new TF1("fCXB",CXBFunc,0.002,10000,2);
    fCXB->SetParameters(2,2);
    G4double uni_mean=fCXB->Integral(bottom,top);//单位角度单位面积上的计数率
    SampleSquare square =SampleSquare();
    //事例率
    G4double mean = 4*3.14*square.GetLength()*square.GetLength()*uni_mean; //mean 个/s
    //G4double mean = 1000; //mean 个/s
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

void CosmicXrayBackground::Initialize(G4double GenTime, std::vector<std::string> PGParameters)
{
    if (PGParameters.size() > 0)
    {
        if(PGParameters.size() != 2)
        {
            G4cout << fClassName << ": Warning!!! Length of Input PGParameters and number of Initialize parameters of CosmicXrayBackground not matched!!!" << G4endl;
            G4cout << fClassName << ": Use default Initialize parameters" << G4endl;
        }
        else
        {
            try
            {
                bottom = ConvertStrToDouble(PGParameters[0]);
                top = ConvertStrToDouble(PGParameters[1]);
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

void CosmicXrayBackground::Generate()
{   
    G4int success = 0;
    G4double energy = 0.;
    while (success != 1)
    {
    //     if(energy<0.02){
    //     return 0.54*pow(energy,-1.4);
    // }
    // else if(energy<0.1){
    //     return 0.0117*pow(energy,-2.38);
    // }
    // else{
    //     return 0.014*pow(energy,-2.3);
    // }

        // G4double max_count =0;
        // if (bottom>=0.002 && bottom<0.06){
        //     max_count = 7.877*pow(bottom*1000,-0.29)*exp(-bottom/0.04113)*1000;
        // }
        // else if(bottom>=0.06){
        //     max_count =(0.0259*pow(bottom/0.06,-5.5)+0.504*pow(bottom/0.06,-1.58)+0.0288*pow(bottom/0.06,-1.05))*1000;
        // }
        // G4double y_rand = G4UniformRand();
        // G4double x_rand = G4UniformRand() * (top - bottom) + bottom;
        // G4double out_y = 0;
        // if(x_rand>=0.002 && x_rand<0.06){
        //     out_y = 7.877*pow(x_rand*1000,-0.29)*exp(-x_rand/0.04113)*1000/max_count;
        // }
        // else if(x_rand>=0.06){
        //     out_y = (0.0259*pow(x_rand/0.06,-5.5)+0.504*pow(x_rand/0.06,-1.58)+0.0288*pow(x_rand/0.06,-1.05))*1000/max_count;
        // }

        G4double max_count = 0; //need optimize4
        if (bottom<0.02)
        {
            max_count = 0.54*pow(bottom,-1.4);
        }
        else if (bottom<0.1)
        {
            max_count = 0.0117*pow(bottom,-2.38);
        }
        else
        {   
            max_count = 0.014*pow(bottom,-2.3);
        }
        G4double y_rand = G4UniformRand();
        G4double x_rand = G4UniformRand() * (top - bottom) + bottom;
        G4double out_y = 0;
        if (x_rand<0.02)
        {
            out_y = 0.54*pow(x_rand,-1.4)/max_count;
        }
        else if(x_rand<0.1)
        {
            out_y =0.0117*pow(x_rand,-2.38)/max_count;
        }
        else{
            out_y=0.014*pow(x_rand,-2.3)/max_count;
        }
        if (y_rand <= out_y)
        {
            energy = x_rand;
            success += 1;
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
