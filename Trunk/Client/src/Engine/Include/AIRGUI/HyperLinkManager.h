// ***************************************************************
//  文件: HyperLinkManager.h
//  日期: 2010-2-26
//  作者: liangairan
//  说明: 超连接管理器
// ***************************************************************
#pragma once

#include "GUIHyperLink.h"

class AIRGuiExport HyperLinkManager : public Singleton<HyperLinkManager>
{
    BE_SINGLETON(HyperLinkManager);
public:
    
    ~HyperLinkManager(void);

    void Clear();

    void DestroyHyperLink(const AIRString& strName);

    void DestroyHyperLink(CGUIHyperLink* pLink);

    void AddHyperLink(CGUIHyperLink* pHyperLink);

    bool OnLButtonDown(int x, int y);

    bool OnRButtonDown(int x, int y);
    
private:
    typedef std::map<AIRString, CGUIHyperLink*> HyperLinkMap;
    typedef HyperLinkMap::iterator HyperLinkMap_iter;
    HyperLinkMap  m_mapHyperLink;
};

#define HYPERLINK_MGR HyperLinkManager::GetInstance()
