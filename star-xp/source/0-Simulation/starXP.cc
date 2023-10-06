//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
// --------------------------------------------------------------
//      GEANT 4 - main
//
// --------------------------------------------------------------

#include <vector>
#include <string>
#include <algorithm>
#include <regex>
#include <sys/stat.h>

#include "MyPhysicsList.hh"
#include "MyActionInitialization.hh"
#include "MyAnalysisManager.hh"
#include "MyDetectorConstruction.hh"
#include "MyRunManager.hh"
#include "CosmicRayList.hh"

#include "G4UImanager.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"
#include "G4GDMLParser.hh"
#include "G4VModularPhysicsList.hh"
#include "G4StepLimiterPhysics.hh"
#include "G4ThreeVector.hh"
#include "Randomize.hh"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"
#include "TSystem.h"
#include "TString.h"
#include "TRandom.h"
#include "TEnv.h"
#pragma GCC diagnostic pop
#include "QBBC.hh"

//template 缺省值：
// 1. 关闭verbose输出，关闭material输出
// 2. 采用particle gun，1GeV/c，电子，(1,0,0)方向出射
// 3. 开启光学过程，采用G4OpticalPhysics
// 4. 若用MyRadiatorDescription定义了穿越辐射探测器结构，则开启TRD物理过程
// 5. 若用XML开启了磁场，则用用户定义的磁场；否则用全局磁场
// 6. 采用G4AnalysisManager保存数据

//starXP 设置：
//

//#Verb 1. 打开输出verbose控制
G4int verbose = 0;

//arglist control variables
PGPSConfig ConfigPS;
OutputControl ControlOutput;
G4String GDMLFile = ""; //GDML file for detector
Int_t RandomSeed_bash = -1;

void PrintUsage()
{
    G4cout << "**************************************************************" << G4endl;
    G4cout << "**************************************************************" << G4endl;
    G4cout << "********** Welcome to Star-XP Simulation Framework! **********" << G4endl;
    G4cout << "********** Usage: ./starXP [-v] [SimuCard.card]     **********" << G4endl;
    G4cout << "**************************************************************" << G4endl;
    G4cout << "**************************************************************" << G4endl;
}

bool IfFileExist(G4String FilePath)
{
    struct stat buffer;   
    return (stat(FilePath.data(), &buffer) == 0);
}

std::vector<std::string> SplitStr(const char* Str, const char* Split)
{
    std::string VStr(Str);
    std::vector<std::string> Vector;
    std::regex Delimiter(Split);
    std::sregex_token_iterator iter(VStr.begin(), VStr.end(), Delimiter, -1);
    std::sregex_token_iterator end;
    while (iter != end)
    {   
        Vector.push_back(std::string(*iter));
        ++iter;
    }
    return Vector;
}

G4String GetDetectorName(G4String GDMLFile)
{
    std::vector<std::string> StrVector = SplitStr(GDMLFile.data(), "/");
    return G4String(SplitStr(StrVector.back().c_str(), "\\.")[0]);
}

std::vector<G4double> ConvertStrToVectordouble(const char* Str)
{
    std::vector<double> VectorDouble;
    VectorDouble.clear();
    if(Str == "")
        return VectorDouble;
    std::vector<std::string> StrVector = SplitStr(Str, ",");
    for(auto str:StrVector)
    {
        if(!(std::isdigit((str.c_str()[0])) || (str.c_str()[0] == '-')))
            throw "ConvertStrToVectordouble: Input str is not a vaild number!";
        VectorDouble.push_back(std::stod(str));
    }
    return VectorDouble;
}

G4ThreeVector ConvertStrToG4ThreeVector(const char* Str)
{
    std::vector<G4double> VectorDouble = ConvertStrToVectordouble(Str);
    if(VectorDouble.size() != 3)
        throw "ConvertStrToG4ThreeVector: Length of input and output number not matched!";
    G4ThreeVector ThreeVector(VectorDouble[0], VectorDouble[1], VectorDouble[2]);
    return ThreeVector;
}

