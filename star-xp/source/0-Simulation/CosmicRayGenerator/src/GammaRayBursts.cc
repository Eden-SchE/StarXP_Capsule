#include "G4SystemOfUnits.hh"
#include "Randomize.hh"
#include "GammaRayBursts.hh"
#include "G4ThreeVector.hh"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"
#include "TFile.h"
#include "TTree.h"
#include "TF1.h"
#include "TRandom3.h"
#include "TVector3.h"
#include "TF1.h"
#pragma GCC diagnostic pop

using namespace std;

double GRBPolarProb(double *angle, double *par){
    double Angle = angle[0];
    double psi = par[0];
    double chi = par[1];
    double localangle = Angle-psi;
    TVector3 localX = TVector3(1,0,0);
    TVector3 localY = tan(chi)*TVector3(0,1,0);
    double a_2 = localX.Mag2();
    double b_2 = localY.Mag2();
    double k_2 = pow(tan(localangle),2);
    double Combin = (a_2*b_2*(k_2+1))/(a_2*k_2+b_2);
    return Combin;
}

double *GRBStokesAngle(TVector3 StokesPara){
    double *par = new double[2];
    double x = StokesPara.x();
    double y = StokesPara.y();
    double z = StokesPara.z();
    if(x == 0.){
        x = 0.0001;
    }
    if(y == 0.){
        y = 0.0001;
    }
    if(z == 0.){
        z = 0.0001;
    }
    par[0] = atan(y/x)/2.;
    par[1] = atan(z/sqrt(pow(x,2.)+pow(y,2.)))/2.;
    return par;
}


double GRBSelectangle(G4ThreeVector StokesPara){
    TVector3 mystokes = TVector3(StokesPara[0],StokesPara[1],StokesPara[2]);
    double *anglePar = GRBStokesAngle(mystokes);
    TF1 *getAngle=new TF1("getAngle",GRBPolarProb,0.,2*M_PI,2);
    getAngle->SetParameters(anglePar[0],anglePar[1]);
    delete anglePar;
    double ChoosedAngle = getAngle->GetRandom();
    delete getAngle;
    return ChoosedAngle;
}

G4double Index_Function(G4double *Eenergy, G4double *par){
    G4double index=par[0];
    G4double Emin =par[1];
    G4double Emax =par[2];
    G4double energy = Eenergy[0];
    return (pow(energy,-index)/0.0000000016);
}

G4double PolarAngDistribution(G4double *Angle,G4double *par){
    G4double angle = Angle[0];
    G4double digree = par[0];
    G4double Imin = (1-digree)/(1+digree);
    return (pow(cos(angle),2)+Imin*pow(sin(angle),2));
}

G4ThreeVector CacuPolar(G4double Angle,G4ThreeVector MomDir){
    G4ThreeVector tmp = G4ThreeVector(1,0.678,0.57);
    G4ThreeVector ProjectMax = tmp.cross(MomDir).unit();//偏振最大的方向
    G4ThreeVector OutPol = ProjectMax.rotate(Angle,MomDir);
    return OutPol;
}


G4double Energy_Spectrum(G4double *Eenergy, G4double *par){
    G4double flux = par[0];
    G4double index = par[1];
    G4double IntergralIndex = par[2];
    G4double Emin = par[3];
    G4double Emax = par[4];
    G4double energy = Eenergy[0];
    // TF1 *IndexFunc = new TF1("IndexFunc",Index_Function,0.2,11,3);
    // IndexFunc->SetParameters(index,0.3,10);
    // G4double IndexIntergral = IndexFunc->Integral(0.3,10);
    // delete IndexFunc;
    return flux * pow(energy,-index)/IntergralIndex;
}


GammaRayBursts::GammaRayBursts()
{   

    fPGType = "Gamma Ray Bursts";
    fClassName = "GammaRayBursts";
    fParticle = "gamma";
    fParticleEnergy = 0. * MeV;
    fParticlePosition = G4ThreeVector(0., 0., 0.);
    fParticlePolarization = G4ThreeVector(0., 0., 0.);
    fParticleMomentumDirection = G4ThreeVector(0., 0., 0.);
    fTimeFlag = 0.;
    fTypeFlag = 1001;
    fTimeVector.clear();
    fEvtFlag = 0;

    GRBname = 3; //bottom && top energy of the spectrum
    theta = 0.;
    phi = 0.;
}

