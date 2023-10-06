#include "TFile.h"

#include "SDBoxContainer.hh"

#include <cmath>
#include <iostream>

#include "TopMetal.hh"
#include "FilterCreator.hh"

Double_t TopMetal::m_PixelTime = 0.;
Double_t TopMetal::m_RowTime = 0.;
Double_t TopMetal::m_FrameTime = 0.;

Double_t GetHistValue(TH2F* th2, UInt_t iX, UInt_t iY)
{
    //读取th2中对应iX, iY bin中的值
    Double_t Value = th2->GetBinContent(th2->GetBin(iX+1, iY+1));
    if (Value != 0)
        return Value;

    //如果值为0，则取周围值做平均
    Int_t ValidBin = 0;
    Double_t Average = 0.;
    Int_t it;
    for(it = 1; ; ++it)
    {
        for(Int_t ix = -it; ix <= it; ++ix)
        {
            for(Int_t iy = -it; iy <= it; ++iy)
            {
                Double_t val = th2->GetBinContent(th2->GetBin(iX+ix+1, iY+iy+1));
                if(val != 0.)
                {
                    ++ValidBin;
                    Average+=val;
                }
            }
        }
        if(ValidBin > 0) break;
    }
    return Average/Double_t(ValidBin);
}

TopMetal::TopMetal(UInt_t iTopMetal):
    m_iTopMetal(iTopMetal), m_nFrame(10), m_iX(0), m_iY(0), TimeAction("TopMetal", "")
{
    SDBox* theBox = &SDBoxContainer::GetInstance()[m_iTopMetal];
    m_NumOfPixelX = theBox->NumOfPixel().first;
    m_NumOfPixelY = theBox->NumOfPixel().second;
    m_PixelTime = theBox->PixelTime();
    m_NumOfFramePerEvent = SDBoxContainer::NumOfFramePerEvent();
    m_nFrame = m_NumOfFramePerEvent;
    m_RowTime = m_PixelTime * m_NumOfPixelY;
    m_FrameTime = m_RowTime * m_NumOfPixelX;
    m_TH2TopMetal = new TH2F(TString::Format("rawTopMetal_%d", m_iTopMetal), "", m_NumOfPixelX, 0, m_NumOfPixelX+1, m_NumOfPixelY, 0, m_NumOfPixelY+1);

    if (theBox->TopMetalCalib() != std::string(""))
    {
        if(TimeControl::Verbose())
        {
            std::cout << "TopMetal::TopMetal(): Use Calib file: " << theBox->TopMetalCalib() << std::endl;
        }
        TFile CalibFile(theBox->TopMetalCalib().c_str());
        TH2F *th2_BKGmean = dynamic_cast<TH2F *>(CalibFile.Get("pede_mean"));
        TH2F *th2_BKGerr = dynamic_cast<TH2F *>(CalibFile.Get("pede_sigma"));
        TH2F *th2_DecayNormal = dynamic_cast<TH2F *>(CalibFile.Get("pix_fit_a"));
        TH2F *th2_DecayFrac = dynamic_cast<TH2F *>(CalibFile.Get("pix_fit_b"));
        TH2F *th2_DecayConst = dynamic_cast<TH2F *>(CalibFile.Get("pix_fit_c"));

        for (UInt_t iX = 0; iX < m_NumOfPixelX; ++iX)
        {
            for (UInt_t iY = 0; iY < m_NumOfPixelY; ++iY)
            {
                Pixel *pixel = new Pixel(m_iTopMetal);
                pixel->SetElectronicsBKG(GetHistValue(th2_BKGmean, iX, iY), GetHistValue(th2_BKGerr, iX, iY));
                pixel->SetDecayFractor(GetHistValue(th2_DecayNormal, iX, iY), GetHistValue(th2_DecayFrac, iX, iY), GetHistValue(th2_DecayConst, iX, iY));
                m_MapPixels[std::pair<UInt_t, UInt_t>(iX, iY)] = pixel;
                m_TH2TopMetal->SetBinContent(m_TH2TopMetal->GetBin(iX + 1, iY + 1), m_MapPixels[std::pair<UInt_t, UInt_t>(iX, iY)]->Flash(0.));
            }
        }
        delete th2_BKGmean;
        delete th2_BKGerr;
        delete th2_DecayNormal;
        delete th2_DecayFrac;
        delete th2_DecayConst;
        CalibFile.Close();
    }
    else
    {
        YAML::Node calibNode = theBox->TopMetalConstCalib();
        if(TimeControl::Verbose())
        {
            std::cout << "TopMetal::TopMetal(): Use const Calib parameters." << std::endl;
            for (auto it = calibNode.begin(); it != calibNode.end(); ++it)
            {
                std::cout << it->first.as<std::string>() << ": " << it->second.as<double>() << std::endl;
            }
        }
        double mean = calibNode["mean"].as<double>(1200);
        double sigma = calibNode["sigma"].as<double>(1e-4);
        double cA = calibNode["cA"].as<double>(360.);
        double cB = calibNode["cB"].as<double>(-0.5);
        double cC = calibNode["cC"].as<double>(100.);

        for (UInt_t iX = 0; iX < m_NumOfPixelX; ++iX)
        {
            for (UInt_t iY = 0; iY < m_NumOfPixelY; ++iY)
            {
                Pixel *pixel = new Pixel(m_iTopMetal);
                pixel->SetElectronicsBKG(mean, sigma);
                pixel->SetDecayFractor(cA, cB, cC);
                m_MapPixels[std::pair<UInt_t, UInt_t>(iX, iY)] = pixel;
                m_TH2TopMetal->SetBinContent(m_TH2TopMetal->GetBin(iX + 1, iY + 1), m_MapPixels[std::pair<UInt_t, UInt_t>(iX, iY)]->Flash(0.));
            }
        }
    }

    m_Filter = static_cast<BasicFilter*>(FilterCreator::GetInstance().CreateNewFilter());
    //m_Filter = (BasicFilter*)FilterCreator::GetInstance().CreateNewFilter();
    m_Filter->SetTopMetalID(m_iTopMetal);
    m_Filter->InitFilter(m_TH2TopMetal);
}

