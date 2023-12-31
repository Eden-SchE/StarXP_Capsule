
#include "MyEventClass.h"
// #include "MyRootClass.h"

const double THRESHOLD = 0;

using namespace std;

//______________________________________________________________________________
// 用阶矩方法重建时所调用的拟合参数及函数
//----
// chisquare function
//
vector<pair<double, double>> coords;
vector<double> values; //Q
vector<double> errors; //dQ
double Qmax;

double meanBx = 0, meanBy = 0, qtot = 0;    //bary
double mCx = 0, mCy = 0, qtot2 = 0; //impact
//double qtot = 0, qtot2 = 0;
double mom3rd, qDistinguish;
double lPk, lPb;
double k2, b2;

ofstream outinfo;

void FcnToFitBaryLine(Int_t & /*nPar*/, Double_t * /*grad*/, Double_t &fval, Double_t *p, Int_t /*iflag */)
{
    int n = coords.size();
    double chi2 = 0;
    double x, y;
    double dist;

    double k = p[0];
    double b = meanBy - k * meanBx;
   // cout<<"hei "<<meanBy<<" "<<meanBx<<endl;
    for (int i = 0; i < n; ++i)
    {
        if (values[i] == 0)
            continue;
        if (p[0] == 0 && p[1] == 0)
            continue;

        x = coords[i].first;
        y = coords[i].second;
        dist = fabs(k * x + b - y) / sqrt(1 + k * k);
        chi2 += values[i] * dist * dist;
    }
    fval = (chi2 == 0) ? 1E19 : chi2;
}


void FcnToFitEjectLine(Int_t & /*nPar*/, Double_t * /*grad*/, Double_t &fval, Double_t *p, Int_t /*iflag */)
{
    int n = coords.size();
    double chi2 = 0;
    double x, y;
    double dist1;
    double dist2;

    double k = p[0];
    double b = mCy - k * mCx;
	//cout<<"dfdf "<<mCy<<" "<<mCx<<endl;

    for (int i = 0; i < n; ++i)
    {
        if (values[i] == 0)
            continue;

        x = coords[i].first;
        y = coords[i].second;
        double dNeed = (lPk == 0) ? (x - meanBx) : (y - b2 - k2 * x) / sqrt(1 + k2 * k2); //去除一半的操作
        if (dNeed * mom3rd < 0)
            continue;

        dist1 = fabs(k * x + b - y) / sqrt(1 + k * k);
        dist2 = sqrt((x - mCx) * (x - mCx) + (y - mCy) * (y - mCy));
        if (dist2 != 0)
            chi2 += values[i] * dist1 * dist1;
    }
    fval = (chi2 == 0) ? 1E19 : chi2;
}

//______________________________________________________________________________
// 用Bessel曲线重建时所调用的函数， 三阶Bessel曲线
// 图像解释：https://www.cnblogs.com/jay-dong/archive/2012/09/26/2704188.html

double FcnOfBesselLine(Double_t t, vector<pair<double, double>> plist, int XY)
{
    if (t < 0 || t > 1 || (int)plist.size() != 4)
        return 0;

    double p0 = (XY == 0) ? plist[0].first : plist[0].second;
    double p1 = (XY == 0) ? plist[1].first : plist[1].second;
    double p2 = (XY == 0) ? plist[2].first : plist[2].second;
    double p3 = (XY == 0) ? plist[3].first : plist[3].second;

    return p0 * pow(1 - t, 3) + 3 * p1 * t * pow(1 - t, 2) + 3 * p2 * t * t * (1 - t) + p3 * pow(t, 3);
}

double BX(Double_t t, vector<pair<double, double>> plist) { return FcnOfBesselLine(t, plist, 0); }
double BY(Double_t t, vector<pair<double, double>> plist) { return FcnOfBesselLine(t, plist, 1); }

//画bessel曲线
void DrawBesselLine(vector<pair<double, double>> plist, double ts = 0.0001, const char *opt = "")
{
    for (double t = 0; t < 1 - ts; t += ts)
    {
        TLine *l = new TLine(BX(t, plist), BY(t, plist), BX(t + ts, plist), BY(t + ts, plist));
        l->SetLineColor(kRed);
        l->Draw(opt);
    }
}

void FcnToFitBaryLineByBesselLine(Int_t & /*nPar*/, Double_t * /*grad*/, Double_t &fval, Double_t *p, Int_t /*iflag */)
{
    int n = coords.size();
    double chi2 = 0;
    double x, y;

    //(p[0], p[1]) & (p[6], p[7])必须在cluster内部

    //参数初始化
    vector<pair<double, double>> ptlist;
    ptlist.push_back(make_pair(p[0], p[1]));
    ptlist.push_back(make_pair(p[2], p[3]));
    ptlist.push_back(make_pair(p[4], p[5]));
    ptlist.push_back(make_pair(p[6], p[7]));
    double ts = p[8]; //bessel的扫描精度，请固定住不要变化

    vector<double> bxlist;
    vector<double> bylist;
    for (double t = 0; t < 1 - ts; t += ts)
    {
        double bx = BX(t, ptlist);
        double by = BY(t, ptlist);
        bxlist.push_back(bx);
        bylist.push_back(by);
    }

    double length = 0;
    for (int i = 1; i < (int)bxlist.size(); i++)
        length += sqrt(pow(bxlist[i] - bxlist[i - 1], 2) + pow(bylist[i] - bylist[i - 1], 2));
    chi2 += pow(length, 3 / 2);

    for (int i = 0; i < n; ++i)
    {
        if (values[i] == 0)
            continue;

        x = coords[i].first;
        y = coords[i].second;
        double distMin = 9999;

        for (int j = 0; j < (int)bxlist.size(); j++)
        {
            double bx = bxlist[j];
            double by = bylist[j];
            double dtmp = sqrt((x - bx) * (x - bx) + (y - by) * (y - by));
            distMin = (dtmp < distMin) ? dtmp : distMin;
        }

        chi2 += values[i] * distMin * distMin / Qmax;
    }

    fval = (chi2 == 0) ? 1E19 : chi2;
}

//______________________________________________________________________________
//
// 用三维Bessel曲线重建时所调用的函数， 三阶Bessel曲线
double FcnOfBesselLine2(Double_t t, vector<vector<double>> plist, int XYZ)
{
    if (t < 0 || t > 1 || (int)plist.size() != 4 || XYZ < 0 || XYZ > 2)
        return 0;

    double p0 = plist[0][XYZ];
    double p1 = plist[1][XYZ];
    double p2 = plist[2][XYZ];
    double p3 = plist[3][XYZ];

    return p0 * pow(1 - t, 3) + 3 * p1 * t * pow(1 - t, 2) + 3 * p2 * t * t * (1 - t) + p3 * pow(t, 3);
}

double BX2(Double_t t, vector<vector<double>> plist) { return FcnOfBesselLine2(t, plist, 0); }
double BY2(Double_t t, vector<vector<double>> plist) { return FcnOfBesselLine2(t, plist, 1); }
double BZ2(Double_t t, vector<vector<double>> plist) { return FcnOfBesselLine2(t, plist, 2); }

