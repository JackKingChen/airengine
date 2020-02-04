// ***************************************************************
//  文件: GUIDragImage.h
//  日期: 2010-4-29
//  作者: liangairan
//  说明: 可拖动的image
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

    //获得该image所属于的类型
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
    int m_nType;   //类型
    CGUIListItem*  m_pListItem;   //它所属于的listItem
};