TopMetal::~TopMetal()
{
    for(auto& iMapPixel:m_MapPixels)
    {
        if(iMapPixel.second != NULL)
            delete iMapPixel.second;
    }
    delete m_Filter;
    //delete m_TH2TopMetal;
}

void TopMetal::ClearAllCharge()
{
    for(UInt_t iX = 0; iX < m_NumOfPixelX; ++iX)
    {
        for (UInt_t iY = 0; iY < m_NumOfPixelY; ++iY)
        {
            m_TH2TopMetal->SetBinContent(m_TH2TopMetal->GetBin(iX+1, iY+1), m_MapPixels[std::pair<UInt_t, UInt_t>(iX, iY)]->ClearCharge());
        }
    }
    m_Filter->InitFilter(m_TH2TopMetal);
    m_nFrame = 0;
    m_iX = 0;
    m_iY = 0;
}

void TopMetal::InitTopMetal(Double_t TimeFlag)
{
    if (m_nFrame >= m_NumOfFramePerEvent)
    {
        Double_t FrameStartTime = TimeFlag - floor(TimeFlag / m_FrameTime) * m_FrameTime;
        m_iY = floor(FrameStartTime / m_RowTime);
        Double_t RowStartTime = FrameStartTime - (m_RowTime * m_iY);
        m_iX = floor(RowStartTime / m_PixelTime);

        for (UInt_t iY = 0; iY < m_NumOfPixelY; ++iY)
        {
            for (UInt_t iX = 0; iX < m_NumOfPixelX; ++iX)
            {
                m_TH2TopMetal->SetBinContent(m_TH2TopMetal->GetBin(iX + 1, iY + 1), m_MapPixels[std::pair<UInt_t, UInt_t>(iX, iY)]->Flash(TimeFlag - FrameStartTime * 2 + (iY * m_NumOfPixelY + iX) * m_PixelTime, 0));
            }
        }
        m_Filter->InitFilter(m_TH2TopMetal);

        if (TimeControl::Verbose())
        {
            std::cout << "m_FrameTime: " << m_FrameTime << "\tm_RowTime: " << m_RowTime << "\tm_PixelTime: " << m_PixelTime << std::endl;
            std::cout << "TimeFlag: " << TimeFlag << "\tFrameStartTime: " << FrameStartTime << "\tRowStartTime: " << RowStartTime << std::endl;
            std::cout << "m_iX: " << m_iX << "\tm_iY: " << m_iY << std::endl;
        }
        if (m_iY > 0)
        {
            for (UInt_t iY = 0; iY < m_iY; ++iY)
            {
                for (UInt_t iX = 0; iX < m_NumOfPixelX; ++iX)
                {
                    m_TH2TopMetal->SetBinContent(m_TH2TopMetal->GetBin(iX + 1, iY + 1), m_MapPixels[std::pair<UInt_t, UInt_t>(iX, iY)]->Flash(TimeFlag - FrameStartTime + (iY * m_NumOfPixelY + iX) * m_PixelTime, 0));
                }
            }
        }

        for (UInt_t iX = 0; iX < m_iX; ++iX)
        {
            m_TH2TopMetal->SetBinContent(m_TH2TopMetal->GetBin(iX + 1, m_iY + 1), m_MapPixels[std::pair<UInt_t, UInt_t>(iX, m_iY)]->Flash(TimeFlag - FrameStartTime + (m_iY * m_NumOfPixelY + iX) * m_PixelTime, 0));
        }
    }
    m_nFrame = 0;
}

