#ifndef _StokesVector_H_
#define _StokesVector_H_

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"
#include "TVector3.h"
#include "TF1.h"
#pragma GCC diagnostic pop
#include "G4ThreeVector.hh"

using namespace std;

double PolarProb(double *angle, double *par){
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

double *StokesAngle(TVector3 StokesPara){
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


double Selectangle(G4ThreeVector StokesPara){
    TVector3 mystokes = TVector3(StokesPara[0],StokesPara[1],StokesPara[2]);
    double *anglePar = StokesAngle(mystokes);
    TF1 *getAngle=new TF1("getAngle",PolarProb,0.,2*M_PI,2);
    getAngle->SetParameters(anglePar[0],anglePar[1]);
    delete anglePar;
    double ChoosedAngle = getAngle->GetRandom();
    delete getAngle;
    return ChoosedAngle;
}
    

#endif

