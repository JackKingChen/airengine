#include "GUIEditCtrl.h"
#include "Engine.h"
#include "GUISystem.h"

CGUIEditCtrl::CGUIEditCtrl() : m_pEditElement(NULL),
m_bMouseClickInSideRect(false),
m_clickType(Side_Inner)
{
    m_bDrawFrame = true;
    m_colorRectFrame = SColor(255, 0, 0, 255);
    m_bMoveable = true;

    //m_ctrlType = eCtrl_EditCtrl;
    m_strType = "CGUIEditCtrl";
    m_uiLayerDef = UI_Layer_Top;

    m_nFrameWidth = 3;
    //m_fDepth = 0.0f;
}

CGUIEditCtrl::~CGUIEditCtrl()
{

}

void CGUIEditCtrl::AttachEditElement(CGUIElement* pElement)
{
    m_pEditElement = pElement;

    if (m_pEditElement)
    {
        m_pEditElement->GetRenderRect(m_renderQuad.m_rectScreen);
        m_realRect = m_renderQuad.m_rectScreen;
        CaculateSideRectPos();
    }
    
}

void CGUIEditCtrl::Move(int dx, int dy)
{
    //if (m_pEditElement)
    //{
    //    if (!m_pEditElement->IsMoveable())
    //    {
    //        return;
    //    }
    //}
    CGUIElement::Move(dx, dy);
    //OutputDebugString(GetString("editctrl move (%d, %d), x=%d, y=%d\n", dx, dy, (int)m_renderQuad.m_rectScreen.left, (int)m_renderQuad.m_rectScreen.top));

    if (m_pEditElement)
    {
        //m_pEditElement->SetRect(m_renderQuad.m_rectScreen);
        m_pEditElement->Move(dx, dy);
    }
    
    m_rectTopLeft.Move(dx, dy);
    m_rectTopMiddle.Move(dx, dy);
    m_rectTopRight.Move(dx, dy);
    m_rectRightMiddle.Move(dx, dy);
    m_rectBottomRight.Move(dx, dy);
    m_rectBottomMiddle.Move(dx, dy);
    m_rectBottomLeft.Move(dx, dy);
    m_rectLeftMiddle.Move(dx, dy);
    
}

void CGUIEditCtrl::RenderSelf()
{
    IRenderer* pRenderer = ENGINE_INST->GetRenderer();
    CGUIElement::RenderSelf();
    //pRenderer->DrawRect(m_renderQuad.m_rectScreen, 1, SColor(255, 255, 255, 0));
    
    pRenderer->DrawRect(m_rectTopLeft, 2, m_colorRectFrame);
    pRenderer->DrawRect(m_rectTopMiddle, 2, m_colorRectFrame);
    pRenderer->DrawRect(m_rectTopRight, 2, m_colorRectFrame);
    pRenderer->DrawRect(m_rectRightMiddle, 2, m_colorRectFrame);
    pRenderer->DrawRect(m_rectBottomRight, 2, m_colorRectFrame);
    pRenderer->DrawRect(m_rectBottomMiddle, 2, m_colorRectFrame);
    pRenderer->DrawRect(m_rectBottomLeft, 2, m_colorRectFrame);
    pRenderer->DrawRect(m_rectLeftMiddle, 2, m_colorRectFrame);
}

bool CGUIEditCtrl::OnLButtonDownSelf(int x, int y)
{
    //if (IsMouseInSideRect(x, y))
    //{
    //    m_bMouseClickInSideRect = true;
    //}
    //else
    //{
    //    m_bMouseClickInSideRect = false;
    //}
    m_clickType = IsMouseInSideRect(x, y);

    return true;
}

bool CGUIEditCtrl::OnLButtonUpSelf(int x, int y)
{
    m_clickType = Side_Inner;
    return true;
}

bool CGUIEditCtrl::OnMouseMoveSelf(int x, int y)
{
#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
    if ((GetAsyncKeyState(VK_LBUTTON) & 0x8000) && m_bMoveable)
    {
        m_nCurUIState = UI_DRAGING;
        if (IsMouseInSideRect(x, y))
        {
        }
        else
        {
            GUISYSTEM->SetDragElement(this);
        }
    }
    else
    {
        GUISYSTEM->SetHoverElement(m_pEditElement);
    }
#endif

    m_ptLastMouse.x = x;
    m_ptLastMouse.y = y;
    return true;
}

