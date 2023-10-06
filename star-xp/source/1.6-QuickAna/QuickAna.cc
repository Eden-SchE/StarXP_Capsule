// #include "MyMethodClass.h"
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include "time.h"
#include "TCanvas.h"
#include "TROOT.h"
#include "TFile.h"
#include "TMath.h"
#include "TGraph.h"
#include "TH2F.h"
#include "TTree.h"
#include "TSystem.h"
#include "TSystemDirectory.h"
#include "TSystemFile.h"
#include "iostream"
#include "fstream"
#include "iterator"
#include "vector"
// #include <bits/stdc++.h>
#include "RooFit.h"
#include "Feature.h"
#include "ReadCard.h"
#include "TEnv.h"
#include "G4String.hh"
#include "Combine.h"
#include "PixelTimeFix.h"

using namespace std;
// using std::bitset;
// using namespace RooFit; 
const int FileRand = rand();

QuickAnaPGPSConfig QuickCardMess;

unsigned long Get(unsigned long data, Int_t a, Int_t b);
void   cleararr(short dtADC_arr[72*72]);
double Vmonitor(unsigned short v);
double Imonitor(unsigned short i);
double BME_T(unsigned int t);
double BME_P(unsigned int p);
unsigned short bswap_16(unsigned short x);
void getLineParameter(short y1,short y2,double *k , double *b );
void readPixelCalibrationCurve(vector<vector<Int_t>>&dataDecayCurve);
void  sumADCFrame(short frame[72][72],Int_t *hitPixel,Int_t *sumADC);
void readData(string filepath , vector<vector<short>>&data2D);
void readPixelCalibrationCurve3(vector<vector<short>>&dataDecayCurve);
void idenEvent(vector<long> dttime, vector<int> dtSumADC, vector<int> dthitPixel, vector<short> *detector, vector<long> gmcpTime, vector<int> gmcpADC, TString, TString, int fileID,int filenumber, int,int StrADD, int windowsacle);
void turndat(TString,TString,TString);

unsigned long Get(unsigned long data, Int_t a, Int_t b){
	Int_t max = a>b? a:b;
	Int_t min = a<b? a:b;
	unsigned long k = 0;
	//cout<<"Get::min = "<<min<<" max = "<<max<<endl;
	data = data>>min;
	//prInt_tf ("Get::data = %x\n", data);
	//prInt_tf2long(data);
	for (Int_t i = min; i<=max; i++){
		k += (data & 1)<<(i-min);
		//cout<<"kstep =";
		//prInt_tf2long(k);
		data = data>>1;
		//prInt_tf ("Get::data = %x\n", data);
	}
	return k;
}

// bool IfFileExist(G4String FilePath)
// {   
//     struct stat buffer;
//     return (stat(FilePath.data(), &buffer) == 0);
// }

void   cleararr(short dtADC_arr[72*72])
{
	for(Int_t i=0;i<72*72;i++)
	{
			dtADC_arr[i]=0;				
	}
}

double Vmonitor(unsigned short v){return ((double)v+1)/1000*1650;}

double Imonitor(unsigned short i){return ((double)i+1)/1000*45;}

double BME_T(unsigned int t){return ((double)t-6559968)/51797;}

double BME_P(unsigned int p){return ((double)p-1.4e7)/(-92.7);}

unsigned short bswap_16(unsigned short x)
{

    return(  (((unsigned short)(x) & 0x00ff) << 8)| \
             (((unsigned short)(x) & 0xff00) >> 8));
}

void readData(string filepath , vector<vector<short>>&data2D)
{
	unsigned short data_tmp;
	vector<short>data;
	std::ifstream infileutc(filepath, std::ios::binary);
	cout<<"The reading file is "<<filepath<<endl;
	if(infileutc.is_open())
	{
		while (!infileutc.eof())
		{	
			infileutc.read( (char*) &data_tmp, sizeof(data_tmp));
			data.push_back(bswap_16(data_tmp));
		}
	}
	else
	{
		cout<<"Open file failed "<<endl;
		return ;
	}
	Int_t startPoInt_t=0;
	for(Int_t i=0;i<data.size();i++)
	{
		if(data[i]<500 && data[i+1]<500 && data[i+2]<500)
		{
			startPoInt_t = i;	
			cout<<"从第"<<i<<"个数据开始取数"<<endl;
			break;
		}
	}
	vector<short>v;	
	for(Int_t pixel=0;pixel<5184;pixel++)
	{
		for(Int_t i=0;i<data.size()/5184-1;i++)
			v.push_back(data[pixel+i*5184]);
		data2D.push_back(v);
		vector<short>().swap(v);
	}
	vector<short>().swap(data);
	//cout<<"data"<<data.capacity()<<endl;
}

