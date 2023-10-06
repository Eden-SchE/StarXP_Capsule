//*********************************************
//  This is Geant4 Template
//                                  author:Qian
//

#include "MyParticleGunMessenger.hh"
#include "MyParticleGun.hh"
#include "MyRunAction.hh"

#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWith3Vector.hh"
#include "G4UIcmdWith3VectorAndUnit.hh"
#include "G4UIcmdWithoutParameter.hh"
#include "G4ios.hh"
#include "Randomize.hh"

#include "Verbose.hh"

MyParticleGunMessenger::MyParticleGunMessenger(MyParticleGun *gun)
    : G4UImessenger(),
      gunAction(gun)
{
    if (verbose)
        G4cout << "====>MyParticleGunMessenger::MyParticleGunMessenger()" << G4endl;

    //#PartGun 5. 初始化粒子枪的命令
    fBGFileNameCmd = new G4UIcmdWithAString("/MyGun/SetGunBGFile", this);
    fBGFileNameCmd->SetGuidance("set the background filename, read it and generate the gun accordingly.");
    fBGFileNameCmd->SetParameterName("fileName", true);
    fBGFileNameCmd->SetDefaultValue("backgrounddata.root");
    fBGFileNameCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

    fGunTypeCmd = new G4UIcmdWithoutParameter("/MyGun/SimpleGun", this);
    fGunTypeCmd->SetGuidance("use the G4 particle gun");
    fGunTypeCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

    //CRY: cosmic ray package
    CRYDir = new G4UIdirectory("/MyGun/CRY/");
    CRYDir->SetGuidance("CRY initialization");

    FileCmd = new G4UIcmdWithAString("/MyGun/CRY/file", this);
    FileCmd->SetGuidance("This reads the CRY definition from a file");
    FileCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

    InputCmd = new G4UIcmdWithAString("/MyGun/CRY/input", this);
    InputCmd->SetGuidance("CRY input lines");
    InputCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

    UpdateCmd = new G4UIcmdWithoutParameter("/MyGun/CRY/update", this);
    UpdateCmd->SetGuidance("Update CRY definition.");
    UpdateCmd->SetGuidance("This command MUST be applied before \"beamOn\" ");
    UpdateCmd->SetGuidance("if you changed the CRY definition.");
    UpdateCmd->AvailableForStates(G4State_Idle);

    MessInput = new std::string;
}

MyParticleGunMessenger::~MyParticleGunMessenger()
{
    delete fBGFileNameCmd;
    delete fGunTypeCmd;

    //CRY: cosmic ray package
    delete CRYDir;
    delete InputCmd;
    delete UpdateCmd;
    delete FileCmd;
}

void MyParticleGunMessenger::SetNewValue(G4UIcommand *command, G4String newValues)
{
    if (verbose)
        G4cout << "====>MyParticleGunMessenger::SetNewValue()" << G4endl;

    //#PartGun 6. 执行粒子枪的命令
    if (command == fGunTypeCmd)
    {
        gunAction->UseSimpleGun();
    }

    if (command == fBGFileNameCmd)
    {
        gunAction->OpenRootFile(newValues);
    }
    
    /*
    //CRY: cosmic ray package
    if (command == InputCmd)
    {
        gunAction->InputCRY();
        (*MessInput).append(newValues);
        (*MessInput).append(" ");
    }

    if (command == UpdateCmd)
    {
        gunAction->UpdateCRY(MessInput);
        *MessInput = "";
    }

    if (command == FileCmd)
    {
        gunAction->CRYFromFile(newValues);
    }
    */
}
