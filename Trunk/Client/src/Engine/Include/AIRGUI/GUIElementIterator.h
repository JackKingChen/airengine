/********************************************************************
	created:	2010/02/03
	created:	3:2:2010   16:17
	filename: 	d:\My Codes\client\Current\Engine\AIRGUI\GUIElementIterator.h
	file path:	d:\My Codes\client\Current\Engine\AIRGUI
	file base:	GUIElementIterator
	file ext:	h
	author:		liangairan
	
	purpose:	GUI¿Ø¼þµø´úÆ÷
*********************************************************************/
#pragma once

#include "AIRGUI.h"

#include <list>

class CGUIElement;

class AIRGuiExport CGUIElementIterator
{
public:
    friend class CGUIEntityList;
    friend class CGUIElement;
    virtual ~CGUIElementIterator(void);

    bool IsEnd();

    CGUIElement* GetNext();
protected:
    std::list<CGUIElement*>::iterator m_Pos;
    std::list<CGUIElement*>::iterator m_Start;
    std::list<CGUIElement*>::iterator m_End;

    CGUIElementIterator(std::list<CGUIElement*>::iterator start, std::list<CGUIElement*>::iterator end);
};