void FcnToFitBaryLineByBesselLine2(Int_t & /*nPar*/, Double_t * /*grad*/, Double_t &fval, Double_t *p, Int_t /*iflag */)
{
    int n = coords.size();
    double chi2 = 0;
    double x, y;

    //(p[0], p[1]) & (p[9], p[10])必须在cluster内部

    //参数初始化
    vector<vector<double>> plist;
    plist.resize(4);
    for (int i = 0; i < 4; i++)
        plist[i].resize(3);
    plist[0][0] = p[0];    
    plist[0][1] = p[1];
    plist[0][2] = p[2];
    plist[1][0] = p[3];
    plist[1][1] = p[4];
    plist[1][2] = p[5];
    plist[2][0] = p[6];
    plist[2][1] = p[7];
    plist[2][2] = p[8];
    plist[3][0] = p[9];
    plist[3][1] = p[10];
    plist[3][2] = p[11];

    double ts = p[12]; //bessel的扫描精度，请固定住不要变化
    double sm = p[13]; //探测器展宽smearing
    double n0 = p[14]; //整体系数N0

    vector<double> bxlist;
    vector<double> bylist;
    vector<double> bzlist;
    for (double t = 0; t < 1 - ts; t += ts)
    {
        double bx = BX2(t, plist);
        double by = BY2(t, plist);
        double bz = BZ2(t, plist);
        bxlist.push_back(bx);
        bylist.push_back(by);
        bzlist.push_back(bz);
    }

    double length = 0;
    for (int i = 1; i < (int)bxlist.size(); i++)
        length += sqrt(pow(bxlist[i] - bxlist[i - 1], 2) + pow(bylist[i] - bylist[i - 1], 2));
    chi2 += pow(length, 3 / 2);

    for (int i = 0; i < n; ++i)
    {
        if (values[i] == 0)
            continue;

        x = coords[i].first;
        y = coords[i].second;
        double distMin = 9999;

        for (int j = 0; j < (int)bxlist.size(); j++)
        {
            double bx = bxlist[j];
            double by = bylist[j];
            double dtmp = sqrt((x - bx) * (x - bx) + (y - by) * (y - by));
            distMin = (dtmp < distMin) ? dtmp : distMin;
        }

        chi2 += values[i] * distMin * distMin / Qmax;
    }

    fval = (chi2 == 0) ? 1E19 : chi2; 
}

//______________________________________________________________________________
// 构造/析构函数
MyEventClass::MyEventClass(int _id, int _xmin, int _xmax, int _ymin, int _ymax)
{
    dataQFlag = QF_PED;
    id = _id;
    xmin = _xmin;
    xmax = _xmax;
    ymin = _ymin;
    ymax = _ymax;
    nx = xmax - xmin + 1;
    ny = ymax - ymin + 1;
    f2D = NULL;
    f2D_raw = NULL;
    info = new TString();
    info->Append(Form("Event Number:      \t%d\n", id));
	llm =0;

    //method 1
    nEtchingMatrix = 3;
    nExpandMatrix = 3;
    nEtchExpand = 2;
    ByMethod = 1;

    nFattyX = 6;
    nFattyY = 6;
    nEllipticity = 1.5;
    rMinScale = 1.;
    rMaxScale = 1.;
	xn = 1.5;
	iteranum = 20;
	shortHead = 2;

    mBx = mBy = lPk = lPb = defVal;
    mCx = mCy = lCk = lCb = defVal;
    aTheta1 = aTheta2 = defVal;

    mBcenter = NULL;
    mCovPoint = NULL;
    lPrinAxis1 = NULL;
    lPrinAxis2 = NULL;
	lP0p1 = NULL;
	lP1p2 = NULL;
	lP2p3 = NULL;
    e1 = NULL;
    e2 = NULL;
    lCovAxis = NULL;
	for (int i=0 ; i <20; i++)
		lPrinAxisN[i] =NULL;
	lPrinAxisN[20] = NULL;

    //method 2
    HitDist = 1.;
    MinHitsAsCluster = 20;

    data.resize((nx));
    for (int i = 0; i < nx; i++)
        data[i].resize(ny);
}

MyEventClass::~MyEventClass()
{
    // Destructor.
    if (f2D != NULL)
        delete f2D;
    if (f2D_raw != NULL)
        delete f2D_raw;
    if (mBcenter != NULL)
        delete mBcenter;
    if (mCovPoint != NULL)
        delete mCovPoint;
    if (lPrinAxis1 != NULL)
        delete lPrinAxis1;
    if (lPrinAxis2 != NULL)
        delete lPrinAxis2;
	if (lP0p1 !=NULL)
		delete lP0p1;
	if (lP1p2 !=NULL)
		delete lP1p2;
	if (lP2p3 !=NULL)
		delete lP2p3;
    if (e1 != NULL)
        delete e1;
    if (e2 != NULL)
        delete e2;
    if (lCovAxis != NULL)
        delete lCovAxis;
    if (info != NULL)
        delete info;
	for (int i=0 ; i <iteranum; i++){
		if (lPrinAxisN[i] != NULL)
			delete lPrinAxisN[i];
		}

    data.clear();
	coords.clear();
	//coords.swap(vector<pair<double, double>>());
    values.clear();
	//values.swap(vector<double>());
    errors.clear();
}

//______________________________________________________________________________
// 腐蚀/膨胀处理
void MyEventClass::EtchHistogram(TH2F *f0, TH2F *f1)
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

void MyEventClass::ExpandHistogram(TH2F *f0, TH2F *f1)
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

//______________________________________________________________________________
// 0. 通用的处理函数
// void MyEventClass::GenerateHist(TH2F *hPed, bool anaflag)
// {
//     if (f2D != NULL)
//     {
//         delete f2D;
//         delete f2D_raw;
//     }

//     if (gDirectory->Get(Form("f2D_%d", id)) != NULL)
//     {
//         TH2F *f1 = (TH2F *)gDirectory->Get(Form("f2D_%d", id));
//         TH2F *f2 = (TH2F *)gDirectory->Get(Form("f2D_%d_0", id));
//         delete f1;
//         delete f2;
//     }

//     f2D = new TH2F(Form("f2D_%d", id), Form("Event %d", id), nx, 1, xmax - xmin + 1, ny, 1, ymax - ymin + 1);
//     f2D_raw = new TH2F(Form("f2D_%d_0", id), Form("Event %d", id), nx, 1, xmax - xmin + 1, ny, 1, ymax - ymin + 1);
//     f2D->GetXaxis()->SetTitle("X Pixel(every pixel=0.08mm)");
//     f2D->GetYaxis()->SetTitle("Y pixel(every pixel=0.08mm)");

//     //... store to data

// 	coords.clear();
//     values.clear();
//     errors.clear();

