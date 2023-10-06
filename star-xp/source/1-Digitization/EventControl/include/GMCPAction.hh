#ifndef _GMCP_h_Digi_
#define _GMCP_h_Digi_

#include "TF1.h"
#include "yaml-cpp/yaml.h"
#include <set>
#include <vector>

#include "TimeAction.hh"

class GMCPLineShape
{
public:
    GMCPLineShape(double, double);
    ~GMCPLineShape();

    inline double Time() const {return m_Time;}
    inline double Amp() const {return m_Amp;}
    bool operator< (const GMCPLineShape& line) const
    {
        return m_Time < line.m_Time;
    };
    bool operator> (const GMCPLineShape& line) const
    {
        return m_Time > line.m_Time;
    };

private:
    double m_Time;
    double m_Amp;
};

class GMCPAction : public TimeAction
{
public:
    GMCPAction(unsigned int);
    ~GMCPAction();

    void AddCharge(double, int);
    void InitGMCP(double);
    std::pair<Double_t, TimeAction*> Action(Double_t);
    void ClearAllCharge();

private:
    TF1* m_LineShape;

    double Evaluate(double);
    unsigned int m_iTopMetal;
    unsigned int m_NoSignal;
    double m_Tau;
    double m_Smear;
    double m_Shift;
    double m_Response;
    double m_TimeWindow;
    double m_FlashTime;
    double m_Threshold;

    double m_GMCPAmp;
    //double m_GMCPTime;
    double m_GMCPClock;

    std::multiset<GMCPLineShape> m_GMCPset;
    std::vector<double> m_GMCPTime;
    std::vector<double> m_GMCPGraph;
};

#endif