bool QuickArgListControl(G4String InitCard)
{   
    cout<<"Start reading AnaCard ..."<<endl;
    TEnv* Env = new TEnv(InitCard.data());
	QuickCardMess.InputUTCFile = G4String(Env->GetValue("InputUTCFile", ""));
	QuickCardMess.EventRootFile = G4String(Env->GetValue("EventRootFile", ""));
	QuickCardMess.RawDat = G4String(Env->GetValue("RawDat", ""));
	QuickCardMess.DatFile = G4String(Env->GetValue("DatFile", ""));
	QuickCardMess.FeatureFile = G4String(Env->GetValue("FeatureFile", ""));
	QuickCardMess.InputUTCFileFolder = G4String(Env->GetValue("InputUTCFileFolder",""));
	QuickCardMess.DecayFile = G4String(Env->GetValue("DecayFile", ""));

	QuickCardMess.HITPIXEL = Env->GetValue("HITPIXEL", 16);
	QuickCardMess.SumADC = Env->GetValue("TOPMETALADCBOTTOM", 1000);
	QuickCardMess.EventTime = Env->GetValue("EVENTTIME", 10000000);
	QuickCardMess.THREPIXELADC = Env->GetValue("THREPIXELADC",20);
	QuickCardMess.ThresholdGMCPADC = Env->GetValue("ThresholdGMCPADC",500);
	QuickCardMess.DETECTORID = Env->GetValue("DETECTORID",0);
	QuickCardMess.GMCPCoincidence = Env->GetValue("GMCPCoincidence",1);
	QuickCardMess.ReadFiles = Env->GetValue("ReadFiles",0);
	QuickCardMess.SpecFix = Env->GetValue("SpecFix",0);
	QuickCardMess.TOPMETALADCUPPER = Env->GetValue("TOPMETALADCUPPER",20000);
	QuickCardMess.IFFindIP = Env->GetValue("IFFindIP",1);
	QuickCardMess.StraightADD = Env->GetValue("StraightADD",0);
	QuickCardMess.WindowScale = Env->GetValue("WindowScale",2);


	if(QuickCardMess.ReadFiles==0){
    	cout<<"InputUTCFile:" <<QuickCardMess.InputUTCFile<<endl;
	}
	else{
		cout<<"Warning, you are going to read several UTC files in: "<<QuickCardMess.InputUTCFileFolder<<endl;
	}
	cout<<"DETECTORID:" <<QuickCardMess.DETECTORID<<endl;
    cout<<"EventRootFile:"<<QuickCardMess.EventRootFile<<endl;
    cout<<"DatFile:" <<QuickCardMess.DatFile<<endl;
    cout<<"FeatureFile:" <<QuickCardMess.FeatureFile<<endl;
    cout<<"HITPIXEL: "<<QuickCardMess.HITPIXEL<<endl;
    cout<<"SumADC: "<<QuickCardMess.SumADC<<endl;
    cout<<"EventTime: "<<QuickCardMess.EventTime<<endl;
    cout<<"THREPIXELADC:" <<QuickCardMess.THREPIXELADC<<endl;
	cout<<"ThresholdGMCPADC:" <<QuickCardMess.ThresholdGMCPADC<<endl;
	if(QuickCardMess.GMCPCoincidence==1){
		cout<<"GMCP conincidence:"<<"ON"<<endl;
	}
	else{
		cout<<"GMCP conincidence:"<<"OFF"<<endl;
	}
    return true;
    //
    
}

