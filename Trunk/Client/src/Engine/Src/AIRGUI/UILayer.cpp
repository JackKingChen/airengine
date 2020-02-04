#include "UILayer.h"
#include "GUISystem.h"
#include "MemoryAllocatorConfig.h"

UILayer::UILayer() : m_pScheme(NULL)
, m_fMinZ(0)
, m_fMaxZ(0)
{

}

UILayer::~UILayer()
{

}

void UILayer::AddEntity(CGUIElement* pEntity)
{
    pEntity->SetSchemeManager( m_pScheme );////////////////

    m_lstElement.push_front(pEntity);
}

void UILayer::ClearEntity(CGUIElement* pEntity)
{
    for (GUIELEMENT_ITER it = m_lstElement.begin(); it != m_lstElement.end(); it++)
    {
        if (pEntity == *it)
        {
            pEntity->CleanUp();
            SafeDelete(pEntity);
            m_lstElement.erase(it);
            return;
        }
    }
}

void UILayer::RemoveEntity(CGUIElement* pEntity, bool bNeedDelElem /* = true */)
{
    //通知父亲
    CGUIElement* pParent = pEntity->GetParent();
    if (pParent != NULL)
    {
        pParent->RemoveChild(pEntity);
        pEntity->CleanUp();
        SafeDelete(pEntity);
        return;
    }
    for (GUIELEMENT_ITER it = m_lstElement.begin(); it != m_lstElement.end(); it++)
    {
        if (pEntity == *it)
        {
            m_lstElement.erase(it);
            if ( bNeedDelElem )
            {
                pEntity->CleanUp();
                SafeDelete(pEntity);
            }
            return;
        }
    }
}

void UILayer::RemoveEntity(const char* szName, bool bNeedDelElem /* = true */)
{
    CGUIElement* pEntity = NULL;
    for (GUIELEMENT_ITER it = m_lstElement.begin(); it != m_lstElement.end(); it++)
    {
        pEntity = *it;
        if (strcmp(pEntity->GetName().c_str(), szName) == 0)
        {
            m_lstElement.erase(it);
            if ( bNeedDelElem )
            {
                pEntity->CleanUp();
                SafeDelete(pEntity);
            }
            return;
        }
    }


    CGUIElement* pElement = GUISYSTEM->FindCtrl(szName);
    CGUIElement* pParent = pElement->GetParent();
    pParent->RemoveChild(pElement);
    pElement->CleanUp();
    SafeDelete(pElement);
}

void UILayer::CleanUp()
{
    CGUIElement* pEntity = NULL;
    for (GUIELEMENT_ITER it = m_lstElement.begin(); it != m_lstElement.end(); it++)
    {
        pEntity = *it;
        pEntity->CleanUp();
        SafeDelete<CGUIElement*>(pEntity);
    }
    m_lstElement.clear();

    for (FrameLayerList_Iter it = m_lstFrameLayers.begin(); it != m_lstFrameLayers.end(); ++it)
    {
        FrameLayer* pLayer = *it;
        AIR_DELETE_T(pLayer, FrameLayer);
    }

    m_lstFrameLayers.clear();
}

void UILayer::Render()
{
    CGUIElement* pEntity = NULL;

    //渲染正常的
    for (GUIELEMENT_ITER it = m_lstElement.begin(); it != m_lstElement.end(); ++it)
    {
        pEntity = *it;
        pEntity->Render();
    }
}

void UILayer::Update()
{
    //float fMaxDepth = GUISYSTEM->GetMaxDepth();
    //float fMinDepth = GUISYSTEM->GetMinDepth();
    //float fDepthDelta = (fMaxDepth - fMinDepth) / (m_lstElement.size() + 1);
    //float fDepth = fMaxDepth - fDepthDelta;
    CGUIElement* pEntity = NULL;
    for (GUIELEMENT_ITER it = m_lstElement.begin(); it != m_lstElement.end(); ++it)
    {
        pEntity = *it;
        if (pEntity->IsVisable())
        {
            //pEntity->SetDepth(fDepth);
            //fDepth -= fDepthDelta;
            pEntity->Update();
        }
    }

    CGUIEditCtrl* pEditCtrl = GUISYSTEM->GetEditCtrl();
    if (pEditCtrl)
    {
        pEditCtrl->Update();
    }
}


CGUIElement* UILayer::FindCtrl(const char* szCtrlName)
{
    CGUIElement* pEntity = NULL;
    CGUIElement* pFindCtrl = NULL;
    for (GUIELEMENT_ITER it = m_lstElement.begin(); it != m_lstElement.end(); ++it)
    {
        pEntity = *it;
        if (strcmp(pEntity->GetName().c_str(), szCtrlName) == 0)
        {
            return pEntity;
        }
        else
        {
            pFindCtrl = pEntity->FindChildElement(szCtrlName);
            if (pFindCtrl)
            {
                return pFindCtrl;
            }
        }
    }

    return NULL;
}

