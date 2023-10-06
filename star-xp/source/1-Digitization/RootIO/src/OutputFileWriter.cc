#include "OutputFileWriter.hh"
#include "SDBoxContainer.hh"
#include "YamlConfig.hh"
#include "TimeControl.hh"

#include <set>
#include <string>

OutputFileWriter::OutputFileWriter()
{
    m_TopMetalHist.clear();
    m_TopMetalTree.clear();
    m_iEvent = -1;

    YAML::Node outNode = YamlConfig::GetInstance().GetYaml()["Output"];
    m_TotalTruth = outNode["TotalTruth"].as<bool>(false);
    m_FileName = outNode["FileName"].as<std::string>("Digi.root");
    m_File = new TFile(m_FileName, "recreate");
    m_SimEvent = new SimEvent();
    m_TruthTree = new TTree("TruthTree", "");
    m_TruthTree->Branch("TruthEnergy",              &m_TruthEnergy);
    m_TruthTree->Branch("TruthParticle",            &m_TruthParticle);
    m_TruthTree->Branch("TruthPosition",            &m_TruthPosition);
    m_TruthTree->Branch("TruthPolarization",        &m_TruthPolarization);
    m_TruthTree->Branch("TruthMomentumDirection",   &m_TruthMomentumDirection);
    m_TruthTree->Branch("TimeFlag",                 &m_TimeFlag);
    m_TruthTree->Branch("FirstSDStepTime",          &m_FirstSDStepTime);
    m_TruthTree->Branch("NumOfFiredSD",             &m_NumOfFiredSD,        "NumOfFiredSD/I");
    m_TruthTree->Branch("FiredSD",                  &m_FiredSD,             "FiredSD[NumOfFiredSD]/I");
    m_TruthTree->Branch("PolPhotElectronPos",       &m_PolPhotElectronPos);
    m_TruthTree->Branch("PolPhotElectronMom",       &m_PolPhotElectronMom);
    m_TruthTree->Branch("IfPolPhot",                &m_IfPolPhot);
    m_TruthTree->Branch("IfPolPhotOnMCP",           &m_IfPolPhotOnMCP);
    if(m_TotalTruth == true)
    {
        m_TruthTree->Branch("SimEvent", &m_SimEvent);
    }

    //m_File->cd();
    //m_ConfigTree = new TTree("ConfigTree", "");
    //m_ConfigTree->Branch("SingleParticleMode",      &m_Config.SingleParticleMode);
    //m_ConfigTree->Branch("TimeLineMode",            &m_Config.TimeLineMode);
    //m_ConfigTree->Branch("InputFiles",              &m_Config.InputFiles);
    //m_ConfigTree->Branch("TopMetalCalibFiles",      &m_Config.TopMetalCalibFiles);
    //m_ConfigTree->Branch("InputFilesTimeShift",     &m_Config.InputFilesTimeShift);
    //m_ConfigTree->Branch("OutputFileName",          &m_Config.OutputFileName);
    //m_ConfigTree->Branch("OutputTotalTruth",        &m_Config.OutputTotalTruth);
    //m_ConfigTree->Branch("OutputVerbose",           &m_Config.OutputVerbose);
    //m_ConfigTree->Branch("SaveAllFrame",            &m_Config.SaveAllFrame);
    //m_ConfigTree->Branch("RandomSeed",              &m_Config.RandomSeed);
    //m_ConfigTree->Branch("DriftSpeed",              &m_Config.DriftSpeed);
    //m_ConfigTree->Branch("DriftSpeedErr",           &m_Config.DriftSpeedErr);
    //m_ConfigTree->Branch("DriftSmear",              &m_Config.DriftSmear);
    //m_ConfigTree->Branch("InitialIonizationEnergy", &m_Config.InitialIonizationEnergy);
    //m_ConfigTree->Branch("MultiplicationFactor",    &m_Config.MultiplicationFactor);
    //m_ConfigTree->Branch("EToHoleEfficiency",       &m_Config.EToHoleEfficiency);
    //m_ConfigTree->Branch("MCPHoleDistance",         &m_Config.MCPHoleDistance);
    //m_ConfigTree->Branch("MCPHoleDiameter",         &m_Config.MCPHoleDiameter);
    //m_ConfigTree->Branch("PixelSize",               &m_Config.PixelSize);
    //m_ConfigTree->Branch("PixelTime",               &m_Config.PixelTime);
    //m_ConfigTree->Branch("NumOfPixel",              &m_Config.NumOfPixel);
    //m_ConfigTree->Branch("MCPTopMetalDistance",     &m_Config.MCPTopMetalDistance);
    //m_ConfigTree->Branch("TopMetalCollectingEfficiency", &m_Config.TopMetalCollectingEfficiency);
    //m_ConfigTree->Branch("ADCResponse",             &m_Config.ADCResponse);
    //m_ConfigTree->Branch("NumOfFramePerEvent",      &m_Config.NumOfFramePerEvent);
    //m_ConfigTree->Branch("ExitTime",                &m_Config.ExitTime);

    //m_Config = *(TimeControl::GetInstance().GetConfig());
    //m_ConfigTree->Fill();
}

OutputFileWriter::~OutputFileWriter()
{
    for (auto MapHist : m_TopMetalHist)
    {
        delete MapHist;
        MapHist = nullptr;
    }
    m_File->Close();
}