//     for (int i = xmin; i <= xmax; i++)
//         for (int j = ymin; j <= ymax; j++)
//         {
//             double ped = THRESHOLD;
//             if (hPed != NULL)
//             {
//                 ped = hPed->GetBinContent(i + 1, j + 1);
// 				//cout<<"here!!!! "<<endl;
//             }
// 			//cout<<"ok!!!! "<<ped<<endl;
//             double q = data[i - xmin][j - ymin] - ped;
//             q = (q < 0) ? 0 : q;
//             f2D_raw->Fill(i + 1, j + 1, q);
//         }

//     if (anaflag)
//     {
//         if (ByMethod == 2){
// 			AnalysisHist2();
// 			PductHist();}
            
//         else
//             AnalysisHist1(&mBx,&mBy);
//     }
//     return;
// }

//______________________________________________________________________________
// 1. 算法1
//    采用阶矩来进行重建的算法
void MyEventClass::PretchPand(){
//----------------------
    //0.0 腐蚀+膨胀
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
    clusterSize = 0;
    pulseHeight = 0;

    for (int i = xmin; i <= xmax; i++)
        for (int j = ymin; j <= ymax; j++)
            if (ftmp1->GetBinContent(i + 1, j + 1) > 0)
            {
                clusterSize++;
                pulseHeight += f2D_raw->GetBinContent(i + 1, j + 1);
                f2D->SetBinContent(i + 1, j + 1, f2D_raw->GetBinContent(i + 1, j + 1));

                //.. stored for fit..
                coords.push_back(make_pair(i + 1, j + 1));
                values.push_back(f2D_raw->GetBinContent(i + 1, j + 1));
                errors.push_back(sqrt(f2D_raw->GetBinContent(i + 1, j + 1)));
            }

    delete ftmp0;
    delete ftmp1;

    //... generate info
    delete info;
    info = new TString();
    info->Append(Form("Event Number:      \t%d\n", id));
    info->Append(Form("Cluster Size:      \t%d\n", clusterSize));
    info->Append(Form("Pulse Height:      \t%.2f\n", pulseHeight));

    // No signal contained, treated as a pedstral
    if (clusterSize < 20 || pulseHeight < 200)
    {
        dataQFlag = QF_PED;
        f2D->SetTitle(Form("Event %d (identified as Ped)", id));
        return;
    }

    // Signal found, need to do a further analysis
    dataQFlag = QF_HIT;
}
void MyEventClass::AnalysisHist1(double *IPx, double *IPy)
{
    // PretchPand();
	// AnalysisHist2();
	// if (/*dataQFlag != QF_HIT*/0)
	// 	return;
    // //----------------------
    // //0.3 init
	// {coords.clear();
    // values.clear();
    // errors.clear();

	// int InN = clist[0].size();
    // cout<<"InN is: "<<InN<<endl;
    // for (int i = 0; i < InN; i++)
    // {
    //     coords.push_back(clist[0][i]);
    //     values.push_back(f2D_raw->GetBinContent(clist[0][i].first + 1, clist[0][i].second + 1));
    //     errors.push_back(sqrt(f2D_raw->GetBinContent(clist[0][i].first + 1, clist[0][i].second + 1)));
    // }} //适配新算法复制而来的内容

    vector<double> coordsX;
    vector<double> coordsY;
    vector<double> coordsQ;
    for(int ix = 0; ix<72;ix++){
        for(int iy =0; iy<72;iy++){
            if(data[ix][iy]>0){
                coordsX.push_back(double(ix));
                coordsY.push_back(double(iy));
                coordsQ.push_back(double(data[ix][iy]));
            }
        }
    }

    int N = coordsX.size();

    double arglist[100];
    double chi2, edm, errdef;
    int nvpar, nparx;
	int xEdge =0;
	int yEdge =0;
    //----------------------
    //1. find barycenter
    {
        mBx = 0;
        mBy = 0;
        qtot = 0;

        for (int i = 0; i < N; i++)
        {
            double x = coordsX[i];
            double y = coordsY[i];
            double q = coordsQ[i];

            //cout<<i<<": "<<x<<", "<<y<<"  - "<<q<<endl;

            qtot += q;
            mBx += q * x;
            mBy += q * y;

			if (y > 5 && y < 67) //边缘判断
				xEdge++;
			if (x > 4 && x < 68)
				yEdge++;
        }

        mBx /= qtot;
        mBy /= qtot;

		meanBx = mBx;
		meanBy = mBy;
        //1.1 draw result
        // mBcenter = new TMarker(mBx, mBy, 30);
        // mBcenter->SetMarkerColor(kRed);
        // mBcenter->SetMarkerSize(2.6);
        // mBcenter->Draw();
    }
		// if (xEdge < 10  || yEdge < 10){
		// 	dataQFlag = QF_FAT;
		// 	llm++;
		// 	f2D->SetTitle(Form("Event %d (identified as edeg events)", id));
		// 	return;
		// }
    //----------------------
    //2. fit barycenter-line
    {
        TVirtualFitter::SetDefaultFitter("Minuit");
        TVirtualFitter *minuit = TVirtualFitter::Fitter(0, 10);
        minuit->SetParameter(0, "k", 0, 0.01, 0, 0); //0.01
        // minuit->SetFCN(FcnToFitBaryLine);

        arglist[0] = 0;
        minuit->ExecuteCommand("SET NOWarnings", arglist, 1);

        arglist[0] = -1;
        minuit->ExecuteCommand("SET PRINT", arglist, 1);

        arglist[0] = 5000;  // number of function calls
        arglist[1] = 0.001; // tolerance
        minuit->ExecuteCommand("MIGRAD", arglist, 2);

        //2.1 get result
        minuit->GetStats(chi2, edm, errdef, nvpar, nparx);

        lPk = minuit->GetParameter(0);
        lPb = mBy - lPk * mBx;
        //if (abs(lPk) < 0.00001)
         //   lPk = 0;

        //2.2 draw result
        double lx1 = (lPk == 0) ? xmin : (ymin - lPb) / lPk;
        double ly1 = (lPk == 0) ? mBy : ymin;
        double lx2 = (lPk == 0) ? xmax : (ymax - lPb) / lPk;
        double ly2 = (lPk == 0) ? mBy : ymax;
        aTheta1 = atan(lPk);

        // lPrinAxis1 = new TLine(lx1, ly1, lx2, ly2);
        // lPrinAxis1->SetLineColor(kBlack);
        // lPrinAxis1->Draw();
    }

    //----------------------
    //2.5 cut the fatty events
    {
        double minx = 99, miny = 99;
        double maxx = -1, maxy = -1;

        for (int i = 0; i < N; i++)
        {
            double x = coordsX[i];
            double y = coordsY[i];

            minx = (minx > x) ? x : minx;
            miny = (miny > y) ? y : miny;
            maxx = (maxx < x) ? x : maxx;
            maxy = (maxy < y) ? y : maxy;
        }

        // if (maxx - minx < nFattyX && maxy - miny < nFattyY)
        // {
        //     if ((maxx - minx) / (maxy - miny) < nEllipticity && (maxx - minx) / (maxy - miny) > 1 / nEllipticity)
        //     { //判断fat事例的圆形程度
        //         dataQFlag = QF_FAT;
		// 		llm++;
        //         f2D->SetTitle(Form("Event %d (identified as Fatty events)", id));

        //         /*mCovPoint = new TMarker(mBx, mBy, 31);
        //         mCovPoint->SetMarkerColor(kRed);
        //         mCovPoint->SetMarkerSize(2.6);
        //         mCovPoint->Draw();

        //         lCovAxis = new TLine(lPrinAxis1->GetX1(), lPrinAxis1->GetY1(), lPrinAxis1->GetX2(), lPrinAxis1->GetY2());
        //         lCovAxis->SetLineColor(kRed);
        //         lCovAxis->Draw();*/
        //         return;
        //     }
        // }
    }
	//cout<<"======== "<<aTheta2<<endl;
    //----------------------
    //3. cal mom3rd
    {
        k2 = (lPk == 0) ? 1 : -1 / lPk;
        b2 = (lPk == 0) ? 1 : mBy - k2 * mBx;

        //3.1 find suitable point for mom3rd
        mom3rd = 0;
        for (int i = 0; i < N; i++)
        {
            double x = coordsX[i];
            double y = coordsY[i];
            double q = coordsQ[i];
            double d = (lPk == 0) ? (x - mBx) : (y - k2 * x - b2) / sqrt(1 + k2 * k2);
            double d3 = d * d * d * q;
            mom3rd += d3;
        }
    }

    //----------------------
    //4. calculate 2nd mom max and define rmin
    double theta2 = (lPk == 0) ? TMath::Pi() / 2 : atan(-1 / lPk);

    double mom2ndMid0 = 100;
    double mom2ndMid1 = 0;
    double mom2ndMid2 = 0;
    //double shapeRatio = 0;
    double rmin = 0;
    double rmax = 0;

    int iterations = 0;
	int babo = -1;

    double chooseIteration[8];
    double intermediateResult = 0;
	double interclusterResult =0;
    double caluDecent = -1;
    double calumom2nd;
    double stepB2 = 0;
	double qSum = 0;
	double lx11, lx21, ly11, ly21;
	int cutDir =0;
	//xn = 1.5;  //monte carlo 1.5

	/*for (int i = 0; i < N; i++)
        {
            double x = coords[i].first;
            double y = coords[i].second;
            double q = values[i];

            double x0 = (lPk * lPk * mBx + lPk * (y - mBy) + x) / (lPk * lPk + 1); //到主轴的mom2nd
            double y0 = lPk * (x0 - mBx) + mBy;

            double d = (lPk == 0) ? (x - mBx) : (y - b2 - k2 * x) / sqrt(1 + k2 * k2);
            if (d * mom3rd < 0)
                continue;

            mom2ndMid0 = (pow(x - x0, 2) + pow(y - y0, 2));
            mom2ndMid1 = q * (pow(mBx - x0, 2) + pow(mBy - y0, 2));
            mom2ndMid2 = q * mom2ndMid0;

            mom2nd += mom2ndMid1 + mom2ndMid2;
            mom2ndMax += mom2ndMid1;
            mom2ndMin += mom2ndMid2;
            pureMom2nd += mom2ndMid2;

            rmin += (mom2ndMid2);
        }*/

    do //迭代找到最佳值
    {
        mom2nd = 0;
        mom2ndMax = 0;
        mom2ndMin = 0;
        rmin = 0;
        pureMom2nd = 0;
		double mCutx = -(b2-lPb)/(k2-lPk);
		double mCuty = mCutx*lPk+lPb;
		//cout<<"xxxYYY!! "<<mCutx<<" "<<mCuty<<endl;
        for (int i = 0; i < N; i++)
        {
            double x = coordsX[i];
            double y = coordsY[i];
            double q = coordsQ[i];

            double x0 = (lPk * lPk * mBx + lPk * (y - mBy) + x) / (lPk * lPk + 1); //到主轴的坐标
            double y0 = lPk * (x0 - mBx) + mBy;

            double d = (lPk == 0) ? (x - mBx) : (y - b2 - k2 * x) / sqrt(1 + k2 * k2);
            if (d * mom3rd < 0)  //初始一半的判断
                continue;

            mom2ndMid0 = (pow(x - x0, 2) + pow(y - y0, 2));//去除q的影响 单纯从径迹看
            //mom2ndMid1 = q * (pow(mBx - x0, 2) + pow(mBy - y0, 2));//并没有随之绿色线变化
			mom2ndMid1 = q * (pow(mCutx - x0, 2) + pow(mCuty - y0, 2));
            mom2ndMid2 = q * mom2ndMid0;

            mom2nd += mom2ndMid1 + mom2ndMid2;
            mom2ndMax += mom2ndMid1;
            mom2ndMin += mom2ndMid2;
            pureMom2nd += mom2ndMid0;

            rmin += (mom2ndMid2);
			//if (iterations == 0)
			//	qSum += q;
        }
			clusterlength = (mom2ndMin == 0) ? 0 : mom2ndMax / mom2ndMin;
        if (iterations == 0) //初始值保存设置
        {
            calumom2nd = pureMom2nd;
			intermediateResult = pureMom2nd;
			interclusterResult = clusterlength;
			stepB2 = xn / cos(theta2);
			chooseIteration[0] = iterations;
            chooseIteration[1] = mom2ndMax;
            chooseIteration[2] = mom2ndMin;
            chooseIteration[3] = rmin;
			if (iteranum == 0) break; //真正的初始算法
			if (clusterlength > shortHead)						//分四类判断
				if (pureMom2nd > 25)							//较长径迹头且径迹比较粗
        			//b2 = (mom3rd > 0) ? b2 + stepB2 : b2 - stepB2;
					cutDir = 1;
				else
					//b2 = (mom3rd < 0) ? b2 + stepB2 : b2 - stepB2;
					cutDir = -1;
			else 
				if (pureMom2nd > 40)
					//b2 = (mom3rd > 0) ? b2 + stepB2 : b2 - stepB2;
					cutDir = 1;
				else
					//b2 = (mom3rd < 0) ? b2 + stepB2 : b2 - stepB2;
					cutDir = -1;
			if (iteranum == 0) break; //真正的初始算法
        }
		b2 = (mom3rd > 0) ? b2 + cutDir*stepB2 : b2 - cutDir*stepB2;

		lx11 = (lPk == 0) ? mBx : (ymin - b2) / k2;
		ly11 = ymin;
		lx21 = (lPk == 0) ? mBx : (ymax - b2) / k2;
		ly21 = ymax;
		// lPrinAxisN[iterations] = new TLine(lx11, ly11, lx21, ly21);
		// lPrinAxisN[iterations]->SetLineColor(kGreen);
		// lPrinAxisN[iterations]->Draw();
		//cout<<"++--++- "<<iterations<<" "<<lPrinAxisN[iterations]<<endl;
		if ( clusterlength > 1 && pureMom2nd > 15){
			//if ( abs(pureMom2nd - intermediateResult) / calumom2nd >= caluDecent) //限制条件找合适的截取矩
			if ((clusterlength - interclusterResult)*cutDir > 0)
				{
				//caluDecent = abs(pureMom2nd - intermediateResult) / calumom2nd;
		        chooseIteration[0] = iterations;
		        chooseIteration[1] = mom2ndMax;
		        chooseIteration[2] = mom2ndMin;
		        chooseIteration[3] = rmin;
		    }
			if ( abs(pureMom2nd - intermediateResult) / calumom2nd >= caluDecent) //限制条件找合适的截取矩				
				{
					caluDecent = abs(pureMom2nd - intermediateResult) / calumom2nd;
					chooseIteration[4] = iterations;
				    chooseIteration[5] = mom2ndMax;
				    chooseIteration[6] = mom2ndMin;
				    chooseIteration[7] = rmin;
			}
		}
		else
			break;
		
        //cout<<"the "<<iterations<<" times "<<clusterlength<<" b "<<b2<<" Iter "<<chooseIteration[0]<<" mom2nd "<<pureMom2nd<<" value "<<caluDecent<<endl;

		if (pureMom2nd == intermediateResult){
			babo ++;
			if (babo>1)
				break;
			}
		else
			babo = 0;
        intermediateResult = pureMom2nd;
		interclusterResult = clusterlength;
        iterations++;
		
    } while (iterations < iteranum );
	//} while (pureMom2nd > 0 );
	//cout<<" "<<chooseIteration[0]<<endl;
	//cout<<id<<endl;
	//cout<<"--------------------"<<endl;
	/*if ( clusterlength > 10){
		aTheta1 = -1000;
		return;
		}*/
	//cout<<id<<endl;
	if (chooseIteration[0] ==0 && iteranum !=0){
		chooseIteration[0] = chooseIteration[4];
		chooseIteration[1] = chooseIteration[5];
		chooseIteration[2] = chooseIteration[6];
		chooseIteration[3] = chooseIteration[7];
	}
	chooseIteration[0] =(chooseIteration[0] == 0 && caluDecent == -1)? iterations+1 : chooseIteration[0];
	if (cutDir > 0)		//旧有的 循环完再拿保存值做处理
        b2 = (mom3rd > 0) ? b2 - (iterations+1 - chooseIteration[0]) * stepB2 : b2 + (iterations+1 - chooseIteration[0]) * stepB2;
	else if (cutDir < 0)
		b2 = (mom3rd < 0) ? b2 - (iterations+1 - chooseIteration[0]) * stepB2 : b2 + (iterations+1 - chooseIteration[0]) * stepB2;

    mom2ndMax = chooseIteration[1];
    mom2ndMin = chooseIteration[2];
    rmin = chooseIteration[3];

	double xintersecton = (lPk == 0) ? meanBx :(b2-lPb)/(lPk-k2);
	double yintersecton = (lPk == 0) ? meanBy :k2 * xintersecton + b2;

	//double xintersecton = meanBx;
	//double yintersecton = meanBy;

    //4.2 draw result
    double lx1 = (lPk == 0) ? mBx : (ymin - b2) / k2;
    double ly1 = ymin;
    double lx2 = (lPk == 0) ? mBx : (ymax - b2) / k2;
    double ly2 = ymax;
    // lPrinAxis2 = new TLine(lx1, ly1, lx2, ly2);
    // lPrinAxis2->SetLineColor(kBlue);
    // lPrinAxis2->Draw();

    //mom2nd /=  qtot;
    mom2ndMax /= qtot;
    mom2ndMin /= qtot;
    //shapeRatio = (mom2ndMin == 0) ? 0 : mom2ndMax / mom2ndMin;

    rmin /= 2 * qtot;
    rmin = sqrt(rmin);
    rmin *= (rMinScale > 0) ? rMinScale : 1.;
    rmax = rmin * 9;
    rmax *= (rMaxScale > 0) ? rMaxScale : 1.;

    //4.1 draw TEllipse
    double phimin = theta2 / TMath::Pi() * 180;
    double phimax = theta2 / TMath::Pi() * 180;
    if (mom3rd < 0)
        phimin -= 180;
    if (mom3rd > 0)
        phimax += 180;
    //    if (k2 < 0 && mom3rd > 0)
    //	    phimin -= 180;
    //    if (k2 < 0 && mom3rd < 0)
    //        phimax += 180;

    // e1 = new TEllipse(xintersecton, yintersecton, rmin, rmin, phimin, phimax);
    // e2 = new TEllipse(xintersecton, yintersecton, rmax, rmax, phimin, phimax);
    // e1->SetFillColorAlpha(kWhite, 0);
    // e1->SetFillStyle(0);
    // e2->SetFillColorAlpha(kWhite, 0);
    // e2->SetFillStyle(0);
    // e1->Draw();
    // e2->Draw();

    //----------------------
    //5. find IP
    mBx2 = 0;
    mBy2 = 0;
    qtot2 = 0;

    for (int i = 0; i < N; i++)
    {
        double x = coordsX[i];
        double y = coordsY[i];
        double q = coordsQ[i];

        double d = (lPk == 0) ? (x - mBx) : (y - b2 - k2 * x) / sqrt(1 + k2 * k2);

        if (d * mom3rd < 0)
            continue;
        if (sqrt(pow(xintersecton - x, 2) + pow(yintersecton - y, 2)) < rmin)
            continue;
        /*if (sqrt(pow(xintersecton - x, 2) + pow(yintersecton - y, 2)) > rmax)
            continue;*/

        mBx2 += q * x;
        mBy2 += q * y;
        qtot2 += q;
    }
	// if (qtot2 == 0){
	// 	dataQFlag = QF_FAT;
	// 	llm++;
    //     // f2D->SetTitle(Form("Event %d (identified as overcut events)", id));
	// 	return;
	// }
    mBx2 /= qtot2;
    mBy2 /= qtot2;
    mCx = mBx2;
    mCy = mBy2;
    *IPx =mCx;
    *IPy =mCy;
	//mCx = 36; //定点值测试
	//mCy = 36;
    //5.1 draw result
    // mCovPoint = new TMarker(mCx, mCy, 31);
    // mCovPoint->SetMarkerColor(kYellow);
//     cout<<"IP point is :("<<mCx<<","<<mCy<<")"<<endl;
//     mCovPoint->SetMarkerSize(2.6);
//     mCovPoint->Draw();

//     //----------------------
//     //6. Fit eject line
//     TVirtualFitter *minuit2 = TVirtualFitter::Fitter(0, 10);
//     minuit2->SetParameter(0, "k", 0, 0.01, 0, 0);
//     minuit2->SetFCN(FcnToFitEjectLine);

//     arglist[0] = 0;
//     minuit2->ExecuteCommand("SET NOWarnings", arglist, 1);

//     arglist[0] = -1;
//     minuit2->ExecuteCommand("SET PRINT", arglist, 1);

//     arglist[0] = 5000;  // number of function calls
//     arglist[1] = 0.001; // tolerance
//     minuit2->ExecuteCommand("MIGRAD", arglist, 2);

//     //5.1 get result
//     minuit2->GetStats(chi2, edm, errdef, nvpar, nparx);

//     double k3 = minuit2->GetParameter(0);
//     double b3 = mBy2 - k3 * mBx2;
//     lCk = k3;
//     lCb = b3;
//     aTheta2 = atan(lCk);
//     if(mom3rd<0){
// 	            aTheta2+=M_PI;
// 		        }
//     aTheta2-=M_PI/2.;

//     //5.2 draw result
//     lx1 = (k3 == 0) ? mBx2 : (ymin - b3) / k3;
//     ly1 = ymin;
//     lx2 = (k3 == 0) ? mBx2 : (ymax - b3) / k3;
//     ly2 = ymax;

//     lCovAxis = new TLine(lx1, ly1, lx2, ly2);
//     lCovAxis->SetLineColor(kRed);
//     lCovAxis->Draw();

//     /*if (aTheta1 > 0)
//         aTheta1 = (mom3rd > 0) ? aTheta1 - TMath::Pi() : aTheta1;
//     if (aTheta1 < 0)
//         aTheta1 = (mom3rd < 0) ? aTheta1 + TMath::Pi() : aTheta1;
// */
//     /*if (aTheta2 > 0){
// 		double tmpTheta1 = (abs(aTheta2 - aTheta1 ) < TMath::Pi() )?  abs(aTheta2 - aTheta1 ) : 2 * TMath::Pi() - abs(aTheta2 - aTheta1 );
// 		double tmpTheta2 = (abs(aTheta2 - TMath::Pi() - aTheta1) < TMath::Pi()) ? abs(aTheta2 - TMath::Pi() - aTheta1) : 2 * TMath::Pi() -abs(aTheta2 - TMath::Pi() - aTheta1);
// 		aTheta2 =  (tmpTheta1 < tmpTheta2) ? aTheta2  : aTheta2 - TMath::Pi();
// 		}
// 		//aTheta2 = (mom3rd > 0) ? aTheta2 - TMath::Pi() : aTheta2;
		
        
//     if (aTheta2 < 0){
// 		double tmpTheta1 = (abs(aTheta2 - aTheta1 ) < TMath::Pi() )?  abs(aTheta2 - aTheta1 ) : 2 * TMath::Pi() - abs(aTheta2 - aTheta1 );
// 		double tmpTheta2 = (abs(aTheta2 + TMath::Pi() - aTheta1) < TMath::Pi()) ? abs(aTheta2 + TMath::Pi() - aTheta1) : 2 * TMath::Pi() -abs(aTheta2 + TMath::Pi() - aTheta1);
// 		aTheta2 =  (tmpTheta1 > tmpTheta2) ? aTheta2 + TMath::Pi() : aTheta2;
// 	}*/
//     //k3 +=0;
//    /* if (mom3rd == 0 || dataQFlag == QF_FAT)
//     {
//         aTheta1 += 10 * TMath::Pi();
//         aTheta2 += 10 * TMath::Pi();
// 		llm++;
//     }*/
//     outinfo.open("./reconstruction.txt",ios::app);
//     //outinfo << aTheta1 << endl;//将aTheta1(重心线)写入reconstruction.txt
//     outinfo << aTheta2 << endl;//将aTheta2(出射线)写入reconstruction.txt
//     outinfo.close();
//     //-------------------
//     //6. generate info
//     //info->Append(Form("Singal to Noise:   \t%d\n", id));
//     info->Append(Form("Bary Center:       \t(%.2f, %.2f)\n", mBx, mBy));
//     info->Append(Form("Principal Axis:  \tk=%.2f, b=%.2f\n", lPk, lPb));
//     info->Append(Form("Conversion Point:  \t(%.2f, %.2f)\n", mCx, mCy));
//     info->Append(Form("Ejection Axis:  \tk=%.2f, b=%.2f\n", lCk, lCb));
//     info->Append(Form("Mom 2rd:        \t%.5f\n", mom2ndMax / mom2ndMin));
//     info->Append(Form("Mom 3rd:        \t%.10f\n", mom3rd));
//     info->Append(Form("r of phi and theta2:        \t%.2f, \t%.2f\n", phimin, phimax));
//     info->Append(Form("aTheta1:  aTheta2: \t%.2f, %.5f\n", aTheta1, aTheta2));
// 	info->Append(Form("clul p2:        \t%.5f, %.2f\n", clusterlength, pureMom2nd));
}

