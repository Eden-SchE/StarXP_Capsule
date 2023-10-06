//*********************************************
//  This is Geant4 Template
//                                  author:Qian
//

#include "MyPhysListTRD.hh"
#include "MyDetectorConstruction.hh"
#include "G4TransitionRadiation.hh"
#include "G4ForwardXrayTR.hh"
#include "G4VXTRenergyLoss.hh"
#include "G4ProcessManager.hh"
#include "G4Electron.hh"
#include "G4Positron.hh"
#include "G4PionMinus.hh"
#include "G4PionPlus.hh"
#include "G4KaonMinus.hh"
#include "G4KaonPlus.hh"
#include "G4Proton.hh"
#include "G4AntiProton.hh"

#include "G4VXTRenergyLoss.hh"
#include "G4RegularXTRadiator.hh"
#include "G4TransparentRegXTRadiator.hh"
#include "G4GammaXTRadiator.hh"
#include "G4StrawTubeXTRadiator.hh"

#include "G4XTRGammaRadModel.hh"
#include "G4XTRRegularRadModel.hh"
#include "G4XTRTransparentRegRadModel.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

MyPhysListTRD::MyPhysListTRD(G4int verb, MyDetectorConstruction *ptr)
    : G4VPhysicsConstructor("XTR"),
      fDetector(ptr),
      fVerbose(verb),
      processName("ForwardXrayTR"),
      //#PhysTRD 6. 选择一个TRD的物理模型.
      /* G4提供的TRD模型共有7种，分别为。
         1. "gammaR" = G4GammaXTRadiator;  "gammaM" = G4XTRGammaRadModel; 描述不规则辐射体，如泡沫/纤维等
         2. "regR" = G4RegularXTRadiator;  "regM" = G4XTRRegularRadModel; 描述规则辐射体
         3. "transpR" = G4TransparentRegXTRadiator; "transpM" = G4XTRTransparentRegRadModel; 描述吸收少的规则辐射体
         4. "strawR" = G4StrawTubeXTRadiator; 稻草管，需要定义三种材料：辐射体/稻草管/气体，暂时没包含在内。
         前三种的第一个是产生TRD并模拟光子产生的位置，不管光子的吸收，光子的吸收是通过光子的物理过程去模拟的; 
         前三种的第二个是产生TRD并模拟光子产生的位置，并考虑光子的吸收，给出吸收后在辐射体最后一层处还剩下的光子分布;
      */
      fXTRModel("transpM")
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

MyPhysListTRD::~MyPhysListTRD()
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void MyPhysListTRD::ConstructProcess()
{
    if ("dummy" == fXTRModel)
    {
        return;
    }
    if (0 < fVerbose)
    {
        G4cout << "MyPhysListTRD: XTR model <" << fXTRModel
               << ">" << G4endl;
    }
    RadiatorDescription *rDescription = fDetector->GetRadiatorDescription();
    if (rDescription == NULL)
    {
        G4cout << "#ERROR: Detector is not constructed with radiator description." << G4endl;
        return;
    }

    if (fXTRModel == "gammaR")
    {

        fXTRProcess = new G4GammaXTRadiator(rDescription->fLogicalVolume,
                                            100., 100.,
                                            rDescription->fFoilMaterial,
                                            rDescription->fGasMaterial,
                                            rDescription->fFoilThickness,
                                            rDescription->fGasThickness,
                                            rDescription->fFoilNumber,
                                            "GammaXTRadiator");
    }
    else if (fXTRModel == "gammaM")
    {
        fXTRProcess = new G4XTRGammaRadModel(rDescription->fLogicalVolume,
                                             100., 100.,
                                             rDescription->fFoilMaterial,
                                             rDescription->fGasMaterial,
                                             rDescription->fFoilThickness,
                                             rDescription->fGasThickness,
                                             rDescription->fFoilNumber,
                                             "XTRgammaRadiator");
    }
    else if (fXTRModel == "strawR")
    {
        fXTRProcess = new G4StrawTubeXTRadiator(rDescription->fLogicalVolume,
                                                rDescription->fFoilMaterial,
                                                rDescription->fGasMaterial,
                                                0.53,
                                                3.14159,
                                                rDescription->fAbsorberMaterial,
                                                true,
                                                "StrawTubeXTRadiator");
    }
    else if (fXTRModel == "regR")
    {
        fXTRProcess = new G4RegularXTRadiator(rDescription->fLogicalVolume,
                                              rDescription->fFoilMaterial,
                                              rDescription->fGasMaterial,
                                              rDescription->fFoilThickness,
                                              rDescription->fGasThickness,
                                              rDescription->fFoilNumber,
                                              "RegularXTRadiator");
    }
    else if (fXTRModel == "regM")
    {
        fXTRProcess = new G4XTRRegularRadModel(rDescription->fLogicalVolume,
                                               rDescription->fFoilMaterial,
                                               rDescription->fGasMaterial,
                                               rDescription->fFoilThickness,
                                               rDescription->fGasThickness,
                                               rDescription->fFoilNumber,
                                               "XTRegularRadiator");
    }
    else if (fXTRModel == "transpR")
    {
        fXTRProcess = new G4TransparentRegXTRadiator(rDescription->fLogicalVolume,
                                                     rDescription->fFoilMaterial,
                                                     rDescription->fGasMaterial,
                                                     rDescription->fFoilThickness,
                                                     rDescription->fGasThickness,
                                                     rDescription->fFoilNumber,
                                                     "TransparentRegXTRadiator");
    }
    else if (fXTRModel == "transpM")
    {
        fXTRProcess = new G4XTRTransparentRegRadModel(rDescription->fLogicalVolume,
                                                      rDescription->fFoilMaterial,
                                                      rDescription->fGasMaterial,
                                                      rDescription->fFoilThickness,
                                                      rDescription->fGasThickness,
                                                      rDescription->fFoilNumber,
                                                      "XTTransparentRegRadiator");
    }
    if (!fXTRProcess)
    {
        if (0 < fVerbose)
        {
            G4cout << "MyPhysListTRD: XTR model <" << fXTRModel
                   << "> is not known - no XTR process defined" << G4endl;
        }
        return;
    }

    fXTRProcess->SetVerboseLevel(fVerbose);

    G4Electron *elec = G4Electron::Electron();
    G4ProcessManager *manager = elec->GetProcessManager();
    manager->AddDiscreteProcess(fXTRProcess);

    G4Positron *posi = G4Positron::Positron();
    manager = posi->GetProcessManager();
    manager->AddDiscreteProcess(fXTRProcess);

    G4PionPlus *pip = G4PionPlus::PionPlus();
    manager = pip->GetProcessManager();
    manager->AddDiscreteProcess(fXTRProcess);

    G4PionMinus *pim = G4PionMinus::PionMinus();
    manager = pim->GetProcessManager();
    manager->AddDiscreteProcess(fXTRProcess);

    G4KaonPlus *kap = G4KaonPlus::KaonPlus();
    manager = kap->GetProcessManager();
    manager->AddDiscreteProcess(fXTRProcess);

    G4KaonMinus *kam = G4KaonMinus::KaonMinus();
    manager = kam->GetProcessManager();
    manager->AddDiscreteProcess(fXTRProcess);

    G4Proton *pro = G4Proton::Proton();
    manager = pro->GetProcessManager();
    manager->AddDiscreteProcess(fXTRProcess);

    G4AntiProton *antip = G4AntiProton::AntiProton();
    manager = antip->GetProcessManager();
    manager->AddDiscreteProcess(fXTRProcess);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
