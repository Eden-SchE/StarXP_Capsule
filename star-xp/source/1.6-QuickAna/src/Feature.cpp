#include "Feature.h"

const Int_t nbinsx = 72;
const Int_t nbinsy = 72;
const Int_t thresh = 5;
const Int_t threshNeib=5;

Double_t cacu_ellip(short data[72][72]){
    double u11=0;
    double u20=0;
    double u02=0;
    double M2max=0;
    double M2min=0;
    double eli=1;
    double mBx=0;
    double mBy=0;
    double qtotl = 0;
    for(int i =0; i<72;i++){
        for(int j =0; j<72;j++){
            qtotl+=double(data[i][j]);
            mBx+=double(data[i][j])*i;
            mBy+=double(data[i][j])*j;
        }
    }
    mBx /= qtotl;
    mBy /= qtotl;
    for(int i =0; i<72;i++){
        for(int j =0; j<72;j++){
            double xx=i;
            double yy = j;
            double qq = double(data[i][j]);
            u11+=(xx-mBx)*(yy-mBy)*qq;
            u20+=pow(xx-mBx,2)*qq;
            u02+=pow(yy-mBy,2)*qq;
        }
    }
    M2max=0.5*(u20+u02+sqrt(pow(u20-u02,2)+4*pow(u11,2)));
    M2min=0.5*(u20+u02-sqrt(pow(u20-u02,2)+4*pow(u11,2)));
    eli=sqrt(1-M2min/M2max);
    return eli;
}

Double_t cacu_circ(short data[72][72]){
    double xx=0;
    double yy=0;
    double qq=0;
    double m10=0;
    double m01=0;
    double m00=0;
    double u00=0;
    double u20=0;
    double u02=0;
    double cir=0;
    double adc=0;
    double pi=M_PI;
    for(int i=0;i<72;i++){
        for(int j =0;j<72;j++){
            xx = i;
            yy = j;
            qq = double(data[i][j]);
            if (qq>0)
            {
                m10+=xx;
                m01+=yy;
                m00+=1;
            }
        }
    }
    for(int i=0;i<72;i++){
        for(int j =0;j<72;j++){
            xx = i;
            yy = j;
            qq = double(data[i][j]);
            if (qq>0)
            {
               u20+=pow((xx-(m10/m00)),2);
                u02+=pow((yy-(m01/m00)),2);
                u00+=1;
                adc+=qq;
            }
        }
    }
    cir=pow(u00,2)/((u20+u02)*2*pi);
    return cir;
}

int nebN(int Px,int Py, Double_t** mysource){
   double sumNeb=0;
   int nx=(Px-1)>0? Px-1:0;
   int ox=(Px+1)<nbinsx? Px+1:71;
   int ny=(Py-1)>0? Py-1:0;
   int oy=(Py+1)<nbinsy? Py+1:71;
   if(mysource[nx][ny]>thresh){
      sumNeb+=1;
   }
   if(mysource[Px][ny]>thresh){
      sumNeb+=1;
   }
   if(mysource[ox][ny]>thresh){
      sumNeb+=1;
   }
   if(mysource[nx][Py]>thresh){
      sumNeb+=1;
   }
   if(mysource[Px][Py]>thresh){
      sumNeb+=1;
   }
   if(mysource[ox][Py]>thresh){
      sumNeb+=1;
   }
   if(mysource[nx][oy]>thresh){
      sumNeb+=1;
   }
   if(mysource[Px][oy]>thresh){
      sumNeb+=1;
   }
   if(mysource[ox][oy]>thresh){
      sumNeb+=1;
   }
   return sumNeb;
}