std::pair<bool, G4ThreeVector> GetThreeVector(TEnv *Env, G4String Name)
{
    std::pair<bool, G4ThreeVector> value(false, G4ThreeVector(0., 0., 0.));
    try
    {
        value.second = ConvertStrToG4ThreeVector(Env->GetValue(Name.data(), "0.0,0.0,0.0"));
    }
    catch (const char *msg)
    {
        G4cerr << msg << G4endl;
        G4cerr << "Please check your \" " << Name.data() << " \" config" << G4endl;
        return value;
    }
    value.first = true;
    return value;
}

bool ConfigParticleGun(TEnv *Env)
{
    G4cout << "Choose to Use ParticleGun" << G4endl;
    ConfigPS.PGEnable = true;
    ConfigPS.PSEnable = false;

    ConfigPS.GenTime = Env->GetValue("GenTime", 0.0);
    ConfigPS.GenEvents = Env->GetValue("GenEvents", 0);
    ConfigPS.GenValidEvents = Env->GetValue("GenValidEvents", 0);
    ConfigPS.OnlyValid = (Env->GetValue("OnlySaveValidEvent", 1) != 0);
    if ((!(ConfigPS.GenTime||ConfigPS.GenEvents||ConfigPS.GenValidEvents)) || (ConfigPS.GenTime&&ConfigPS.GenEvents) || (ConfigPS.GenTime&&ConfigPS.GenValidEvents) || (ConfigPS.GenEvents&&ConfigPS.GenValidEvents))
    {
        G4cerr << "There mush be one and only one variable in GenTime, GenEvents and GenValidEvents could be initialiezd." << G4endl;
        return false;
    }

    ConfigPS.ParticleGunParameters = SplitStr(Env->GetValue("PGParameters", ""), ",");
    if(ConfigPS.ParticleGunParameters[0] == "")
        ConfigPS.ParticleGunParameters.clear();

    ConfigPS.AdditionalMACCommand = Env->GetValue("AdditionalMACCommand", "0");
    if (ConfigPS.AdditionalMACCommand != G4String("0") && !IfFileExist(ConfigPS.AdditionalMACCommand))
    {
        G4cerr << "Error!!! MAC File \" " << ConfigPS.AdditionalMACCommand << " \" NOT FOUND!!!" << G4endl;
        return false;
    }

    if (ConfigPS.ParticleGunType == G4String("Simple"))
    {
        G4cout << "Use Simple ParticleGun" << G4endl;

        ConfigPS.ParticleName = Env->GetValue("PGName", "e+");
        ConfigPS.ParticleEnergy = Env->GetValue("PGEnergy", 1.0);

        G4String ReadThreeVector[5] = {"PGPos", "PGPol", "PGMomDir","RandParameters","StokesParameters"};
        G4ThreeVector ThreeVector[5];
        for (int i = 0; i < 5; ++i)
        {
            std::pair<bool, G4ThreeVector> pair3V = GetThreeVector(Env, ReadThreeVector[i].data());
            if(!pair3V.first) return false;
            else ThreeVector[i] = pair3V.second;
        }
        ConfigPS.ParticlePosition = ThreeVector[0];
        ConfigPS.ParticlePolarization = ThreeVector[1];
        ConfigPS.ParticlePolarizationFactor = Env->GetValue("ParticlePolarizationFactor", -1.);
        ConfigPS.ParticleMomentumDirection = ThreeVector[2];
        ConfigPS.IsArea  = Env->GetValue("IsArea","no");
        ConfigPS.IsRandSphere = Env->GetValue("IsRandSphere","no");
        ConfigPS.IsStokes = Env->GetValue("IsStokes","no");
        if(ConfigPS.IsArea == G4String("yes")){
            ConfigPS.RandomArea = ThreeVector[3];
        }
        if(ConfigPS.IsStokes == G4String("yes")){
            ConfigPS.StokesParameters = ThreeVector[4];
        }

        G4cout << "Particle Type              : " << ConfigPS.ParticleName << G4endl;
        G4cout << "Particle Energy            : " << ConfigPS.ParticleEnergy << " MeV" << G4endl;
        G4cout << "Particle Position          : " << ConfigPS.ParticlePosition << G4endl;
        G4cout << "Particle Polarization      : " << ConfigPS.ParticlePolarization << G4endl;
        G4cout << "Particle Momentum Direction: " << ConfigPS.ParticleMomentumDirection << G4endl;
        G4cout << "Set Random Area: "<< ConfigPS.IsArea <<G4endl;
        G4cout << "Set Stokes Parameter Model: "<< ConfigPS.IsStokes <<G4endl;
    }
    return true;
}

