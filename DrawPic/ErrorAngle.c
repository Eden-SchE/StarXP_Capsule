#include <TCanvas.h>
#include <TGraphErrors.h>

void ErrorAngle() {
    // ?????
    Double_t x[] = {3, 3.5, 4, 4.5, 5, 5.5, 6, 6.5, 7, 7.5, 8};
    Double_t y[] = {0.4584, -0.3438, 0.1146, 0.1745, 0.3438, -0.0573, -0.1719, -0.4011, 0.2438, 0.1146, 0.1516};
    // ???
    Double_t ex[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    Double_t ey[] = {0.8594, 0.5157, 0.4584, 0.3438, 0.3438, 0.3730, 0.3438, 0.4011, 0.3438, 0.2865, 0.2865};
    Int_t n = sizeof(x) / sizeof(x[0]);

    TGraphErrors *graph = new TGraphErrors(n, x, y, ex, ey);

    TCanvas *canvas = new TCanvas("canvas", "Data with Errors", 800, 600);
    canvas->SetGrid();
    graph->SetMarkerStyle(kFullCircle);
    graph->SetMarkerColor(kBlue);
    graph->SetMarkerSize(1.5);
    graph->SetLineWidth(2);

    TAxis *xAxis = graph->GetXaxis();
    TLatex *latex = new TLatex();
    latex->SetTextAlign(22);
    latex->SetTextSize(0.1); 
    latex->SetTextSize(xAxis->GetTitleSize());
    latex->SetTextFont(xAxis->GetTitleFont());

    TAxis *yAxis = graph->GetYaxis();
    TLatex *latey = new TLatex();
    latey->SetTextSize(0.1); 
    latey->SetTextAlign(22);
    latey->SetTextSize(yAxis->GetTitleSize());
    latey->SetTextFont(yAxis->GetTitleFont());
    latey->SetTextAngle(90); 

    graph->GetYaxis()->SetRangeUser(-6, 6);
    graph->Draw("AP");
    latex->DrawLatexNDC(0.5, 0.025, "Energy [keV]");
    latey->DrawLatexNDC(0.025, 0.5, "Deviation[degree]");

    canvas->Draw();
}
