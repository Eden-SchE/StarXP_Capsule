
#ifndef MyRunManager_h
#define MyRunManager_h 1

#include "G4RunManager.hh"
#include "CosmicRayList.hh"

class MyRunManager: public G4RunManager
{
public:
    MyRunManager();
    ~MyRunManager();

    void GenEvents(G4double, G4int, G4int);

private:
    void GenValidEvents(G4int);
    CosmicRayList* fCosmicRayList;
};

#endif