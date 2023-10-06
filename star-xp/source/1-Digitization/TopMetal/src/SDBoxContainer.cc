#include "SDBoxContainer.hh"
#include "TMath.h"
#include "YamlConfig.hh"
#include "TimeControl.hh"

#include <iostream>
#include <string>
#include <cmath>

SDBoxContainer *SDBoxContainer::m_Instance = NULL;

Int_t SDBoxContainer::m_NumOfFramePerEvent = 1;                    //每个Event应存的帧数
UInt_t SDBoxContainer::m_NumOfSD = 0;                                       //GasSD个数
//TVector3 SDBox::m_SDSize = TVector3(6., 6., 10.);                  //GasSD尺寸

SDBoxContainer::SDBoxContainer()
{
    m_SDBoxVector.clear();
}

SDBoxContainer::~SDBoxContainer(){}

void SDBoxContainer::ReadSDConfig(TChain* Chain)
{
    //读取SD大小及个数
    Double_t SDSize[3] = {0.};
    UInt_t NumOfSD = 0;
    Double_t SDPosition[100][3];

    Chain->SetBranchAddress("SDSize", &SDSize);
    Chain->SetBranchAddress("NumOfSD", &NumOfSD);
    Chain->SetBranchAddress("SDPosition", &SDPosition);

    Chain->GetEntry(0);

    m_NumOfSD = NumOfSD;
    if(NumOfSD == 0)
    {
        std::cerr << "Error!!! Number Of SD == 0 " << std::endl;
        exit(EXIT_FAILURE);
    }
    m_SDSize = TVector3(SDSize[0], SDSize[1], SDSize[2]);

    for(UInt_t iS = 0; iS < NumOfSD; ++iS)
    {
        m_SDBoxVector.push_back(SDBox(m_SDSize, TVector3(SDPosition[iS][0], SDPosition[iS][1], SDPosition[iS][2])));
    }
    Chain->ResetBranchAddresses();

    YAML::Node detectorNode = YamlConfig::GetInstance().GetYaml()["Detector"];
    m_NumOfFramePerEvent = detectorNode["NumOfFramePerEvent"].as<int>(1);
    if (detectorNode["Default"].IsDefined())
    {
        for (int i = 0; i < m_NumOfSD; ++i)
        {
            std::ostringstream buffer;
            buffer << "Detector_" << i;
            std::string detectorName = buffer.str();
            if(!detectorNode[detectorName].IsDefined())
            {
                std::cout << "Warning! Yaml: Yaml node of Detector \" " << i << " \" NOT FOUND. Using default value." << std::endl;
                detectorNode[detectorName] = detectorNode["Default"];
            }
            else
            {
                for (auto itd = detectorNode["Default"].begin(); itd != detectorNode["Default"].end(); ++itd)
                {
                    std::string key = itd->first.as<std::string>();
                    if (!detectorNode[detectorName][key].IsDefined())
                    {
                        detectorNode[detectorName][key] = itd->second;
                    }
                }
            }
            m_SDBoxVector[i].InitCalib(detectorNode[detectorName]);
        }
    }
    else
    {
        std::cerr << "Error!!! Yaml: Default setting of Detector NOT FOUND!" << std::endl;
        exit(0);
    }
    if(TimeControl::Verbose()) std::cout << detectorNode << std::endl;

}

SDBox& SDBoxContainer::operator[](unsigned int iSD)
{
    if (iSD >= m_NumOfSD)
    {
        std::cerr << "Error!!! Input iSD " << iSD << " larger than number of SDBox " << m_NumOfSD << " !!!" << std::endl;
        exit(EXIT_FAILURE);
    }
    return m_SDBoxVector[iSD];
}

SDBox& SDBoxContainer::GetSD(unsigned int iSD)
{
    if(iSD >= m_NumOfSD)
    {
        std::cerr << "Error!!! Input iSD " << iSD << " larger than number of SDBox " << m_NumOfSD << " !!!" << std::endl;
        exit(EXIT_FAILURE);
    }
    return m_SDBoxVector[iSD];
}

Int_t SDBoxContainer::WhichSD(TVector3 Point, bool Loose)
{
    bool IfFindSD = false;
    Int_t iSD = 0;
    for (iSD = 0; iSD < m_NumOfSD; ++iSD)
    {
        if(m_SDBoxVector[iSD].IfPointInSD(Point, Loose)) 
        {
            IfFindSD = true;
            break;   
        }
    }
    if(IfFindSD == true) return iSD;
    
    return -1;
}