//_____________________________
// 采用阶矩来进行重建的中间变量填图
// void MyEventClass::Fill2DPlot(TH2F *h)
// {
//     for (int i = xmin; i <= xmax; i++)
//         for (int j = ymin; j <= ymax; j++)
//             h->SetBinContent(i + 1, j + 1, h->GetBinContent(i + 1, j + 1) + f2D->GetBinContent(i + 1, j + 1));
// }

// void MyEventClass::Draw2DResultMethod1(const char *opt)
// {
//     if (f2D != NULL)
//         f2D->Draw(opt);
//     if (mBcenter != NULL)
//         mBcenter->Draw();
//     if (mCovPoint != NULL)
//         mCovPoint->Draw();
//     if (lPrinAxis1 != NULL)
//         lPrinAxis1->Draw();
//     if (lCovAxis != NULL)
//         lCovAxis->Draw();
// 	for (int i=0 ; i <iteranum; i++){ //stepB2 line
// 		//cout<<"``~~~~ "<<lPrinAxisN[i]<<endl;
// 		if (lPrinAxisN[i] != NULL)
// 			lPrinAxisN[i] ->Draw();
// 		}
// }

// void MyEventClass::DrawSearchResultMethod1()
// {
//     if (lPrinAxis2 != NULL)
//         lPrinAxis2->Draw();
//     if (e1 != NULL)
//         e1->Draw();
//     if (e2 != NULL)
//         e2->Draw();
// 	//ofstream culout;  //径迹详细信息查看
//     //culout.open("./culset.txt", ios::app);
// 	//culout<<xn<<" "<<clusterlength<<" "<<aTheta2<<endl;
// }