bool UILayer::OnLButtonDown(int x, int y)
{
    //  if (m_pFocusElement)
    //  {
    //      m_pFocusElement->SetFocus(false);
    //m_pFocusElement = NULL;
    //  }
    CGUIElement* pFocusElement = GUISYSTEM->GetFocusElement();
    if (pFocusElement)
    {
        pFocusElement->SetFocus(false);
        GUISYSTEM->SetFocusElement(NULL);

    }

    CGUIElement* pDragElement = GUISYSTEM->GetDragElement();
    if (pDragElement)
    {
        GUISYSTEM->SetDragElement(NULL);
    }

    if (GUISYSTEM->GetMouseDragImage())
    {
        GUISYSTEM->SetMouseDragImage(NULL);
        return true;
    }


    CGUIElement* pEntity = NULL;

    for (GUIELEMENT_RESITER it = m_lstElement.rbegin(); it != m_lstElement.rend(); ++it)
    {
        pEntity = *it;
        if (pEntity->IsVisable() && pEntity->IsInRect(x, y) && pEntity->IsEnable())
        {
            //GUIELEMENT_ITER curIt(it);
            m_lstElement.erase((++it).base());          //焦点所属的控件要在最开上方显示。
            m_lstElement.push_back(pEntity);
            //if (m_bEditMode)
            //{
            //    CGUIEditCtrl* pCtrl = GUISYSTEM->GetEditCtrl();
            //    pCtrl->AttachEditElement(pEntity);
            //}
            return pEntity->OnLButtonDown(x, y);
        }
    }

    return false;
}

bool UILayer::OnLButtonUp(int x, int y)
{ 
    CGUIElement* pDragElement = GUISYSTEM->GetDragElement();
    if (pDragElement)
    {
        return m_pScheme->EndDragElement(GUISYSTEM->GetDragElement(), x, y);
    }
    CGUIElement* pEntity = NULL;

    //中间
    for (GUIELEMENT_RESITER it = m_lstElement.rbegin(); it != m_lstElement.rend(); ++it)
    {
        pEntity = *it;
        if (pEntity->IsVisable() && pEntity->IsInRect(x, y) && pEntity->IsEnable())
        {
            return pEntity->OnLButtonUp(x, y);

        }
    }

    /*   拖动不应该这样做
    CGUIDragImage* pDragImage = GUISYSTEM->GetMouseDragImage();
    if (pDragImage)
    {
        //处理没drop成功的pDragImage
        pDragImage->OnDragToNull();
        GUISYSTEM->GetGUIMouse()->SetImageSrc("");
    }
    */

    return false;
}

bool UILayer::OnRButtonDown(int x, int y)
{
    CGUIElement* pEntity = NULL;


    for (GUIELEMENT_RESITER it = m_lstElement.rbegin(); it != m_lstElement.rend(); ++it)
    {
        pEntity = *it;
        if (pEntity->IsInRect(x, y) && pEntity->IsVisable() && pEntity->IsEnable())
        {
            return pEntity->OnRButtonDown(x, y);

        }
    }

    return false;
}

bool UILayer::OnRButtonUp(int x, int y)
{
    CGUIElement* pEntity = NULL;

    for (GUIELEMENT_RESITER it = m_lstElement.rbegin(); it != m_lstElement.rend(); ++it)
    {
        pEntity = *it;
        if (pEntity->IsVisable() && pEntity->IsInRect(x, y) && pEntity->IsEnable())
        {
            return pEntity->OnRButtonUp(x, y);
        }
    }

    return false;
}

bool UILayer::OnMouseMove(int x, int y)
{
    CGUIElement* pHoverElement = GUISYSTEM->GetHoverElement();
    //if (pHoverElement && !pHoverElement->IsInRect(x, y))
    //{
    //    pHoverElement->SetUICurrentState(UI_NORMAL);
    //    GUISYSTEM->SetHoverElement(NULL);
    //}
    if ( pHoverElement && pHoverElement->IsVisable() && pHoverElement->IsEnable() )
    {
        if ( !pHoverElement->IsInRect(x, y) )
        {
            pHoverElement->SetCurrentUIState(UI_NORMAL);
            GUISYSTEM->SetHoverElement(NULL);
        }
    }

    static int nLastX = x;
    static int nLastY = y;

    CGUIElement* pDragElement = GUISYSTEM->GetDragElement();
    if (pDragElement)
    {

        int dx = x - nLastX;
        int dy = y - nLastY;

        if (pDragElement == GUISYSTEM->GetEditCtrl())
        {
            //CGUIEditCtrl* pEditCtrl = GUISYSTEM->GetEditCtrl();
            //if (pEditCtrl->GetClickSide() == CGUIEditCtrl::Side_Inner)
            //{
            //    pEditCtrl->Move(dx, dy);
            //}
            //else
            //    pEditCtrl->Resize(dx, dy);
        }
        else    //cut by liangairan at 2010-7-2
        {
            //OutputDebugString("move drag element!\n");
            pDragElement->Move(dx, dy);
        }
        nLastX = x;
        nLastY = y;
        return true;
    }
    nLastX = x;
    nLastY = y;

    CGUIElement* pEntity = NULL;


    for (GUIELEMENT_RESITER it = m_lstElement.rbegin(); it != m_lstElement.rend(); ++it)
    {
        pEntity = *it;
        if (pEntity->IsVisable() && pEntity->IsInRect(x, y) && pEntity->IsEnable())
        {
            return pEntity->OnMouseMove(x, y);
            //m_pHoverElement = pEntity->OnMouseMove(x, y);
        }
        else if (pEntity == GUISYSTEM->GetDragElement())
        {
            //EndDragElement();
        }
    }

    return false;
}

