#ifndef _TopMetalContainer_h_Digi_
#define _TopMetalContainer_h_Digi_

#include "TimeControl.hh"
#include "TopMetal.hh"
#include "GMCPAction.hh"
#include "OutputFileWriter.hh"

#include <vector>

class OutputFileWriter;
class TopMetalContainer
{
public:    
    static TopMetalContainer& GetInstance()
    {
        static TopMetalContainer Instance;
        return Instance;
    }
    TopMetalContainer();
    ~TopMetalContainer();

    TopMetal* iTopMetal(UInt_t);
    GMCPAction* iGMCP(UInt_t);
    OutputFileWriter* GetOutputFilePtr() {return m_OutputFilePtr;}
    void Initialize();
    void ClearAllCharge();

private:
    static TopMetalContainer* m_Instance;
    std::vector<TopMetal*> m_TopMetalVector;
    std::vector<GMCPAction*> m_GMCPVector;
    UInt_t m_NumOfSD;

    OutputFileWriter* m_OutputFilePtr;
};

#endif