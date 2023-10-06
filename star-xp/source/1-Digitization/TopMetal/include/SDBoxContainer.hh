#ifndef _SDBoxContainer_h_Digi_
#define _SDBoxContainer_h_Digi_

#include "TChain.h"
#include "TVector3.h"

#include "SDBox.hh"
#include "TimeControl.hh"

#include <vector>

class SDBoxContainer
{
public:
    static SDBoxContainer& GetInstance()
    {
        static SDBoxContainer Instance;
        return Instance;
    }
    static inline Int_t NumOfFramePerEvent() {return m_NumOfFramePerEvent;}

    SDBoxContainer();
    ~SDBoxContainer();

    void ReadSDConfig(TChain*);
    Int_t WhichSD(TVector3 Point, bool Loose = false);

    SDBox& GetSD(unsigned int);
    SDBox& operator[](unsigned int);

    static inline UInt_t NumOfSD() {return m_NumOfSD;}
    //static inline TVector3 SDSize() {return m_SDSize;}

private:
    static SDBoxContainer* m_Instance;

    static UInt_t m_NumOfSD;
    //static TVector3 m_SDSize;
    static Int_t m_NumOfFramePerEvent;

    TVector3 m_SDSize;
    std::vector<SDBox> m_SDBoxVector;

};

#endif