#include "SDBoxContainer.hh"
#include "TChain.h"
#include "YamlConfig.hh"

#include <iostream>

#include "TopMetalContainer.hh"

TopMetalContainer* TopMetalContainer::m_Instance = NULL;

TopMetalContainer::TopMetalContainer()
{
    m_TopMetalVector.clear();
    m_OutputFilePtr = NULL;
}

TopMetalContainer::~TopMetalContainer()
{

    for(auto& iTopMetal:m_TopMetalVector)
    {
        if(iTopMetal != NULL)
            delete iTopMetal;
    }
    for(auto& iGMCP:m_GMCPVector)
    {
        if(iGMCP != NULL)
            delete iGMCP;
    }
    delete m_OutputFilePtr;
}

TopMetal* TopMetalContainer::iTopMetal(UInt_t iT)
{
    if(iT >= m_TopMetalVector.size())
    {
        std::cerr << "Error!!! Required iT "<< iT <<" larger than number of TopMetals in Container!!!" << std::endl;
        exit(EXIT_FAILURE);
    }
    return m_TopMetalVector[iT];
}

GMCPAction* TopMetalContainer::iGMCP(UInt_t iT)
{
    if(iT >= m_GMCPVector.size())
    {
        std::cerr << "Error!!! Required iT "<< iT <<" larger than number of GMCPs in Container!!!" << std::endl;
        exit(EXIT_FAILURE);
    }
    return m_GMCPVector[iT];
}

void TopMetalContainer::Initialize()
{
    m_OutputFilePtr = new OutputFileWriter();
    
    m_NumOfSD = SDBoxContainer::NumOfSD();
    for (UInt_t iT = 0; iT < m_NumOfSD; ++iT)
    {
        m_TopMetalVector.push_back(new TopMetal(iT));
        m_GMCPVector.push_back(new GMCPAction(iT));
    }
}

void TopMetalContainer::ClearAllCharge()
{
    for(auto iTopMetal:m_TopMetalVector)
    {
        iTopMetal->ClearAllCharge();
    }
    for(auto iGMCP:m_GMCPVector)
    {
        iGMCP->ClearAllCharge();
    }
}