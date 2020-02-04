#include "stdafx.h"
#include "EventReader.h"
#include "Util.h"
//#include "DetectMemLeak.h"



void CInputEventList::Clear()
{
	while( m_pHead != NULL )
	{
		LPINPUTEVENT pTmp = m_pHead;
		m_pHead = m_pHead->pNext;

        //m_IEP.DeleteObj( pTmp );
		////delete pTmp;
		SafeDelete( pTmp );
		pTmp = NULL;
	}
    m_pHead = NULL;
}

//void CInputEventList::AddInputEvent(LPINPUTEVENT pEvent)
void CInputEventList::AddInputEvent( int _type, int _key, int _flags, int _chr, int _wheel, int _x, int _y )
{
    LPINPUTEVENT pEvent = new INPUTEVENT;
    pEvent->nType   = _type;
    pEvent->nKey    = _key;
    pEvent->nFlags  = _flags;
    pEvent->nChr    = _chr;
    pEvent->nWheel  = _wheel;
    pEvent->x       = _x;
    pEvent->y       = _y;
    pEvent->pNext   = NULL;

	if (m_pHead == NULL)
	{
		m_pHead = pEvent;
		m_pEnd = m_pHead;
	}
	else
	{
		m_pEnd->pNext = pEvent;
		m_pEnd = pEvent;
	}
}
//
//
//CEventReader* CEventReader::m_pInstance = NULL;
//
//CEventReader::CEventReader()
//{
//
//}
//
//CEventReader::~CEventReader()
//{
//    ClearEvent();
//    //SafeDelete(m_pInstance);
//}
//
//LPINPUTEVENT CEventReader::BuildEvent(int nType, int nKey, int nFlag, int nCh, int nWheel, int nX, int nY)
//{
//    LPINPUTEVENT pEvent = /*new*/AURO_NEW_VERBOSE INPUTEVENT(nType, nKey, nFlag, nCh, nWheel, nX, nY);
//
//    m_lstEvent.push_back(pEvent);
//    return pEvent;
//}
//
//void CEventReader::ClearEvent()
//{
//    for (EVENT_ITER it = m_lstEvent.begin(); it != m_lstEvent.end(); it++)
//    {
//        LPINPUTEVENT pEvent = *it;
//        SafeDelete( pEvent );
//    }
//
//    m_lstEvent.clear();
//}
//
//void CEventReader::KillSelf()
//{
//	if (m_pInstance)
//	{
//		//delete m_pInstance;
//		//m_pInstance = NULL;
//		SafeDelete( m_pInstance );
//	}
//}