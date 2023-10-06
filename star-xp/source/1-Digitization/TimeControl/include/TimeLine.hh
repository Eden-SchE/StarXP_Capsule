#ifndef _TimeLine_h_Digi_
#define _TimeLine_h_Digi_

#include "Rtypes.h"
#include "TimeAction.hh"

struct TimeActionInfo
{
    TString m_ActionType;
    Int_t m_ActionID;
    TimeActionInfo(){};
    TimeActionInfo(TString ActionType, Int_t ActionID)
    {
        this->m_ActionType = ActionType;
        this->m_ActionID = ActionID;
    }
};

struct TimeNode
{
    Double_t m_TimeFlag;
    TimeActionInfo m_Action;
    TimeNode* m_PreNodePtr;
    TimeNode* m_PostNodePtr;
    TimeNode(){};
    TimeNode(const TimeActionInfo &action) { this->m_Action = action; }
    TimeNode(const TimeActionInfo &action, Double_t TimeFlag, TimeNode* thePre, TimeNode* thePost)
    {
        this->m_Action = action;
        this->m_TimeFlag = TimeFlag;
        this->m_PreNodePtr = thePre;
        this->m_PostNodePtr = thePost;
    }
};

class TimeLine
{
public:
    TimeLine();
    TimeLine(const TimeLine& theList);
    ~TimeLine();

    void Clear();
    void Print();
    bool Empty() const {return m_Length == 0;} 
    Double_t Size() const {return m_Length;}
    TimeAction& Get(Double_t) const;
    void InsertNode(Double_t TimeFlag, const TimeActionInfo& theTimeActionInfo, bool Backword = false);
    inline TimeNode* GetFirstNode() const {return m_FirstNodePtr;}
    bool DeleteFirstNode();

private:
    int m_Length;
    TimeNode* m_FirstNodePtr;
    TimeNode* m_LastNodePtr;

};

#endif