// // 通用的结果填图

// void MyEventClass::DrawSearchResultMethod2()
// {
// 	for (int i = 0; i < (int)plist.size(); i++)
//     	DrawBesselLine(plist[i], 0.0001);
//     if (lP0p1 != NULL)
//         lP0p1->Draw();
//     if (lP1p2 != NULL)
//         lP1p2->Draw();
//     if (lP2p3 != NULL)
//         lP2p3->Draw();
// }
//______________________________________________________________________________
// 2. 算法2
//    根据hit击中位置来分cluster
void MyEventClass::AnalysisHist2()
{
	for (int i = xmin; i <= xmax; i++){
        for (int j = ymin; j <= ymax; j++){
            cout<<"Point 5"<<endl;
            if (f2D_raw->GetBinContent(i + 1, j + 1) > 0)
            {   cout<<"Point 5"<<endl;
                f2D->SetBinContent(i + 1, j + 1, f2D_raw->GetBinContent(i + 1, j + 1));

                //.. stored for fit..
                coords.push_back(make_pair(i + 1, j + 1));
                values.push_back(f2D_raw->GetBinContent(i + 1, j + 1));
                errors.push_back(sqrt(f2D_raw->GetBinContent(i + 1, j + 1)));
            }
        }
    }
    cout<<"Point 5"<<endl;
    vector<vector<pair<double, double>>> tmplist;

    //1. cluster分组算法-全扫描方式
    tmplist.clear();

    for (int ix = xmin; ix <= xmax; ix++)
        for (int iy = ymin; iy <= ymax; iy++)
        {
            //1.1 无击中继续循环
            if (f2D_raw->GetBinContent(ix + 1, iy + 1) <= 0)
                continue;

            //1.2 有击中判断一个HistDist范围内是否有击中
            bool saveFlag = 0;
            for (int iix = ix - HitDist - 1; iix <= ix + HitDist + 1; iix++)
                for (int iiy = iy - HitDist - 1; iiy <= iy + HitDist + 1; iiy++)
                    if (f2D_raw->GetBinContent(iix + 1, iiy + 1) > 0)
                    {
                        saveFlag = 1;
                        break;
                    }

            //1.3 HitDist范围内无击中则继续循环
            if (saveFlag == 0)
                break;

            //1.4 将此击中纳入到之前找到的cluster里
            saveFlag = 0;
            for (int ip = 0; ip < (int)tmplist.size(); ip++)
                for (int ih = 0; ih < (int)tmplist[ip].size(); ih++)
                {
					if (saveFlag ==1)
						break;//避免二次记录cluster
                    int jx = tmplist[ip][ih].first; 
                    int jy = tmplist[ip][ih].second;
                    if (fabs(jx - ix) <= HitDist + 1 && fabs(jy - iy) <= HitDist + 1)
                    {
                        saveFlag = 1;
                        tmplist[ip].push_back(make_pair(ix, iy));
                        break;
                    }
                }

            //1.5 若此击中唯一新的击中，则保存为一个新的vector entry
            if (saveFlag == 0)
            {
                vector<pair<double, double>> v;
                v.push_back(make_pair(ix, iy));
                tmplist.push_back(v);
            }
        }

	//cout <<"========= "<<"tmplist size "<<tmplist.size()<<endl;
	/*for (int ip = 0; ip < (int)tmplist.size(); ip++){
				cout<<" point ";
                for (int ih = 0; ih < (int)tmplist[ip].size(); ih++)
                {
                    int jx = tmplist[ip][ih].first; 
                    int jy = tmplist[ip][ih].second;
					cout<<"("<<jx<<","<<jy<<") ";
		}
		cout<<endl;
	}*/
    //1.6 合并cluster  会全部进入vetolist 但不影响结果

    vector<double> vetolist;
    while (1)
    {
        //1.6.1 找到最大的cluster
        int clusterMax = 0;
        int clusterMaxId = -1;
        for (int ip1 = 0; ip1 < (int)tmplist.size(); ip1++)
        {
            //cout << "for every = " << tmplist[ip1].size()<< endl;
            bool veto = 0;
            for (int ip2 = 0; ip2 < (int)vetolist.size(); ip2++)
                if (ip1 == vetolist[ip2])
                {
                    veto = 1;
                    break;
                }
            if (veto)
                continue;
            if ((int)tmplist[ip1].size() > clusterMax)
            {
                clusterMax = tmplist[ip1].size();
                clusterMaxId = ip1;
            }
        }

        //cout << "MaxId = " << clusterMaxId << " vetosize:" <<vetolist.size()<<endl;
        if (clusterMaxId == -1 || clusterMax == 0)
            break;

        //1.6.2 循环并合并cluster
        int veto = 0;
        for (int ip1 = 0; ip1 < (int)tmplist.size(); ip1++)
        {
            if (ip1 == clusterMaxId)
                continue;

            bool combine = 0;
            for (int ih1 = 0; ih1 < (int)tmplist[ip1].size(); ih1++)
            {
                for (int ih2 = 0; ih2 < (int)tmplist[clusterMaxId].size(); ih2++)
                {
                    int jx1 = tmplist[ip1][ih1].first;
                    int jy1 = tmplist[ip1][ih1].second;
                    int jx2 = tmplist[clusterMaxId][ih2].first;
                    int jy2 = tmplist[clusterMaxId][ih2].second;
                    if (fabs(jx1 - jx2) <= HitDist + 1 && fabs(jy1 - jy2) <= HitDist + 1)
                    {
                        combine = 1;
                        break;
                    }
                }
                if (combine)
                    break;
            }

            if (combine)
            {
                //cout << "Combine " << ip1 << " size = " << tmplist[ip1].size() << " to " << clusterMaxId << endl;
                for (int ih1 = 0; ih1 < (int)tmplist[ip1].size(); ih1++)
                    tmplist[clusterMaxId].push_back(tmplist[ip1][ih1]);
                tmplist[ip1].clear();
                veto = 1;
            }
        }

        if (veto == 0)
        {
            //cout << "Set " << clusterMaxId << " to veto list" << endl;
            vetolist.push_back(clusterMaxId);
        }
    }

    //1.7 若cluster的长度太短则丢弃
    clist.clear();
    for (int ip = 0; ip < (int)tmplist.size(); ip++)
    {
        if ((int)tmplist[ip].size() <= MinHitsAsCluster)
            continue;
        clist.push_back(tmplist[ip]);
    }

    delete info;
    info = new TString();
    // info->Append(Form("Event Number:  \t%d\n", id));
    // info->Append(Form("Cluster Size:  \t%d\n", (int)clist.size()));

    //1.7 不存在足够长度的cluster则判断为ped
    if (clist.size() == 0)
    {
        dataQFlag = QF_PED;
        f2D->SetTitle(Form("Event %d (identified as Ped)", id));
        return;
    }

	//cout <<"+++++++++++ "<<"clist size "<<clist.size()<<endl;
    //1.8 找到cluster，进行下一步分析
    dataQFlag = QF_HIT;

    if (clist.size() > 1)
    {
        dataQFlag = QF_MultiCluster;
        f2D->SetTitle(Form("Event %d (identified as Multi-Cluster contained, nCluster = %d)", id, (int)clist.size()));
        return;
    }
	//2. 生成histogram
	f2D->Reset();
    for (int i = 0; i < (int)clist.size(); i++)
        for (int j = 0; j < (int)clist[i].size(); j++)
			f2D->SetBinContent(clist[i][j].first, clist[i][j].second, f2D_raw->GetBinContent(clist[i][j].first + 1, clist[i][j].second + 1));

	//cout <<"////////// "<<"clist  "<<clist[0].size()<<endl;
}
// void MyEventClass::PductHist(){ //拆分便于更改算法
//     //3. 确定cluster后，给出其特征量
//     plist.clear();
//     for (int i = 0; i < (int)clist.size(); i++)
//         plist.push_back(AnalysisCluster2(clist[i]));

