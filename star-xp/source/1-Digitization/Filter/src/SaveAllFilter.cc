#include "SaveAllFilter.hh"
#include "FilterCreator.hh"
#include "TopMetalContainer.hh"

REGISTFILTER(SaveAllFilter);

void SaveAllFilter::SetTopMetalID(UInt_t iT)
{
    m_iTopMetal = iT;
    m_InHist = nullptr;
    m_OutputHist = TopMetalContainer::GetInstance().GetOutputFilePtr()->CreateTopMetalTree(m_iTopMetal);
    m_nBinX = m_OutputHist->GetNbinsX();
    m_nBinY = m_OutputHist->GetNbinsY();
}

void SaveAllFilter::Filter(TH2* thFilter, int nFrame, double FlashTime)
{
    m_nFrame = nFrame;
    m_FlashTime = FlashTime;
    m_InHist = thFilter;
    this->Save();   
}

void SaveAllFilter::Save()
{
    for(int iX = 0; iX < m_nBinX; ++iX)
        for(int iY = 0; iY < m_nBinY; ++iY)
        {
            m_OutputHist->SetBinContent(iX+1, iY+1, m_InHist->GetBinContent(iX+1, iY+1));
        }
    TopMetalContainer::GetInstance().GetOutputFilePtr()->WriteFrame(m_iTopMetal, m_FlashTime, m_nFrame);
}
