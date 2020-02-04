//#include "StdAfx.h"
#include "GUIScrollBar.h"
#include "GUISystem.h"

CGUIScrollBar::CGUIScrollBar(void) : m_pRelatedElement(NULL)
{
    //m_ctrlType = eCtrl_ScrollBar;
    m_strType = "CGUIScrollBar";
	m_bMoveable = false;
}

CGUIScrollBar::~CGUIScrollBar(void)
{
}

void CGUIScrollBar::SetRelatedCtrl(const char* szCtrlName)
{
    if ( strcmp(szCtrlName, "Parent") == 0 )
    {
        SetRelatedCtrl(m_pParent);
    }
    else
	{
        SetRelatedCtrl(GUISYSTEM->FindCtrl(szCtrlName));
	}
}

void CGUIScrollBar::SetRelatedCtrl(CGUIElement* pElement)
{
    m_pRelatedElement = pElement;
	if ( !pElement )
	{
		m_strRelatedCtrl.clear();
	}
	else
	{
		m_strRelatedCtrl = pElement->GetName();
	}
}

bool CGUIScrollBar::CreateSelf(CGUIElement* pParent, CMarkupSTL& xml)
{
    if (!CGUIElement::CreateSelf(pParent, xml))
    {
        return false;
    }

    AIRString strValue;
    strValue = xml.GetAttrib("RelatedElement").c_str();

    SetRelatedCtrl(strValue.c_str());

    m_strBtnUp = xml.GetAttrib("ButtonUp").c_str();
    m_strBtnDown = xml.GetAttrib("ButtonDown").c_str();
    m_strSlider = xml.GetAttrib("Slider").c_str();
    
    return true;
}

/*
bool CGUIScrollBar::IsInRect(int x, int y)
{
    if (m_pBtnStart.IsInRect(x, y))
    {
        return true;
    }

    if (m_slider.IsInRect(x, y))
    {
        return true;
    }

    if (m_btnEnd.IsInRect(x, y))
    {
        return true;
    }

    return false;
}
*/

bool CGUIScrollBar::OnMouseWheel(int dz)
{
    m_pSlider->OnMouseWheel(dz);

    CalculateRelatedCtrlPos();
    return true;
}

bool CGUIScrollBar::Create(CGUIElement* pParent, CMarkupSTL& xml)
{
    if (!CGUIElement::Create(pParent, xml))
    {
        return false;
    }

    m_strBtnUp = xml.GetAttrib("BtnUp").c_str();
    m_strBtnDown = xml.GetAttrib("BtnDown").c_str();
    m_strSlider = xml.GetAttrib("Slider").c_str();

    m_pBtnStart = dynamic_cast<CGUIButton*>(GetChild(m_strBtnUp.c_str()));
    m_pBtnEnd = dynamic_cast<CGUIButton*>(GetChild(m_strBtnDown.c_str()));
    m_pSlider = dynamic_cast<CGUISlider*>(GetChild(m_strSlider.c_str()));

    m_strRelatedCtrl = xml.GetAttrib("RelatedCtrl").c_str();
    
    return true;
}

bool CGUIScrollBar::SaveSelfToXML(CMarkupSTL& xml )
{
	CGUIElement::SaveSelfToXML( xml );
	xml.AddAttrib( "BtnUp", m_strBtnUp.c_str() );
	xml.AddAttrib( "BtnDown", m_strBtnDown.c_str() );
	xml.AddAttrib( "Slider", m_strSlider.c_str() );
	xml.AddAttrib( "RelatedCtrl", m_strRelatedCtrl.c_str() );
	return true;
}

bool CGUIScrollBar::OnLButtonUp(int x, int y)
{
    if (m_pSlider)
    {
    }
    if (m_pBtnStart->IsInRect(x, y) && m_pBtnStart->OnLButtonUp(x, y))
    {
        //向上移动
        m_pSlider->MinusCallBack();
        CalculateRelatedCtrlPos();
        return true;
    }

    if (m_pBtnEnd->IsInRect(x, y) && m_pBtnEnd->OnLButtonUp(x, y))
    {
        //向下移动
        m_pSlider->PlusCallBack();
        CalculateRelatedCtrlPos();
        return true;
    }

    if (m_pSlider->IsInRect(x, y) && m_pSlider->OnLButtonUp(x, y))
    {
        return true;
    }

    return true;
}

