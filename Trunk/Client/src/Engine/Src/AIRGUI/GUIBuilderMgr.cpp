//#include "stdafx.h"
#include "GUIBuilderMgr.h"
#include "Util.h"

//CGUIBuilderMgr* CGUIBuilderMgr::m_pInstance = NULL;

CGUIBuilderMgr::CGUIBuilderMgr()
{

}

CGUIBuilderMgr::~CGUIBuilderMgr()
{
    //CleanUp();
}

bool CGUIBuilderMgr::Register(const char* szBuilder, CGUIBuilder* pBuilder)
{
    if (m_mapBuilder.find(AIRString(szBuilder)) != m_mapBuilder.end())
    {
        return false;
    }
    m_mapBuilder.insert(make_pair(AIRString(szBuilder), pBuilder));
    return true;
}

CGUIBuilder* CGUIBuilderMgr::GetBuilder(const AIRString& strName)
{
    BUILDER_ITER it = m_mapBuilder.find(AIRString(strName));

    if (it != m_mapBuilder.end())
    {
        return it->second;
    }

    return NULL;
}

void CGUIBuilderMgr::CleanUp()
{
    if (m_mapBuilder.size() == 0)
    {
        return;
    }
    for (BUILDER_ITER it = m_mapBuilder.begin(); it != m_mapBuilder.end(); it++)
    {
        CGUIBuilder* pBuilder = it->second;
        SafeDelete(pBuilder);
    }

    m_mapBuilder.clear();
}
