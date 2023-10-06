#ifndef PIXELFIX_H
#define PIXELFIX_H
#include "TH2F.h"
#include "TFile.h"
#include <iostream>
#include "TTree.h"
#include "TCanvas.h"
#include "time.h"


short PixelfixReverse(int ix, int iy, short value, double deltaT, TFile* CalibFile);

#endif