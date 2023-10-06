#include "Combine.h"

using namespace std;
int nx=72;
int ny =72;
int xmin=1;
int xmax=72;
int ymin =1;
int ymax=72;
int nEtchingMatrix =2;
int nExpandMatrix = 2;
int nEtchExpand =2;

double mydata[72][72];


void EtchHistogram(TH2F *f0, TH2F *f1)
{
    int netch = nEtchingMatrix;
    int ietch = netch / 2.;
    netch = (netch < 2) ? 2 : netch;
    netch = (netch > 5) ? 5 : netch;

    vector<vector<int>> etchData;
    etchData.resize(netch);
    for (int i = 0; i < netch; i++)
    {
        etchData[i].resize(netch);
        for (int j = 0; j < netch; j++)
            etchData[i][j] = 1;
    }

    //... etching
    for (int i = xmin; i <= xmax; i++)
        for (int j = ymin; j <= ymax; j++)
            f1->SetBinContent(i + 1, j + 1, 0);

    for (int i = xmin; i <= xmax; i++)
        for (int j = ymin; j <= ymax; j++)
        {
            int imin = (i - ietch < 0) ? 0 : i - ietch;
            int jmin = (j - ietch < 0) ? 0 : j - ietch;
            int imax = (i + ietch > xmax + 1 - ietch) ? xmax + 1 - ietch : i + ietch;
            int jmax = (j + ietch > ymax + 1 - ietch) ? ymax + 1 - ietch : j + ietch;

            int flag = 1;
            for (int ii = imin; ii <= imax; ii++)
                for (int jj = jmin; jj <= jmax; jj++)
                {
                    int m = ii - imin;
                    int n = jj - jmin;
                    if (m >= netch || n >= netch || etchData[m][n] == 0)
                        continue;
                    flag *= etchData[m][n] * f0->GetBinContent(ii + 1, jj + 1);
                }

            if (flag == 0)
                f1->SetBinContent(i + 1, j + 1, 0);
            else
                f1->SetBinContent(i + 1, j + 1, 1);
        }

    return;
}

void ExpandHistogram(TH2F *f0, TH2F *f1)
{
    int netch = nExpandMatrix;
    int ietch = netch / 2.;
    netch = (netch < 2) ? 2 : netch;
    netch = (netch > 5) ? 5 : netch;

    vector<vector<int>> etchData;
    etchData.resize(netch);
    for (int i = 0; i < netch; i++)
    {
        etchData[i].resize(netch);
        for (int j = 0; j < netch; j++)
            etchData[i][j] = 1;
    }

    //... expand
    for (int i = xmin; i <= xmax; i++)
        for (int j = ymin; j <= ymax; j++)
            f1->SetBinContent(i + 1, j + 1, 0);

    for (int i = xmin; i <= xmax; i++)
        for (int j = ymin; j <= ymax; j++)
        {
            if (f0->GetBinContent(i + 1, j + 1) == 0)
                continue;

            int imin = (i - ietch < 0) ? 0 : i - ietch;
            int jmin = (j - ietch < 0) ? 0 : j - ietch;
            int imax = (i + ietch > xmax + 1 - ietch) ? xmax + 1 - ietch : i + ietch;
            int jmax = (j + ietch > ymax + 1 - ietch) ? ymax + 1 - ietch : j + ietch;

            for (int ii = imin; ii <= imax; ii++)
                for (int jj = jmin; jj <= jmax; jj++)
                {
                    int m = ii - imin;
                    int n = jj - jmin;
                    if (m >= netch || n >= netch || etchData[m][n] == 0)
                        continue;
                    f1->SetBinContent(ii + 1, jj + 1, 1);
                }
        }
    return;
}

