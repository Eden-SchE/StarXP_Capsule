#include "TimeLine.hh"
#include <iostream>

TimeLine::TimeLine()
{
    m_FirstNodePtr = NULL;
    m_LastNodePtr = NULL;
    m_Length = 0;
}

TimeLine::TimeLine(const TimeLine& theList)
{
    m_Length = theList.m_Length;
    if(m_Length == 0)
    {
        m_FirstNodePtr = NULL;
        m_LastNodePtr = NULL;
        return;
    }

    TimeNode* otherNode = theList.m_FirstNodePtr;
    m_FirstNodePtr = new TimeNode(otherNode->m_Action);
    TimeNode* thisNode = m_FirstNodePtr;
    otherNode = otherNode->m_PostNodePtr;

    while (otherNode != NULL)
    {
        TimeNode* lastNode = thisNode;
        thisNode->m_PostNodePtr = new TimeNode(otherNode->m_Action);
        thisNode->m_PreNodePtr = lastNode;
        thisNode = thisNode->m_PostNodePtr;
        otherNode = otherNode->m_PostNodePtr;
    }
    thisNode->m_PostNodePtr = NULL;
    m_LastNodePtr = thisNode;
}

TimeLine::~TimeLine()
{
    this->Clear();
}

void TimeLine::Clear()
{
    while (m_FirstNodePtr != NULL)
    {
        TimeNode* tempNode = m_FirstNodePtr->m_PostNodePtr;
        delete m_FirstNodePtr;
        m_FirstNodePtr = tempNode;
    }
    m_LastNodePtr = NULL;
}

void TimeLine::InsertNode(Double_t TimeFlag, const TimeActionInfo& theTimeActionInfo, bool Backword)
{
    if(m_Length == 0)
    {
        TimeNode* newNode = new TimeNode(theTimeActionInfo, TimeFlag, NULL, NULL);
        m_FirstNodePtr = newNode;
        m_LastNodePtr = newNode;
    }
    else if(TimeFlag < m_FirstNodePtr->m_TimeFlag)
    {
        TimeNode* newNode = new TimeNode(theTimeActionInfo, TimeFlag, NULL, m_FirstNodePtr);
        m_FirstNodePtr->m_PreNodePtr = newNode;
        m_FirstNodePtr = newNode;
    }
    else if(TimeFlag >= m_LastNodePtr->m_TimeFlag)
    {
        TimeNode* newNode = new TimeNode(theTimeActionInfo, TimeFlag, m_LastNodePtr, NULL);
        m_LastNodePtr->m_PostNodePtr = newNode;
        m_LastNodePtr = newNode;
    }
    else
    {
        if (!Backword)
        {
            TimeNode* currNode = m_FirstNodePtr;
            while (TimeFlag >= currNode->m_TimeFlag)
                currNode = currNode->m_PostNodePtr;
            TimeNode *newNode = new TimeNode(theTimeActionInfo, TimeFlag, currNode->m_PreNodePtr, currNode);
            currNode->m_PreNodePtr->m_PostNodePtr = newNode;
            currNode->m_PreNodePtr = newNode;
        }
        else
        {
            TimeNode* currNode = m_LastNodePtr;
            while (TimeFlag < currNode->m_TimeFlag)
                currNode = currNode->m_PreNodePtr;
            TimeNode *newNode = new TimeNode(theTimeActionInfo, TimeFlag, currNode, currNode->m_PostNodePtr);
            currNode->m_PostNodePtr = newNode;
            currNode->m_PostNodePtr->m_PreNodePtr = newNode;
        }
    }
    ++m_Length;
}

void TimeLine::Print()
{
    TimeNode* currNode = m_FirstNodePtr;
    int NodeNo = 0;
    std::cout << "TimeLine::Print()" << std::endl;
    while(currNode != NULL)
    {
        std::cout << "\tNode: " << NodeNo << "\tTime: " << currNode->m_TimeFlag << "\tType: " << currNode->m_Action.m_ActionType << "\tID: " << currNode->m_Action.m_ActionID << std::endl;
        currNode = currNode->m_PostNodePtr;
        ++NodeNo;
    }
}

bool TimeLine::DeleteFirstNode()
{
    if (m_FirstNodePtr == NULL)
        return false;

    TimeNode* secondNode = m_FirstNodePtr->m_PostNodePtr;
    delete m_FirstNodePtr;
    m_FirstNodePtr = secondNode;
    --m_Length;
    if(m_Length == 0)
        m_LastNodePtr = NULL;

    return true;
}