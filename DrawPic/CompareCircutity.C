#include "TH1F.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TTree.h"

void CompareCircutity(){
    double ellipcut =0.806;
    double sumADCCut = 33000;
    double sumADCCutMin = 26000;
    gStyle->SetOptTitle(0);
    TFile *tfileEXP = new TFile("8keV_Polarized_EXP.root");
    TFile *tfileMC = new TFile("8keV_Polarized_MC.root");
    TFile *tfileSimpMC = new TFile("SimpleTrack_8keV_Feature.root");
    TCanvas* can = new TCanvas("","",800,600);
    can->SetGrid();
    // can->Divide(3,2);
    TTree *t_data = (TTree*)tfileEXP->Get("t_data");
    // Short_t data[NX][NY];    t_data->SetBranchAddress("data",&data);
    int numEvent;          t_data->SetBranchAddress("numEvent",&numEvent);
    int numFrame;          t_data->SetBranchAddress("numFrame",&numFrame);
    int timeYear;          t_data->SetBranchAddress("timeYear",&timeYear);
    int timeMon;           t_data->SetBranchAddress("timeMon",&timeMon);
    int timeMday;          t_data->SetBranchAddress("timeMday",&timeMday);
    int timeHour;          t_data->SetBranchAddress("timeHour",&timeHour);
    int timeMin;           t_data->SetBranchAddress("timeMin",&timeMin);
    int timeSec;           t_data->SetBranchAddress("timeSec",&timeSec);
    int timeNsec;          t_data->SetBranchAddress("timeNsec",&timeNsec);
    int peakADC;           t_data->SetBranchAddress("peakADC",&peakADC);
    double peakX;          t_data->SetBranchAddress("peakX",&peakX);
    double peakY;          t_data->SetBranchAddress("peakY",&peakY);
    int sumADC;            t_data->SetBranchAddress("sumADC",&sumADC);
    int sumHit;            t_data->SetBranchAddress("sumHit",&sumHit);
    double lenXY;          t_data->SetBranchAddress("lenXY",&lenXY);
    double ratioXY;        t_data->SetBranchAddress("ratioXY",&ratioXY);
    double BcenterX;       t_data->SetBranchAddress("BcenterX",&BcenterX);
    double BcenterY;       t_data->SetBranchAddress("BcenterY",&BcenterY);
    double lenBtoP;        t_data->SetBranchAddress("lenBtoP",&lenBtoP);
    int length;            t_data->SetBranchAddress("length",&length);
    double width;          t_data->SetBranchAddress("width",&width);
    int outline;           t_data->SetBranchAddress("outline",&outline);
    int edgeHit;           t_data->SetBranchAddress("edgeHit",&edgeHit);
    double ellip;          t_data->SetBranchAddress("ellip",&ellip);
    double circ;           t_data->SetBranchAddress("circ",&circ);

    // TCanvas * c1 = new TCanvas();
    // TH2F *hist = new TH2F("events","events",128,0,128,128,0,128);
    TH1F* histellipEXP = new TH1F("Eccentricity Experiment","Eccentricity Experiment",50,0.8,1);
    TH1F* histellipMC = new TH1F("Eccentricity Simulation","Eccentricity Simulation",50,0.8,1);

    TH1F* histcirEXP = new TH1F("Experiment","Experiment",50,0.3,1);
    TH1F* histcirMC = new TH1F("Simulation","Simulation",50,0.3,1);
    TH1F* histcirSimpMC = new TH1F("Simulation Simple Gaussain","Simulation Simple Gaussain",50,0.3,1);

    TH1F* histTrackEXP = new TH1F("Track Lenght Experiment","Track Lenght Experiment",25,0.,25);
    TH1F* histTrackMC = new TH1F("Track Lenght Simulation","Track Lenght Simulation",25,0.,25);

    TH1F* histSumADCEXP = new TH1F("SumADC Experiment","SumADC Experiment",50,25000.,35000);
    TH1F* histSumADCMC = new TH1F("SumADC Simulation","SunADC Simulation",50,25000.,35000);

    TH1F* histSumHitEXP = new TH1F("SumHit Experiment","SumHit Experiment",50,50.,250);
    TH1F* histSumHitMC = new TH1F("SumHit Simulation","SumHit Simulation",50,50.,250);
    // TH1F* histcir = new TH1F("Ellipcity","Ellipcity",40,0.,1.);
    Long64_t entriesEXP = t_data->GetEntries();
    // cout << "sum : " << entries << endl;
    double EXPNum =0.;
    for(Long64_t i = 0; i < entriesEXP; i ++)
    {
        t_data->GetEntry(i);
        if(sumADC>sumADCCut) continue;
        if(sumADC<sumADCCutMin) continue;
        // if(length>25) continue;
        if(sumHit>251) continue;
        histellipEXP->Fill(ellip);
        histcirEXP->Fill(circ);
        histTrackEXP->Fill(length);
        histSumADCEXP->Fill(sumADC);
        histSumHitEXP->Fill(sumHit);
        EXPNum++;
    }

    TTree *t_dataMC = (TTree*)tfileMC->Get("t_data");
    // Short_t data[NX][NY];    t_data->SetBranchAddress("data",&data);
    t_dataMC->SetBranchAddress("numEvent",&numEvent);
    t_dataMC->SetBranchAddress("numFrame",&numFrame);
    t_dataMC->SetBranchAddress("timeYear",&timeYear);
    t_dataMC->SetBranchAddress("timeMon",&timeMon);
    t_dataMC->SetBranchAddress("timeMday",&timeMday);
    t_dataMC->SetBranchAddress("timeHour",&timeHour);
    t_dataMC->SetBranchAddress("timeMin",&timeMin);
    t_dataMC->SetBranchAddress("timeSec",&timeSec);
    t_dataMC->SetBranchAddress("timeNsec",&timeNsec);
    t_dataMC->SetBranchAddress("peakADC",&peakADC);
    t_dataMC->SetBranchAddress("peakX",&peakX);
    t_dataMC->SetBranchAddress("peakY",&peakY);
    t_dataMC->SetBranchAddress("sumADC",&sumADC);
    t_dataMC->SetBranchAddress("sumHit",&sumHit);
    t_dataMC->SetBranchAddress("lenXY",&lenXY);
    t_dataMC->SetBranchAddress("ratioXY",&ratioXY);
    t_dataMC->SetBranchAddress("BcenterX",&BcenterX);
    t_dataMC->SetBranchAddress("BcenterY",&BcenterY);
    t_dataMC->SetBranchAddress("lenBtoP",&lenBtoP);
    t_dataMC->SetBranchAddress("length",&length);
    t_dataMC->SetBranchAddress("width",&width);
    t_dataMC->SetBranchAddress("outline",&outline);
    t_dataMC->SetBranchAddress("edgeHit",&edgeHit);
    t_dataMC->SetBranchAddress("ellip",&ellip);
    t_dataMC->SetBranchAddress("circ",&circ);

    Long64_t entriesMC = t_dataMC->GetEntries();
    // cout << "sum : " << entries << endl;
    short tdata;
    double MCNumber=0.; 
    for(Long64_t i = 0; i < entriesMC; i ++)
    {
        t_dataMC->GetEntry(i);
        if(ellip<ellipcut) continue;
        if(sumADC>sumADCCut) continue;
        // if(sumADC<sumADCCutMin) continue;
        histellipMC->Fill(ellip);
        if(circ>0) histcirMC->Fill(circ);
        histTrackMC->Fill(length);
        histSumADCMC->Fill(sumADC);
        histSumHitMC->Fill(sumHit);
        MCNumber++;
    }


    double MCSimpNumber=0.; 
    TTree *t_dataSimpMC = (TTree*)tfileSimpMC->Get("t_data");
    // Short_t data[NX][NY];    t_data->SetBranchAddress("data",&data);
    t_dataSimpMC->SetBranchAddress("numEvent",&numEvent);
    t_dataSimpMC->SetBranchAddress("numFrame",&numFrame);
    t_dataSimpMC->SetBranchAddress("timeYear",&timeYear);
    t_dataSimpMC->SetBranchAddress("timeMon",&timeMon);
    t_dataSimpMC->SetBranchAddress("timeMday",&timeMday);
    t_dataSimpMC->SetBranchAddress("timeHour",&timeHour);
    t_dataSimpMC->SetBranchAddress("timeMin",&timeMin);
    t_dataSimpMC->SetBranchAddress("timeSec",&timeSec);
    t_dataSimpMC->SetBranchAddress("timeNsec",&timeNsec);
    t_dataSimpMC->SetBranchAddress("peakADC",&peakADC);
    t_dataSimpMC->SetBranchAddress("peakX",&peakX);
    t_dataSimpMC->SetBranchAddress("peakY",&peakY);
    t_dataSimpMC->SetBranchAddress("sumADC",&sumADC);
    t_dataSimpMC->SetBranchAddress("sumHit",&sumHit);
    t_dataSimpMC->SetBranchAddress("lenXY",&lenXY);
    t_dataSimpMC->SetBranchAddress("ratioXY",&ratioXY);
    t_dataSimpMC->SetBranchAddress("BcenterX",&BcenterX);
    t_dataSimpMC->SetBranchAddress("BcenterY",&BcenterY);
    t_dataSimpMC->SetBranchAddress("lenBtoP",&lenBtoP);
    t_dataSimpMC->SetBranchAddress("length",&length);
    t_dataSimpMC->SetBranchAddress("width",&width);
    t_dataSimpMC->SetBranchAddress("outline",&outline);
    t_dataSimpMC->SetBranchAddress("edgeHit",&edgeHit);
    t_dataSimpMC->SetBranchAddress("ellip",&ellip);
    t_dataSimpMC->SetBranchAddress("circ",&circ);

    for(Long64_t i = 0; i < entriesMC; i ++)
    {
        t_dataSimpMC->GetEntry(i);
        if(ellip<ellipcut) continue;
        if(sumADC>sumADCCut) continue;
        histcirSimpMC->Fill(circ);
        MCSimpNumber+=1.;
    }

    histcirEXP->SetMarkerStyle(20);
    histcirEXP->SetMarkerColor(kRed);
    histcirEXP->SetMarkerSize(1);
    histcirMC->SetMarkerStyle(22);
    histcirMC->SetMarkerColor(kBlue);
    histcirMC->SetMarkerSize(1);
    histcirSimpMC->SetLineWidth(1);
    histcirSimpMC->SetLineColor(kGreen);
    

    histcirEXP->Scale(1./double(EXPNum));
    histcirMC->Scale(1./histcirMC->Integral(0, histcirMC->GetNbinsX() + 1));
    histcirSimpMC->Scale(1./double(MCSimpNumber));


    histcirSimpMC->SetFillStyle(3005);
    histcirSimpMC->SetFillColor(kGreen);
    histcirEXP->SetLineColor(kRed);
    histcirEXP->SetFillStyle(3003);
    histcirMC->SetFillStyle(3004);
    histcirEXP->SetFillColor(kRed);
    histcirMC->SetFillColor(kBlue);
    histcirEXP->GetYaxis()->SetRangeUser(0,0.052);
    histcirMC->GetYaxis()->SetRangeUser(0,0.052);
    histcirEXP->SetStats(kFALSE);
    histcirMC->SetStats(kFALSE);
    TAxis *xAxis = histcirMC->GetXaxis();
    TLatex *latex = new TLatex();
    latex->SetTextAlign(22); 
    latex->SetTextSize(0.1); 
    latex->SetTextSize(xAxis->GetTitleSize());
    latex->SetTextFont(xAxis->GetTitleFont());

    TAxis *yAxis = histcirMC->GetYaxis();
    TLatex *latey = new TLatex();
    latey->SetTextSize(0.1); 
    latey->SetTextAlign(22); 
    latey->SetTextSize(yAxis->GetTitleSize());
    latey->SetTextFont(yAxis->GetTitleFont());
    latey->SetTextAngle(90); 
    
   
    histcirEXP->Draw("HIST E0");
    histcirMC->Draw("SAME HIST");
    histcirSimpMC->Draw("SAME HIST");
    TLegend* leg = can->BuildLegend();
    leg->SetBorderSize(0);
    leg->SetTextSize(0.03);
    latex->DrawLatexNDC(0.5, 0.025, "Circularity");
    latey->DrawLatexNDC(0.025, 0.5, "Normalized entries");

    can->Update();
}