// 	//cout <<"-------------- "<<"plist size "<<plist.size()<<endl;
// }
//_____________________________
//    根据Bessel来进行径迹寻找
// vector<pair<double, double>> MyEventClass::AnalysisCluster2(vector<pair<double, double>> hitlist)
// {
//     //fit parameters
//     double arglist[100];
//     double chi2, edm, errdef;
//     int nvpar, nparx;
//     vector<pair<double, double>> blist;

//     //for fit functions
//     coords.clear();
//     values.clear();
//     errors.clear();

//     //0.1 init
//     int N = hitlist.size();

//     for (int i = 0; i < N; i++)
//     {
//         coords.push_back(hitlist[i]);
//         values.push_back(f2D_raw->GetBinContent(hitlist[i].first + 1, hitlist[i].second + 1));
//         errors.push_back(sqrt(f2D_raw->GetBinContent(hitlist[i].first + 1, hitlist[i].second + 1)));
//     }

//     //0.2 x/y范围确定
//     double rxmin = coords[0].first;
//     double rxmax = coords[0].first;
//     double rymin = coords[0].second;
//     double rymax = coords[0].second;

//     for (int i = 0; i < N; i++)
//     {
//         rxmin = (rxmin < coords[i].first) ? rxmin : coords[i].first;
//         rxmax = (rxmax > coords[i].first) ? rxmax : coords[i].first;
//         rymin = (rymin < coords[i].second) ? rymin : coords[i].second;
//         rymax = (rymax > coords[i].second) ? rymax : coords[i].second;
//     }
// 	//cout <<" "<<rxmin<<" "<<rxmax<<" "<<endl;
//     //0.3 确定Q最大值对应的坐标
//     Qmax = -1;
//     int imax = -1;
//     for (int i = 0; i < N; i++)
//         if (values[i] > Qmax)
//         {
//             imax = i;
//             Qmax = values[i];
//         }
// 		//cout <<"value "<<" "<<qlist[i]<<" "<<endl;
        
