#ifndef FEATURE_H
#define FEATURE_H

#include <iostream>
#include <fstream>
#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TGraph.h"
#include "TSpectrum.h"
#include "TMath.h"
#include "TSystem.h"
#include "TPostScript.h"
#include "TTimeStamp.h"
#include "TSpectrum.h"
#include "TSpectrum2.h"
#include "TH2F.h"
#include "iostream"

#include "trackHandle.h"
#include "trackStruct.h"

#include "MyEventClass.h"
using namespace std;

int Feature(TString filename, TString featurefilename, Int_t IFFindIP);

#endif