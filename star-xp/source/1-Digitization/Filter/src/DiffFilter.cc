#include "DiffFilter.hh"
#include "FilterCreator.hh"
#include "TEnv.h"
#include "SDBoxContainer.hh"
#include <iostream>

REGISTFILTER(DiffFilter);

DiffFilter::DiffFilter():
    m_ThresholdV(20.), m_ThresholdN(20.), m_Start(false)
{
    m_ThresholdN = FilterCreator::GetInstance().GetYaml()["ThresholdN"].as<int>(20);
    m_ThresholdV = FilterCreator::GetInstance().GetYaml()["ThresholdV"].as<int>(20);
    m_OutputHist = nullptr;

    //m_thPre->clear();
    //m_thThis->clear();
}
DiffFilter::~DiffFilter()
{
    delete m_thPre;
    delete m_thThis;
    delete m_thDiff;
}

void DiffFilter::InitFilter(TH2* thFilter)
{
    HistToVector(thFilter, m_thPre);
}

void DiffFilter::SetTopMetalID(UInt_t iTopMetal)
{
    m_iTopMetal = iTopMetal;
    m_OutputHist = TopMetalContainer::GetInstance().GetOutputFilePtr()->CreateTopMetalTree(m_iTopMetal);

    m_nBinX = m_OutputHist->GetNbinsX();
    m_nBinY = m_OutputHist->GetNbinsY();
    //m_nBinX = SDBoxContainer::GetInstance()[m_iTopMetal].NumOfPixel().first;
    //m_nBinY = SDBoxContainer::GetInstance()[m_iTopMetal].NumOfPixel().second;
    m_thPre  = new std::vector<std::vector<double> >(m_nBinX, std::vector<double>(m_nBinY, 0.));
    m_thThis = new std::vector<std::vector<double> >(m_nBinX, std::vector<double>(m_nBinY, 0.));
    m_thDiff = new std::vector<std::vector<double> >(m_nBinX, std::vector<double>(m_nBinY, 0.));
    //for (int iX = 0; iX < m_nBinX; ++iX)
    //{
    //    (*m_thPre)[iX].clear();
    //    (*m_thThis)[iX].clear();
    //    (*m_thThis)[iX].clear();
    //}
}

void DiffFilter::VectorToVector(std::vector<std::vector<double>> *vectorIn, std::vector<std::vector<double>> *vectorOut)
{
    for (int iX = 0; iX < m_nBinX; ++iX)
    {
        for (int iY = 0; iY < m_nBinY; ++iY)
        {
            (*vectorOut)[iX][iY] = (*vectorIn)[iX][iY];
        }
    }
}

void DiffFilter::HistToVector( TH2 *histIn, std::vector<std::vector<double>> *vectorOut)
{
    for (int iX = 0; iX < m_nBinX; ++iX)
    {
        for (int iY = 0; iY < m_nBinY; ++iY)
        {
            (*vectorOut)[iX][iY] = histIn->GetBinContent(iX+1, iY+1);
        }
    }
}

void DiffFilter::VectorToHist(std::vector<std::vector<double>> *vectorIn, TH2 *histOut)
{
    for (int iX = 0; iX < m_nBinX; ++iX)
    {
        for (int iY = 0; iY < m_nBinY; ++iY)
        {
            histOut->SetBinContent(iX+1, iY+1, (*vectorIn)[iX][iY]);
        }
    }
}

void DiffFilter::Filter(TH2* thFilter, int nFrame, double FlashTime)
{
    if (m_OutputHist == nullptr)
    {
        std::cout << "Error!!! Using Filter before initialization!" << std::endl;
        exit(EXIT_FAILURE);
    }
    m_nFrame = nFrame;
    m_FlashTime = FlashTime;

    HistToVector(thFilter, m_thThis);

    int NCount = 0;
    for(int iX = 0; iX < m_nBinX; ++iX)
    {
        for (int iY = 0; iY < m_nBinY; ++iY)
        {
            (*m_thDiff)[iX][iY] = thFilter->GetBinContent(iX+1, iY+1) - (*m_thPre)[iX][iY];
            if((*m_thDiff)[iX][iY] > m_ThresholdV)
            {
                ++NCount;
            }
            else
            {
                (*m_thDiff)[iX][iY] = 0.;
            }
        }
    }
    VectorToVector(m_thThis, m_thPre);

    if(NCount >= m_ThresholdN)
    {
        this->Save();
    }
}

void DiffFilter::Save()
{
    VectorToHist(m_thDiff, m_OutputHist);
    TopMetalContainer::GetInstance().GetOutputFilePtr()->WriteFrame(m_iTopMetal, m_FlashTime, m_nFrame);
}