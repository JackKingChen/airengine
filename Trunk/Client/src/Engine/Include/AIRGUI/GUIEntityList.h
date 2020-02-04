// ***************************************************************
//  GUIEntityList   version:  1.0   ·  date: 05/20/2009
//  -------------------------------------------------------------
//  存放一个界面中的所有根元素
//  -------------------------------------------------------------
//  Copyright (C) 2009 - All Rights Reserved
// ***************************************************************
// 
// ***************************************************************

#pragma once

#include "EventReader.h"
#include "GUIElement.h"

class AIRGuiExport CGUIEntityList //: public EngineAllocator(CGUIEntityList)
{
	EngineAllocator(CGUIEntityList)
public:
    CGUIEntityList(void);
    virtual ~CGUIEntityList(void);

    //添加控件
    //void AddEntity(CGUIElement* pEntity);

    //删除控件
    virtual void RemoveEntity(CGUIElement* pEntity, bool bNeedDelElem = true);

    //根据名字删除控件
    void RemoveEntity(const char* szName, bool bNeedDelElem = true);
    virtual void Render();
    virtual void Update();
    virtual void CleanUp();

    //清空某控件
    void ClearEntity(CGUIElement* pEntity);

    //根据名字查找控件
    CGUIElement* FindCtrl(const char* szCtrlName);

	virtual bool			SaveToXML(CMarkupSTL& xml);

    CGUIElementIterator     GetTopGUIElementIterator(void);

    CGUIElementIterator     GetNormalGUIElementIterator(void);

    CGUIElementIterator     GetBottomGUIElementIterator(void);

    //计算所有控件的align位置
    virtual void CaculateEntityAlignPosition();

protected:
    typedef std::list<CGUIElement*> GUIELEMENT_LIST;
    typedef std::list<CGUIElement*>::iterator  GUIELEMENT_ITER;
    typedef std::list<CGUIElement*>::reverse_iterator GUIELEMENT_RESITER;


    GUIELEMENT_LIST  m_lstElement;        //正常的UI控件列表
    GUIELEMENT_LIST  m_lstElementBottom;  //最底端的UI
    GUIELEMENT_LIST  m_lstElementTop;     //最顶端的UI
};