bool CGUIScrollBar::OnLButtonDown(int x, int y)
{
    if (m_pBtnStart && m_pBtnStart->IsInRect(x, y))
    {
        return m_pBtnStart->OnLButtonDown(x, y);
    }

    if (m_pBtnEnd && m_pBtnEnd->IsInRect(x, y))
    {
        return m_pBtnEnd->OnLButtonDown(x, y);
    }

    if (m_pSlider && m_pSlider->IsInRect(x, y))
    {
        //double dOrinValue = m_pSlider->GetCurValue();
        if (m_pSlider->OnLButtonDown(x, y))
        {
            CalculateRelatedCtrlPos();
        }
    }
    return true;
}

bool CGUIScrollBar::OnMouseMove(int x, int y)
{
    if (m_pBtnStart && m_pBtnStart->IsInRect(x, y))
    {
        return m_pBtnStart->OnMouseMove(x, y);
    }

    if (m_pBtnEnd && m_pBtnEnd->IsInRect(x, y))
    {
        return m_pBtnEnd->OnMouseMove(x, y);
    }

    if (m_pSlider && m_pSlider->IsInRect(x, y))
    {
#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
        //double dOrinValue = m_pSlider->GetCurValue();
        if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
        {
            if (m_pSlider->OnMouseMove(x, y) )
            {
                CalculateRelatedCtrlPos();
            }
            
        }
#endif
    }
    return true;
}

void CGUIScrollBar::CalculateRelatedCtrlPos()
{
    if (m_pRelatedElement)
    {
        //double dPercentValue = m_pSlider->GetPercent();
        double dCurValue = m_pSlider->GetCurValue();
        u8 byOrient = m_pSlider->GetOrient();
        RECT rectScissor;
        m_pRelatedElement->GetScissorRect(rectScissor);
        int nPosX = rectScissor.left;
        int nPosY = rectScissor.top;

        if (byOrient == eORIENT_HORZ)
        {
            nPosX -= dCurValue * m_nRelatedCtrlOffset;
            //int dx = -dPercentValue * m_nRelatedCtrlOffset; //(m_pRelatedElement->GetRealWidth() - (rectScissor.right - rectScissor.left));
            m_pRelatedElement->MoveRealRectTo(nPosX, nPosY);
        }
        else 
        {
            int nHTopDelta = m_nRelatedCtrlOffset * dCurValue;
            nPosY = m_renderQuad.m_rectScreen.top - nHTopDelta;
            m_pRelatedElement->MoveRealRectTo(nPosX, nPosY);
            /*
            nPosY -= dCurValue * m_nRelatedCtrlOffset;
            
            m_pRelatedElement->MoveRealRectTo(nPosX, nPosY);

            if (dCurValue == 0.0)
            {
                Rectf rectReal;
                m_pRelatedElement->GetRealRect(rectReal);
                int dy = rectScissor.top - rectReal.top; 
                m_pRelatedElement->MoveRealRect(0, dy);
            }
            else if (dCurValue == 1.0)
            {

            }
            */
        } 
    }
}

void CGUIScrollBar::SetAfterLoadSelf()
{
    m_pRelatedElement = m_pScheme->FindCtrl(m_strRelatedCtrl.c_str());

    if (m_pRelatedElement && m_pSlider)
    {
        //计算出slider的deltavalue
        //m_pSlider->SetRelatedCtrl(m_pRelatedElement);
        RECT rectScissor;
        m_pRelatedElement->GetScissorRect(rectScissor);
        Rectf rectCtrl;
        m_pRelatedElement->GetRealRect(rectCtrl);
        if (m_pSlider->GetOrient() == eORIENT_HORZ)
        {
            m_nRelatedCtrlOffset = rectCtrl.right - rectScissor.right;
        }
        else
            m_nRelatedCtrlOffset = rectCtrl.bottom - rectScissor.bottom;

        m_pSlider->SetMovable(false);
    }
}

void CGUIScrollBar::CreateDefault()
{
    m_renderQuad.m_rectScreen.SetRect(10, 10, 26, 60);
    m_pBtnStart = new CGUIButton;
    m_pBtnStart->SetName("ScrollBarBtnUp");
    m_pBtnStart->SetParent(this);
    AddChild(m_pBtnStart);
    m_pBtnStart->SetRect(Rectf(10, 10, 26, 20));
    m_strBtnUp = "ScrollBarBtnUp";

    m_pBtnEnd = new CGUIButton;
    m_pBtnEnd->SetParent(this);
    m_pBtnEnd->SetName("ScrollBarBtnDown");
    m_pBtnEnd->SetRect(Rectf(10, 50, 26, 60));
    AddChild(m_pBtnEnd);
    m_strBtnDown = "ScrollBarBtnDown";

    m_pSlider = new CGUISlider;
    m_pSlider->SetName("ScrollBarSlider");
    m_pSlider->SetOrientation(eORIENT_VERT);
    m_pSlider->SetParent(this);
    m_pSlider->SetRect(Rectf(10, 20, 26, 50));
    m_pSlider->CreateDefault();
    AddChild(m_pSlider);
    m_strSlider = "ScrollBarSlider";
}

