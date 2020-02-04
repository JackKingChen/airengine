//#include "StdAfx.h"
#include "GUIListItem.h"
#include "StringUtil.h"
#include "Engine.h"
#include "GUISystem.h"
#include "GUIListCtrl.h"
#include "GUIRenderLine.h"
#include "GUIRenderer.h"
#include "GUIDragImage.h"

#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
#include <MMSystem.h>
#endif

CGUIListItem::CGUIListItem(void) : 
m_pDragImage(NULL),
m_pImage(NULL), 
m_dwCoolEndTime(0), 
m_byItemStatus(eItem_Normal), 
m_byLastItemStatus(eItem_Normal),
m_bSelected(false),
m_pListCtrl(NULL),
m_pDetailText(NULL),
m_pItemData(NULL), 
m_bAutoDeleteElement(false)
{
    //m_ctrlType = eCtrl_ListItem;
    m_strType = "CGUIListItem";
    m_bDrawFrame = false;
    m_align = eAlign_RightBottom;
	m_bMoveable = false;
}

CGUIListItem::~CGUIListItem(void)
{
}

void CGUIListItem::CleanUp()
{
    if (m_pImage)
    {
        m_pImage->CleanUp();
        SafeDelete(m_pImage);
    }
}

/*
bool CGUIListItem::Create(CGUIElement* pParent, CMarkupSTL& xml)
{
    return CGUIElement::Create(pParent, xml);
}
*/

bool CGUIListItem::Create(CGUIListCtrl* pParent, const GUILIST_ITEM& itemData, int nParentOffsetX, int nParentOffsetY)
{
    m_pParent = pParent;
    m_pListCtrl = pParent;
    m_pItemData = (&itemData);
    //m_itemData = itemData;
    m_pScheme = pParent->GetScheme();
    

    int nTopLeftX = nParentOffsetX;
    int nTopLeftY = nParentOffsetY;
    if (m_pParent)
    {
        Rectf rectParent;
        pParent->GetRenderRect(rectParent);
        nTopLeftX += rectParent.left;
        nTopLeftY += rectParent.top;
    }
    m_renderQuad.m_rectScreen.SetRect(nTopLeftX, nTopLeftY, nTopLeftX + itemData.nWidth, nTopLeftY + itemData.nHeight);
    m_realRect = m_renderQuad.m_rectScreen;

    return true;
}

bool CGUIListItem::SaveSelfToXML(CMarkupSTL& xml )
{
    return true;
	//return CGUIElement::SaveSelfToXML(xml);
}

void CGUIListItem::UpdateSelf()
{
    if (m_byItemStatus != m_byLastItemStatus)
    {
        OnStatusChange();
    }
    m_byLastItemStatus = m_byItemStatus;

    if (m_pImage)
    {
        m_pImage->Update();
    }
}

void CGUIListItem::RenderSelf()
{
    //CGUIElement::RenderSelf();
    //GUIRENDERER->Render(m_renderQuad.m_rectScreen, m_pListCtrl->m_uvItem, m_pFrameLayer->m_fZ, m_pListCtrl->m_renderQuad.m_pTexture/*, m_fDepth*/);
    

    if (m_pImage)
    {
        m_pImage->Render();
    }
    
    if (m_pDragImage)
    {
        m_pDragImage->Render();
    }

    DrawRenderQuad();
}

void CGUIListItem::SetCurrentUIState(GUIELEMENT_STATE state)
{
    if (state == UI_NORMAL)
    {
        m_byItemStatus = eItem_Normal;
    }
    else if (state == UI_HOVER)
    {
        m_byItemStatus = eItem_Hover;
    }
    else if (state == UI_GRAY)
    {
        m_byItemStatus = eItem_UnUsed;
    }
    CGUIElement::SetCurrentUIState(state);
    
}

void CGUIListItem::OnStatusChange()
{
    switch (m_byItemStatus)
    {
    case eItem_Normal:
        m_renderQuad.m_uv = Rectf::ZERO;
        break;
    case eItem_Hover:
        m_renderQuad.m_uv = m_pListCtrl->m_uvItemHover;
        break;
    case eItem_UnUsed:
        m_renderQuad.m_uv = m_pListCtrl->m_uvItemUnused;
        break;
    }
}

bool CGUIListItem::OnMouseMoveSelf(int x, int y)
{
#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
    if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
    {
        //m_nCurUIState = UI_DRAGING;
        if (m_pListCtrl->IsItemDragable())
        {
            OnDrag();
        }
        //if (m_pImage)
        //{
        //    GUISYSTEM->SetDragElement(m_pImage);
        //}
    }
    else
    {
        GUISYSTEM->SetHoverElement(this);
        m_byItemStatus = eItem_Hover;
    }
#endif

    if (m_pMouseMoveFun)
    {
        return m_pMouseMoveFun(this);
    }

    return true;
}

bool CGUIListItem::OnDragSelf(int x, int y)
{
    //把image加到该list中
    return true;
}

bool CGUIListItem::OnLButtonDownSelf(int x, int y)
{
    //m_byItemStatus = eItem_Selected;
    m_bSelected = true;
    return true;
}

void CGUIListItem::SetCoolTime(u32 dwTime)
{
    m_dwCoolEndTime = dwTime;
}