std::pair<Double_t, TimeAction*> TopMetal::Action(Double_t TimeFlag)
{
    if(TimeControl::Verbose()) std::cout << __LINE__ << " TopMetal::Action() ";

    Double_t FlashTime = ceil(TimeFlag / m_PixelTime) * m_PixelTime;
    if(m_iX == m_NumOfPixelX)
    {
        m_iX = 0;
        ++m_iY;
    }
    if(m_iY == m_NumOfPixelY)
    {
        //if(m_Filter->Filter(m_TH2TopMetal, m_nFrame, FlashTime))
        //{
        //    TopMetalContainer::GetInstance().GetOutputFilePtr()->WriteFrame(m_iTopMetal, FlashTime, m_nFrame);
        //}
        m_Filter->Filter(m_TH2TopMetal, m_nFrame, FlashTime);
        m_iX = 0;
        m_iY = 0;
        ++m_nFrame;
    }
    if(m_nFrame >= m_NumOfFramePerEvent && m_NumOfFramePerEvent > 0)
    {
        return std::pair<Double_t, TimeAction*>(-1., NULL);
    }

    if(TimeControl::Verbose()) std::cout << "m_iX: " << m_iX << "\tm_iY: " << m_iY << std::endl;
    m_TH2TopMetal->SetBinContent(m_TH2TopMetal->GetBin(m_iX+1, m_iY+1), m_MapPixels[std::pair<UInt_t, UInt_t>(m_iX, m_iY)]->Flash(FlashTime, 0));

    ++m_iX;
    return std::pair<Double_t, TimeAction *>(FlashTime + m_PixelTime, NULL);
}

Pixel* TopMetal::iPixel(UInt_t iX, UInt_t iY)
{
    if(iX >= m_NumOfPixelX || iY >= m_NumOfPixelY)
    {
        std::cerr << "Error!!! Required Pixel ( " << iX << ", " << iY << " ) out of range!!!" << std::endl;
        exit(EXIT_FAILURE);
    }
    return m_MapPixels[std::pair<UInt_t, UInt_t>(iX, iY)];
}