// 	//cout <<" "<<rymin<<" "<<rymax<<" "<<endl;
//     //0.4 确定距离Q最大值最远的坐标
//     int imin = -1;
//     double dist = -1;
//     for (int i = 0; i < N; i++)
//         if (dist < sqrt(pow(coords[imax].first - coords[i].first, 2) + pow(coords[imax].second - coords[i].second, 2)))
//         {
//             dist = sqrt(pow(coords[imax].first - coords[i].first, 2) + pow(coords[imax].second - coords[i].second, 2));
//             imin = i;
//         }

//     //1. 拟合bessel曲线的形式
//     TVirtualFitter::SetDefaultFitter("Minuit");
//     TVirtualFitter *minuit = TVirtualFitter::Fitter(0);
//     minuit->SetParameter(0, "p0x", coords[imax].first, 0.01, coords[imax].first - 4, coords[imax].first + 4);
//     minuit->SetParameter(1, "p0y", coords[imax].second, 0.01, coords[imax].second - 4, coords[imax].second + 4);
//     minuit->SetParameter(2, "p1x", coords[imax].first, 0.01, 0, 0);
//     minuit->SetParameter(3, "p1y", coords[imax].second, 0.01, 0, 0);
//     minuit->SetParameter(4, "p2x", coords[imin].first, 0.01, 0, 0);
//     minuit->SetParameter(5, "p2y", coords[imin].second, 0.01, 0, 0);
//     minuit->SetParameter(6, "p3x", coords[imin].first, 0.01, rxmin, rxmax);
//     minuit->SetParameter(7, "p3y", coords[imin].second, 0.01, rymin, rymax);
//     minuit->SetParameter(8, "precision", 0.01, 0.01, 0, 0);
//     minuit->FixParameter(8);
//     minuit->SetFCN(FcnToFitBaryLineByBesselLine);