void CGUIScrollBar::ModifyChildrenPosition()
{
    if (m_pSlider)
    {
        //垂直方向
        if (m_pSlider->GetOrient() == eORIENT_VERT)
        {
            if (m_pBtnStart)
            {
                m_pBtnStart->MoveTo(m_renderQuad.m_rectScreen.left + (m_renderQuad.m_rectScreen.GetWidth() - m_pBtnStart->GetWidth()) / 2, m_renderQuad.m_rectScreen.top);
            }
            if (m_pBtnEnd)
            {
                m_pBtnEnd->MoveTo(m_renderQuad.m_rectScreen.left + (m_renderQuad.m_rectScreen.GetWidth() - m_pBtnEnd->GetWidth()) / 2, m_renderQuad.m_rectScreen.bottom - m_pBtnEnd->GetHeight());
            }
            m_pSlider->SetRect(Rectf(m_renderQuad.m_rectScreen.left, m_pBtnStart->GetBottom(), m_renderQuad.m_rectScreen.right, m_pBtnEnd->GetTop()));
        }
        else
        {
            if (m_pBtnStart)
            {
                m_pBtnStart->MoveTo(m_renderQuad.m_rectScreen.left, m_renderQuad.m_rectScreen.top + (m_renderQuad.m_rectScreen.GetHeight() - m_pBtnStart->GetHeight()) / 2);
            }
            if (m_pBtnEnd)
            {
                m_pBtnEnd->MoveTo(m_renderQuad.m_rectScreen.right - m_pBtnEnd->GetWidth(), m_renderQuad.m_rectScreen.top + (m_renderQuad.m_rectScreen.GetHeight() - m_pBtnEnd->GetHeight()) / 2);
            }
            m_pSlider->SetRect(Rectf(m_pBtnStart->GetRight(), m_renderQuad.m_rectScreen.top, m_pBtnEnd->GetLeft(), m_renderQuad.m_rectScreen.bottom));
        }
    }
}

void CGUIScrollBar::SetButtonWidth(int nWidth)
{
    if (m_pBtnStart)
    {
        m_pBtnStart->SetWidth(nWidth);
    }

    if (m_pBtnEnd)
    {
        m_pBtnEnd->SetWidth(nWidth);
    }

    ModifyChildrenPosition();
}

void CGUIScrollBar::SetButtonHeight(int nHeight)
{
    if (m_pBtnStart)
    {
        m_pBtnStart->SetHeight(nHeight);
    }

    if (m_pBtnEnd)
    {
        m_pBtnEnd->SetHeight(nHeight);
    }

    ModifyChildrenPosition();
}

void CGUIScrollBar::SetSliderBtnWidth(int nWidth)
{
    if (m_pSlider)
    {
        m_pSlider->SetSliderWidth(nWidth);
    }

    ModifyChildrenPosition();
}

void CGUIScrollBar::SetSliderBtnHeight(int nHeight)
{
    if (m_pSlider)
    {
        m_pSlider->SetSliderHeight(nHeight);
    }

    ModifyChildrenPosition();
}

void CGUIScrollBar::SetRect(const Rectf& rect)
{
    CGUIElement::SetRect(rect);
    ModifyChildrenPosition();
}

void CGUIScrollBar::UpdateSelf()
{
    if (m_pRelatedElement)
    {
        if (GUISYSTEM->GetDragElement() == m_pSlider->GetSliderButton())
        {
            CalculateRelatedCtrlPos();
            return;
        }
        m_nRelatedCtrlOffset = m_pRelatedElement->GetRealHeight() - GetRealHeight();
        m_nRelatedCtrlOffset = m_nRelatedCtrlOffset < 0 ? 0 : m_nRelatedCtrlOffset;
        if (m_nRelatedCtrlOffset == 0)
        {
            m_pSlider->SetCurValue(0);
            m_pSlider->CaculateSliderPosition();
        }
        else
        {
            float fCurValue = m_pSlider->GetCurValue();
            //这里要调整大小
            if (m_pSlider->GetOrient() == eORIENT_VERT)
            {
                int nHTop = m_renderQuad.m_rectScreen.top - m_pRelatedElement->GetRealRect().top;

                float realCurValue = (float)nHTop / m_nRelatedCtrlOffset; 
                realCurValue = realCurValue > 1.0f ? 1.0f : realCurValue;
                m_pSlider->SetCurValue(realCurValue);
            }
            else
            {

            }
            
            m_pSlider->CaculateSliderPosition();
        }
    }
}