int Feature(TString filename, TString featurefilename, Int_t IFFindIP)
{
    // 100为ADC阈值
    TrackHandle TH(10);
    vector<myTrack> Vtrack;
    TFile *tfile = new TFile(featurefilename,"recreate"); 
    TTree *t_data = new TTree("t_data","data feature result");
    TH2F* IP = new TH2F("PhotonElectron_IP","PhotonElectron_IP",72,0,72,72,0,72);//光电作用位点二维投影
    Double_t** source = new Double_t*[nbinsx];
    for (int i=0;i<nbinsx;i++)
        source[i]=new Double_t[nbinsy];
    Double_t** dest = new Double_t*[nbinsx];
    for (int i=0;i<nbinsx;i++)
        dest[i]=new Double_t[nbinsy];

    // 处理单个数据文件
    TH.calculate(filename.Data(), Vtrack);
    cout << Vtrack.size() << endl;
    ifstream input;
    input.open(filename, ios::binary);

    // 批量处理指定目录下多文件
    // string folder = "/public/home/LPD/data/Topmetal-M2/20220630_He+70%DME_polarized5.4keV_I2mm_GMCP1240V";
    // TH.calculateFromFolder(folder, Vtrack);
    // ifstream input;
    // input.open(Vtrack[0].filename,ios::binary);
    // cout << Vtrack.size() << endl;

    //UShort_t data[72][72];  t_data->Branch("data",&data,"data[72][72]/s");  //for unsigned short
    Short_t data[72][72];   t_data->Branch("data",&data,"data[72][72]/S");
    Int_t numEvent;         t_data->Branch("numEvent",&numEvent,"numEvent/I");
    Int_t numFrame;         t_data->Branch("numFrame",&numFrame,"numFrame/I");
    Int_t timeYear;         t_data->Branch("timeYear",&timeYear,"timeYear/I");
    Int_t timeMon;          t_data->Branch("timeMon",&timeMon,"timeMon/I");
    Int_t timeMday;         t_data->Branch("timeMday",&timeMday,"timeMday/I");
    Int_t timeHour;         t_data->Branch("timeHour",&timeHour,"timeHour/I");
    Int_t timeMin;          t_data->Branch("timeMin",&timeMin,"timeMin/I");
    Int_t timeSec;          t_data->Branch("timeSec",&timeSec,"timeSec/I");
    Int_t timeNsec;         t_data->Branch("timeNsec",&timeNsec,"timeNsec/I");
    Int_t peakADC;          t_data->Branch("peakADC",&peakADC,"peakADC/I");
    Double_t peakX;         t_data->Branch("peakX",&peakX,"peakX/D");
    Double_t peakY;         t_data->Branch("peakY",&peakY,"peakY/D");
    Int_t sumADC;           t_data->Branch("sumADC",&sumADC,"sumADC/I");
    Int_t HitADC;           t_data->Branch("HitADC",&HitADC,"HitADC/I");
    Int_t sumHit;           t_data->Branch("sumHit",&sumHit,"sumHit/I");
    Double_t lenXY;         t_data->Branch("lenXY",&lenXY,"lenXY/D");
    Double_t ratioXY;       t_data->Branch("ratioXY",&ratioXY,"ratioXY/D");
    Double_t BcenterX;      t_data->Branch("BcenterX",&BcenterX,"BcenterX/D");
    Double_t BcenterY;      t_data->Branch("BcenterY",&BcenterY,"BcenterY/D");
    Double_t lenBtoP;       t_data->Branch("lenBtoP",&lenBtoP,"lenBtoP/D");
    Int_t length;           t_data->Branch("length",&length,"length/I");
    Double_t width;         t_data->Branch("width",&width,"width/D");
    Int_t outline;          t_data->Branch("outline",&outline,"outline/I");
    Int_t edgeHit;          t_data->Branch("edgeHit",&edgeHit,"edgeHit/I");
    Double_t ellip;         t_data->Branch("ellip",&ellip,"ellip/D");
    Double_t circ;          t_data->Branch("circ",&circ,"circle/D");
    Double_t IPX;           t_data->Branch("IPX",&IPX,"IPX/D");
    Double_t IPY;           t_data->Branch("IPY",&IPY,"IPY/D");

    
    string lastFielName = Vtrack[0].filename;
    cout << lastFielName << endl;
    // test
    //cout << Vtrack[0].topTime.year << endl;
    for(int i =0; i < Vtrack.size(); i ++)
    {
        
        if(Vtrack[i].filename != lastFielName)
        {
            input.close();
            input.open(Vtrack[i].filename, ios::binary);
            cout << Vtrack[i].filename << endl;
        }
        MyEventClass *fEvent = new MyEventClass(1, 0, 71, 0, 71);
        for(int lin = 0; lin < 72; lin ++){
            for(int row = 0; row < 72; row ++){
                //data[lin][row] = Vtrack[i].data[lin][row];
                input.read((char *)(&data[lin][row]), sizeof(data[lin][row]));
                //if(data[lin][row] > 10 && i == 1)cout << data[lin][row] << endl;
                fEvent->Fill(lin, row, data[lin][row]);
                double q = double(data[lin][row]);
                // hAll1->Fill(lin + 1, row + 1, (q < 0) ? 0 : q);
            }
        }
        numEvent = Vtrack[i].numEvent;
        numFrame = Vtrack[i].numFrame; 
        timeYear = Vtrack[i].topTime.year;
        timeMon = Vtrack[i].topTime.mon;
        timeMday = Vtrack[i].topTime.mday;
        timeMin = Vtrack[i].topTime.min;
        timeSec = Vtrack[i].topTime.sec;
        timeNsec = Vtrack[i].topTime.nsec;
        peakADC = Vtrack[i].peakADC;
        peakX = Vtrack[i].Peak.x;
        peakY = Vtrack[i].Peak.y;
        sumADC = Vtrack[i].sumADC;
        HitADC = Vtrack[i].HitADC;
        sumHit = Vtrack[i].sumHit;
        lenXY = Vtrack[i].lenXY;
        ratioXY = Vtrack[i].ratioXY;
        BcenterX = Vtrack[i].Bcenter.x;
        BcenterY = Vtrack[i].Bcenter.y;
        lenBtoP = Vtrack[i].lenBtoP;
        length = Vtrack[i].length;
        width = Vtrack[i].width;
        outline = Vtrack[i].outline;
        edgeHit = Vtrack[i].edgeHit;
        ellip = cacu_ellip(data);
        circ = cacu_circ(data);
        TSpectrum2* spectrum = new TSpectrum2();
        for(int ii=0;ii<nbinsx;ii++){
            for(int jj=0;jj<nbinsy;jj++){
                source[ii][jj]=Double_t(data[ii][jj]);
            }
        }
        if(i%1000==0){
            cout<<"Event number :"<< i<<endl;
        }
        if(IFFindIP==1){
            // int nfound = spectrum->SearchHighRes(source, dest, nbinsx, nbinsy, 1, 1, kTRUE, 100, kFALSE, 1);
            // Double_t *PositionX = spectrum->GetPositionX();
            // Double_t *PositionY = spectrum->GetPositionY();
            // Int_t startPositionX = 0;
            // Int_t startPositionY = 0;
            // Double_t sumRQ = 0;
            // Double_t MaxsumRQ=-999;
            // for (int imy=0;imy<nfound;imy++) {
            //     sumRQ = 0;
            //     Int_t tmpx = Int_t(PositionX[imy]+0.5);
            //     Int_t tmpY = Int_t(PositionY[imy]+0.5);
            //     if(nebN(tmpx,tmpY,source)<threshNeib){
            //         continue;
            //     }
            //     // printf("posx= %d, posy= %d, value=%d\n",(Int_t)(PositionX[i]+0.5), (Int_t)(PositionY[i]+0.5),
            //     // (Int_t)source[(Int_t)(PositionX[i]+0.5)][(Int_t)(PositionY[i]+0.5)]);
            //     // m->DrawMarker(PositionX[i],PositionY[i]); //画出所有的峰值点
            //     for(int iix = 0; iix<nbinsx;iix++){
            //         for(int iiy = 0; iiy<nbinsy;iiy++){
            //             double pixelADC = double(data[iix][iiy]);
            //             if(pixelADC>thresh){
            //             sumRQ += pixelADC*sqrt(pow(tmpx-iix,2)+pow(tmpY-iiy,2));
            //             // cout<<"sumRQ is:"<<sumRQ<<endl;
            //             }
            //         }
            //     }
            //     if(sumRQ>MaxsumRQ){
            //         MaxsumRQ=sumRQ;
            //         startPositionX=tmpx;
            //         startPositionY=tmpY;
            //     }
            // }
            // IP->Fill(startPositionX,startPositionY);
            // IPX=startPositionX;
            // IPY=startPositionY;
            fEvent->AnalysisHist1(&IPX,&IPY);
            IP->Fill(IPX,IPY);
        }
        else{
            IP->Fill(BcenterX,BcenterY);
        }
        delete spectrum;
        delete fEvent;
        // if(sumADC>220000){
        //     cout<<"sumADC: "<<sumADC<<endl;
        //     cout<<"HitNum: "<<sumHit<<endl;
        //     cout<<"PeakADC: "<<peakADC<<endl;
        //     continue;
        // }
        t_data->Fill();
    
        // if(i < 10) cout << "id: " << i 
        //                 << " sumHit: " << Vtrack[i].sumHit 
        //                 << " length: " << Vtrack[i].length 
        //                 << " outline: " << Vtrack[i].outline 
        //                 << " width: " << Vtrack[i].width
        //                 << " numEvent: " << numEvent
        //                 << " numFrame: " << numFrame
        //                 << endl;
        // if(i > 1000) break;
        
        lastFielName = Vtrack[i].filename;
    }
    //t_data->Print();
    tfile->cd();
    tfile->Write();
    tfile->Close();

    cout << "File Fearture Extraction Completed" << endl;
    return 0;
}

