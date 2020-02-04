// ***************************************************************
//  文件: GUIHyperLink.h
//  日期: 2010-2-26
//  作者: liangairan
//  说明: 超连接
// ***************************************************************
#pragma once

#include "GUIElement.h"

class AIRGuiExport CGUIHyperLink
{
public:
    CGUIHyperLink(void);
    virtual ~CGUIHyperLink(void);

    typedef std::list<CGUIElement*>    ListElement;
    typedef ListElement::iterator  ListElement_Iter;
    ListElement  m_lstElements;

    //CGUIElement*   m_pOwnerElement;

    bool OnLButtonDown(int x, int y);

    bool OnRButtonDown(int x, int y);

    bool IsInRange(int x, int y);

    void AddElement(CGUIElement* pElement);

    virtual const char* ToString();

    char m_szContext[256];

    AIRString m_strName;
};