bool ConfigGPS(TEnv* Env, G4String InitCard)
{
    ConfigPS.PSEnable = true;
    ConfigPS.PGEnable = false;
    if (!IfFileExist(ConfigPS.PSSignal))
    {
        G4cerr << "Error!!! MAC File \" " << ConfigPS.PSSignal << " \" NOT FOUND!!!"  << G4endl;
        G4cerr << "Please check your config file \" " << InitCard << " \""  << G4endl;
        return false;
    }
    ConfigPS.ParticleGunType = G4String("GPS");
    ConfigPS.OnlyValid = (Env->GetValue("OnlySaveValidEvent", 1) != 0);
    return true;
}

bool ArgListControl(G4String InitCard)
{
    TEnv* Env = new TEnv(InitCard.data());

    //Particle Gun or GPS
    ConfigPS.ParticleGunType = G4String(Env->GetValue("PGType", "0"));
    ConfigPS.PSSignal     = G4String(Env->GetValue("PSSignal", "0"));
    if(ConfigPS.ParticleGunType == G4String("0") && ConfigPS.PSSignal == G4String("0"))
    {
        G4cerr << "Please either initialize ParticleGun or GPS!!!"  << G4endl;
        return false;
    }
    else if(ConfigPS.ParticleGunType != G4String("0") && ConfigPS.PSSignal != G4String("0"))
    {
        G4cerr << "Cannot initialize both ParticleGun and GPS at the same time!!!"  << G4endl;
        return false;
    }
    else if(ConfigPS.ParticleGunType != G4String("0") && ConfigPS.PSSignal == G4String("0"))
    {
        if (!ConfigParticleGun(Env)) return false;
    }
    else if(ConfigPS.ParticleGunType == G4String("0") && ConfigPS.PSSignal != G4String("0"))
    {
        if (!ConfigGPS(Env, InitCard)) return false;
    }

    //Detector GDML files
    GDMLFile = G4String(Env->GetValue("DetectorGDML", "gdml/main.gdml"));
    if (!IfFileExist(GDMLFile))
    {
        G4cerr << "Error!!! Detector GDML File \" " << GDMLFile << " \" NOT FOUND!!!"  << G4endl;
        G4cerr << "Please check your config file \" " << InitCard << " \""  << G4endl;
        return false;
    }

    //Output config
    ControlOutput.DetectorName = GetDetectorName(GDMLFile);
    //std::vector<G4String> DetectorList{"CoinSys", "CubeSat_det_v01", "Test2"};
    //if(!std::count(DetectorList.begin(), DetectorList.end(), ControlOutput.DetectorName))
    //{
    //    G4cerr << "Error!!! " << ControlOutput.DetectorName <<  " is not a validate Detector Name !!!"  << G4endl;
    //    return false;
    //}
    G4cout << "Create output tuple for detector " << ControlOutput.DetectorName << "." << G4endl;

    ControlOutput.OutputLevel = G4int(Env->GetValue("OutputLevel", 1));
    if (ControlOutput.OutputLevel < 1) 
    {
        G4cout << "Warning!!! Output level " << ControlOutput.OutputLevel << "\" is lower than 1, reset to 1."  << G4endl;
        ControlOutput.OutputLevel = 1;
    }
    if (ControlOutput.OutputLevel > 3)
    {
        G4cout << "Warning!!! Output level " << ControlOutput.OutputLevel << "\" is higher than 3, reset to 3."  << G4endl;
        ControlOutput.OutputLevel = 3;
    }
    ControlOutput.OutputFile = G4String(Env->GetValue("OutputFile", "sim.root"));
    if (G4String(SplitStr(ControlOutput.OutputFile.data(), "\\.").back()) != G4String("root"))
    {
        G4cerr << "Error!!! Output File name \" " << ControlOutput.OutputFile << " \" Should end with \" .root \"!!!"  << G4endl;
        return false;
    }

    //Set Random Seed
    G4int RandomSeed = Env->GetValue("RandomSeed", 2022);
    if(RandomSeed_bash != -1)
    {
        RandomSeed = RandomSeed_bash;
    }
    G4cout << "Set Random Seed to: " << RandomSeed << G4endl;
    G4Random::setTheEngine(new CLHEP::MTwistEngine);
    G4Random::setTheSeed(RandomSeed);
    gRandom->SetSeed(RandomSeed);

    return true;
}

