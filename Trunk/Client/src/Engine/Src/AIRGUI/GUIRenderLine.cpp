//#include "StdAfx.h"
#include "GUIRenderLine.h"
#include "GUISystem.h"

CGUIRenderLine::CGUIRenderLine(void) : m_nLineIndex(0), m_bSelected(false), m_clrSelected(128, 255, 255, 255)
{
    //m_bDrawFrame = true;
	m_strType = "CGUIRenderLine";
}

CGUIRenderLine::~CGUIRenderLine(void)
{
}

void CGUIRenderLine::SetIndex(int nIndex)
{
    m_nLineIndex = nIndex;
}

CGUIElement* CGUIRenderLine::GetLastElement()
{
    if (m_lstChildren.size() == 0)
    {
        return NULL;
    }

    std::list<CGUIElement*>::reverse_iterator it = m_lstChildren.rbegin();//m_lstChildren[m_lstChildren.size() - 1];
    return *it;
}

void CGUIRenderLine::AddChild(CGUIElement* pChild)
{
    bool bNeedReset = false;
    //int nBottom = m_renderQuad.m_rectScreen.bottom;
    if (m_lstChildren.size() == 0)
    {
        pChild->SetScreenPos(m_renderQuad.m_rectScreen.left, m_renderQuad.m_rectScreen.top);
    }
    else
    {
        /*
        CGUIElement* pLast = GetLastElement();  //取出最后一个渲染的元素
        Rectf rectLast;
        pLast->GetRenderRect(rectLast);
        int nChildY = rectLast.bottom - pChild->GetHeight();
        //if (nChildY < m_renderQuad.m_rectScreen.top)
        //{
        //    nChildY += pChild->GetHeight() - GetHeight();
        //}
        pChild->SetScreenPos(rectLast.right, nChildY);

        Rectf rectChild;
        pChild->GetRenderRect(rectChild);
        if (rectChild.top < m_renderQuad.m_rectScreen.top)
        {
            Move(0, rectChild.top - rectLast.top);//rectChild.bottom - m_renderQuad.m_rectScreen.bottom);
            m_renderQuad.m_rectScreen.top = rectChild.top;   //调整line的区域
            m_renderQuad.m_rectScreen.bottom = rectChild.bottom;
        }
        //-------------------------
        // 调整实际框的右边界
        m_renderQuad.m_rectScreen.right = rectChild.right;
        */
        CGUIElement* pLast = GetLastElement();  //取出最后一个渲染的元素
        Rectf rectLast;
        pLast->GetRenderRect(rectLast);
        int nChildY = m_renderQuad.m_rectScreen.top - (pChild->GetHeight() - GetHeight());
        //if (nChildY < m_renderQuad.m_rectScreen.top)
        //{
        //    nChildY = m_renderQuad.m_rectScreen.top;
        //}
        pChild->SetScreenPos(rectLast.right, nChildY);
        m_renderQuad.m_rectScreen.right = pChild->GetRight();//rectChild.right;
    }
    
    //-------------------------
    m_lstChildren.push_back(pChild);

    if (pChild->GetHeight() > GetHeight())
    {
        int nOffsetY = m_renderQuad.m_rectScreen.top - pChild->GetTop();
        RecaculateChildPosition(nOffsetY);
        m_renderQuad.m_rectScreen.bottom = pChild->GetBottom();
    }
}

/*
void CGUIRenderLine::CaculateWordPosition()
{
    switch (m_align)
    {
    case eAlign_Left:
        SetWordPosLeft();
        break;
    case eAlign_Middle:
        SetWordPosMiddle();
        break;
    case eAlign_RightTop:
        SetWordPosRight();
        break;
    }
}
*/

void CGUIRenderLine::SetWordPosLeft()
{
    if (m_lstChildren.size() == 0)
    {
        return;
    }

    CGUIElement* pWord = *m_lstChildren.begin();
    int nLeft = pWord->GetLeft();
    MoveChild(nLeft - m_renderQuad.m_rectScreen.left, 0);
}

void CGUIRenderLine::SetWordPosMiddle()
{
    if (m_lstChildren.size() == 0)
    {
        return;
    }

    CGUIElement* pFirst = *m_lstChildren.begin();
    CGUIElement* pLast = GetLastElement();

    int nChildMiddle = pLast->GetRight() - pLast->GetLeft();
    int nMiddle = (m_renderQuad.m_rectScreen.right - m_renderQuad.m_rectScreen.left) / 2;

    MoveChild(nMiddle - nChildMiddle, 0);
}

void CGUIRenderLine::SetWordPosRight()
{
    if (m_lstChildren.size() == 0)
    {
        return;
    }

    CGUIElement* pLast = GetLastElement();
    MoveChild(m_renderQuad.m_rectScreen.right - pLast->GetRight(), 0);
}

void CGUIRenderLine::SetWordsColor(const SColor& color)
{
    CGUIElement* pElement = NULL;
    //CGUIWord* pWord = NULL;
    for (GUIELEMENT_ITER it = m_lstChildren.begin(); it != m_lstChildren.end(); ++it)
    {
        pElement = *it;
        if (pElement->GetType() == "CGUIWord")
        {
            pElement->SetColor(color);
        }
    }
}

void CGUIRenderLine::SetSelected(bool bSelected)
{
    m_bSelected = bSelected;
}

void CGUIRenderLine::RenderSelf()
{
    CGUIElement::RenderSelf();
    //if (m_bSelected)
    //{
    //    GUISYSTEM->GetGUIRenderer()->Render(m_renderQuad.m_rectScreen, Rectf::ZERO, NULL, m_clrSelected);
    //}
}

void CGUIRenderLine::RecaculateChildPosition(int nOffsetY)
{
    CGUIElement* pElement = NULL;
    //CGUIWord* pWord = NULL;
    GUIELEMENT_ITER itEnd = m_lstChildren.end();
    for (GUIELEMENT_ITER it = m_lstChildren.begin(); it != m_lstChildren.end(); ++it)
    {
        pElement = *it;
        pElement->Move(0, nOffsetY);
    }
}
