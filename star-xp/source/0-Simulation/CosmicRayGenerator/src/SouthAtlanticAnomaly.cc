#include "G4SystemOfUnits.hh"
#include "Randomize.hh"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"
#include "TF1.h"
#pragma GCC diagnostic pop
#include "SouthAtlanticAnomaly.hh"


double SAApFunc(double *Energy, double *par){
    double energy =Energy[0];
    if(energy<100){
        printf("There is no need to consider energy blow 100MeV for south Atlatic abnormal proton.\n");
        return 0;
    }
    return 26657.6 * pow(energy, -1.9419);
}

SouthAtlanticAnomaly::SouthAtlanticAnomaly()
{
    fPGType = "South Atlantic Anomaly";
    fClassName = "SouthAtlanticAnomaly";
    fParticle = "proton";
    fParticleEnergy = 0. * MeV;
    fParticlePosition = G4ThreeVector(0., 0., 0.);
    fParticlePolarization = G4ThreeVector(0., 0., 0.);
    fParticleMomentumDirection = G4ThreeVector(0., 0., 0.);
    fTimeFlag = 0.;
    fTypeFlag = 5001;
    fTimeVector.clear();
    fEvtFlag = 0;

    bottom = 100; //bottom && top energy of the spectrum
    top = 200;
}

SouthAtlanticAnomaly::~SouthAtlanticAnomaly(){}

void SouthAtlanticAnomaly::GenTimeVector()
{
    fTimeVector.clear();
    fEvtFlag = 0;
    TF1 *SAAprotonFunc = new TF1("SAAprotonFunc",SAApFunc,100,10000,2);
    SAAprotonFunc->SetParameters(2,2);

    G4double uni_mean = SAAprotonFunc->Integral(bottom,top);
    SampleSquare square =SampleSquare();
    //事例率
    G4double mean = square.GetLength()*square.GetLength()*uni_mean;; //mean 个/s
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

void SouthAtlanticAnomaly::Initialize(G4double GenTime, std::vector<std::string> PGParameters)
{
    if (PGParameters.size() > 0)
    {
        if(PGParameters.size() != 2)
        {
            G4cout << "Warning!!! Length of Input PGParameters and number of Initialize parameters of SouthAtlanticAnomaly not matched!!!" << G4endl;
            G4cout << "Use default Initialize parameters" << G4endl;
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

void SouthAtlanticAnomaly::Generate()
{
    G4int success = 0;
    G4double energy = 0.;
    double param[2];
    param[0]=1;
    param[1]=1;
    while (success != 1)
    {
        G4double max_count = 0; //need optimize4
        if (bottom >= 100)
        {
            max_count = SAApFunc(&bottom,param);
        }
        else
        {
            printf("there is no need to describe energy below 100 MeV for SAA_proton");
            exit(1);
        }
        G4double y_rand = G4UniformRand();
        G4double x_rand = G4UniformRand() * (top - bottom) + bottom;
        G4double out_y = 0;

        out_y = SAApFunc(&x_rand,param) / max_count;

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