int main(int argc, char **argv){
	TrackHandle TH(20);
	G4String InitCard = "QuickAnaCard.card";
	bool UseDefaultCard = true;
	if(UseDefaultCard)
    {
        cout << "Warning!!! Using default " << InitCard << endl;
        // if (!IfFileExist(InitCard))
        // {
        //     cout << "Error!!! Initialize card \" " << InitCard << " \" NOT FOUND!!!" << endl;
        //     return 1;
        // }
        if (!QuickArgListControl(InitCard))
            return 1;
    }

    string PATH = QuickCardMess.InputUTCFile.data();//读取的文件目录
    Int_t HITPIXEL  = QuickCardMess.HITPIXEL;//着火点个数阈值
	Int_t SUMADC    = QuickCardMess.SumADC;//总ADC值阈值
	Int_t EVENTTIME = QuickCardMess.EventTime;//放电事例的间隔阈值
	Int_t THREPIXELADC = QuickCardMess.THREPIXELADC;//阈值ADC


    //定义存储数据的容器

    char time_gmcp0[100];  
    char time_gmcp1[100]; 
	char time_dt0[100];
    char time_dt1[100];

    int eventnumdt0time=0,eventnumdt0data=0;
    int eventnumdt1time=0,eventnumdt1data=0;

	vector<myTrack> Vtrack;
	string folder = QuickCardMess.InputUTCFileFolder.data();
	vector<string> filepath = TH.calculateFromFolder(folder, Vtrack);
	std::ifstream infileutc;
	for(int ifile =0; ifile<filepath.size() || QuickCardMess.ReadFiles==0;ifile++){
		if(QuickCardMess.ReadFiles==0){
			infileutc.open(QuickCardMess.InputUTCFile, std::ios::binary);
		}
		else{
			infileutc.open(filepath[ifile], std::ios::binary);//读取UTC数据
			cout<<"Start reading:"<<filepath[ifile]<<endl;
		}
		
		short dtADC_arr0[72*72]={0};//存探测器0的数据
		// vector<vector<short>>detector0{5184};//存取探测器0所有数据
		vector<long>dt0Time;//存取探测器0 时间戳
		short dtADC_arr1[72*72]={0};//存探测器1的数据
		// vector<vector<short>>detector1{5184};//存取探测器1所有数据
		// short detector0[80][5184];
		// short detector1[80][5184];
		vector<short>detector0ADC;
		vector<short>detector1ADC;
		vector<short>detector0Posi;
		vector<short>detector1Posi;
		vector<int>PicEndPoint;
		int Topmetal0Pic =0;
		int Topmetal1Pic =0;
		vector<long>dt1Time;//存取探测器1时间戳

		vector<int>gmcp0ADC;//存取gmcp0 ADC值
		vector<long>gmcp0Time;//存取gmcp0 时间戳
		vector<int>gmcp1ADC;//存取gmcp0 ADC值
		vector<long>gmcp1Time;//存取gmcp0 时间戳

		vector<unsigned long>SIPM0Time;//存取SIPM0时间戳
		vector<unsigned long>SIPM1Time;//存取SIPM1时间戳

		vector<unsigned short>HVImon;//存取高压电流数据
		vector<unsigned short>HVVmon;;//存取高压电电压数据时间戳

		vector<double>bme0_temp;//存取bme0温度
		vector<double>bme0_pressure;//存取bme0气压
		vector<double>bme1_temp;//存取bme1温度
		vector<double>bme1_pressure;//存取bme1气压
		unsigned long UTCTIME;//存取UTCTIME

		vector<int>dt1sumADC;
		vector<int>dt1hitPixel;
		vector<int>dt0sumADC;
		vector<int>dt0hitPixel;
		// std::ofstream timeTopmetalGMCP(TString::Format("timeTopmetalGMCP.txt"));//在刚刚读取数据时把数据存下来

		int dt1sumADC_tmp=0,dt1hitPixel_tmp=0;	
		int dt0sumADC_tmp=0,dt0hitPixel_tmp=0;
		char Buff[8] = {0};
		int TopmetalPicNum =0;

		if(infileutc.is_open()){
			while(!infileutc.eof()){
				infileutc.read((char*)Buff, sizeof(char) * 1);//读header
				unsigned int header = Buff[0]>>4&0xf;    
				if (header == 0 || header == 1 ){    
					infileutc.read(&Buff[1], sizeof(char)*3);//前面已经读一个header所以只读3个char
					unsigned int data = 0;//腔室数据，32bit
					for(int i  = 0; i<4; i++){
						data +=(int)(Buff[i] & 0xff)<<((3-i)*8);//读入data
					}
					unsigned int hitAddress = Get(data,12,24);//着火ADC坐标
					unsigned int hitADC = Get(data,0,11);//着火ADC值

					if(header == 0){//Topmetal0
						dtADC_arr0[hitAddress] = hitADC -500;
						dt0sumADC_tmp+=dtADC_arr0[hitAddress];
						dt0hitPixel_tmp += 1;
					}
					else{//Topmetal1
						dtADC_arr1[hitAddress] = hitADC -500;
						dt1sumADC_tmp+=dtADC_arr1[hitAddress];
						dt1hitPixel_tmp += 1;
					}
				}

				else if(header == 2 ||header == 3 ||header == 4 ||header == 5 ||header == 8 ||header == 9 ||header == 10 ||header == 14 || header == 15 ){
					infileutc.read (&Buff[1], sizeof(char) * 7);
					unsigned long longdata = 0; // for data 64bit;
					for (int i = 0; i< 8; i++){
						longdata += (long)(Buff[i]&0xff)<<((7-i)*8);
						//cout<<"Decode::data::hex "<<setbase(16)<<longdata<<endl;
					}
					if(header==15 || header==14)//腔室0/1
					{
						unsigned long timeTopmetal = Get(longdata, 0, 43);
						//cout<<"腔室：  "<<header%2-1<<" 时间戳是： "<<timeTopmetal<<endl;
						// timeTopmetalGMCP<<"腔室：  "<<header%2-1<<" 时间戳是： "<<timeTopmetal<<endl;
						if(header==15)//腔室topmetal0时间戳
						{	
							dt0Time.push_back(timeTopmetal);	
							dt0sumADC.push_back(dt0sumADC_tmp);
							dt0hitPixel.push_back(dt0hitPixel_tmp);
							// sprintf(&time_dt0[0],"%f",timeTopmetal/1e5);
							dt0sumADC_tmp=0;
							dt0hitPixel_tmp=0;			
							for(int i=0;i<5184;i++)
							{	
								// detector0[i].push_back(dtADC_arr0[i]);//存入TopmetalADC值，但这里存的是上一帧事例
								// detector0[Topmetal0Pic][i]= dtADC_arr0[i];
								detector0ADC.push_back(dtADC_arr0[i]);
							}
							Topmetal0Pic+=1;
							cleararr(dtADC_arr0);//存完之后清除数组
						}
						else//腔室topmetal1时间戳 
						{
							dt1Time.push_back(timeTopmetal);
							dt1sumADC.push_back(dt1sumADC_tmp);
							dt1hitPixel.push_back(dt1hitPixel_tmp);
							// sprintf(&time_dt1[0],"%f",timeTopmetal/1e5);
							if(dt1Time.size()>1){
								// timeTopmetalGMCP<<"第"<<dt1Time.size()-1<<" Topmatal事例,腔室：  "<<1<<" 时间戳是： "<<time_dt1<<",总ADC值是："<<dt1sumADC_tmp<<",着火点数是："<<dt1hitPixel_tmp<<endl;
							}
							dt1sumADC_tmp=0;
							dt1hitPixel_tmp=0;
							for(int i=0;i<5184;i++)
							{	
								// detector0[i].push_back(dtADC_arr0[i]);//存入TopmetalADC值，但这里存的是上一帧事例
								// detector1[Topmetal1Pic][i]= dtADC_arr1[i];
								detector1ADC.push_back(dtADC_arr1[i]);
							}
							Topmetal1Pic+=1;
							cleararr(dtADC_arr1);
						}
					}

					else if(header ==3 || header ==2){//GMCP时间戳和ADC值放在一起,GMCP 0，1下表面
						unsigned long GMCPTime = Get(longdata, 0, 42);//GMCP时间
						unsigned short GMCPADC = Get(longdata, 43, 54);
						if(header ==2)
						{
							gmcp0Time.push_back(GMCPTime);
							gmcp0ADC.push_back(GMCPADC);
						}
						else
						{
							gmcp1Time.push_back(GMCPTime);
							gmcp1ADC.push_back(GMCPADC);
							// sprintf(&time_gmcp1[0],"%f",GMCPTime/1e5);
							// sprintf(&time_gmcp1[0],"%f",GMCPTime/1e5); 
							// timeTopmetalGMCP<<"第"<<gmcp1Time.size()<<" GMCP事例,GMCP通道： "<<header%2<<" 时间戳是： "<<time_gmcp1<<" , ADC值是:"<<GMCPADC<<endl;
						}

					}

					else if(header == 4 ||header == 5){//闪烁体探测器时间戳，目前暂时没有安装
						unsigned long scintilatorTime = Get(longdata, 0, 42);				
						if(header == 4)SIPM0Time.push_back(scintilatorTime);
						else SIPM1Time.push_back(scintilatorTime);

					}

					else if(header == 8 || header == 9)//高压模块时间戳
					{
						unsigned short monitor       = Get(longdata, 0, 11);	
						unsigned int bme680_temp     = Get(longdata, 12, 35);
						unsigned int bme680_pressure = Get(longdata, 36, 59);
						if(header == 8)
						{
							bme0_temp.push_back(BME_T(bme680_temp));
							bme0_pressure.push_back(BME_P(bme680_pressure));
							HVVmon.push_back(monitor);
						}
						else 
						{
							bme1_temp.push_back(BME_T(bme680_temp));
							bme1_pressure.push_back(BME_P(bme680_pressure));
							HVImon.push_back(monitor);
						}
					}

					else if(header == 10){ 
						UTCTIME = Get(longdata, 0, 31);
						cout<<"UTC时间："<< UTCTIME <<endl;
					}
				} 

				else{
					cout<<"undecode failed "<<endl;
					return -1;
				}
			}
			infileutc.close();
			// timeTopmetalGMCP.close();
		}

		if(QuickCardMess.DETECTORID==0){
			idenEvent(dt0Time,dt0sumADC,dt0hitPixel, &detector0ADC ,gmcp0Time,gmcp0ADC, QuickCardMess.EventRootFile, QuickCardMess.DecayFile, ifile, QuickCardMess.ReadFiles>0? filepath.size():1,QuickCardMess.TOPMETALADCUPPER,QuickCardMess.StraightADD,QuickCardMess.WindowScale);
		}
		else{
			idenEvent(dt1Time,dt1sumADC,dt1hitPixel, &detector1ADC ,gmcp1Time,gmcp1ADC, QuickCardMess.EventRootFile, QuickCardMess.DecayFile, ifile, QuickCardMess.ReadFiles>0? filepath.size():1,QuickCardMess.TOPMETALADCUPPER,QuickCardMess.StraightADD,QuickCardMess.WindowScale);
		}
		if(QuickCardMess.ReadFiles==0){//读单个文件，则循环一次就退出
			break;
		}
	}
	turndat(QuickCardMess.RawDat, QuickCardMess.DatFile, QuickCardMess.EventRootFile);
	Feature(QuickCardMess.DatFile,QuickCardMess.FeatureFile,QuickCardMess.IFFindIP);
	return 0;
}

