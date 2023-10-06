#ifndef _TimeControl_h_Digi_
#define _TimeControl_h_Digi_

#include "TH2F.h"

#include "SimEvent.h"

#include "TimeLine.hh"
#include "InputFileReader.hh"

struct DigitizationConfig
{
    Int_t Verbose;
    bool SingleParticleMode;
    bool TimeLineMode;
    std::vector<std::string> InputFiles;
    std::vector<std::string> TopMetalCalibFiles;
    std::vector<double> InputFilesTimeShift;
    std::string OutputFileName;
    UInt_t OutputTotalTruth;
    UInt_t OutputVerbose;
    bool SaveAllFrame;
    UInt_t RandomSeed;
    TString FilterType;
    TString FilterConfig;

    //原初电子参数
    std::pair<Double_t, Double_t> DriftSpeed;
    std::pair<Double_t, Double_t> DriftSpeedErr;
    std::pair<Double_t, Double_t> DriftSmear;
    Double_t InitialIonizationEnergy;
    //MCP参数
    Double_t MultiplicationFactor;
    Double_t EToHoleEfficiency;
    Double_t MCPHoleDistance;
    Double_t MCPHoleDiameter;
    //TopMetal参数
    Double_t PixelSize;
    Double_t PixelTime;
    std::pair<UInt_t, UInt_t> NumOfPixel;
    Double_t MCPTopMetalDistance;
    Double_t TopMetalCollectingEfficiency;
    Double_t ADCResponse;
    Int_t NumOfFramePerEvent;
    Double_t ExitTime;
};

#include "TopMetalContainer.hh"

#include <vector>
#include <map>
#include <string>

class TopMetalContainer;

class TimeControl
{
public:
    static TimeControl& GetInstance()
    {
        static TimeControl Instance;
        return Instance;
    }
    static inline Int_t Verbose() {return m_Verbose;}
    static inline bool GMCPOutput() {return m_GMCPOutput;}
    static inline bool DropAllMCPPol() {return m_DropAllMCPPol;}
    static inline bool RandomTime() {return m_RandomTime;}
    TimeControl();
    ~TimeControl();

    //inline void SetConfig(DigitizationConfig Config) 
    //{
    //    m_ConfigDigi = Config;
    //    m_Verbose = Config.Verbose;
    //    m_OutputTotalTruth = Config.OutputTotalTruth;
    //    m_OutputVerbose = Config.OutputVerbose;
    //}

    //DigitizationConfig* GetConfig() {return &m_ConfigDigi;}

    void Process();
    void SingleParticleMode();
    void TimeLineMode();

    TString ReadTruthInfo(Int_t, TString);

private:
    static Int_t m_Verbose;
    static bool m_GMCPOutput;
    static bool m_DropAllMCPPol;
    static bool m_RandomTime;

    bool m_SaveAllFrame;
    double m_ExitTime;
    double m_ExitEvents;

    TimeLine* m_TimeLine;
    TopMetalContainer* m_TopMetalContainer;
    //DigitizationConfig m_ConfigDigi;

    std::vector<std::string> m_InputFiles;
    std::vector<double> m_InputFilesTimeShift;

    //std::vector<InputFileReader*> m_InputFileContainer;
    std::vector<TString> m_ActionTypes;
    std::map<TString, std::vector<TimeAction*>*> m_ActionPtrContainer;

};

#endif