//*********************************************
//  This is Geant4 Template
//                                  author:Qian
//

#include "MyParticleGun.hh"
#include "MyParticleGunMessenger.hh"

#include "G4RunManager.hh"
#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"
#include "Verbose.hh"

#include "MyDetectorConstruction.hh"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"
#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#pragma GCC diagnostic pop

#include <map>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
// Readme
/*
   目前particleGun提供了3种方式
   1. GunType=0, mac命令来选择, /MyGun/SimpleGun
   2. GunType=1, mac命令来选择, /MyGun/SetGunBGFile **root。读取root文件，按root文件来设置
   3. GunType=2, mac命令来选择, /MyGun/Cry/file setup.file。调用宇宙线cry包，按setup设置
*/
MyParticleGun::MyParticleGun(MyDetectorConstruction *det)
    : G4VUserPrimaryGeneratorAction(),
      fParticleGun(0),
      fDetector(det)
{
    if (verbose)
        G4cout << "====>MyParticleGun::MyParticleGun()" << G4endl;

    fGunMessenger = new MyParticleGunMessenger(this);

    G4int n_particle = 1;
    fParticleGun = new G4ParticleGun(n_particle);
    particleTable = G4ParticleTable::GetParticleTable();

    //Particle Ptr
    G4String ParticleList[4] = {"gamma", "e+", "e-", "proton"};
    for (int i = 0; i < 4; ++i)
        mapGenParticlePtr[ParticleList[i]] = particleTable->FindParticle(ParticleList[i]);

    //Get Cosmic Ray Generator
    fCosmicRayList = CosmicRayList::GetInstance();
    fCosmicRay = fCosmicRayList->GetGenerator();
    fConfigPS = fCosmicRayList->GetPGPSConfig();
    fCosmicRay->Initialize(fConfigPS.GenTime, fConfigPS.ParticleGunParameters);

    //#PartGun 2. 初始化变量
    //以下这些参数：粒子种类/能量/位置/动量等可以被mac文件重置

    G4cout << __LINE__ <<  fCosmicRay->GetParticle() << G4endl;
    if (fConfigPS.ParticleGunType != G4String("Simple"))
    {
        fParticleGun->SetParticleDefinition(mapGenParticlePtr[fCosmicRay->GetParticle()]);
    }
    else
    {
        G4ParticleDefinition *particle = particleTable->FindParticle(fCosmicRay->GetParticle());

        fParticleGun->SetParticleDefinition(particle);
        fParticleGun->SetParticleEnergy(fCosmicRay->GetParticleEnergy());
        fParticleGun->SetParticlePosition(fCosmicRay->GetParticlePosition());
        fParticleGun->SetParticlePolarization(fCosmicRay->GetParticlePolarization());
        fParticleGun->SetParticleMomentumDirection(fCosmicRay->GetParticleMomentumDirection());
    }

    //GunType的值也会被mac文件的命令重置
    GunType = 0; //a simple flag for gun type: 0 for simple gun, 1 for read from root file. 2 for cosmic-ray-package

    //以下为cry的初始化变量
    /*
    InputState = -1;
    // create a vector to store the CRY particle properties
    vect = new std::vector<CRYParticle *>;
    */
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//GunType=1，根据rootfile分布来设置
void MyParticleGun::OpenRootFile(G4String)
{
    GunType = 1;

    // 从用户提供的root文件来生成粒子，需要根据root文件来修改下面的内容。
    //
    /*
    rootfile = new TFile(fname);
    if (!rootfile->IsOpen())
    {
        G4cout << "####> Can't open the " << fname << ", will use simple-Gun instead." << G4endl;
        GunType = 0;
        return;
    }

    tree = (TTree *)rootfile->Get("datatree");
    tree->SetBranchAddress("XPosition", &x);
    tree->SetBranchAddress("YPosition", &y);
    tree->SetBranchAddress("ZPosition", &z);
    tree->SetBranchAddress("XMomentum", &px);
    tree->SetBranchAddress("YMomentum", &py);
    tree->SetBranchAddress("ZMomentum", &pz);
    tree->SetBranchAddress("BackgroundFrequency", &freq);

    Nentries = tree->GetEntries();
    */
}

/*
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//GunType=2, CRY：cosmic ray package的函数
//----------------------------------------------------------------------------//
void MyParticleGun::InputCRY()
{
    InputState = 1;
}

//----------------------------------------------------------------------------//
void MyParticleGun::UpdateCRY(std::string *MessInput)
{
    CRYSetup *setup = new CRYSetup(*MessInput, "../packages/cry_v1.7/data/");

    gen = new CRYGenerator(setup);

    // set random number generator
    RNGWrapper<CLHEP::HepRandomEngine>::set(CLHEP::HepRandom::getTheEngine(), &CLHEP::HepRandomEngine::flat);
    setup->setRandomFunction(RNGWrapper<CLHEP::HepRandomEngine>::rng);
    InputState = 0;
    GunType = 2;
}

//----------------------------------------------------------------------------//
void MyParticleGun::CRYFromFile(G4String newValue)
{
    // Read the cry input file
    std::ifstream inputFile;
    inputFile.open(newValue, std::ios::in);
    char buffer[1000];

    if (inputFile.fail())
    {
        G4cout << "Failed to open input file " << newValue << G4endl;
        G4cout << "Make sure to define the cry library on the command line" << G4endl;
        InputState = -1;
        GunType = 0;
    }
    else
    {
        std::string setupString("");
        while (!inputFile.getline(buffer, 1000).eof())
        {
            setupString.append(buffer);
            setupString.append(" ");
        }

        CRYSetup *setup = new CRYSetup(setupString, "../packages/cry_v1.7/data/");

        gen = new CRYGenerator(setup);

        // set random number generator
        RNGWrapper<CLHEP::HepRandomEngine>::set(CLHEP::HepRandom::getTheEngine(), &CLHEP::HepRandomEngine::flat);
        setup->setRandomFunction(RNGWrapper<CLHEP::HepRandomEngine>::rng);
        InputState = 0;
        GunType = 2;
    }
}
*/

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

MyParticleGun::~MyParticleGun()
{
    delete fParticleGun;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void MyParticleGun::GeneratePrimaries(G4Event *anEvent)
{
    if (verbose)
        G4cout << "====>MyParticleGun::GeneratePrimaries()" << G4endl;

    //#PartGun 3. 对粒子枪进行抽样
    if (GunType == 0) //根据mac或者XML里的定义产生
    {
        if (fConfigPS.PGEnable)
            fCosmicRay->Generate();
        if (fConfigPS.PGEnable && (fConfigPS.ParticleGunType != G4String("Simple")))
        {
            fParticleGun->SetParticleDefinition(mapGenParticlePtr[fCosmicRay->GetParticle()]);
            fParticleGun->SetParticleEnergy(fCosmicRay->GetParticleEnergy());
            fParticleGun->SetParticlePosition(fCosmicRay->GetParticlePosition());
            fParticleGun->SetParticlePolarization(fCosmicRay->GetParticlePolarization());
            fParticleGun->SetParticleMomentumDirection(fCosmicRay->GetParticleMomentumDirection());
        }
        else if(fConfigPS.PGEnable && (fConfigPS.ParticleGunType == G4String("Simple"))){
            fParticleGun->SetParticlePolarization(fCosmicRay->GetParticlePolarization());
            if(fConfigPS.IsArea ==G4String("yes")){
                fParticleGun->SetParticlePosition(fCosmicRay->GetParticlePosition());
            }
            if(fConfigPS.IsRandSphere ==G4String("yes")){
                fParticleGun->SetParticlePosition(fCosmicRay->GetParticlePosition());
                fParticleGun->SetParticleMomentumDirection(fCosmicRay->GetParticleMomentumDirection());
            }
        }
        //如果xml里定义里particleGun，那么用xml里的参数初始化
        //else if (fDetector->GetPrimaryDescription() != 0)
        //{
        //    particleTable = G4ParticleTable::GetParticleTable();
        //    G4ParticleDefinition *particle = particleTable->FindParticle(fDetector->GetPrimaryDescription()->particle);
        //    if (particle != NULL)
        //        fParticleGun->SetParticleDefinition(particle);

        //    fParticleGun->SetParticlePosition(fDetector->GetPrimaryDescription()->particlePos);
        //    fParticleGun->SetParticleEnergy(fDetector->GetPrimaryDescription()->particleMom.mag());
        //    fParticleGun->SetParticleMomentumDirection(fDetector->GetPrimaryDescription()->particleMom);
        //    fParticleGun->SetParticlePolarization(fDetector->GetPrimaryDescription()->particlePol);
        //}

        //也可以根据需要来自己定义参数来初始化，下面的例子是一个2pi的平面光
        /*
        {
            double Eng = 3 * eV; //~400nm
            double x, y, z;
            z = -200;
            x = G4UniformRand() * 20 - 10; //-1~1
            y = G4UniformRand() * 20 - 10; //-1~1

            double px, py, pz;
            double phi, theta;
            double pi = 3.1415926;
            theta = G4UniformRand() * pi / 2;
            phi = G4UniformRand() * pi * 2;
            pz = cos(theta);
            px = sin(theta) * sin(phi);
            py = sin(theta) * cos(phi);

            fParticleGun->SetParticleEnergy(Eng);
            fParticleGun->SetParticlePosition(G4ThreeVector(x, y, z));
            fParticleGun->SetParticleMomentumDirection(G4ThreeVector(px, py, pz));
        }
        */

        fParticleGun->GeneratePrimaryVertex(anEvent);
    }
    else if (GunType == 1) //根据root文件产生
    {
        /*
        G4int i = anEvent->GetEventID();
        if (i < Nentries)
            tree->GetEntry(i);
        if (i % 1000 == 0)
            G4cout << i << G4endl;

        G4ParticleTable *particleTable = G4ParticleTable::GetParticleTable();
        G4ParticleDefinition *particle = particleTable->FindParticle("e-");

        fParticleGun->SetParticleDefinition(particle);
        fParticleGun->SetParticleEnergy(sqrt(px * px + py * py + pz * pz));
        fParticleGun->SetParticlePosition(G4ThreeVector(x, y, z));
        fParticleGun->SetParticleMomentumDirection(G4ThreeVector(px, py, pz));
        //G4cout<<"ID: "<<i<<": "<<sqrt(px * px + py * py + pz * pz)<<G4endl;
        */
        fParticleGun->GeneratePrimaryVertex(anEvent);
    }
    else //调用cry产生宇宙线
    {
        /*
        if (InputState != 0)
        {
            G4String *str = new G4String("CRY library was not successfully initialized");
            G4Exception("PrimaryGeneratorAction", "1",
                        RunMustBeAborted, *str);
        }
        G4String particleName;
        vect->clear();
        gen->genEvent(vect);

        //....debug output
        G4cout << "\nEvent=" << anEvent->GetEventID() << " "
               << "CRY generated nparticles=" << vect->size()
               << G4endl;

        for (unsigned j = 0; j < vect->size(); j++)
        {
            particleName = CRYUtils::partName((*vect)[j]->id());

            //....debug output
            std::cout << "  " << particleName << " "
                      << "charge=" << (*vect)[j]->charge() << " "
                      << std::setprecision(4)
                      << "energy (MeV)=" << (*vect)[j]->ke() * MeV << " "
                      << "pos (m)"
                      << G4ThreeVector((*vect)[j]->x(), (*vect)[j]->y(), (*vect)[j]->z())
                      << " "
                      << "direction cosines "
                      << G4ThreeVector((*vect)[j]->u(), (*vect)[j]->v(), (*vect)[j]->w())
                      << " " << std::endl;

            fParticleGun->SetParticleDefinition(particleTable->FindParticle((*vect)[j]->PDGid()));
            fParticleGun->SetParticleEnergy((*vect)[j]->ke() * MeV);
            fParticleGun->SetParticlePosition(G4ThreeVector((*vect)[j]->x() * m, (*vect)[j]->y() * m, (*vect)[j]->z() * m));
            fParticleGun->SetParticleMomentumDirection(G4ThreeVector((*vect)[j]->u(), (*vect)[j]->v(), (*vect)[j]->w()));
            fParticleGun->SetParticleTime((*vect)[j]->t());
            fParticleGun->GeneratePrimaryVertex(anEvent);
            delete (*vect)[j];
        }
        */
    }
}
