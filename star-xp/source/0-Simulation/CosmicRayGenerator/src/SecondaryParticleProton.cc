#include "G4SystemOfUnits.hh"
#include "Randomize.hh"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"
#include "TF1.h"
#pragma GCC diagnostic pop
#include "SecondaryParticleProton.hh"


G4double spectrum_function(G4double *Eenergy, G4double *par)
{   
    G4double energy = Eenergy[0];
    G4double part = par[0];
    G4double direction = par[1];
    G4double F0 = 0.136;
    G4double F1 = 0.1233;
    G4double a = 0.155;
    G4double b = 0.1;
    G4double Ec = 510;
    G4double Ebk = 600000;
    if (part>= 0. && part<0.2 )
    {
        F0 = 0.136;
        F1 = 0.1233;
        a = 0.155;
        Ec = 510;
        if (energy < 100)
        {
            return F0 * pow(energy / 100, -1);
        }
        else
        {
             return F1 * pow(energy / 1000, -a) * exp(-pow(energy / Ec, -a + 1));
        }
    }
    else if (part>= 0.2 && part<0.3)
    {
        F0 = 0.1;
        a = 0.87;
        Ebk = 600000;
        b = 2.53;
        if (energy < 100)
        {
            return F0 * pow(energy / 100, -1);
         }
        else if (energy >= 100 && energy < Ebk)
        {
            return F0 * pow(energy / 100, -a);
        }
        else if (energy >= Ebk)
        {
            return F0 * pow(Ebk / 100, -a) * pow(energy / Ebk, -b);
        }
        else
        {
            printf("something wrong!\n");
            exit(1);
         }
    }
    else if (part>= 0.3 && part<0.4)
    {
        F0 = 0.1;
        a = 1.09;
        Ebk = 600000;
        b = 2.40;
        if (energy < 100)
        {
            return F0 * pow(energy / 100, -1);
        }
        else if (energy >= 100 && energy < Ebk)
        {
            return F0 * pow(energy / 100, -a);
        }
        else if (energy >= Ebk)
        {
            return F0 * pow(Ebk / 100, -a) * pow(energy / Ebk, -b);
        }
        else
        {
            printf("something wrong!\n");
            exit(1);
        }
    }
    else if (part>= 0.4 && part<0.5)
    {
        F0 = 0.1;
        a = 1.19;
        Ebk = 600000;
        b = 2.54;
        if (energy < 100)
        {
            return F0 * pow(energy / 100, -1);
        }
        else if (energy >= 100 && energy < Ebk)
        {
            return F0 * pow(energy / 100, -a);
        }
        else if (energy >= Ebk)
        {
            return F0 * pow(Ebk / 100, -a) * pow(energy / Ebk, -b);
        }
        else
        {
            printf("something wrong!\n");
            exit(1);
        }
    }
    else if (part>= 0.5 && part<0.6)
    {
        F0 = 0.1;
        a = 1.18;
        Ebk = 400000;
        b = 2.31;
        if (energy < 100)
        {
            return F0 * pow(energy / 100, -1);
        }
        else if (energy >= 100 && energy < Ebk)
        {
            return F0 * pow(energy / 100, -a);
        }
        else if (energy >= Ebk)
        {
            return F0 * pow(Ebk / 100, -a) * pow(energy / Ebk, -b);
        }
        else
         {
            printf("something wrong!\n");
            exit(1);
        }
    }
    else if (part>= 0.6 && part<0.7 && direction == 0)
    {
        F0 = 0.13;
        a = 1.1;
        Ebk = 300000;
        b = 2.25;
        if (energy < 100)
        {
             return F0 * pow(energy / 100, -1);
        }
        else if (energy >= 100 && energy < Ebk)
        {
            return F0 * pow(energy / 100, -a);
        }
        else if (energy >= Ebk)
        {
            return F0 * pow(Ebk / 100, -a) * pow(energy / Ebk, -b);
        }
        else
        {
            printf("something wrong!\n");
            exit(1);
        }
    }
    else if (part>= 0.6 && part<0.7 && direction == 1)
    {
        F0 = 0.13;
        a = 1.1;
        Ebk = 300000;
        b = 2.95;
        if (energy < 100)
        {
            return F0 * pow(energy / 100, -1);
        }
            else if (energy >= 100 && energy < Ebk)
        {
            return F0 * pow(energy / 100, -a);
        }
        else if (energy >= Ebk)
        {
            return F0 * pow(Ebk / 100, -a) * pow(energy / Ebk, -b);
        }
        else
        {
            printf("something wrong!\n");
            exit(1);
    }
        }
    else if (part>= 0.7 && part<0.8 && direction == 0)
    {
        F0 = 0.2;
        a = 1.5;
        Ebk = 400000;
        b = 1.85;
        if (energy < 100)
        {
            return F0 * pow(energy / 100, -1);
        }
            else if (energy >= 100 && energy < Ebk)
        {
            return F0 * pow(energy / 100, -a);
        }
        else if (energy >= Ebk)
        {
            return F0 * pow(Ebk / 100, -a) * pow(energy / Ebk, -b);
        }
        else
        {
            printf("something wrong!\n");
            exit(1);
        }
    }
    else if (part>= 0.7 && part<0.8 && direction == 1)
    {
        F0 = 0.2;
            a = 1.5;
            Ebk = 400000;
            b = 4.16;
            if (energy < 100)
            {
                  return F0 * pow(energy / 100, -1);
            }
            else if (energy >= 100 && energy < Ebk)
            {
                  return F0 * pow(energy / 100, -a);
            }
            else if (energy >= Ebk)
            {
                  return F0 * pow(Ebk / 100, -a) * pow(energy / Ebk, -b);
            }
            else
            {
                  printf("something wrong!\n");
                  exit(1);
            }
      }
      else if (part>= 0.8 && part<0.9 && direction == 0)
      {
            F0 = 0.23;
            F1 = 0.017;
            a = 1.83;
            Ec = 177;
            if (energy < 100)
            {
                  return F0 * pow(energy / 100, -1);
            }
            else
            {
                  return F1 * pow(energy / 1000, -a) * exp(-pow(energy / Ec, -a + 1));
            }
      }
      else if (part>= 0.8 && part<0.9 && direction == 1)
      {
            F0 = 0.23;
            a = 1.53;
            Ebk = 400000;
            b = 4.86;
            if (energy < 100)
            {
                  return F0 * pow(energy / 100, -1);
            }
            else if (energy >= 100 && energy < Ebk)
            {
                  return F0 * pow(energy / 100, -a);
            }
            else if (energy >= Ebk)
            {
                  return F0 * pow(Ebk / 100, -a) * pow(energy / Ebk, -b);
            }
            else
            {
                  printf("something wrong!\n");
                  exit(1);
            }
      }
      else if (part >= 0.9 && direction == 1)
      {
            F0 = 0.44;
            F1 = 0.037;
            a = 1.98;
            Ec = 210;
            if (energy < 100)
            {
                  return F0 * pow(energy / 100, -1);
            }
            else
            {
                  return F1 * pow(energy / 1000, -a) * exp(-pow(energy / Ec, -a + 1));
            }
      }
      else if (part >= 0.9 && direction == 0)
      {
            F0 = 0.44;
            a = 2.25;
            Ebk = 400000;
            b = 3.09;
            if (energy < 100)
            {
                  return F0 * pow(energy / 100, -1);
            }
            else if (energy >= 100 && energy < Ebk)
            {
                  return F0 * pow(energy / 100, -a);
            }
            else if (energy >= Ebk)
            {
                  return F0 * pow(Ebk / 100, -a) * pow(energy / Ebk, -b);
            }
            else
            {
                  printf("something wrong!\n");
                  exit(1);
            }
      }
      else
      {
            printf("no mach!\n");
            exit(1);
      }
}