void idenEvent(vector<long> dttime, vector<int> dtSumADC, vector<int> dthitPixel, vector<short> *detector, vector<long> gmcpTime, vector<int> gmcpADC, TString filename, TString decayfile, int fileID,int filenumber, int TOPMETALUPPER,int strADD, int windowscale){
	cout<<"GMCP time size: "<< gmcpADC.size() << ", GMCP ADC size is :"<< gmcpADC.size() <<endl;
    cout<<"Tometal time size: "<<dttime.size() << ", Tometal ADC size is: "<< detector->size() <<endl;
	TFile* fcalib = new TFile(decayfile);
	TFile* f0=new TFile(TString::Format("outtemp_%d_%d.root",FileRand,fileID),"recreate");
	ofstream outNegafile("negative.dat",ios::binary);

	TH2F* aTopmetal = new TH2F("","",72,0,73,72,0,73);
	int matchnumber  = 0;
	int morepic = 0;


	TTree* tr = new TTree("tree","tree");

	double topmetaltime;
	TH2F* topmetalpic = new TH2F("","",72,0,73,72,0,73);
	double trgmcpTime;
	int trgmcpADC;
	int trSumTopmetalADC;
	double deltaTime;
	double T1=0;

	tr->Branch("topmetaltime",&topmetaltime);
	tr->Branch("topmetalpic","TH2F",&topmetalpic);
	tr->Branch("gmcpTime",&trgmcpTime);
	tr->Branch("gmcpADC",&trgmcpADC);
	tr->Branch("SumTopmetalADC",&trSumTopmetalADC);
	tr->Branch("DeltaTime",&deltaTime);

	if(access("Plot", 00) == -1) gSystem->Exec("mkdir Plot");
	TCanvas* can = new TCanvas("can", "", 4000, 1200);
	can->Divide(3,1);
    // int iTopmetal = 1;
	int MarkTopmetal = 0;//标记读到第几个topmetal时间刻度
    int totalevent = 0;
	TH2F* firstone = new TH2F("","",72,0,73,72,0,73);
	TH2F* secondone = new TH2F("","",72,0,73,72,0,73);
	int QUalifiedPicNum=0;
	if(QuickCardMess.GMCPCoincidence==1){
		for(int i = 0; i < gmcpADC.size()-1; i++){
			if(gmcpADC[i]<QuickCardMess.ThresholdGMCPADC){
				continue;
			}
			double igmcptime = double(gmcpTime[i])/double(1e5);
			// double idttime = double(dttime[iTopmetal])/double(1e5);
			vector<int> QualifiedHitPixel;
			vector<double> Qualifiedtime;
			vector<int> QualifiedSumADC;
			vector<short> Qualifieddetector;
			
			for(int iTopmetal=MarkTopmetal; iTopmetal<dttime.size();iTopmetal++){
				if(dtSumADC[iTopmetal]<QuickCardMess.SumADC || iTopmetal>=dttime.size()){
					continue;
				}
				double idttime = double(dttime[iTopmetal])/double(1e5);
				if((idttime-igmcptime)>=0 && (idttime-igmcptime)<=(2.592*windowscale)){
					// cout<<"Sum ADC is: "<< dtSumADC[iTopmetal]<<endl;
					QualifiedHitPixel.push_back(dthitPixel[iTopmetal+1]);//+1是因为时间戳与ADC值错位
					// cout<<"Sum ADC is:"<<dtSumADC[iTopmetal+1]<<endl;
					QualifiedSumADC.push_back(dtSumADC[iTopmetal+1]);
					Qualifiedtime.push_back(idttime);
					if(iTopmetal+1!=dttime.size()){
						for(int NN=0;NN<(72*72);NN++){
							Qualifieddetector.push_back(detector->at(((long(iTopmetal)+1)*(5184)+long(NN))));//+1是因为时间戳与ADC值错位
						}
						QUalifiedPicNum+=1;
					}
					MarkTopmetal=iTopmetal;
				}
				if((idttime-igmcptime)>(2.592*windowscale)){//Topmetal时间已经超过阈值范围，没有必要继续搜索
					break;
				}
			}
			// while((idttime-igmcptime)<=(2.592*2) && iTopmetal<(dttime.size()-1)){//事例符合
			//     if((idttime-igmcptime)>=0 && (idttime-igmcptime)<=(2.592*2)){
			// 		// cout<<"Sum ADC is: "<< dtSumADC[iTopmetal]<<endl;
			//         QualifiedHitPixel.push_back(dthitPixel[iTopmetal+1]);//+1是因为时间戳与ADC值错位
			// 		Qualifiedtime.push_back(idttime);
			//         for(int NN=0;NN<(72*72);NN++){
			//             Qualifieddetector[NN].push_back(detector[NN][iTopmetal+1]);//+1是因为时间戳与ADC值错位
			//         }
			//     }
			//     iTopmetal+=1;
			//     idttime = double(dttime[iTopmetal])/double(1e5);
			// }
			if(QualifiedHitPixel.size()>0){//如果有符合事例
				deltaTime=0;
				int IFCombinePic  =0;
				int max_hit =-999;
				double itopmetaltime = 0.;
				totalevent +=1;
				TH2F CombinPic = TH2F("fixed_pic","fixed_pic",72,0,73,72,0,73);
				if(QualifiedHitPixel.size()>1){
					if(strADD==0){
						for(int iPic = 0; iPic<QualifiedHitPixel.size()-1; iPic++){//两帧合并能谱修正
							if(int((Qualifiedtime[Qualifiedtime.size()-1-iPic]-Qualifiedtime[Qualifiedtime.size()-2-iPic])*1e5)==259200||int((Qualifiedtime[Qualifiedtime.size()-1-iPic]-Qualifiedtime[Qualifiedtime.size()-2-iPic])*1e5)==259201){
								IFCombinePic=1;
								itopmetaltime = Qualifiedtime[Qualifiedtime.size()-2-iPic];
								// cout<<"Event id: "<<totalevent<<", dif time is:"<<(Qualifiedtime[Qualifiedtime.size()-1-iPic]-Qualifiedtime[Qualifiedtime.size()-2-iPic])<<endl;
								for(int npix = 0; npix<(72*72);npix++){
									firstone->SetBinContent((npix/72)+1,(npix%72)+1,float(Qualifieddetector.at(((Qualifiedtime.size()-2-iPic)*5184)+npix)));
									secondone->SetBinContent((npix/72)+1,(npix%72)+1,float(Qualifieddetector.at(((Qualifiedtime.size()-1-iPic)*5184)+npix)));
								}
								firstone->SetTitle(Form("First Picture Event %d",totalevent));
								secondone->SetTitle(Form("Second Picture Event %d",totalevent));
								// clock_t start = clock();
								CombinPic = ComBin(firstone,secondone);
								CombinPic.SetTitle(Form("Fixed Picture Event %d",totalevent));
								// clock_t stop = clock();
								double Combinesum=0;
								// double firstsum =0;
								// double secondsum =0;
								for(int ia =1;ia<73;ia++){
									for(int ib =1;ib<73;ib++){
										Combinesum+=CombinPic.GetBinContent(ia,ib);
										// firstsum+=firstone->GetBinContent(ia,ib);
										// secondsum+=secondone->GetBinContent(ia,ib);
									}
								}
								// CombinPic.SetBinContent(1,1,2000);
								max_hit = int(Combinesum);
								// cout<<(double)(stop - start)/CLOCKS_PER_SEC<<" s passed."<<"Sum ADC is: " <<Combinesum <<" ,first sum is: " <<firstsum<<" ,second sum is: "<<secondsum<<endl;
								// can->cd(1);
								// CombinPic.Draw("colz");
								// can->cd(2);
								// firstone->Draw("colz");
								// can->cd(3);
								// secondone->Draw("colz");
								// can->Update();
								// can->SaveAs(TString::Format("Plot/Event_%d.png", totalevent));
							}
						}
					}
					else{
						itopmetaltime = Qualifiedtime[0];
						IFCombinePic=1;
						for(int ia = 0; ia<72; ia++){
							for(int ib = 0; ib<72; ib++){
								CombinPic.SetBinContent(ia,ib,0.);
							}
						}
						for(int ithpic =0; ithpic<Qualifiedtime.size(); ithpic++){
							for(int iNpixle =0; iNpixle<(72*72);iNpixle++){
								CombinPic.SetBinContent(iNpixle/72 +1, iNpixle%72 +1,CombinPic.GetBinContent(iNpixle/72 +1, iNpixle%72 +1)+float(Qualifieddetector.at((ithpic*5184)+iNpixle)));
							}
						}
						double Combinesum=0;
						for(int ia =1;ia<73;ia++){
							for(int ib =1;ib<73;ib++){
								Combinesum+=CombinPic.GetBinContent(ia,ib);
							}
						}
						max_hit = int(Combinesum);
					}
						// cout<<"match size is: "<<QualifiedHitPixel.size()<<endl;
						morepic+=1;
				}
				else{
						matchnumber+=1;
				}
				for(int ith = 0; ith < QualifiedHitPixel.size(); ith++){//如果有多帧符合，挑出最符合的一帧,即HitNum最大的帧
					if(IFCombinePic==1){//做过多帧符合
						for(int NN =0; NN<(72*72) ; NN++ ){
							topmetalpic->SetBinContent((NN/72)+1,(NN%72)+1,float(CombinPic.GetBinContent((NN/72)+1,(NN%72)+1)));
							// cout<<"Pixel is :"<<Qualifieddetector[NN][ith]<<endl;
						}
					}
					else if(QualifiedSumADC[ith]>max_hit && QualifiedHitPixel[ith]<500){
						max_hit=QualifiedSumADC[ith];
						itopmetaltime = Qualifiedtime[ith];
						for(int NN =0; NN<(72*72) ; NN++ ){
							topmetalpic->SetBinContent((NN/72)+1,(NN%72)+1,float(Qualifieddetector[(ith*5184)+NN]));
							// cout<<"Pixel is :"<<Qualifieddetector[NN][ith]<<endl;
						}
					}
				} 
				if(max_hit>0 &&max_hit<TOPMETALUPPER){//上面部分没有做能谱修正
					if(QuickCardMess.SpecFix==0){
						topmetaltime= itopmetaltime;
						trgmcpTime = igmcptime;
						trgmcpADC = (gmcpADC[i]);
						trSumTopmetalADC = max_hit;
					}
					else{
						trSumTopmetalADC =0;
						topmetaltime= itopmetaltime;
						trgmcpTime = igmcptime;
						trgmcpADC = (gmcpADC[i]);
						double intervtime = topmetaltime-trgmcpTime-2.592;
						int negativetimenum = 0;
						for(int iNN=0; iNN<(72*72); iNN++){
							if(topmetalpic->GetBinContent(iNN/72 +1,iNN%72 +1)>0){
								double firstPix  = topmetalpic->GetBinContent(iNN/72 +1,iNN%72 +1);
									// cout<<"delta time is: "<<intervtime+double(iNN)*5e-4<<endl;
								if((intervtime+double(iNN)*5e-4)>0){	
									topmetalpic->SetBinContent(iNN/72 +1,iNN%72 +1,double(PixelfixReverse(iNN/72 +1,iNN%72 +1,short(topmetalpic->GetBinContent(iNN/72 +1,iNN%72 +1)),intervtime+double(iNN)*5e-4,fcalib)));
								}
								else{
									negativetimenum+=1;
								}
								// double secPix = topmetalpic->GetBinContent(iNN/72 +1,iNN%72 +1);
								// if(secPix!=firstPix){
								// 	cout<<"orig pix is: "<<firstPix<<" , fixed pix is: "<<secPix<<endl;
								// }
								trSumTopmetalADC+=int(topmetalpic->GetBinContent(iNN/72 +1,iNN%72 +1)+0.5)>0? int(topmetalpic->GetBinContent(iNN/72 +1,iNN%72 +1)+0.5):0;
							}
						}
						short negadata[72*72];
						if(negativetimenum>15){//有15个有信号的点，时间戳为负值
							for(int iii =0; iii<(72*72);iii++){
								negadata[iii]= short(topmetalpic->GetBinContent(iii/72 +1, iii%72 +1));
								if((intervtime+double(iii)*5e-4)<0 && (intervtime+double(iii+1)*5e-4>0)){
									negadata[iii]= 2000;
									topmetalpic->SetBinContent((iii+1)/72 +1, (iii+1)%72 +1, 2000);
								}
								outNegafile.write((char*)&negadata[iii],sizeof(negadata[iii]));
							}
						}
					}
					deltaTime = trgmcpTime-T1;
					if(deltaTime>6000.){
						deltaTime=0;
					}
					T1 = trgmcpTime;
					tr->Fill();
				}
			}
			else{//没有符合事例就跳到下一个GMCP记录
				continue;
			}
			if(totalevent%1000==0){
				cout<<"find "<<totalevent<<" events."<<endl;
			}
		}
	}
	else{//不做GMCP符合直接填
		for(int i =0; i<(dttime.size()-1);i++){
			topmetaltime = double(dttime[i])/double(1e5);
			for(int NN =0; NN<(72*72) ; NN++ ){
							topmetalpic->SetBinContent((NN/72)+1,(NN%72)+1,float(detector->at(((i+1)*5184) +NN)));
							// cout<<"Pixel is :"<<Qualifieddetector[NN][ith]<<endl;
			}
			trSumTopmetalADC = dtSumADC[i+1];
			trgmcpTime = 0;//不做符合，因此都填0
			trgmcpADC = 0;
			deltaTime=0;
			if(deltaTime>6000.){
				deltaTime==0;
			}
			tr->Fill();
		}

	}
	tr->Print();
    tr->AutoSave();
    f0->Close();
	outNegafile.close();
	cout<<"match events: "<<matchnumber <<" ,more pic events:"<<morepic<<endl;
	if(fileID==(filenumber-1)){
		gSystem->Exec(TString::Format("rm -r %s", filename.Data()));
		gSystem->Exec(TString::Format("hadd %s outtemp_%d_*.root", filename.Data(), FileRand));
		gSystem->Exec(TString::Format("rm -r outtemp_%d_*.root",FileRand));
	}
}

