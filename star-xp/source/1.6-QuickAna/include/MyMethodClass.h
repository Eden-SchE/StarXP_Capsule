#ifndef MYMETHODCLASS_H
#define MYMETHODCLASS_H
#include <stdio.h>
#include <string.h>
#include <dirent.h>
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
#include <bits/stdc++.h>
#include "RooFit.h"

using namespace std;
using std::bitset;
// using namespace RooFit;

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

#endif