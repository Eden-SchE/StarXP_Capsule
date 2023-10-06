#include "GMCPAction.hh"
#include "TopMetalContainer.hh"
#include "SDBoxContainer.hh"
#include "TimeControl.hh"
#include <algorithm>

Double_t LineShapeFunction(Double_t* x, Double_t* par){
    if((x[0] - par[1]) < -500 ){
        return 0.;
    }
    Double_t expTerm = TMath::Exp(TMath::Power(par[3], 2) / (2 * TMath::Power(par[2], 2)) - (x[0] - par[1]) / par[2]);
    Double_t erfTerm = 1 + TMath::Erf((par[2] * (x[0] - par[1]) - TMath::Power(par[3], 2)) / (TMath::Sqrt(2) * par[2] * par[3]));
    return (par[0] / (2 * par[2]) * expTerm * erfTerm);
}

GMCPLineShape::GMCPLineShape(double Amp, double Time):
    m_Time(Time),m_Amp(Amp)
{}

GMCPLineShape::~GMCPLineShape()
{}

GMCPAction::GMCPAction(unsigned int iTopMetal): 
   m_iTopMetal(iTopMetal), TimeAction("GMCP", "")
{
    m_NoSignal = 0;
    m_GMCPset.clear();
    m_GMCPTime.clear();
    m_GMCPGraph.clear();

    YAML::Node node = SDBoxContainer::GetInstance()[iTopMetal].GMCPConfig();

    m_Tau = node["Tau"].as<double>(200); //ns
    m_Smear = node["Smear"].as<double>(20);    //ns
    m_Shift = node["Shift"].as<double>(0);    //ns
    m_Response = node["Response"].as<double>(0.01);  //
    m_TimeWindow = node["TimeWindow"].as<double>(5000);  //ns
    m_FlashTime = node["FlashTime"].as<double>(200) / 1e9;  //ns
    m_Threshold = node["Threshold"].as<double>(50);     //ADC

    m_LineShape = new TF1("GMCPLineShape", LineShapeFunction, -10, 1e4, 4);
    m_LineShape->SetParameters(1., m_Shift, m_Tau, m_Smear);
}

GMCPAction::~GMCPAction()
{
    delete m_LineShape;
} 

void GMCPAction::AddCharge(double Time, int Charge)
{
    m_GMCPset.emplace(Charge*m_Response, Time);
}

void GMCPAction::InitGMCP(double Time)
{
    if(m_GMCPClock >= 0.) return;
    m_NoSignal = 0;
    m_GMCPset.clear();
    m_GMCPTime.clear();
    m_GMCPGraph.clear();
}

void GMCPAction::ClearAllCharge()
{
    m_NoSignal = 0;
    m_GMCPClock = -1.;
    m_GMCPset.clear();
    m_GMCPTime.clear();
    m_GMCPGraph.clear();
}

double GMCPAction::Evaluate(double Time)
{
    if(m_GMCPset.empty()) return 0;
    double ampTotal = 0.;
    for(auto iter = m_GMCPset.begin(); iter != m_GMCPset.end(); ++iter)
    {
        double T = (Time - iter->Time())*1e9;
        if(T > 1e5)
        {
            m_GMCPset.erase(iter);
            continue;
        }
        if(T <= 0)
        {
            continue;
        }
        double amp = iter->Amp() * m_LineShape->Eval(T);
        //std::cout << "\tamp: " << amp << "\tT: " << T << "\tA: " << iter->Amp() << "\tEval: " << m_LineShape->Eval(T) << std::endl;
        ampTotal += amp;
    }
    return ampTotal;
}

std::pair<Double_t, TimeAction*> GMCPAction::Action(Double_t TimeFlag)
{
    if(m_GMCPClock >= m_TimeWindow)
    {
        m_GMCPClock = -1.;
        TopMetalContainer::GetInstance().GetOutputFilePtr()->WriteGMCP(m_iTopMetal, m_GMCPTime[0], *std::max_element(m_GMCPGraph.begin(), m_GMCPGraph.end()));
        m_GMCPset.clear();
        m_GMCPTime.clear();
        m_GMCPGraph.clear();
        return std::pair<Double_t, TimeAction*>(-1., NULL);
    }

    Double_t FlashTime = ceil(TimeFlag / (m_FlashTime)) * (m_FlashTime);

    if(m_GMCPClock < 0.)
    {
        m_GMCPClock = 0.;
        return std::pair<Double_t, TimeAction*>(FlashTime + m_FlashTime, NULL);
    }

    double GMCPADC = Evaluate(FlashTime);
    //std::cout << "FlastTime: " << FlashTime << "\tGMCPADC: " << GMCPADC << std::endl;
    //if(TimeControl::Verbose()) std::cout << "FlastTime: " << FlashTime << "\tGMCPADC: " << GMCPADC << std::endl;
    if(GMCPADC < m_Threshold)
    {
        if(m_NoSignal >= 1000)
        {
            if(TimeControl::Verbose()) std::cout << "GMCPAction::Action(): GMCP signal NOT FOUND after 1000 iterations. No GMCP signal for this event." << std::endl;
            m_NoSignal = 0;
            return std::pair<Double_t, TimeAction*>(-1., NULL);
        }
        ++m_NoSignal;
        return std::pair<Double_t, TimeAction*>(FlashTime + m_FlashTime, NULL);
    }

    m_GMCPTime.push_back(FlashTime);
    m_GMCPGraph.push_back(GMCPADC);

    m_GMCPClock += m_FlashTime;

    return std::pair<Double_t, TimeAction*>(FlashTime + m_FlashTime, NULL);
}