void CGUIListItem::SetSelected(bool bSelected)
{
    m_bSelected = bSelected;
}

void CGUIListItem::RenderText()
{
    for (GUILINE_ITER it = m_vtRenderLines.begin(); it != m_vtRenderLines.end(); ++it)
    {
        (*it)->Render();
    }
}

void CGUIListItem::SetIndex(int nIndex)
{
    m_nIndex = nIndex;
}


void CGUIListItem::AttachDragImage(CGUIDragImage* pDragImage)
{
    m_pDragImage = pDragImage;
    if (m_pDragImage)
    {
        
        m_pDragImage->SetGUIListItem(this);
        m_pDragImage->MoveTo(m_renderQuad.m_rectScreen.left, m_renderQuad.m_rectScreen.top);
        m_pDragImage->SetCenterPos((m_renderQuad.m_rectScreen.right + m_renderQuad.m_rectScreen.left) / 2, 
            (m_renderQuad.m_rectScreen.bottom + m_renderQuad.m_rectScreen.top) / 2);
        m_pDragImage->CaculateAlignPosition();
    }
    
}


void CGUIListItem::OnDrop(CGUIDragImage* pImage)
{
    if (pImage->GetDragImageType() != m_pListCtrl->m_nDragItemType)
    {
        CGUIListItem* pItem = pImage->GetListItem();
        pItem->UnLock();
        return;
    }
    //假如放到的位置已经有了一个drapimage
    if (m_pDragImage && m_pDragImage != pImage)
    {
        
        //交换位置
        CGUIListItem* pDropImageItem = pImage->GetListItem();
        if (pDropImageItem->GetParent() == m_pListCtrl)    //假如是同一个ctrl，侧交换位置
        {
            pDropImageItem->UnLock();
            CGUIListItem* pOrigItem = pImage->GetListItem();
            if (pOrigItem)
            {
                pOrigItem->AttachDragImage(NULL);
            }
            pDropImageItem->AttachDragImage(m_pDragImage);
            if (m_pListCtrl->m_pOnDropFun)
            {
                m_pListCtrl->m_pOnDropFun(m_pDragImage);
            }

            if (m_pListCtrl->m_pScriptObject)
            {
                m_pListCtrl->m_pScriptObject->RunOnDropScript();
            }
            return;
        }
        else    //假如不是同一个ctrl
        {
            if (m_pDragImage)
            {
                m_pDragImage->CleanUp();
                SafeDelete(m_pDragImage);
            }
            
        }
    }

    CGUIListItem* pOrigItem = pImage->GetListItem();
    if (pOrigItem)
    {
        pOrigItem->AttachDragImage(NULL);
    }
    AttachDragImage(pImage);
    GUISYSTEM->SetMouseDragImage(NULL);

    if (m_pListCtrl->m_pOnDropFun)
    {
        m_pListCtrl->m_pOnDropFun(m_pDragImage);
    }

    if (m_pListCtrl->m_pScriptObject)
    {
        m_pListCtrl->m_pScriptObject->RunOnDropScript();
    }

    CGUIMouse* pGUIMouse = GUISYSTEM->GetGUIMouse();
    pGUIMouse->SetImageSrc("");
    //pGUIMouse->SetRect(m_pDragImage->GetRenderRect());
    //m_pImage = pImage;
    //移动该image到
}

void CGUIListItem::OnDrag()
{
    if (m_pDragImage)
    {
        GUISYSTEM->SetMouseDragImage(m_pDragImage);
        CGUIMouse* pGUIMouse = GUISYSTEM->GetGUIMouse();
        pGUIMouse->SetImageSrc(m_pDragImage->GetImageSrc());
        pGUIMouse->SetRect(m_pDragImage->GetRenderRect());
    }
    Lock();
}

void CGUIListItem::Move(int dx, int dy)
{
    CGUIElement::Move(dx, dy);

    if (m_pImage)
    {
        m_pImage->Move(dx, dy);
    }

    if (m_pDragImage)
    {
        m_pDragImage->Move(dx, dy);
    }
}

bool CGUIListItem::OnLButtonUpSelf(int x, int y)
{
    if (m_pListCtrl->IsItemDragable())
    {
        CGUIDragImage* pDropElement = GUISYSTEM->GetMouseDragImage();
        if (pDropElement && pDropElement->GetType() == "CGUIDragImage")
        {
            OnDrop(pDropElement);
        }
    }
    return true;
}

void CGUIListItem::Lock()
{
    m_byItemStatus = eItem_Locked;
    m_renderQuad.m_uv = m_pListCtrl->m_uvItemLock;
}

void CGUIListItem::UnLock()
{
    m_byItemStatus = eItem_Normal;
    m_renderQuad.m_uv = Rectf::ZERO;
}

void CGUIListItem::SetContentElement(CGUIElement* pElement)
{
    m_pImage = pElement;
    m_pImage->MoveTo(m_renderQuad.m_rectScreen.left, m_renderQuad.m_rectScreen.top);
    m_pImage->SetCenterPos((m_renderQuad.m_rectScreen.right + m_renderQuad.m_rectScreen.left) / 2, 
        (m_renderQuad.m_rectScreen.bottom + m_renderQuad.m_rectScreen.top) / 2);
    m_pImage->CaculateAlignPosition();
}
