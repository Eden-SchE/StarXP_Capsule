#include "PixelTimeFix.h"

short PixelfixReverse(int ix, int iy, short value, double deltaT, TFile* CalibFile){
    // std::cout<<"delta time is: "<<deltaT<<std::endl;
    TH2F* pix_fit_a = (TH2F*)CalibFile->Get("pix_fit_a");
    TH2F* pix_fit_b = (TH2F*)CalibFile->Get("pix_fit_b");
    TH2F* pix_fit_c = (TH2F*)CalibFile->Get("pix_fit_c");
    double cacu =0;
    double a = pix_fit_a->GetBinContent(ix,iy);
    double b = pix_fit_b->GetBinContent(ix,iy);
    double c = pix_fit_c->GetBinContent(ix,iy);
    double t_0 = double((-c*log(double(value)/a))/(b*log(double(value)/a)+1.)-deltaT);
    cacu = a*exp((-t_0)/(b*t_0 +c));
    // std::cout<<"origi pic is: "<<value<<" , fixed pic is: "<<short(cacu+0.5)<<std::endl;
    return short(cacu+0.5);
}