void CGUIEditCtrl::CaculateSideRectPos()
{
    int nRectSize = 6;
    int nHalfRectSize = nRectSize / 2;
    m_rectTopLeft.left = m_renderQuad.m_rectScreen.left;
    m_rectTopLeft.top = m_renderQuad.m_rectScreen.top;
    m_rectTopLeft.bottom = m_rectTopLeft.top + nRectSize;
    m_rectTopLeft.right = m_rectTopLeft.left + nRectSize;

    m_rectTopMiddle.left = m_renderQuad.m_rectScreen.left + m_renderQuad.m_rectScreen.GetWidth() / 2 - nHalfRectSize;
    m_rectTopMiddle.top = m_renderQuad.m_rectScreen.top;
    m_rectTopMiddle.right = m_rectTopMiddle.left + nRectSize;
    m_rectTopMiddle.bottom = m_rectTopMiddle.top + nRectSize;

    m_rectTopRight.right = m_renderQuad.m_rectScreen.right;
    m_rectTopRight.top = m_renderQuad.m_rectScreen.top;
    m_rectTopRight.left = m_rectTopRight.right - nRectSize;
    m_rectTopRight.bottom = m_rectTopRight.top + nRectSize;

    m_rectRightMiddle.right = m_renderQuad.m_rectScreen.right;
    m_rectRightMiddle.top = m_renderQuad.m_rectScreen.top + m_renderQuad.m_rectScreen.GetHeight() / 2 - nHalfRectSize;
    m_rectRightMiddle.left = m_rectRightMiddle.right - nRectSize;
    m_rectRightMiddle.bottom = m_rectRightMiddle.top + nRectSize;

    m_rectBottomRight.right = m_renderQuad.m_rectScreen.right;
    m_rectBottomRight.bottom = m_renderQuad.m_rectScreen.bottom;
    m_rectBottomRight.left = m_renderQuad.m_rectScreen.right - nRectSize;
    m_rectBottomRight.top = m_rectBottomRight.bottom - nRectSize;

    m_rectBottomMiddle.left = m_renderQuad.m_rectScreen.left + m_renderQuad.m_rectScreen.GetWidth() / 2 - nHalfRectSize;
    m_rectBottomMiddle.bottom = m_renderQuad.m_rectScreen.bottom;
    m_rectBottomMiddle.right = m_rectBottomMiddle.left + nRectSize;
    m_rectBottomMiddle.top = m_rectBottomMiddle.bottom - nRectSize;

    m_rectBottomLeft.left = m_renderQuad.m_rectScreen.left;
    m_rectBottomLeft.bottom = m_renderQuad.m_rectScreen.bottom;
    m_rectBottomLeft.right = m_rectBottomLeft.left + nRectSize;
    m_rectBottomLeft.top = m_rectBottomLeft.bottom - nRectSize;

    m_rectLeftMiddle.left = m_renderQuad.m_rectScreen.left;
    m_rectLeftMiddle.right = m_rectLeftMiddle.left + nRectSize;
    m_rectLeftMiddle.top = m_renderQuad.m_rectScreen.top + m_renderQuad.m_rectScreen.GetHeight() / 2 - nHalfRectSize;
    m_rectLeftMiddle.bottom = m_rectLeftMiddle.top + nRectSize;
}

CGUIEditCtrl::eClickSide CGUIEditCtrl::IsMouseInSideRect(int x, int y)
{
    if (m_rectTopLeft.IsInRect(x, y))
    {
        return Side_Topleft;
    }
    if (m_rectTopMiddle.IsInRect(x, y))
    {
        return Side_TopMiddle;
    }
    if (m_rectTopRight.IsInRect(x, y) )
    {
        return Side_TopRight;
    }
    if (m_rectRightMiddle.IsInRect(x, y))
    {
        return Side_RightMiddle;
    }
    if (m_rectBottomRight.IsInRect(x, y))
    {
        return Side_BottomRight;
    }
    if (m_rectBottomMiddle.IsInRect(x, y))
    {
        return Side_BottomMiddle;
    }
    if (m_rectBottomLeft.IsInRect(x, y))
    {
        return Side_BottomLeft;
    }
    if (m_rectLeftMiddle.IsInRect(x, y))
    {
        return Side_LeftMiddle;
    }

    return Side_Inner;
    /*
    return m_rectTopLeft.IsInRect(x, y) ||
        m_rectTopMiddle.IsInRect(x, y) ||
        m_rectTopRight.IsInRect(x, y) ||
        m_rectRightMiddle.IsInRect(x, y) ||
        m_rectBottomRight.IsInRect(x, y) ||
        m_rectBottomMiddle.IsInRect(x, y) ||
        m_rectBottomLeft.IsInRect(x, y) ||
        m_rectLeftMiddle.IsInRect(x, y);
        */
}

