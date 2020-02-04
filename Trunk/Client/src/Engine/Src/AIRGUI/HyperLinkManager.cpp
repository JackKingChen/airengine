#include "HyperLinkManager.h"

HyperLinkManager::HyperLinkManager(void)
{
}

HyperLinkManager::~HyperLinkManager(void)
{
}

void HyperLinkManager::AddHyperLink(CGUIHyperLink* pHyperLink)
{
    m_mapHyperLink.insert(std::make_pair(pHyperLink->m_strName, pHyperLink));
}

void HyperLinkManager::Clear()
{
    HyperLinkMap_iter itEnd = m_mapHyperLink.end();
    CGUIHyperLink* pLink = NULL;
    for (HyperLinkMap_iter itCur = m_mapHyperLink.begin(); itCur != itEnd; ++itCur)
    {
        pLink = itCur->second;
        SafeDelete(pLink);
    }
}

void HyperLinkManager::DestroyHyperLink(const AIRString& strName)
{
    HyperLinkMap_iter it = m_mapHyperLink.find(strName);
    if (it != m_mapHyperLink.end())
    {
        SafeDelete(it->second);
        m_mapHyperLink.erase(it);
    }
}

void HyperLinkManager::DestroyHyperLink(CGUIHyperLink* pLink)
{
    DestroyHyperLink(pLink->m_strName);
}

bool HyperLinkManager::OnLButtonDown(int x, int y)
{
    return true;
}

bool HyperLinkManager::OnRButtonDown(int x, int y)
{
    return true;
}