SecondaryParticleProton::SecondaryParticleProton()
{
    fPGType = "Secondary Particle - Proton";
    fClassName = "SecondaryParticleProton";
    fParticle = "proton";
    fParticleEnergy = 0. * MeV;
    fParticlePosition = G4ThreeVector(0., 0., 0.);
    fParticlePolarization = G4ThreeVector(0., 0., 0.);
    fParticleMomentumDirection = G4ThreeVector(0., 0., 0.);
    fTimeFlag = 0.;
    fTypeFlag = 6002;
    fTimeVector.clear();
    fEvtFlag = 0;

    bottom = 30; //bottom && top energy of the spectrum
    top = 20000;
}

void SecondaryParticleProton::GenTimeVector()
{
    fTimeVector.clear();
    fEvtFlag = 0;
    TF1 *SCPpFunc = new TF1("SCPpFunc",spectrum_function,10,10000000,2);
    SCPpFunc->SetParameters(latitude,direction);

    G4double uni_mean=SCPpFunc->Integral(bottom,top);//单位角度单位面积上的计数率
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


SecondaryParticleProton::~SecondaryParticleProton(){}

void SecondaryParticleProton::Initialize(G4double GenTime, std::vector<std::string> PGParameters)
{
    if (PGParameters.size() > 0)
    {
        if(PGParameters.size() != 4)
        {
            G4cout << "Warning!!! Length of Input PGParameters and number of Initialize parameters of SecondaryParticleProton not matched!!!" << G4endl;
            G4cout << "Use default Initialize parameters" << G4endl;
        }
        else
        {
            try
            {
                bottom = ConvertStrToDouble(PGParameters[0]);
                top = ConvertStrToDouble(PGParameters[1]);
                latitude=ConvertStrToDouble(PGParameters[2]);
                direction=ConvertStrToDouble(PGParameters[3]);
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

void SecondaryParticleProton::Generate()
{
    G4int success = 0;
    G4double energy = 0.;
    while (success != 1)
    {
        double y_rand = G4UniformRand();
        double x_rand = G4UniformRand() * (top - bottom) + bottom;
        double max_count;
        double *Energy;
        double parameter[2];
        Energy = &bottom;
        parameter[0]=latitude;
        parameter[1]=direction;
        max_count = spectrum_function(Energy,parameter);
        double out_y;
        Energy= &x_rand;
        out_y= spectrum_function(Energy,parameter)/max_count;
        if(y_rand<=out_y){
            success += 1;
            energy=x_rand;
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