//main function
int main(int argc, char **argv)
{
    G4String InitCard = "SimuCard.card";
    bool UseDefaultCard = true;
    G4UIExecutive *ui = NULL;
    // Add arglist control
    PrintUsage();
    if (argc >= 2)
    {
        for(int iA = 1; iA < argc; ++iA)
        {
            if(G4String(argv[iA][0]) == G4String("-"))
            {
                if(G4String(argv[iA]) == G4String("-v"))
                {
                    G4cout << " Use interactive mode" << G4endl;
                    ui = new G4UIExecutive(argc, argv);
                }
                if (std::string(argv[iA]) == std::string("-s"))
                {
                    RandomSeed_bash = std::stoi(argv[iA + 1]);
                    ++iA;
                }
            }
            else if(SplitStr(argv[iA], "\\.").back() == std::string("card"))
            {
                InitCard = G4String(argv[iA]);
                UseDefaultCard = false;
            }
        }
    }
    if(UseDefaultCard)
    {
        G4cout << "Warning!!! Using default " << InitCard << G4endl;
        if (!IfFileExist(InitCard))
        {
            G4cerr << "Error!!! Initialize card \" " << InitCard << " \" NOT FOUND!!!" << G4endl;
            return 1;
        }
    }
    if (!ArgListControl(InitCard))
        return 1;

    //
    // Detect interactive mode (if only one argument) and define UI session
    //

    //Set Particle Source
    CosmicRayList* fCosmicRayList = CosmicRayList::GetInstance();
    fCosmicRayList->SetPGPSConfig(ConfigPS);

    // Detector construction
    MyRunManager *runManager = new MyRunManager;
    //MyDetectorConstruction *detector = new MyDetectorConstruction();
    MyDetectorConstruction *detector = new MyDetectorConstruction(GDMLFile);
    runManager->SetUserInitialization(detector);

    // PhysicsList
    runManager->SetUserInitialization(new MyPhysicsList(detector));
    //runManager->SetUserInitialization(new QBBC);

    //Set Output info
    //MyAnalysisManager *analysisManager = new MyAnalysisManager();
    MyAnalysisManager *analysisManager = MyAnalysisManager::GetInstance();
    analysisManager->SetOutputInfo(ControlOutput);

    // User action initialization
    runManager->SetUserInitialization(new MyActionInitialization(detector));
    runManager->Initialize();

    // Initialize visualization
    G4VisManager *visManager = NULL;
    // Get the pointer to the User Interface manager
    G4UImanager *UImanager = G4UImanager::GetUIpointer();

    if (!ui) // batch mode
    {
        if (ConfigPS.PGEnable)
        {
            if (ConfigPS.AdditionalMACCommand != G4String("0"))
            {
                G4String command = "/control/execute ";
                UImanager->ApplyCommand(command + ConfigPS.AdditionalMACCommand);
            }
            runManager->GenEvents(ConfigPS.GenTime, ConfigPS.GenEvents, ConfigPS.GenValidEvents);
        }
        if (ConfigPS.PSEnable)
        {
            G4String command = "/control/execute ";
            UImanager->ApplyCommand(command + ConfigPS.PSSignal);
        }
    }
    else // interactive mode
    {
        visManager = new G4VisExecutive;
        visManager->Initialize();
        UImanager->ApplyCommand("/control/execute vis.mac");
        ui->SessionStart();
        delete ui;
        delete visManager;
    }

    delete runManager;
}
