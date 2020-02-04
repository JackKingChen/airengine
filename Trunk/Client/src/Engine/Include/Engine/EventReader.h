#pragma once
//#include <Windows.h>
#include <queue>
#include <list>
#include "EngineConfig.h"
#include "MemoryAllocatorConfig.h"
//#include "ObjectPool.h"



enum InputType
{
    eINPUT_MOUSE,
    eINPUT_KEYBOARD,
};

enum InputKey
{
    eINPUT_MOUSE_LBUTTONDOWN,
    eINPUT_MOUSE_LBUTTONUP,
    eINPUT_MOUSE_RBUTTONDOWN,
    eINPUT_MOUSE_RBUTTONUP,
    eINPUT_MOUSE_MOVE,
    eINPUT_MOUSE_WHEEL,
    eINPUT_KEYDOWN,
    eINPUT_KEYUP,
    eINPUT_MOUSE_LBUTTONDBCLICK,
};

class INPUTEVENT// : public EngineAllocator(INPUTEVENT)
{
	EngineAllocator(INPUTEVENT)
public:
    INPUTEVENT() :
		nType(0),
        nKey(0),
        nFlags(0),
        nChr(0),
        nWheel(0),
        x(0),
        y(0)
    {}

    INPUTEVENT(int _type, int _key, int _flags, int _chr, int _wheel, int _x, int _y) : nType(_type),
        nKey(_key), nFlags(_flags), nChr(_chr), nWheel(_wheel), x(_x), y(_y), pNext(NULL)
    {}

    int			nType;			// event type
    int			nKey;			// key code
    int			nFlags;			// event flags
    int			nChr;			// character code
    int			nWheel;			// wheel shift
    int			x;				// mouse cursor x-coordinate
    int 		y;				// mouse cursor y-coordinate
    INPUTEVENT	*pNext;         // next event
};

typedef INPUTEVENT*                 LPINPUTEVENT;
//typedef AIRDeque<INPUTEVENT> InputEventPool;


class EngineExport CInputEventList
{
public:
    CInputEventList() : m_pHead(NULL), m_pEnd(NULL)
    {        
    }

    virtual ~CInputEventList() 
    { 
        Clear();
    }

    void            Clear();

    void            AddInputEvent( int _type, int _key, int _flags, int _chr, int _wheel, int _x, int _y );

    LPINPUTEVENT    GetHead()  { return m_pHead; }
    LPINPUTEVENT    GetTail()  { return m_pEnd; }

protected:
    LPINPUTEVENT    m_pHead;
    LPINPUTEVENT    m_pEnd;
    //InputEventPool  m_IEP;
};



//
//class EngineExport CEventReader
//{
//public:
//    virtual ~CEventReader();
//
//    static CEventReader* GetInstance()
//    {
//        if (m_pInstance == NULL)
//        {
//            m_pInstance = new CEventReader;
//        }
//        return m_pInstance;
//    }
//
//    LPINPUTEVENT   BuildEvent(int nType, int nKey, int nFlag, int nCh, int nWheel, int nX, int nY);
//
//    void   ClearEvent();
//
//    static void KillSelf();
//protected:
//private:
//    CEventReader();
//
//    std::list<LPINPUTEVENT>   m_lstEvent;
//    typedef std::list<LPINPUTEVENT>::iterator EVENT_ITER;
//
//    static CEventReader* m_pInstance;
//};
