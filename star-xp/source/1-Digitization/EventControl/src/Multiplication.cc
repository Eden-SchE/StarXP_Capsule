#include "TRandom.h"

#include "SDBoxContainer.hh"
#include "TopMetalContainer.hh"
#include "TimeControl.hh"
#include "GMCPAction.hh"

#include "Multiplication.hh"

#include <cmath>

Multiplication::Multiplication(IonizationInfo ionInfo):
    m_IonInfo(ionInfo), m_TH2ElectronOnTopMetal(NULL), m_iPixel(0), m_NumOfTouchedPixel(0), TimeAction("Multiplication", "Step")
{
    m_TouchedPixelVector.clear();

    SDBox* theBox = &SDBoxContainer::GetInstance()[m_IonInfo.SDNum];

    UInt_t XPixel = theBox->NumOfPixel().first;
    UInt_t YPixel = theBox->NumOfPixel().second;
    Double_t XAxisLength = XPixel * theBox->PixelSize();
    Double_t YAxisLength = YPixel * theBox->PixelSize();
    m_TH2ElectronOnTopMetal = new TH2F("th2_ElecOnTopMetal", "", XPixel, -XAxisLength/2., XAxisLength/2., YPixel, -YAxisLength/2., YAxisLength/2.);

    Int_t NumOfMCPElectron = gRandom->Poisson(theBox->MultiplicationFactor());
    std::pair<Double_t, Double_t> MCPHolePosition = theBox->MCPHole(m_IonInfo.InitElectronPosition);

    for(int i = 0; i < NumOfMCPElectron; ++i)
    {
        m_TH2ElectronOnTopMetal->Fill(gRandom->Gaus(MCPHolePosition.first, theBox->DriftSmear().second * sqrt(theBox->MCPTopMetalDistance())), gRandom->Gaus(MCPHolePosition.second, theBox->DriftSmear().second * sqrt(theBox->MCPTopMetalDistance())));
    }
    for(UInt_t iY = 0; iY < YPixel; ++iY)
    {
        for(UInt_t iX = 0; iX < XPixel; ++iX)
        {
            Double_t NumOfElectron = m_TH2ElectronOnTopMetal->GetBinContent(m_TH2ElectronOnTopMetal->GetBin(iX+1, iY+1));
            if(NumOfElectron > 0)
            {
                TouchedPixel tPixel;
                tPixel.iX = iX;
                tPixel.iY = iY;
                tPixel.NumOfElectron = NumOfElectron;
                m_TouchedPixelVector.push_back(tPixel);
            }
        }
    }
    m_TimeOntoTopMetal = m_IonInfo.MCPTime + theBox->MCPTopMetalDistance() / theBox->DriftSpeed().second;
    m_NumOfTouchedPixel = m_TouchedPixelVector.size();
    if(TimeControl::GMCPOutput())
        TopMetalContainer::GetInstance().iGMCP(m_IonInfo.SDNum)->AddCharge(m_IonInfo.MCPTime, NumOfMCPElectron);
    //std::cout << "NumOfMCPElectron: " << NumOfMCPElectron << "\tMCPHolePosition: (" << MCPHolePosition.first << ", " << MCPHolePosition.second << " )\tm_NumOfTouchedPixel: " << m_NumOfTouchedPixel << std::endl;

    delete m_TH2ElectronOnTopMetal;
}

Multiplication::~Multiplication()
{}

std::pair<Double_t, TimeAction*> Multiplication::Action(Double_t TimeFlag)
{
    if(m_iPixel == m_NumOfTouchedPixel)
        return std::pair<Double_t, TimeAction*>(-1., NULL);

    TouchedPixel* tPixel = &m_TouchedPixelVector[m_iPixel];
    //std::cout << "Time: " << TimeFlag << "\tTouched Pixel iX: " << tPixel->iX << "\tiY: " << tPixel->iY << "\tNumOfEnectron: " << tPixel->NumOfElectron << std::endl;
    TopMetalContainer::GetInstance().iTopMetal(m_IonInfo.SDNum)->iPixel(tPixel->iX, tPixel->iY)->Flash(TimeFlag, tPixel->NumOfElectron);

    ++m_iPixel;
    return std::pair<Double_t, TimeAction*>(TimeFlag, NULL);
}