TH2F* OutputFileWriter::CreateTopMetalTree(UInt_t iTopMetal)
{
    if(iTopMetal != m_TopMetalTree.size())
    {
        std::cerr << "Input iTopMetal and Number of exist TopMetalTree not matched!!!" << std::endl;
        exit(EXIT_FAILURE);
    }
    if(iTopMetal >= MAXSD)
    {
        std::cerr << "OutputFileWriter: Number of SD overflow!!! Please enlarge MAXSD in OutputFileWriter.hh" << std::endl;

    }
    std::pair<UInt_t, UInt_t> numOfPixel = SDBoxContainer::GetInstance()[iTopMetal].NumOfPixel();
    m_File->cd();
    m_TopMetalTree.push_back(new TTree(TString::Format("TopMetal_%d", iTopMetal), ""));
    TH2F* th_TopMetal = new TH2F(TString::Format("th2_TopMetal_%d", iTopMetal), "", numOfPixel.first, 0, numOfPixel.first+1, numOfPixel.second, 0, numOfPixel.second+1);
    m_TopMetalHist.push_back(th_TopMetal);
    m_TopMetalTree[iTopMetal]->Branch("FrameWriteTime", &(m_FrameWriteTime[iTopMetal]));
    m_TopMetalTree[iTopMetal]->Branch("TopMetalHist", m_TopMetalHist[iTopMetal]);
    m_TopMetalTree[iTopMetal]->Branch("iEvent", &m_iEvent);
    m_TopMetalTree[iTopMetal]->Branch("iFrame", &m_iFrame[iTopMetal]);
    m_TopMetalTree[iTopMetal]->Branch("iGMCP", &m_iGMCP[iTopMetal]);

    m_GMCPTree.push_back(new TTree(TString::Format("GMCP_%d", iTopMetal), ""));
    m_GMCPTree[iTopMetal]->Branch("iEvent", &m_iEvent);
    m_GMCPTree[iTopMetal]->Branch("iFrame", &m_iFrame[iTopMetal]);
    m_GMCPTree[iTopMetal]->Branch("GMCPTime", &m_GMCPTime[iTopMetal]);
    m_GMCPTree[iTopMetal]->Branch("GMCPAmp", &m_GMCPAmp[iTopMetal]);
    m_iGMCP[iTopMetal] = 0;

    return m_TopMetalHist[iTopMetal];
}

void OutputFileWriter::WriteTruthInfo(SimEvent* aEvent, TruthInfo* truthInfo)
{
    //std::cout << "Write Truth Info at Time " << FirstSDStepTime << std::endl;
    m_TruthEnergy = aEvent->GetTruthEnergy();
    m_TruthParticle = aEvent->GetTruthParticle();
    m_TruthPosition = aEvent->GetTruthPosition();
    m_TruthPolarization = aEvent->GetTruthPolarization();
    m_TruthMomentumDirection = aEvent->GetTruthMomentumDirection();
    m_TimeFlag = aEvent->GetTimeFlag();
    m_FirstSDStepTime = truthInfo->FirstSDStepTime;

    m_NumOfFiredSD = 0;
    for (auto iSD : truthInfo->FiredSD)
    {
        m_FiredSD[m_NumOfFiredSD] = iSD;
        ++m_NumOfFiredSD;
    }
    m_IfPolPhot = truthInfo->IfPolPhot;
    m_IfPolPhotOnMCP = truthInfo->IfPolPhotOnMCP;
    m_PolPhotElectronPos = truthInfo->PolPhotElectronPos;
    m_PolPhotElectronMom = truthInfo->PolPhotElectronMom;

    if(m_TotalTruth == true)
    {
        m_SimEvent->MyClear();
        *m_SimEvent = *aEvent;
    }

    m_TruthTree->Fill();
    ++m_iEvent;
}

void OutputFileWriter::WriteGMCP(UInt_t iTopMetal, Double_t GMCPTime, Double_t GMCPAmp)
{
    m_GMCPTime[iTopMetal] = GMCPTime;
    m_GMCPAmp[iTopMetal] = GMCPAmp;

    m_GMCPTree[iTopMetal]->Write();
    ++m_iGMCP[iTopMetal];
}

void OutputFileWriter::WriteFrame(UInt_t iTopMetal, Double_t FrameWriteTime, UInt_t iFrame)
{
    //std::cout << "Write Frame of TopMetal " << iTopMetal << std::endl;
    m_FrameWriteTime[iTopMetal] = FrameWriteTime;
    m_iFrame[iTopMetal] = iFrame;

    m_TopMetalTree[iTopMetal]->Fill();
    if(TimeControl::Verbose())
    {
        std::cout << "OutputFileWriter::WriteFrame(): TopMetal: " << iTopMetal << "\tFrame: " << iFrame << "\tTime: " << FrameWriteTime << std::endl;
    }
}

void OutputFileWriter::SaveAll()
{
    if(TimeControl::Verbose()) std::cout << "OutputFileWriter::SaveAll(): Save all info." << std::endl;
    m_File->cd();
    //m_ConfigTree->AutoSave();
    m_TruthTree->AutoSave();
    for(auto iTMTree:m_TopMetalTree)
    {
        m_File->cd();
        iTMTree->AutoSave();
    }
    for(auto iGMCPTree:m_GMCPTree)
    {
        m_File->cd();
        iGMCPTree->AutoSave();
    }
}