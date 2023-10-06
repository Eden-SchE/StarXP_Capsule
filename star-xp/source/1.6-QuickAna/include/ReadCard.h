#ifndef _ReadCard_h
#define _ReadCard_h
#include "TString.h"
#include "G4String.hh"
#include <iostream>
#include <vector>
// #include "TVector3.h"
using namespace std;

struct QuickAnaPGPSConfig
{
    G4String InputUTCFile;
    G4String EventRootFile;
    G4String DatFile;
    G4String FeatureFile;
    G4String RawDat;
    G4String InputUTCFileFolder;
    G4String DecayFile;
    int ReadFiles;
    int GMCPCoincidence;//是否采用GMCP符合
    int HITPIXEL;
    int SumADC;//Topmetal ADC阈值
    int EventTime;
    int THREPIXELADC;
    int ThresholdGMCPADC;
    int DETECTORID ;
    int SpecFix;//是否开启能谱修正
    int IFFindIP;//是否寻找光电作用点
    int TOPMETALADCUPPER;
    int StraightADD;//直接将有记录图片拼接
    int WindowScale;//时间窗大小
};


#endif