void MPretchPand(TH2F *f2D_raw,int id){
//----------------------
    //0.0 腐蚀+膨胀
    TH2F *f2D = new TH2F(Form("f2D_%d", id), Form("Event %d", id), nx, 1, xmax - xmin + 1, ny, 1, ymax - ymin + 1);
    TH2F *ftmp0 = new TH2F("ftmp0", "ftmp", nx, 1, xmax - xmin + 1, ny, 1, ymax - ymin + 1);
    TH2F *ftmp1 = new TH2F("ftmp1", "ftmp", nx, 1, xmax - xmin + 1, ny, 1, ymax - ymin + 1);

    if (nEtchExpand > 0)
    {
        EtchHistogram(f2D_raw, ftmp0);
        ExpandHistogram(ftmp0, ftmp1);
        // cout << "nEtchExpand: "<< nEtchExpand << endl;

        for (int i = 1; i < nEtchExpand; i++)
        {
            ExpandHistogram(ftmp1, ftmp0);
            EtchHistogram(ftmp0, ftmp1);
			ExpandHistogram(ftmp0, ftmp1);
        }
    }
    //----------------------
    //0.1 保存到coords，用于拟合
    int clusterSize = 0;
    int pulseHeight = 0;

    for (int i = xmin; i <= xmax; i++)
        for (int j = ymin; j <= ymax; j++)
            if (ftmp1->GetBinContent(i + 1, j + 1) > 0)
            {
                clusterSize++;
                pulseHeight += f2D_raw->GetBinContent(i + 1, j + 1);
                f2D->SetBinContent(i + 1, j + 1, f2D_raw->GetBinContent(i + 1, j + 1));
            }
            else{
                f2D->SetBinContent(i + 1, j + 1, 0);
            }

    delete ftmp0;
    delete ftmp1;

    //... generate info

    // No signal contained, treated as a pedstral


    // Signal found, need to do a further analysis
    for(int i =0; i<72;i++){
        for(int j =0; j<72; j++){
            mydata[i][j] = f2D->GetBinContent(i+1,j+1);
        }
    }
    delete f2D;
    return;
}


short reverse(int ix, int iy, short value, double deltaT){
    TFile* CalibFile = new TFile("./CalibFiles/decay.root");
    TH2F* pix_fit_a = (TH2F*)CalibFile->Get("pix_fit_a");
    TH2F* pix_fit_b = (TH2F*)CalibFile->Get("pix_fit_b");
    TH2F* pix_fit_c = (TH2F*)CalibFile->Get("pix_fit_c");
    double cacu =0;
    double a = pix_fit_a->GetBinContent(ix,iy);
    double b = pix_fit_b->GetBinContent(ix,iy);
    double c = pix_fit_c->GetBinContent(ix,iy);
    double t_0 = double((-c*log(double(value)/a))/(b*log(double(value)/a)+1.)-deltaT);
    cacu = a*exp((-t_0)/(b*t_0 +c));
    delete CalibFile;
    // printf("value is %d, t0 is %lf, cacu is %lf\n",value, t_0, cacu);
    return short(cacu+0.5);
}

TH2F ComBin(TH2F* FirstPic, TH2F* SecondPic){
        // FirstPic->Draw();
    TH2F* ready_one = new TH2F("", "", nx, 1, xmax - xmin + 1, ny, 1, ymax - ymin + 1);
    // MPretchPand(FirstPic,0);
    for(int m =0; m<72;m++){
        for(int n=0; n<72;n++){
            // ready_one->SetBinContent(m+1,n+1,mydata[m][n]);
            ready_one->SetBinContent(m+1,n+1,FirstPic->GetBinContent(m+1,n+1));
        }
    }
    TH2F* ready_two = new TH2F("", "", nx, 1, xmax - xmin + 1, ny, 1, ymax - ymin + 1);
    // MPretchPand(SecondPic,0);
    for(int m =0; m<72;m++){
        for(int n=0; n<72;n++){
            // ready_two->SetBinContent(m+1,n+1,mydata[m][n]);
            ready_two->SetBinContent(m+1,n+1,SecondPic->GetBinContent(m+1,n+1));
        }
    }

    // ready_one->Draw();
    //TH2F aCopy = TH2F(ready_one);
    TH2F *fixed_pic = new TH2F("","", nx, 1, xmax - xmin + 1, ny, 1, ymax - ymin + 1);
    for(int ix =1; ix<=xmax; ix++){
        for(int iy = 1; iy<=ymax; iy++){
            if((ready_one->GetBinContent(ix,iy)-ready_two->GetBinContent(ix,iy)<-4)){
                short backward_Pic = reverse(ix,iy,short(ready_two->GetBinContent(ix,iy)),2.592);
                fixed_pic->SetBinContent(ix,iy, double(backward_Pic));
            }
            else{
                fixed_pic->SetBinContent(ix,iy, ready_one->GetBinContent(ix,iy));
            }
        }
    }
    // ready_one->Draw("BOX");
    TH2F fixedTH2F = TH2F("","", nx, 1, xmax - xmin + 1, ny, 1, ymax - ymin + 1);
    for(int ii = 1; ii<73;ii++){
        for(int jj = 1; jj<73;jj++){
            fixedTH2F.SetBinContent(ii,jj,fixed_pic->GetBinContent(ii,jj));
        }
    }
    delete ready_one;
    delete ready_two;
    delete fixed_pic;
    return fixedTH2F;
}
