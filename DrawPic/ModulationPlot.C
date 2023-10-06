#include <TCanvas.h>
#include <TGraph.h>
#include <TSpline.h>
#include <TLine.h>

void ModulationPlot() {
        // 第一组数据
    Double_t x1[] = {3, 3.5, 4, 4.5, 5, 5.5, 6, 6.5, 7, 7.5, 8};
    Double_t y1[] = {.257, .3205, .3954, .4478, .4969, .5252, .5422, .5522, .5625, .5737, .5837};
    Int_t n1 = sizeof(x1) / sizeof(x1[0]);

    // 第二组数据
    Double_t x2[] = {2.98, 4.5, 6.4, 8.047};
    Double_t y2[] = {.2615, .4372, .5329, .5806};
    Double_t ey2[] = {0.0033, 0.0040, 0.0041, 0.0043};
    Int_t n2 = sizeof(x2) / sizeof(x2[0]);

    TGraphErrors *graph2 = new TGraphErrors(n2, x2, y2, 0, ey2);

    TGraph *graph1 = new TGraph(n1, x1, y1);
    

    TCanvas *canvas = new TCanvas("canvas", "", 800, 600);
    canvas->SetGrid();
    graph1->SetMarkerStyle(kFullCircle);
    graph1->SetMarkerColor(kBlue);
    graph1->SetMarkerSize(1);
    graph1->SetLineWidth(2);
    graph1->Draw("AP");
    

    TAxis *xAxis = graph1->GetXaxis();
    TLatex *latex = new TLatex();
    latex->SetTextAlign(22);
    latex->SetTextSize(0.1); 
    latex->SetTextSize(xAxis->GetTitleSize());
    latex->SetTextFont(xAxis->GetTitleFont());

    TAxis *yAxis = graph1->GetYaxis();
    TLatex *latey = new TLatex();
    latey->SetTextSize(0.1); 
    latey->SetTextAlign(22);
    latey->SetTextSize(yAxis->GetTitleSize());
    latey->SetTextFont(yAxis->GetTitleFont());
    latey->SetTextAngle(90); 

    TSpline5 *spline = new TSpline5("spline", graph1->GetX(), graph1->GetY(), graph1->GetN());

    graph1->Draw("P SAME");
    spline->SetLineColor(kBlue);
    spline->SetLineWidth(2);
    spline->Draw("L SAME");

    graph2->SetMarkerStyle(kOpenCircle);
    graph2->SetMarkerColor(kRed);
    graph2->SetMarkerSize(1);
    graph2->SetLineWidth(2);
    graph2->Draw("P SAME");

    TLegend *legend = new TLegend(0.7, 0.8, 0.9, 0.9);
    legend->SetBorderSize(0);
    legend->AddEntry(graph1, "Simulation", "p");
    legend->AddEntry(graph2, "Experiment", "p");
    legend->Draw();

    latex->DrawLatexNDC(0.5, 0.025, "Energy [keV]");
    latey->DrawLatexNDC(0.025, 0.5, "Modulation factor");

    canvas->Draw();
}
