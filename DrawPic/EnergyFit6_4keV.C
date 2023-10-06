#include "TH1F.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TTree.h"

Double_t Erfc(Double_t* x, Double_t* par){
    Double_t erf_amp = par[0];    // Amplitude
    Double_t erf_mean = par[1];   // Mean
    Double_t erf_sigma = par[2];  // Sigma
    Double_t erfc = erf_amp * TMath::Erfc((x[0] - erf_mean) / (sqrt(2) * erf_sigma));
    return erfc;
}


Double_t fitFunction(Double_t* x, Double_t* par)
{
    // Parameters for the Gaussian
    Double_t amp = par[0];   // Amplitude
    Double_t mean = par[1];  // Mean
    Double_t sigma = par[2]; // Sigma
    
    // Parameters for the error function
    Double_t erf_amp = par[3];    // Amplitude
    Double_t erf_mean = par[4];   // Mean
    Double_t erf_sigma = par[5];  // Sigma
    
    // Calculate the value of the fit function
    Double_t gauss = amp * TMath::Gaus(x[0], mean, sigma);
    Double_t erfc = erf_amp * TMath::Erfc((x[0] - erf_mean) / (sqrt(2) * erf_sigma));
    
    return gauss + erfc;
}


void EnergyFit6_4keV(){
    TFile* f1 = new TFile("6_4keV_MC_GMCP.root");
    TTree* tr2 = (TTree*)f1->Get("t_data");
    TCanvas* can = new TCanvas("","",800,600);
    can->SetGrid();

    TH1F* h2 = new TH1F(" Simulation","",60,300,1000);
    Int_t gmcpADC;
    double MCgmcpADC;
    tr2->SetBranchAddress("GMCPADC",&MCgmcpADC);
    Int_t Number = tr2->GetEntries();
    for(int i =0; i<Number;i++){
        tr2->GetEntry(i);
        h2->Fill((MCgmcpADC));
    }
    h2->Scale(1./double(Number));
    TF1* fitFunc1 = new TF1("fitFunc", fitFunction, 300, 1000, 6);
    fitFunc1->SetLineStyle(2);
    fitFunc1->SetParameters(8.95705e-02, 5.89895e+02, 4.79306e+01, 1.46778e-03, 4.66992e+02, 7.55472e+00);
    // TF1 *fitFunc1 = new TF1("fit","crystalball", 200, 1000); 
    // fitFunc1->SetParameters(8.95354e-02, 7.21392e+02, 5.05364e+01,-2,1.5);

    // h1->Fit("gaus");
    TFitResultPtr fitResult = h2->Fit(fitFunc1, "R");
    // std::cout<<"MC Porpotion is:"<<fitFunc1->Integral(300.1,999.9)/(700./60.)<<std::endl;
    Double_t amp_fit1 = fitFunc1->GetParameter(0);
    Double_t mean_fit1 = fitFunc1->GetParameter(1);
    Double_t sigma_fit1 = fitFunc1->GetParameter(2);
    // Double_t sigma_fit1_error = fitResult->ParError(2);
    Double_t erf_amp_fit1 = fitFunc1->GetParameter(3);
    Double_t erf_mean_fit1 = fitFunc1->GetParameter(4);
    Double_t erf_sigma_fit1 = fitFunc1->GetParameter(5);
    // Double_t resolution = 2.354*sigma_fit/(mean_fit);
    // std::cout<<"Energy resolution is:"<<resolution<<std::endl;
    TF1* ErfcMC = new TF1("ErfcMC", Erfc, 300, 1000, 3);
    ErfcMC->SetParameters(erf_amp_fit1, erf_mean_fit1, erf_sigma_fit1);
    std::cout<<"Porpotion is:"<<ErfcMC->Integral(300.1,999.9)/(700./60.)<<std::endl;

    Double_t resolution1 = 2.354*sigma_fit1/(mean_fit1);
    Double_t errorRes1 = 2.354*5.81449e-01/(mean_fit1);
    std::cout<<"Energy resolution is:"<<resolution1<<std::endl;
    std::cout<<"Energy resolution error is:"<<errorRes1<<std::endl;

    TFile* f0 = new TFile("6_4keV_GMCP_EXP.root");
    TTree* tr = (TTree*)f0->Get("tree");
    TH1F* h1 = new TH1F(" Experiment","",60,300,1000);
    tr->SetBranchAddress("gmcpADC",&gmcpADC);
    Int_t Number0 = tr->GetEntries();
    for(int i =0; i<Number0;i++){
        tr->GetEntry(i);
        h1->Fill(gmcpADC);
    }
    h1->Scale(1./double(Number0));
    TF1* fitFunc0 = new TF1("fitFunc", fitFunction, 300, 1000, 6);
    fitFunc0->SetParameters(8.95354e-02, 5.21392e+02, 5.05364e+01, 6.26083e-03, 2.20641e+02, 3.55966e+02);
    // h1->Fit("fitFunc");
    h1->Fit(fitFunc0, "R");
    Double_t amp_fit0 = fitFunc0->GetParameter(0);
    Double_t mean_fit0 = fitFunc0->GetParameter(1);
    Double_t sigma_fit0 = fitFunc0->GetParameter(2);
    Double_t erf_amp_fit0 = fitFunc0->GetParameter(3);
    Double_t erf_mean_fit0 = fitFunc0->GetParameter(4);
    Double_t erf_sigma_fit0 = fitFunc0->GetParameter(5);
    Double_t resolution0 = 2.354*sigma_fit0/(mean_fit0);
    Double_t errorRes = 2.354*2.00510e-01/(mean_fit0);

    ErfcMC->SetParameters(erf_amp_fit0, erf_mean_fit0, erf_sigma_fit0);
    std::cout<<"EXP Porpotion is:"<<ErfcMC->Integral(300.1,999.9)/(700./60.)<<std::endl;


    std::cout<<"Energy resolution is:"<<resolution0<<std::endl;
    std::cout<<"Energy resolution error is:"<<errorRes<<std::endl;
    h1->GetYaxis()->SetRangeUser(0,0.1);
    h2->GetYaxis()->SetRangeUser(0,0.1);
    h1->SetMarkerStyle(20);
    h1->SetMarkerColor(kRed);
    h1->SetMarkerSize(1);
    h2->SetMarkerStyle(22);
    h2->SetMarkerColor(kBlue);
    h2->SetMarkerSize(1);
    h1->SetLineColor(kRed);
    h1->SetFillStyle(3003);
    h2->SetFillStyle(3004);
    h1->SetFillColor(kRed);
    h2->SetFillColor(kBlue);
    h1->SetStats(kFALSE);
    h2->SetStats(kFALSE);
    TAxis *xAxis = h2->GetXaxis();
    TLatex *latex = new TLatex();
    latex->SetTextAlign(22); 
    latex->SetTextSize(0.1); 
    latex->SetTextSize(xAxis->GetTitleSize());
    latex->SetTextFont(xAxis->GetTitleFont());

    TAxis *yAxis = h2->GetYaxis();
    TLatex *latey = new TLatex();
    latey->SetTextSize(0.1); 
    latey->SetTextAlign(22); 
    latey->SetTextSize(yAxis->GetTitleSize());
    latey->SetTextFont(yAxis->GetTitleFont());
    latey->SetTextAngle(90); 
    
   

    h1->Draw("HIST E0");
    h2->Draw("SAME HIST");

    fitFunc0->SetLineColor(kRed);
    fitFunc1->SetLineColor(kBlue);
    fitFunc0->SetTitle("Experiment Gaussian Fit");
    fitFunc1->SetTitle("Simulation Gaussian Fit");
    fitFunc0->Draw("SAME");
    fitFunc1->Draw("SAME");
    TLegend* leg = can->BuildLegend();
    leg->SetBorderSize(0);
    leg->SetTextSize(0.03);
    latex->DrawLatexNDC(0.5, 0.025, "GMCP pluse height(ADC)");
    latey->DrawLatexNDC(0.025, 0.5, "Normalized entries");
    can->Update();
}