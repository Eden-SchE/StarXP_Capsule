#include "Pixel.hh"
#include "TRandom.h"
#include "SDBoxContainer.hh"

#include <cmath>

Pixel::Pixel(int iTopMetal):
    m_TimeFlag(0.), m_Charge(0.), m_BKGmean(0.), m_BKGerr(0.), m_DecayNormal(0.), m_DecayFrac(0.), m_DecayConst(0.)
{
    m_ADCResponse = SDBoxContainer::GetInstance()[iTopMetal].ADCResponse();
    m_TopMetalCollectingEfficiency = SDBoxContainer::GetInstance()[iTopMetal].TopMetalCollectingEfficiency();
}

Pixel::~Pixel(){}

Double_t Pixel::ClearCharge()
{
    m_TimeFlag = 0.;
    //m_Charge = gRandom->Gaus(m_BKGmean, m_BKGerr);
    m_Charge = 0.;
    return gRandom->Gaus(m_BKGmean, m_BKGerr);
}

Double_t Pixel::Flash(Double_t TimeFlag, Double_t AddCharge)
{
    //原有电荷衰减
    if (m_Charge > 0)
    {
        Double_t DeltaT = (TimeFlag - m_TimeFlag) * 1000;
        if (DeltaT < 0)
        {
            if (TimeControl::Verbose())
            {
                std::cout << "Warning!!! Input TimeFlag smaller than the last TimeFlag in Pixel, reset DeltaT to 0" << std::endl;
                std::cout << "TimeFlag: " << TimeFlag << "\tm_TimeFlag:" << m_TimeFlag << "\tDeltaT: " << DeltaT << std::endl;
            }
            DeltaT = 0;
            return m_Charge + gRandom->Gaus(m_BKGmean, m_BKGerr);
        }
        if (m_DecayFrac * DeltaT + m_DecayConst > 0)
        {
            m_Charge = m_Charge * exp(-DeltaT / (m_DecayFrac * DeltaT + m_DecayConst));
            if (m_Charge < 0.)
                m_Charge = 0.;
        }
        else
        {
            m_Charge = 0.;
        }
    }

    //新增电荷
    m_Charge = m_Charge + AddCharge * m_ADCResponse * m_TopMetalCollectingEfficiency;
    m_TimeFlag = TimeFlag;

    return m_Charge + gRandom->Gaus(m_BKGmean, m_BKGerr);
}