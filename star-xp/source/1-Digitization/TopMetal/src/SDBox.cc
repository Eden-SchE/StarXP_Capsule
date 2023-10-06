#include "SDBox.hh"

#include <iostream>

Double_t SDBox::m_InitialIonizationEnergy = 20.;                   //原初电离能
std::pair<Double_t, Double_t> SDBox::m_DriftSpeed(30000., 30000.); //漂移速度     (漂移区, 感应区) (毫米/秒)
std::pair<Double_t, Double_t> SDBox::m_DriftSpeedErr(3., 3.);      //漂移速度误差 (漂移区, 感应区)
std::pair<Double_t, Double_t> SDBox::m_DriftSmear(0.0001, 0.0001); //电子漂移横向扩散 (漂移区, 感应区)
Double_t SDBox::m_MultiplicationFactor = 1000.;                    //MCP倍增系数
Double_t SDBox::m_EToHoleEfficiency = 0.30;                        //电子入孔效率
Double_t SDBox::m_TopMetalCollectingEfficiency = 0.10;             //TopMetal电子收集效率
Double_t SDBox::m_MCPHoleDistance = 0.060;                         //MCP孔间距(毫米)
Double_t SDBox::m_PolPhotOnMCPScale = 0.35;                        //MCP上发生光电效应的scale
Double_t SDBox::m_PixelSize = 0.080;                               //TopMetal 像素大小, 毫米
Double_t SDBox::m_PixelTime = 640.;                                //TopMetal 像素读取间隔时间(纳秒)
std::pair<UInt_t, UInt_t> SDBox::m_NumOfPixel(72, 72);             //TopMetal 像素个数(X*Y)
Double_t SDBox::m_MCPTopMetalDistance = 3.39;                      //MCP下表面与TopMetal上表面间距(毫米)
Double_t SDBox::m_ADCResponse = 24.;                               //Pixel每接收1个e-对应的ADC上升值
std::string SDBox::m_TopMetalCalib = "";                           //TopMetal 刻度文件
YAML::Node SDBox::m_GMCPConfig = YAML::Node(YAML::NodeType::Map);                                // GMCP 波形输出控制
YAML::Node SDBox::m_TopMetalConstCalib = YAML::Node(YAML::NodeType::Map);                        // TopMetal 常量刻度

SDBox::SDBox():
    m_Box(TVector3(6., 6., 10.)), m_Position(TVector3(0., 0., 0.))
{}

SDBox::SDBox(TVector3 Box, TVector3 Position):
    m_Box(Box), m_Position(Position)
{}

SDBox::~SDBox(){}

bool SDBox::IfPointInSD(TVector3 Point, bool Loose)
{
    TVector3 RelativePoint = Point - m_Position;
    Double_t LooseDis = 1e-10;
    if(Loose) LooseDis = 1;

    if( (fabs(RelativePoint[0]) <= m_Box[0] / 2. + LooseDis) && 
        (fabs(RelativePoint[1]) <= m_Box[1] / 2. + LooseDis) && 
        (fabs(RelativePoint[2]) <= m_Box[2] / 2. + LooseDis)    )
    {
        return true;
    }

    //std::cout << "m_Box/2 (" << m_Box[0]/2 << ", " << m_Box[1]/2 << ", " << m_Box[2]/2 << " )";
    //std::cout << "\tPoint (" << Point[0] << ", " << Point[1] << ", " << Point[2] << " )";
    //std::cout << "\tRelativePoint (" << RelativePoint[0] << ", " << RelativePoint[1] << ", " << RelativePoint[2] << " )" << std::endl;
    //std::cout << fabs(RelativePoint[2]) << " <= " << m_Box[2] / 2. << " ?: " << (fabs(RelativePoint[2]) <= m_Box[2] / 2.) << std::endl;
    //std::cout << "fabs(RelativePoint[2]) " << fabs(RelativePoint[2]) << " <= m_Box[2] / 2. " << m_Box[2] / 2. << " ?: " << (fabs(RelativePoint[2]) <= m_Box[2] / 2.) << std::endl;

    return false;
}

void SDBox::InitCalib(YAML::Node yamlNode)
{
    //静态参量赋值
    YAML::Node gasCalib = YAML::LoadFile(yamlNode["GasCalib"].as<std::string>("CalibFiles/Ne_70_DME_30.yml"));
    m_InitialIonizationEnergy = gasCalib["InitialIonizationEnergy"].as<double>(20.);
    m_DriftSpeed = std::pair<Double_t, Double_t>(gasCalib["DDriftSpeed"].as<double>(2900000.), gasCalib["IDriftSpeed"].as<double>(6800000.));
    m_DriftSpeedErr = std::pair<Double_t, Double_t>(gasCalib["DDriftSpeedErr"].as<double>(1.), gasCalib["IDriftSpeedErr"].as<double>(1.));
    m_DriftSmear = std::pair<Double_t, Double_t>(gasCalib["DDriftSmear"].as<double>(0.02066865), gasCalib["IDriftSmear"].as<double>(0.02131691));

    m_MultiplicationFactor = yamlNode["GMCPMultipleFactor"].as<double>(2000.);
    m_EToHoleEfficiency = yamlNode["GMCPEToHoleEfficiency"].as<double>(0.6);
    double gMCPHoleDistance = yamlNode["GMCPHoleDistance"].as<double>(0.06);
    double gMCPHoleDiameter = yamlNode["GMCPHoleDiameter"].as<double>(0.05);
    m_PolPhotOnMCPScale = 1 - TMath::Power(gMCPHoleDiameter / 2., 2.) * TMath::Pi() / TMath::Power(gMCPHoleDistance, 2.);

    m_MCPTopMetalDistance = yamlNode["GMCPTopMetalDistance"].as<double>(3.39);
    m_PixelSize = yamlNode["PixelSize"].as<double>(0.080);
    m_PixelTime = yamlNode["PixelTime"].as<double>(640.) / 1.e9;
    m_NumOfPixel = std::pair<UInt_t, UInt_t>(yamlNode["NumOfPixelX"].as<int>(72), yamlNode["NumOfPixelY"].as<int>(72));
    m_ADCResponse = yamlNode["ADCResponse"].as<double>(0.2);
    if(yamlNode.IsScalar())
        m_TopMetalCalib = yamlNode["TopMetalCalib"].as<std::string>("");
    else
        m_TopMetalConstCalib = yamlNode["TopMetalCalib"];

    m_GMCPConfig = yamlNode["GMCP"];
}


Double_t SDBox::DriftDistance(TVector3 Point)
{
    TVector3 RelativePoint = Point - m_Position;
    return RelativePoint[2] - ( - m_Box[2] / 2. ); //漂移距离为点到Box Z平面的距离
}

std::pair<Double_t, Double_t> SDBox::MCPHole(TVector3 ElectronPosition)
{
    //if (iSD >= m_NumOfSD)
    //{
    //    std::cerr << "Error!!! Input iSD " << iSD << " larger than number of SDBox " << m_NumOfSD << " !!!" << std::endl;
    //    exit(EXIT_FAILURE);
    //}

    ElectronPosition = ElectronPosition - m_Position;
    Double_t XAxis = round( ElectronPosition[0] / m_MCPHoleDistance ) * m_MCPHoleDistance;
    Double_t YAxis = round( ElectronPosition[1] / m_MCPHoleDistance ) * m_MCPHoleDistance;

    return std::pair<Double_t, Double_t>(XAxis, YAxis);
}
