// ***************************************************************
//  �ļ�: GUIDragImage.h
//  ����: 2010-4-29
//  ����: liangairan
//  ˵��: ���϶���image
// ***************************************************************
#pragma once
#include "guielement.h"


class CGUIListItem;

class AIRGuiExport CGUIDragImage :
    public CGUIElement
{
public:
    CGUIDragImage(void);
    virtual ~CGUIDragImage(void);

    void SetDragImageType(int nType)
    {
        m_nType = nType;
    }

    //��ø�image�����ڵ�����
    int GetDragImageType() const
    {
        return m_nType;
    }

    //void OnDropSuccess();

    CGUIListItem*  GetListItem()
    {
        return m_pListItem;
    }

    void SetGUIListItem(CGUIListItem* pItem);

    virtual void OnDragSuccess() {}

    virtual void OnDragToNull() {}
private:
    int m_nType;   //����
    CGUIListItem*  m_pListItem;   //�������ڵ�listItem
};
