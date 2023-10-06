#include "G4SystemOfUnits.hh"
#include "Randomize.hh"
#include "AtmosphericAlbedoPhoton.hh"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"
#include "TF3.h"
#pragma GCC diagnostic pop


double AAPFunc(double *x, double *par){
    double theta = x[0];
    double epsilon = x[1];
    double energy = x[2];
    double in_latitude =par[0];
    double solar_modulation = par[1];
    energy=(energy*1000);//tranverse the unit to keV
    double Rcut = 59.4*(pow(in_latitude,4))/pow(1+pow(1+pow(cos(in_latitude),3)*sin(theta)*sin(epsilon),0.5),2);
    double my_C =(3*cos(theta)*(1+cos(theta))/(5*M_PI))*(1.47 * 0.0178 *pow(pow(solar_modulation/2.8,0.4)+pow(solar_modulation/2.8,1.5),-1)/sqrt(1+pow(Rcut/(1.3*pow(solar_modulation,0.25)*(1+2.5*pow(solar_modulation,0.4))),2)));
    double count = my_C/(pow(energy/44,-5)+pow(energy/44,1.4));
    return count;

}

AtmosphericAlbedoPhoton::AtmosphericAlbedoPhoton()
{
    fPGType = "Atmospheric Albedo Photon";
    fClassName = "AtmosphericAlbedoPhoton";
    fParticle = "gamma";
    fParticleEnergy = 0. * MeV;
    fParticlePosition = G4ThreeVector(0., 0., 0.);
    fParticlePolarization = G4ThreeVector(0., 0., 0.);
    fParticleMomentumDirection = G4ThreeVector(0., 0., 0.);
    fTimeFlag = 0.;
    fTypeFlag = 2001;
    fTimeVector.clear();
    fEvtFlag = 0;

    bottom = 3; //bottom && top energy of the spectrum
    top = 200;
    solar_modulation = 0.25;
    latitude = 0.5;
}

AtmosphericAlbedoPhoton::~AtmosphericAlbedoPhoton(){}

void AtmosphericAlbedoPhoton::GenTimeVector()
{
    fTimeVector.clear();
    fEvtFlag = 0;

    TF3 *f1 = new TF3("f1",AAPFunc,0,M_PI,0,2*M_PI,0.001,1,2);
    f1->SetParameters(latitude,solar_modulation);
    SampleSquare suqre=SampleSquare();

    //事例率
    G4double mean = 1000*suqre.GetLength()*suqre.GetLength()*f1->Integral(0,M_PI,0,2*M_PI,bottom,top); //原计数率单位为keV-1,这里统一为MeV，因此需要乘以1000
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

void AtmosphericAlbedoPhoton::Initialize(G4double GenTime, std::vector<std::string> PGParameters)
{
    if (PGParameters.size() > 0)
    {
        if(PGParameters.size() != 4)
        {
            G4cout << "Warning!!! Length of Input PGParameters and number of Initialize parameters of AtmosphericAlbedoPhoton not matched!!!" << G4endl;
            G4cout << "Use default Initialize parameters" << G4endl;
        }
        else
        {
            try
            {
                bottom = ConvertStrToDouble(PGParameters[0]);
                top = ConvertStrToDouble(PGParameters[1]);
                solar_modulation = ConvertStrToDouble(PGParameters[2]);
                latitude = ConvertStrToDouble(PGParameters[3]);
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

void AtmosphericAlbedoPhoton::Generate()
{
    G4int success = 0;
    G4double energy = 0.;
    G4double theta = 0;
    //G4double theta = G4UniformRand() * M_PI;
    G4double phi = 0;
    while (success != 1)
    {
        // G4double max_count = 0; //need optimize4
        // if (bottom >= 1 && bottom < 20)
        // {
        //     max_count = pow(Rcut / 4.5, -1.13) * 1010 * pow(bottom, -1.34) / bottom;
        // }
        // else if (bottom >= 20 && bottom < 1000)
        // {
        //     max_count = pow(Rcut / 4.5, -1.13) * 7290 * pow(bottom, -2.0) / bottom;
        // }
        // else if (bottom >= 1000)
        // {
        //     max_count = pow(Rcut / 4.5, -1.13) * 29000 * pow(bottom, -2.2) / bottom;
        // }
        // else
        // {
        //     printf("there is no function describe energy below 1000 KeV for CXB");
        //     exit(1);
        // }
        G4double y_rand = G4UniformRand();
        G4double x_rand = G4UniformRand() * (top - bottom) + bottom;
        G4double theta_rand =G4UniformRand()*M_PI;
        G4double epsilon_rand =G4UniformRand()*2*M_PI;
        G4double max_xx[3];
        G4double parameter[2];
        G4double out_y = 0;
        max_xx[0]=0;
        max_xx[1]=0;
        if(bottom<=0.055 && top>=0.055){max_xx[2]=0.055;}
        else if(top<=0.055){max_xx[2]==top;}
        else if(bottom>=0.055){max_xx[2]==bottom;}
        parameter[0]=latitude;
        parameter[1]=solar_modulation;
        G4double rand_xx[3];
        rand_xx[0]=theta_rand;
        rand_xx[1]=epsilon_rand;
        rand_xx[2]=x_rand;
        G4double max_count =AAPFunc(max_xx,parameter);
        out_y =AAPFunc(rand_xx,parameter)/max_count;
        if (y_rand <= out_y)
        {
            energy = x_rand;
            theta=theta_rand;
            phi=epsilon_rand;
            success += 1;
        }
    }
    SampleSquare square =SampleSquare();
    G4double my_x = square.GetLength() * (G4UniformRand() - 0.5);
    G4double my_y = square.GetLength() * (G4UniformRand() - 0.5);
    //theta = theta+M_PI;
    std::pair<G4ThreeVector, G4ThreeVector> PosAndMomDir;
    PosAndMomDir.first = G4ThreeVector(-my_x * sin(phi) + my_y * cos(theta) * cos(phi) + 100 * sin(theta) * cos(phi),
                                      my_x * cos(phi) + my_y * cos(theta) * sin(phi) + 100 * sin(theta) * sin(phi),
                                      -my_y * sin(theta) + 100 * cos(theta));
    PosAndMomDir.second = G4ThreeVector(-sin(theta) * cos(phi), -sin(theta) * sin(phi), -cos(theta));

    fParticleEnergy = energy * MeV;
    fParticlePosition = PosAndMomDir.first * cm;
    fParticleMomentumDirection = PosAndMomDir.second;
    if(fGenTime > 0)
        fTimeFlag = fTimeVector[fEvtFlag];
    ++fEvtFlag;

}