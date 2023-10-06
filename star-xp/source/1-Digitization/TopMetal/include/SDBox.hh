#ifndef _SDBox_h_Digi_
#define _SDBox_h_Digi_

#include "Rtypes.h"
#include "TVector3.h"
#include "yaml-cpp/yaml.h"

#include <string>

class SDBox
{
public:
    SDBox();
    SDBox(TVector3, TVector3);
    ~SDBox();

    inline TVector3 GetBox() const {return m_Box;}
    inline TVector3 GetPosition() const {return m_Position;}

    inline void SetBox(TVector3 val) {m_Box = val;}
    inline void SetPosition(TVector3 val) {m_Position = val;}

    inline TVector3 GetBox() {return m_Box;}
    inline TVector3 GetPosition() {return m_Position;}
    inline TVector3 GetRelativePoint(TVector3 Point) {return (Point - m_Position);}

    bool IfPointInSD(TVector3 Point, bool Loose = false);
    Double_t DriftDistance(TVector3);
    std::pair<Double_t, Double_t> MCPHole(TVector3);
    void InitCalib(YAML::Node);

    static inline Double_t InitialIonizationEnergy() {return m_InitialIonizationEnergy;}
    static inline std::pair<Double_t, Double_t> DriftSpeed() {return m_DriftSpeed;}
    static inline std::pair<Double_t, Double_t> DriftSpeedErr() {return m_DriftSpeedErr;}
    static inline std::pair<Double_t, Double_t> DriftSmear() {return m_DriftSmear;}
    static inline Double_t MultiplicationFactor() {return m_MultiplicationFactor;}
    static inline Double_t EToHoleEfficiency() {return m_EToHoleEfficiency;}
    static inline Double_t TopMetalCollectingEfficiency() {return m_TopMetalCollectingEfficiency;}
    static inline Double_t MCPHoleDistance() {return m_MCPHoleDistance;}
    static inline Double_t PolPhotOnMCPScale() {return m_PolPhotOnMCPScale;}
    static inline Double_t PixelSize() {return m_PixelSize;}
    static inline Double_t PixelTime() {return m_PixelTime;}
    static inline std::pair<UInt_t, UInt_t> NumOfPixel() {return m_NumOfPixel;}
    static inline Double_t MCPTopMetalDistance() {return m_MCPTopMetalDistance;}
    static inline Double_t ADCResponse() {return m_ADCResponse;}
    static inline std::string TopMetalCalib() {return m_TopMetalCalib;}
    static inline YAML::Node GMCPConfig() {return m_GMCPConfig;}
    static inline YAML::Node TopMetalConstCalib() {return m_TopMetalConstCalib;}

private:
    TVector3 m_Box;
    TVector3 m_Position;

    static Double_t m_InitialIonizationEnergy;
    static std::pair<Double_t, Double_t> m_DriftSpeed;
    static std::pair<Double_t, Double_t> m_DriftSpeedErr;
    static std::pair<Double_t, Double_t> m_DriftSmear;
    static Double_t m_MultiplicationFactor;
    static Double_t m_EToHoleEfficiency;
    static Double_t m_TopMetalCollectingEfficiency;
    static Double_t m_MCPHoleDistance;
    static Double_t m_PolPhotOnMCPScale;
    static UInt_t m_NumOfSD;
    static TVector3 m_SDSize;
    static Double_t m_PixelSize;
    static Double_t m_PixelTime;
    static std::pair<UInt_t, UInt_t> m_NumOfPixel;
    static Double_t m_MCPTopMetalDistance;
    static Double_t m_ADCResponse;
    static std::string m_TopMetalCalib;

    static YAML::Node m_GMCPConfig;
    static YAML::Node m_TopMetalConstCalib;
};

#endif