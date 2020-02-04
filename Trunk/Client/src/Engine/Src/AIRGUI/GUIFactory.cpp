//#include "StdAfx.h"
#include "GUIFactory.h"

CGUIFactory::CGUIFactory()
{
    m_nElementCount = 0;
}

CGUIFactory::~CGUIFactory(void)
{
}

CGUIElement* CGUIFactory::CreateElement(const char* szTypename, const char* szElementName)
{
    CREATOR_MAP::iterator it = m_mapCreator.find(AIRString(szTypename));
    if( it != m_mapCreator.end() )
    {
        tCreator aCreator = (*it).second;
        CGUIElement* pElem = aCreator();
        if( pElem )
        {
            pElem->SetName( szElementName );
            //pElem->SetHandle( ++m_nElementCount );
            pElem->SetType( AIRString(szTypename) );
        }
        return pElem;
    }
    return NULL;
}

void CGUIFactory::Register(const char* szType, tCreator aCreator)
{
    m_mapCreator.insert(make_pair(AIRString(szType), aCreator));
}


