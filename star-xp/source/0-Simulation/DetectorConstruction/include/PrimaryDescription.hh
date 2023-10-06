//*********************************************
//  This is Geant4 Template
//                                  author:Qian
//

#ifndef PrimaryDescription_h
#define PrimaryDescription_h 1

#include "globals.hh"
#include "G4ThreeVector.hh"

struct PrimaryDescription
{
  PrimaryDescription()
      : PGorGPS(0), particle("e-"),
        particlePos(G4ThreeVector(0, 0, 0)), particleMom(G4ThreeVector(1000, 0, 0)), particlePol(G4ThreeVector(0, 0, 1)){};

  int PGorGPS; //0 单粒子，其他 general particle source
  G4String particle;
  G4ThreeVector particlePos;
  G4ThreeVector particleMom;
  G4ThreeVector particlePol;
};

#endif
