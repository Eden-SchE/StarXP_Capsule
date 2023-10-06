//*********************************************
//  This is Geant4 Template
//                                  author:Qian
//

#ifndef RadiatorDescription_h
#define RadiatorDescription_h 1

#include "globals.hh"

class G4LogicalVolume;
class G4Material;

struct RadiatorDescription
{
  RadiatorDescription()
    : fLogicalVolume(0), fFoilMaterial(0), fGasMaterial(0), 
      fFoilThickness(0.), fGasThickness(0.), fFoilNumber(0) {}

  G4LogicalVolume*  fLogicalVolume;
  G4Material*       fFoilMaterial;
  G4Material*       fGasMaterial;
  G4Material*       fAbsorberMaterial;
  G4double          fFoilThickness;
  G4double          fGasThickness;
  G4int             fFoilNumber;
};

#endif