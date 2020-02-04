// ***************************************************************
//  GUIEntityList   version:  1.0   ��  date: 05/20/2009
//  -------------------------------------------------------------
//  ���һ�������е����и�Ԫ��
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

    //��ӿؼ�
    //void AddEntity(CGUIElement* pEntity);

    //ɾ���ؼ�
    virtual void RemoveEntity(CGUIElement* pEntity, bool bNeedDelElem = true);

    //��������ɾ���ؼ�
    void RemoveEntity(const char* szName, bool bNeedDelElem = true);
    virtual void Render();
    virtual void Update();
    virtual void CleanUp();

    //���ĳ�ؼ�
    void ClearEntity(CGUIElement* pEntity);

    //�������ֲ��ҿؼ�
    CGUIElement* FindCtrl(const char* szCtrlName);

	virtual bool			SaveToXML(CMarkupSTL& xml);

    CGUIElementIterator     GetTopGUIElementIterator(void);

    CGUIElementIterator     GetNormalGUIElementIterator(void);

    CGUIElementIterator     GetBottomGUIElementIterator(void);

    //�������пؼ���alignλ��
    virtual void CaculateEntityAlignPosition();

protected:
    typedef std::list<CGUIElement*> GUIELEMENT_LIST;
    typedef std::list<CGUIElement*>::iterator  GUIELEMENT_ITER;
    typedef std::list<CGUIElement*>::reverse_iterator GUIELEMENT_RESITER;


    GUIELEMENT_LIST  m_lstElement;        //������UI�ؼ��б�
    GUIELEMENT_LIST  m_lstElementBottom;  //��׶˵�UI
    GUIELEMENT_LIST  m_lstElementTop;     //��˵�UI
};
