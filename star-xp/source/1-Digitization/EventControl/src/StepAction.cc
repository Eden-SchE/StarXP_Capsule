#include "TMap.h"
#include "TRandom.h"

#include "TimeControl.hh"
#include "StepAction.hh"
#include "SDBoxContainer.hh"
#include "Multiplication.hh"

#include <iostream>
#include <cmath>

StepAction::StepAction(SimDeposit aStep, Double_t EventStartTime)
    :m_Step(aStep), m_EventStartTime(EventStartTime), m_iInitElectron(0), m_SDNum(-1), TimeAction("Step", "Event")
{
    m_IonizationInfo.clear();

    TVector3 postPosition = m_Step.GetPostPosition();
    m_SDNum = SDBoxContainer::GetInstance().WhichSD(postPosition);
    SDBox* theBox = &SDBoxContainer::GetInstance()[m_SDNum];
    //原初电子个数
    m_NumOfInitElectron = gRandom->Poisson(m_Step.GetEdep()*1e6 / theBox->InitialIonizationEnergy());
    if(m_NumOfInitElectron == 0)
    {
        std::cout << "Warning!!! Number of initial ionization electron is 0, reset to 1" << std::endl;
        m_NumOfInitElectron = 1;
    }

    //原初电子产生位置
    Double_t StepLength = m_Step.GetStepLength();
    std::vector<Double_t> LengthOfEachIon;  LengthOfEachIon.clear();
    for(int iE = 0; iE < m_NumOfInitElectron; ++iE){
        LengthOfEachIon.push_back(gRandom->Uniform(StepLength));
    }
    std::sort(LengthOfEachIon.begin(), LengthOfEachIon.end(), StepAction::mySort);

    Double_t preTime = m_Step.GetPreT();
    Double_t postTime = m_Step.GetPostT();
    Double_t StepTime = postTime - preTime;

    TVector3 prePosition  = m_Step.GetPrePosition();
    TVector3 UnitStep = (postPosition - prePosition).Unit();

    if(m_SDNum == -1){
        std::cerr << "Error!!! Step ( " << postPosition[0] << ", " << postPosition[1] << ", " << postPosition[2] << " ) dosen't belone to any GasSD!!!" << std::endl;
        exit(EXIT_FAILURE);
    }

    for(auto Length:LengthOfEachIon)
    {
        IonizationInfo ionInfo;
        ionInfo.SDNum = m_SDNum;
        //原初电子初始位置
        ionInfo.InitElectronPosition = prePosition + UnitStep * Length;
        //原初电子漂移
        ionInfo.DriftDistance = theBox->DriftDistance(ionInfo.InitElectronPosition);
        //原初电子到达MCP时刻
        ionInfo.MCPTime = m_EventStartTime + preTime + StepTime * (Length / StepLength) + gRandom->Gaus((ionInfo.DriftDistance / theBox->DriftSpeed().first), (ionInfo.DriftDistance / theBox->DriftSpeed().first) * (theBox->DriftSpeedErr().first / theBox->DriftSpeed().first));//原初电子漂移速度3-4厘米每微秒 30000-40000毫米每秒
        //原初电子横向扩散
        ionInfo.InitElectronPosition[0] = gRandom->Gaus(ionInfo.InitElectronPosition[0], theBox->DriftSmear().first * sqrt(ionInfo.DriftDistance));
        ionInfo.InitElectronPosition[1] = gRandom->Gaus(ionInfo.InitElectronPosition[1], theBox->DriftSmear().first * sqrt(ionInfo.DriftDistance));
        //电子入孔效率
        if(SDBoxContainer::GetInstance().WhichSD(ionInfo.InitElectronPosition) == -1 || gRandom->Uniform(1.) > theBox->EToHoleEfficiency())
            continue;
        m_IonizationInfo.push_back(ionInfo);
        if(TimeControl::Verbose()) std::cout << "Length of IonDistance: " << Length << "\t Time of Step: " << ionInfo.MCPTime << "\tPoint ( " << ionInfo.InitElectronPosition[0] << ", " << ionInfo.InitElectronPosition[1] << ", " << ionInfo.InitElectronPosition[2] << " )" << std::endl;
    }
    std::sort(m_IonizationInfo.begin(), m_IonizationInfo.end(), StepAction::myIonSort);
    m_NumOfInitElectron = m_IonizationInfo.size();
}

StepAction::~StepAction(){}

std::pair<Double_t, TimeAction*> StepAction::Action(Double_t)
{
    //return std::pair<Double_t, TimeAction*>(-1, NULL);

    if(m_NumOfInitElectron == m_iInitElectron)
        return std::pair<Double_t, TimeAction*>(-1, NULL);

    Multiplication* multiplication = new Multiplication(m_IonizationInfo[m_iInitElectron]);
    ++m_iInitElectron;
    return std::pair<Double_t, TimeAction*>(multiplication->GetMCPTimeFlag(), multiplication);


    
}