//     arglist[0] = 0;
//     minuit->ExecuteCommand("SET NOWarnings", arglist, 1);

//     arglist[0] = -1;
//     minuit->ExecuteCommand("SET PRINT", arglist, 1);

//     arglist[0] = 10000; // number of function calls
//     arglist[1] = 0.001; // tolerance
//     minuit->ExecuteCommand("MIGRAD", arglist, 2);

//     //2.1 get result
//     minuit->GetStats(chi2, edm, errdef, nvpar, nparx);

//     blist.push_back(make_pair(minuit->GetParameter(0), minuit->GetParameter(1)));
//     blist.push_back(make_pair(minuit->GetParameter(2), minuit->GetParameter(3)));
//     blist.push_back(make_pair(minuit->GetParameter(4), minuit->GetParameter(5)));
//     blist.push_back(make_pair(minuit->GetParameter(6), minuit->GetParameter(7)));

// 	//2.2 draw result
// 	lP0p1 = new TLine(blist[0].first, blist[0].second, blist[1].first, blist[1].second);
// 	lP0p1->SetLineColor(kBlue);
// 	lP1p2 = new TLine(blist[1].first, blist[1].second, blist[2].first, blist[2].second);
// 	lP1p2->SetLineColor(kBlue);
// 	lP2p3 = new TLine(blist[2].first, blist[2].second, blist[3].first, blist[3].second);
// 	lP2p3->SetLineColor(kBlack);
	
// 	//cout <<"|||||||| "<<"blist size "<<blist.size()<<endl;
// 	//3 result output
// 	double lck = defVal;
// 	if (blist[2].first == blist[3].first)
// 		{aTheta2 = TMath::Pi();}

// 	else{
// 		lck = (blist[2].second-blist[3].second)/(blist[2].first-blist[3].first);
// 		aTheta2 = atan(lck);
// 		//cout<<"---- "<<aTheta2<<"  "<<lck<<endl;
// 		}
// 	if (aTheta2 >0){
// 		aTheta2 = ((blist[2].second-blist[3].second)>0) ? aTheta2 :aTheta2 -TMath::Pi();
// 		//cout<<"one+++++ "<<aTheta2<<"  "<<lck<<endl;
// 		}
		
// 	if (aTheta2 <0){
// 		aTheta2 = ((blist[2].second-blist[3].second)>0) ? aTheta2 + TMath::Pi() :aTheta2;
// 		//cout<<"two---- "<<aTheta2<<"  "<<lck<<endl;
// 		}
		

// 	// 结果提取输出
// 	info->Append(Form("P0 :       \t(%.2f, %.2f)\n", blist[0].first, blist[0].second));
// 	info->Append(Form("P1 :       \t(%.2f, %.2f)\n", blist[1].first, blist[1].second));
// 	info->Append(Form("P2 :       \t(%.2f, %.2f)\n", blist[2].first, blist[2].second));
// 	info->Append(Form("P3 :       \t(%.2f, %.2f)\n", blist[3].first, blist[3].second));
// 	info->Append(Form("aTheta2:   \t%.2f\n", aTheta2));

//     return blist;
// }

// void MyEventClass::Draw2DResultMethod2(const char *opt)
// {
//     if (f2D != NULL)
//         f2D->Draw(opt);
// }