void turndat(TString rawdatname ,TString datname, TString filename){

	cout<<"start clustering ..."<<endl;
	ofstream outfileFrame(datname,ios::binary);
	ofstream outrawfile(rawdatname,ios::binary);
	
	/*团簇识别*/
	const int nXpixel = 72;
  	const int nYpixel = 72;
	short sigdata[nXpixel*nYpixel] = {0};
	short outsigdata[nXpixel*nYpixel] = {0};
	int ldata,data,lpixel;
	int Npixels,Xm,Ym,npixels;
	int Xb,Yb,mx,my;
	int no = 0;
	int Xpixel[5184] = {0};
  	int Ypixel[5184] = {0};
	double Gad;

	ofstream outfile(datname,ios::binary);
	TFile* f = new TFile(filename);
	TTree* tr1 = (TTree*)f->Get("tree");
	TH2F* topmetalpic = NULL;
	tr1->SetBranchAddress("topmetalpic",&topmetalpic);
	int EntryNum = tr1->GetEntries();
	for(int i =0; i<EntryNum; i++){
		tr1->GetEntry(i);
		
		for ( int ii=0; ii<5184; ii++){
            sigdata[ii] = short(topmetalpic->GetBinContent((ii/72) +1,(ii%72) +1));
			outrawfile.write((char*)&sigdata[ii],sizeof(sigdata[ii]));    
		}
		// for ( int jj=0; jj<5184; jj++){
		// 	outfileFrame.write((char*)&sigdata[jj],sizeof(sigdata[jj]));
		// }
		for (int lk=0; lk<5184; lk++){
			Npixels = 0;
			int Zpixel[5184] = {0};
			Zpixel[0]=1;
			npixels = 0;
			
			for (int iii=0; iii<5184; iii++){
			outsigdata[iii] = sigdata[iii];
			if (sigdata[iii]>QuickCardMess.THREPIXELADC){
				Xpixel[Npixels] = iii/72;
				Ypixel[Npixels] = iii%72;
				Npixels = Npixels + 1;
				}
			}

			for (int n1=0; n1<Npixels; n1++){
			for (int n2=0; n2<Npixels; n2++){
				Gad = (Xpixel[n1] - Xpixel[n2])*(Xpixel[n1] - Xpixel[n2]) + (Ypixel[n1] - Ypixel[n2])*(Ypixel[n1] - Ypixel[n2]);
				if (Gad<18.1) {
					if (Zpixel[n2] ==1) Zpixel[n1]=1;
					Zpixel[n2] = Zpixel[n1];
				}
				}
			}

			for (int n3=0; n3<Npixels; n3++){
				if (Zpixel[n3] == 1){
					lpixel = 72*Xpixel[n3] + Ypixel[n3];
					npixels = npixels + 1;
					sigdata [lpixel] = 3;
				}
				else {
				lpixel = 72*Xpixel[n3] + Ypixel[n3];
				outsigdata [lpixel] = 3;
				}
			}

			if (npixels>QuickCardMess.THREPIXELADC&&npixels<2000){
				for(int jj=0; jj<5184; jj++){
				outfileFrame.write((char*)&outsigdata[jj],sizeof(outsigdata[jj]));
				}
			}

			if (Npixels -  npixels < 25) break;
		}

	}
	outrawfile.close();
	outfileFrame.close();
}