CGUIEditCtrl::eClickSide CGUIEditCtrl::GetClickSide(int x, int y)
{
    
    if (m_rectTopLeft.IsInRect(x, y))
    {
        m_clickType = Side_Topleft;
        return Side_Topleft;
    }
    if (m_rectTopMiddle.IsInRect(x, y))
    {
        m_clickType = Side_TopMiddle;
        return Side_TopMiddle;
    }
    if (m_rectTopRight.IsInRect(x, y) )
    {
        m_clickType = Side_TopRight;
        return Side_TopRight;
    }
    if (m_rectRightMiddle.IsInRect(x, y))
    {
        m_clickType = Side_RightMiddle;
        return Side_RightMiddle;
    }
    if (m_rectBottomRight.IsInRect(x, y))
    {
        m_clickType = Side_BottomRight;
        return Side_BottomRight;
    }
    if (m_rectBottomMiddle.IsInRect(x, y))
    {
        m_clickType = Side_BottomMiddle;
        return Side_BottomMiddle;
    }
    if (m_rectBottomLeft.IsInRect(x, y))
    {
        m_clickType = Side_BottomLeft;
        return Side_BottomLeft;
    }
    if (m_rectLeftMiddle.IsInRect(x, y))
    {
        m_clickType = Side_LeftMiddle;
        return Side_LeftMiddle;
    }
    if (IsInRect(x, y))
    {
        m_clickType = Side_Inner;
        return Side_Inner;
    }
    
    m_clickType = Side_Out;
    return m_clickType;
}

void CGUIEditCtrl::Resize(int dx, int dy)
{
    switch (m_clickType)
    {
    case Side_TopMiddle:
        m_renderQuad.m_rectScreen.top += dy;
        //m_rectTopLeft.Move(0, dy);
        //m_rectTopMiddle.Move(0, dy);
        //m_rectTopRight.Move(0, dy);
        break;
    case Side_BottomMiddle:
        m_renderQuad.m_rectScreen.bottom += dy;
        //m_rectBottomRight.Move(0, dy);
        //m_rectBottomMiddle.Move(0, dy);
        //m_rectBottomLeft.Move(0, dy);
        break;
    case Side_LeftMiddle:
        m_renderQuad.m_rectScreen.left += dx;
        break;
    case Side_RightMiddle:
        m_renderQuad.m_rectScreen.right += dx;
        break;
    case Side_Topleft:
        m_renderQuad.m_rectScreen.left += dx;
        m_renderQuad.m_rectScreen.top += dy;
        break;
    case Side_TopRight:
        m_renderQuad.m_rectScreen.right += dx;
        m_renderQuad.m_rectScreen.top += dy;
        break;
    case Side_BottomRight:
        m_renderQuad.m_rectScreen.right += dx;
        m_renderQuad.m_rectScreen.bottom += dy;
        break;
    case Side_BottomLeft:
        m_renderQuad.m_rectScreen.left += dx;
        m_renderQuad.m_rectScreen.bottom += dy;
        break;
    }

    m_realRect = m_renderQuad.m_rectScreen;
    CaculateSideRectPos();
    if (m_pEditElement)
    {
        m_pEditElement->SetRect(m_renderQuad.m_rectScreen);
    }
}

void CGUIEditCtrl::SetClickSide(eClickSide side)
{
    m_clickType = side;
}

void CGUIEditCtrl::Update()
{
    if (m_pEditElement)
    {
        m_pEditElement->GetRenderRect(m_renderQuad.m_rectScreen);
        m_realRect = m_renderQuad.m_rectScreen;
        CaculateSideRectPos();
    }
}

void CGUIEditCtrl::Render()
{
    if (m_bShow)
    {
        RenderSelf();
        RenderChild();
    }
    
}
