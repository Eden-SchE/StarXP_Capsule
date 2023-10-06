#ifndef _Pixel_h_Digi_
#define _Pixel_h_Digi_

#include "Rtypes.h"
//#include "TimeControl.hh"
#include <iostream>

class Pixel
{
public:
    Pixel(int);
    ~Pixel();
    inline void SetElectronicsBKG(Double_t mean, Double_t sigma)
    {
        m_BKGmean = mean;
        m_BKGerr = sigma;
        //std::cout << "m_BKGmean: " << m_BKGmean << "\tm_BKGerr: " << m_BKGerr;
    }
    inline void SetDecayFractor(Double_t Normal, Double_t Frac, Double_t Const)
    {
        m_DecayNormal = Normal;
        m_DecayFrac = Frac;
        m_DecayConst = Const;
        //std::cout << "\tm_DecayNormal: " << m_DecayNormal << "\tm_DecayFrac: " << m_DecayFrac << "\tm_DecayConst: " << m_DecayConst << std::endl;
    }

    Double_t Flash(Double_t TimeFlag, Double_t AddCharge = 0.);
    Double_t ClearCharge();

private:
    Double_t m_TimeFlag;
    Double_t m_Charge;
    Double_t m_BKGmean;
    Double_t m_BKGerr;
    Double_t m_DecayNormal;
    Double_t m_DecayFrac;
    Double_t m_DecayConst;
    Double_t m_ADCResponse;
    Double_t m_TopMetalCollectingEfficiency;

};

#endif