#include "TH2F.h"
#include "TFile.h"
#include <iostream>
#include "TTree.h"
#include "TCanvas.h"
#include "time.h"

void EtchHistogram(TH2F *f0, TH2F *f1);

void ExpandHistogram(TH2F *f0, TH2F *f1);

void MPretchPand(TH2F *f2D_raw,int id);

short reverse(int ix, int iy, short value, double deltaT);

TH2F ComBin(TH2F* FirstPic, TH2F* SecondPic);