bool UILayer::OnLButtonDBClick(int x, int y)
{
    CGUIElement* pEntity = NULL;

    for (GUIELEMENT_RESITER it = m_lstElement.rbegin(); it != m_lstElement.rend(); ++it)
    {
        pEntity = *it;
        if (pEntity->IsVisable() && pEntity->IsInRect(x, y) && pEntity->IsEnable())
        {
            return pEntity->OnLButtonDBClick(x, y);
        }
    }

    return false;
}

bool UILayer::OnKeyUp(int nKey)
{
    CGUIElement* pEntity = NULL;
    //最顶端
    for (GUIELEMENT_RESITER it = m_lstElement.rbegin(); it != m_lstElement.rend(); it++)
    {
        pEntity = *it;
        if (pEntity->IsVisable() && pEntity->IsEnable())
        {
            return pEntity->OnKeyUp(nKey);
            //m_pHoverElement = pEntity->OnMouseMove(x, y);
        }

    }
    return false;
}

bool UILayer::OnMouseWheel(int dz)
{
    CGUIElement* pFocusElement = GUISYSTEM->GetFocusElement();
    if (pFocusElement)
    {
        return pFocusElement->OnMouseWheel(dz);
    }

    return false;
}

FrameLayer* UILayer::GetFrameLayer(u32 nIndex)
{
    if (nIndex >= m_lstFrameLayers.size())
    {
        FrameLayer* pLayer = AIR_NEW_T (FrameLayer);
        pLayer->m_nIndex = nIndex;
        m_lstFrameLayers.push_back(pLayer);
        return pLayer;
    }
    else
    {
        for (FrameLayerList_Iter it = m_lstFrameLayers.begin(); it != m_lstFrameLayers.end(); ++it)
        {
            if ((*it)->m_nIndex == nIndex)
            {
                return *it;
            }
            
        }
    }

    return NULL;
}

void UILayer::SaveToXML(CMarkupSTL& xml)
{
    GUIELEMENT_ITER itor = m_lstElement.begin();
    for ( ; itor != m_lstElement.end(); ++itor )
    {
        (*itor)->SaveToXML( xml );
    }
}

void UILayer::CaculateEntityAlignPosition()
{
    GUIELEMENT_ITER itor = m_lstElement.begin();
    for ( ; itor != m_lstElement.end(); ++itor )
    {
        (*itor)->CaculateAlignPosition( );
    }
}

void UILayer::MoveFrameLayerTop(FrameLayer* pFrameLayer)
{
    for (FrameLayerList_Iter itor = m_lstFrameLayers.begin(); itor != m_lstFrameLayers.end(); ++itor)
    {
        if (pFrameLayer == *itor)
        {
            m_lstFrameLayers.erase(itor);
            m_lstFrameLayers.push_front(pFrameLayer);
        }
    }

    //重新计算所有layer的深度
    CaculateLayerDepth();
}

void UILayer::SetAfterLoad()
{
    CGUIElement* pEntity = NULL;
    //最顶端
    for (GUIELEMENT_RESITER it = m_lstElement.rbegin(); it != m_lstElement.rend(); it++)
    {
        pEntity = *it;
        pEntity->SetAfterLoad();
    }
}

void UILayer::CaculateLayerDepth()
{
    f32 fZDelta = (m_fMaxZ - m_fMinZ) / (m_lstFrameLayers.size() + 1);
    f32 fZ = m_fMinZ + fZDelta;
    u32 nIndex = 0;
    for (FrameLayerList_Iter it = m_lstFrameLayers.begin(); it != m_lstFrameLayers.end(); ++it)
    {
        FrameLayer* pLayer = *it;
        pLayer->m_fZ = fZ;
        pLayer->m_nIndex = nIndex++;
        fZ += fZDelta;
    }
}

FrameLayer* UILayer::CreateFrameLayer()
{
    FrameLayer* pLayer = AIR_NEW_T (FrameLayer);
    pLayer->m_nIndex = m_lstFrameLayers.size();
    m_lstFrameLayers.push_back(pLayer);
    CaculateLayerDepth();
    return pLayer;
}