GammaRayBursts::~GammaRayBursts(){}

void GammaRayBursts::GenTimeVector()
{   
    fTimeVector.clear();
    fEvtFlag = 0;
    TFile *f = new TFile("/mnt/e/star-xp/source/0-Simulation/GRBdata.root");
    if(!f){
        G4cerr <<"can not find GRBdata.root!" << G4endl;
                exit(EXIT_FAILURE);
    }
    TTree *t1=(TTree*)f->Get(GRBname);
    Double_t GRBTotalCountNumber = 0.;
    Double_t Tbottom=0;
    Double_t Tup=0;
    Double_t aBincount=0;
    SampleSquare square =SampleSquare();
    t1->SetBranchAddress("Totalcount",&GRBTotalCountNumber);
    t1->SetBranchAddress("Tdown",&Tbottom);
    t1->SetBranchAddress("Tup",&Tup);
    t1->SetBranchAddress("Bincount",&aBincount);
    Int_t nentries =(Int_t)t1->GetEntries();
    for(int ibin=0; ibin<nentries; ibin++){
        t1->GetEntry(ibin);
        double bttomtime = Tbottom;
        double uptime = Tup;
        if(fGenTime<uptime){
            break;
        }
        TRandom3* myrand= new TRandom3(0);
        double poissionnumber = square.GetLength()*square.GetLength()*aBincount;
        Int_t expBinnumber=myrand->Poisson(poissionnumber);
        for(int inumber =0;inumber<expBinnumber;inumber++){
            fTimeVector.push_back((G4UniformRand()*(Tup-Tbottom) + Tbottom));
        }
    }
    std::sort(fTimeVector.begin(), fTimeVector.end(), CosmicRay::mySort);
    delete f;
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

void GammaRayBursts::Initialize(G4double GenTime, std::vector<std::string> PGParameters)
{
    if (PGParameters.size() > 0)
    {
        if(PGParameters.size() != 10)
        {
            G4cout << "Warning!!! Length of Input PGParameters and number of Initialize parameters of GammaRayBursts not matched!!!" << G4endl;
            G4cout << "Use default Initialize parameters" << G4endl;
        }
        else
        {
            try
            {
                GRBname = PGParameters[0];
                theta = ConvertStrToDouble(PGParameters[1]);
                phi = ConvertStrToDouble(PGParameters[2]);
                fParticlePolarizationFactor = ConvertStrToDouble(PGParameters[3]);
                fOrigiPaticlePol = G4ThreeVector(ConvertStrToDouble(PGParameters[4]),ConvertStrToDouble(PGParameters[5]),ConvertStrToDouble(PGParameters[6]));
                fStokesParameters = G4ThreeVector(ConvertStrToDouble(PGParameters[7]),ConvertStrToDouble(PGParameters[8]),ConvertStrToDouble(PGParameters[9]));
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
    this->GenTimeVector();
    if(fGenTime==0)
    {
        G4cout<<"Warning: GRB only support GenTime Model!"<<G4endl;
    }    
}

void GammaRayBursts::Generate()//需要知道对应的产生时间才能够拿到能谱
{
    fTimeFlag = fTimeVector[fEvtFlag];
    G4double generTime =fTimeFlag;
    G4double energy = 0.;
    TFile *f = new TFile("/mnt/e/star-xp/source/0-Simulation/GRBdata.root");
    TTree *t1=(TTree*)f->Get(GRBname);
    Double_t GRBTotalCountNumber = 0.;
    Double_t Tbottom=0.;
    Double_t Tup=0.;
    Double_t flux=0.;
    Double_t myindex=0.;
    Double_t myIntergralIndex = 0.;
    t1->SetBranchAddress("Totalcount",&GRBTotalCountNumber);
    t1->SetBranchAddress("Tdown",&Tbottom);
    t1->SetBranchAddress("Tup",&Tup);
    t1->SetBranchAddress("Flux",&flux);
    t1->SetBranchAddress("Index",&myindex);
    t1->SetBranchAddress("IntergralIndex",&myIntergralIndex);
    Int_t nentries =(Int_t)t1->GetEntries();
    for(Int_t iS =0; iS<nentries;iS++){
        t1->GetEntry(iS);
        if(generTime>=Tbottom && generTime<Tup){
            TF1 *GRBEnergySpectrum = new TF1("GRBEnergySpectrum",Energy_Spectrum,2,10,5);
            GRBEnergySpectrum->SetParameters(flux,myindex,myIntergralIndex,2,10);
            energy = GRBEnergySpectrum->GetRandom();
            delete GRBEnergySpectrum;
            break;
        }
    }

    

    // G4int success = 0;
    // G4double energy = 0.;
    // while (success != 1)
    // {
    //     G4double max_count = 0; //need optimize4
    //     if (bottom <= (alpha - beta) * E_0)
    //     {
    //         max_count = pow(bottom / 100, alpha) * exp(-bottom / E_0);
    //     }
    //     else
    //     {
    //         max_count = pow((alpha - beta) * E_0 / 100, alpha - beta) * exp(beta - alpha) * pow(bottom / 100, beta);
    //     }
    //     G4double y_rand = G4UniformRand();
    //     G4double x_rand = G4UniformRand() * (top - bottom) + bottom;
    //     G4double out_y = 0;
    //     if (x_rand <= (alpha - beta) * E_0)
    //     {
    //         out_y = pow(x_rand / 100, alpha) * exp(-x_rand / E_0) / max_count;
    //     }
    //     else
    //     {
    //         out_y = pow((alpha - beta) * E_0 / 100, alpha - beta) * exp(beta - alpha) * pow(x_rand / 100, beta) / max_count;
    //     }
    //     if (y_rand <= out_y)
    //     {
    //         energy = x_rand;
    //         success += 1;
    //     }
    // }

    fParticleEnergy = energy * keV;
    SampleSquare square =SampleSquare();
    double squareLength = square.GetLength();
    //printf("!!!!!!!!! Lenght is %lf\n",squareLength);
    G4double my_x = square.GetLength() * (G4UniformRand() - 0.5);
    G4double my_y = square.GetLength() * (G4UniformRand() - 0.5);
    fParticlePosition = G4ThreeVector( 100 * sin(theta) * cos(phi)+my_x,
                                       100 * sin(theta) * sin(phi)+my_y,
                                       100 * cos(theta)) * cm;
    fParticleMomentumDirection = G4ThreeVector(-sin(theta) * cos(phi), -sin(theta) * sin(phi), -cos(theta));

    
    // TF1 *myPol = new TF1("myPol",PolarAngDistribution,0,2*M_PI,2);
    // //A,a,solar_modulation,orbit_hight,Rearth,latitude,R,r
    // myPol->SetParameters(PolarDigree,0);
    // G4double aPolangle=myPol->GetRandom();
    // fParticlePolarization = CacuPolar(aPolangle,fParticlePosition);
    // delete myPol;

    if( fParticlePolarizationFactor >= 0)
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
            fParticlePolarization = fOrigiPaticlePol;
        }
    }
    double rotAngle = GRBSelectangle(fStokesParameters);
    TVector3 aParticleMomentumDirection = TVector3(fParticleMomentumDirection.x(),fParticleMomentumDirection.y(),fParticleMomentumDirection.z());
    TVector3 aParticlePolarization = TVector3(fParticlePolarization.x(),fParticlePolarization.y(),fParticlePolarization.z());
    aParticlePolarization.Rotate(rotAngle,aParticleMomentumDirection);
    fParticlePolarization = G4ThreeVector(aParticlePolarization.x(),aParticlePolarization.y(),aParticlePolarization.z());

    ++fEvtFlag;
    delete f;
}