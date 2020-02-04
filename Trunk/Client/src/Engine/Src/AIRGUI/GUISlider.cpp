//#include "StdAfx.h"
#include "GUISlider.h"
#include "Engine.h"
#include "StringUtil.h"
#include "ImageSet.h"
#include "GUISystem.h"
#include "GUIRenderer.h"

CGUISlider::CGUISlider(void) : m_pDragBarFun(NULL), m_colorBar(0xffffffff), m_colorSlider(0xffffffff), m_byOrient(eORIENT_HORZ), m_bySliderState(eSlider_Normal), m_dCurValue(0.0),
m_dDeltaValue(0.1),
m_nSliderWidth(10),
m_nSliderHeight(10),
m_nBarWidth(4),
m_nBarHeight(4),
m_pSliderBtn(NULL)
{
    //m_ctrlType = eCtrl_Slider;
    m_strType = "CGUISlider";
	m_szImgSrcBar.clear();
	m_szImgSrcSliderBtn.clear();
    m_rectSlider.SetRect(0, 0, m_nSliderWidth, m_nSliderHeight);
	m_bMoveable = true;
}

CGUISlider::~CGUISlider(void)
{
}

void CGUISlider::UpdateSelf()
{
    if (m_pSliderBtn/* && GUISYSTEM->GetDragElement() == m_pSliderBtn*/)
    {
        AdjustSliderBtnPosition();
        CaculateCurValue();
    }
    /*
    if (m_byOrient == eORIENT_HORZ)   //水平
    {
        if (m_rectSlider.left < m_renderQuad.m_rectScreen.left)
        {
            int dx = m_rectBar.left - m_rectSlider.left;
            m_rectSlider.Move(dx, 0);
        }
        else if (m_rectSlider.right > m_rectBar.right)
        {
            int dx = m_rectBar.right - m_rectSlider.right;
            m_rectSlider.Move(dx, 0);
        }
    }
    else
    {
        if (m_rectSlider.top < m_rectBar.top)
        {
            int dy = m_rectBar.top - m_rectSlider.top;
            m_rectSlider.Move(0, dy);
        }
        else if (m_rectSlider.bottom > m_rectBar.bottom)
        {
            int dy = m_rectBar.bottom - m_rectSlider.bottom;
            m_rectSlider.Move(0, dy);
        }
    }
    */
}

bool CGUISlider::CreateSelf(CGUIElement* pParent, CMarkupSTL& xml)
{
    if (!CGUIElement::CreateSelf(pParent, xml))
        return false;

    AIRString strValue;
    strValue = xml.GetAttrib("Orient").c_str();
    if (strValue == "eORIENT_HORZ")
    {
        m_byOrient = eORIENT_HORZ;
    }
    else if (strValue == "eORIENT_VERT")
    {
        m_byOrient = eORIENT_VERT;
    }

    strValue = xml.GetAttrib("BarImgSrc").c_str();
	m_szImgSrcBar = strValue;
    AIRString strImgSet;
    AIRString strImgUnit;

    CImageSet* pImageSet = NULL;
    CImageUnit* pIU = NULL;

    CStringUtil::SplitFilename( strValue, strImgSet, strImgUnit );
    if( strImgSet.size() == 0 && strImgUnit.size() == 0 )
        strImgSet = strValue;

    pImageSet = GetScheme()->GetImageSet(strImgSet);
    if ( pImageSet )
    {
        m_renderQuad.m_pTexture = pImageSet->GetTexture();

        pIU = GetScheme()->GetImageUnit(strImgSet, strImgUnit); //TextureMgr::GetInstancePtr()->GetImageUnit( strImgSet, strImgUnit );
        if( pIU ) 
        {
            m_barUV = pIU->m_fUV;
        }
    }


    strValue = xml.GetAttrib("SliderImgSrc").c_str();
	m_szImgSrcSliderBtn = strValue;
    CStringUtil::SplitFilename( strValue, strImgSet, strImgUnit );
    if( strImgSet.size() == 0 && strImgUnit.size() == 0 )
        strImgSet = strValue;
    pIU = GetScheme()->GetImageUnit(strImgSet, strImgUnit); //TextureMgr::GetInstancePtr()->GetImageUnit( strImgSet, strImgUnit );
    if( pIU ) 
    {
        m_sliderUV = pIU->m_fUV;
    }

    strValue = xml.GetAttrib("BarWidth").c_str();
    m_nBarWidth = atoi(strValue.c_str());
    strValue = xml.GetAttrib("BarHeight").c_str();
    m_nBarHeight = atoi(strValue.c_str());

	strValue = xml.GetAttrib("SliderWidth").c_str();
	m_nSliderWidth = atoi(strValue.c_str());
	strValue = xml.GetAttrib("SliderHeight").c_str();
	m_nSliderHeight = atoi(strValue.c_str());

    m_strSliderBtn = xml.GetAttrib("SliderButton").c_str();

    strValue = xml.GetAttrib("DeltaValue").c_str();
    m_dDeltaValue = atof(strValue.c_str());

    //下面代码屏蔽掉，改成是button
    //strValue = xml.GetAttrib("SliderWidth");
    //int nSliderWidth = atoi(strValue.c_str());
    //strValue = xml.GetAttrib("SliderHeight");
    //int nSliderHeight = atoi(strValue.c_str());

    /*
    m_rectSlider.left = m_renderQuad.m_rectScreen.left + m_renderQuad.m_rectScreen.GetWidth() / 2 - nSliderWidth / 2;
    m_rectSlider.top = m_renderQuad.m_rectScreen.top + m_renderQuad.m_rectScreen.GetHeight() / 2 - nSliderHeight / 2;
    m_rectSlider.right = m_rectSlider.left + nSliderWidth;
    m_rectSlider.bottom = m_rectSlider.top + nSliderHeight;

    if (m_byOrient == eORIENT_HORZ)   //水平方向
    {
        m_rectBar.left = m_renderQuad.m_rectScreen.left;
        m_rectBar.top = (int)(m_renderQuad.m_rectScreen.top + m_renderQuad.m_rectScreen.GetHeight() / 2 - m_nBarHeight / 2);
        m_rectBar.right = m_renderQuad.m_rectScreen.right;
        m_rectBar.bottom = m_rectBar.top + nSliderHeight;

        m_rectCenter = m_rectBar;
        m_rectCenter.left += nSliderWidth / 2;
        m_rectCenter.right -= nSliderWidth / 2;
    }
    else
    {
        m_rectBar.top = m_renderQuad.m_rectScreen.top;
        m_rectBar.bottom = m_renderQuad.m_rectScreen.bottom;
        m_rectBar.left = (int)(m_renderQuad.m_rectScreen.left + m_renderQuad.m_rectScreen.GetWidth() / 2 - m_nBarWidth / 2);
        m_rectBar.right = m_rectBar.left + m_nBarWidth;

        m_rectCenter = m_rectBar;
        m_rectCenter.top += nSliderHeight / 2;
        m_rectCenter.bottom -= nSliderHeight / 2;
    }
    */

    strValue = xml.GetAttrib("CurValue").c_str();
    m_dCurValue = atof(strValue.c_str());

    CaculateSliderPosition();

    return true;
}

void CGUISlider::SetAfterLoad()
{
    m_pSliderBtn = static_cast<CGUIButton*>(GetChild(m_strSliderBtn.c_str()));
    if (m_pSliderBtn)
    {
        m_pSliderBtn->SetRect(m_rectSlider);
        m_pSliderBtn->SetMovable(true);
    }
    CaculateSliderRects();
    CaculateSliderPosition();
}

bool CGUISlider::SaveSelfToXML(CMarkupSTL& xml )
{
	CGUIElement::SaveSelfToXML(xml);

	if (m_byOrient == eORIENT_HORZ)
	{
		xml.AddAttrib( "Orient", "eORIENT_HORZ" );
	}
	else if (m_byOrient == eORIENT_VERT)
	{
		xml.AddAttrib( "Orient", "eORIENT_VERT" );
	}

	if ( m_szImgSrcBar.length() > 0 )
	{
		xml.AddAttrib( "BarImgSrc", m_szImgSrcBar.c_str() );
	}

	if ( m_szImgSrcSliderBtn.length() > 0 )
	{
		xml.AddAttrib( "SliderImgSrc", m_szImgSrcSliderBtn.c_str() );
	}

	xml.AddAttrib( "BarWidth", GetString("%d", (int)m_rectBar.GetWidth()) );
	xml.AddAttrib( "BarHeight", GetString("%d", (int)m_rectBar.GetHeight()) );
	xml.AddAttrib( "DeltaValue", GetString("%.4f", m_dDeltaValue) );
	xml.AddAttrib( "SliderWidth", GetString("%d",(int)m_rectSlider.GetWidth() ) );
	xml.AddAttrib( "SliderHeight", GetString("%d",(int)m_rectSlider.GetHeight() ) );
    if (m_pSliderBtn)
    {
        xml.AddAttrib("SliderButton", m_pSliderBtn->GetName().c_str() );
    }
	
	m_dCurValue = m_dCurValue > 1.0 ? 1.0 : m_dCurValue;
	m_dCurValue = m_dCurValue < 0.0 ? 0.0 : m_dCurValue;
	xml.AddAttrib( "CurValue", GetString("%.2f",m_dCurValue) );

	return true;
}


void CGUISlider::RenderSelf()
{
    //CGUIElement::RenderSelf();

    //IRenderer* pRenderer = ENGINE_INST->GetRenderer();
    //pRenderer->Draw2DImage(m_rectBar, m_barUV, m_renderQuad.m_pTexture, m_colorBar);
    //pRenderer->Draw2DImage(m_rectSlider, m_sliderUV, m_renderQuad.m_pTexture, m_colorSlider);
    if (m_renderQuad.m_pTexture)
    {
        m_renderQuad.m_fDepth = m_pFrameLayer->m_fZ;
        GUIRENDERER->Render( m_rectBar, m_barUV, m_renderQuad.m_fDepth, m_renderQuad.m_pTexture, m_colorBar );
        if (!m_pSliderBtn)
        {
            GUIRENDERER->Render( m_rectSlider, m_sliderUV, m_renderQuad.m_fDepth, m_renderQuad.m_pTexture, m_colorSlider );
        }
    }   
}

void CGUISlider::Move(int dx, int dy)
{
    CGUIElement::Move(dx, dy);
    m_rectBar.Move(dx, dy);
    m_rectSlider.Move(dx, dy);
    m_rectCenter.Move(dx, dy);
}

bool CGUISlider::OnLButtonDownSelf(int x, int y)
{
    GUISYSTEM->SetFocusElement(this);
    m_bFocus = true;
    if (/*m_rectSlider.IsInRect(x, y) || */m_pSliderBtn->IsInRect(x, y))
    {
        m_bySliderState = eSlider_LButtonDown;
    }
    else 
    {
        m_bySliderState = eSlider_Normal;
        //设置位置
        if (m_byOrient == eORIENT_HORZ)
        {
            if (m_pSliderBtn)
            {
                int dx = x - (m_pSliderBtn->GetLeft() + m_pSliderBtn->GetWidth() / 2);
                int dy = 0;
                m_pSliderBtn->Move(dx, dy);
                //m_rectSlider.Move(dx, dy);
            }
            else
            {
                int dx = x - (m_rectSlider.left + m_rectSlider.GetWidth() / 2);
                int dy = 0;
                m_rectSlider.Move(dx, dy);
            }
        }
        else 
        {
            if (m_pSliderBtn)
            {
                int dx = 0;
                int dy = y - (m_pSliderBtn->GetTop() + m_pSliderBtn->GetHeight() / 2);
                m_pSliderBtn->Move(dx, dy);
            }
            else
            {
                int dx = 0;
                int dy = y - (m_rectSlider.top + m_rectSlider.GetHeight() / 2);
                m_rectSlider.Move(dx, dy);
            }
            
        }
    }

    AdjustSliderBtnPosition();

    CaculateCurValue();

    return true;
}

void CGUISlider::SetCurValue(double dwValue)
{
    m_dCurValue = dwValue;
    //OutputDebugString(GetString("set slider value %f\n", m_dCurValue));
}

void CGUISlider::SetDeltaValue(double dwValue)
{
    m_dDeltaValue = dwValue;
}

bool CGUISlider::OnMouseWheel(int dz)
{
    m_dCurValue += dz * m_dDeltaValue;
    int dx = 0;
    int dy = 0;
    
    if (m_byOrient == eORIENT_HORZ)
    {
        dx = m_rectBar.GetWidth() * dz * m_dDeltaValue;
        m_rectSlider.Move(dx, 0);
    }
    else
    {
        dy = m_rectBar.GetHeight() * dz * m_dDeltaValue;
        m_rectSlider.Move(0, dy);
    }

    AdjustSliderPosition();
    CaculateCurValue();
    m_dCurValue = m_dCurValue > 1.0 ? 1.0 : m_dCurValue;
    m_dCurValue = m_dCurValue < 0.0 ? 0.0 : m_dCurValue;



    return true;
}

double CGUISlider::GetDeltaValue() const 
{
    return m_dDeltaValue;
}

double CGUISlider::GetCurValue() const 
{
    return m_dCurValue;
}

eGUIOrientation CGUISlider::GetOrient() const 
{
    return m_byOrient;
}

void CGUISlider::MinusCallBack()
{
    if (m_dCurValue == 0.0)
    {
        return;
    }
    m_dCurValue -= m_dDeltaValue; 
    if( m_dCurValue < 0.0 )
        m_dCurValue = 0.0;
    
    //调整位置
    CaculateSliderPosition();
}

void CGUISlider::PlusCallBack()
{
    if (m_dCurValue == 1.0)
    {
        return;
    }
    m_dCurValue += m_dDeltaValue;

    if( m_dCurValue > 1.0 )
        m_dCurValue = 1.0;
    CaculateSliderPosition();
}

void CGUISlider::SetRect(const Rectf& rect)
{
    CGUIElement::SetRect(rect);
    CaculateSliderRects();
}

void CGUISlider::CaculateSliderRects()
{
    int nSliderWidth = m_byOrient == eORIENT_VERT ? m_renderQuad.m_rectScreen.GetWidth() : m_nSliderWidth;
    int nSliderHeight = m_byOrient == eORIENT_VERT ? m_nSliderHeight : m_renderQuad.m_rectScreen.GetHeight();
    //if (m_pSliderBtn)
    //{
    //    nSliderWidth = m_pSliderBtn->GetWidth();
    //    nSliderHeight = m_pSliderBtn->GetHeight();
    //}
    m_rectSlider.left = m_renderQuad.m_rectScreen.left; // + m_renderQuad.m_rectScreen.GetWidth() / 2 - nSliderWidth / 2;
    m_rectSlider.top = m_renderQuad.m_rectScreen.top; // + m_renderQuad.m_rectScreen.GetHeight() / 2 - nSliderHeight / 2;
    
    int dx = 0;
    int dy = 0;

    if (m_byOrient == eORIENT_HORZ)   //水平方向
    {
        m_rectSlider.right = m_rectSlider.left + nSliderWidth;
        m_rectSlider.bottom = m_renderQuad.m_rectScreen.bottom;
        m_rectBar.left = m_renderQuad.m_rectScreen.left;
        m_rectBar.top = m_renderQuad.m_rectScreen.top + m_renderQuad.m_rectScreen.GetHeight() / 2 - m_nBarHeight / 2;
        m_rectBar.right = m_renderQuad.m_rectScreen.right;
        m_rectBar.bottom = m_rectBar.top + m_nBarHeight;

        m_rectCenter = m_rectBar;
        m_rectCenter.left += nSliderWidth / 2;
        m_rectCenter.right -= nSliderWidth / 2;

        dx = m_dCurValue * (m_renderQuad.m_rectScreen.GetWidth() - nSliderWidth); 
    }
    else
    {
        m_rectSlider.right = m_renderQuad.m_rectScreen.right;
        m_rectSlider.bottom = m_renderQuad.m_rectScreen.top + nSliderHeight;

        m_rectBar.top = m_renderQuad.m_rectScreen.top;
        m_rectBar.bottom = m_renderQuad.m_rectScreen.bottom;
        m_rectBar.left = m_renderQuad.m_rectScreen.left + m_renderQuad.m_rectScreen.GetWidth() / 2 - m_nBarWidth / 2;
        m_rectBar.right = m_rectBar.left + m_nBarWidth;

        m_rectCenter = m_rectBar;
        m_rectCenter.top += nSliderHeight / 2;
        m_rectCenter.bottom -= nSliderHeight / 2;

        dy = m_dCurValue * (m_renderQuad.m_rectScreen.GetHeight() - nSliderHeight);
    }

    m_rectSlider.Move(dx, dy);

    if (m_pSliderBtn)
    {
        m_pSliderBtn->SetRect(m_rectSlider);
    }
}

void CGUISlider::CreateDefault()
{
    m_pSliderBtn = new CGUIButton;
    m_strSliderBtn = "SliderButton";
    m_pSliderBtn->SetName(m_strSliderBtn);
    AddChild(m_pSliderBtn);
    m_pSliderBtn->SetSchemeManager(m_pScheme);
    m_pSliderBtn->SetRect(m_rectSlider);
    m_pSliderBtn->SetMovable(true);
}

void CGUISlider::CaculateSliderPosition()
{
    //int dx = 0;
    //int dy = 0;
    if (m_byOrient == eORIENT_HORZ)    //水平方向
    {
        if (m_pSliderBtn)
        {
            m_pSliderBtn->MoveTo(m_renderQuad.m_rectScreen.left + m_dCurValue * (m_renderQuad.m_rectScreen.GetWidth() - m_pSliderBtn->GetWidth()), m_renderQuad.m_rectScreen.top);
        }
        else
        {
            
            //int nCenterX = m_rectCenter.left + m_dCurValue / 1.0 * m_rectCenter.GetWidth();
            //dx = nCenterX - (m_rectSlider.left + m_rectSlider.right) / 2;
        }
        m_rectSlider.MoveTo(m_renderQuad.m_rectScreen.left + m_dCurValue * (m_renderQuad.m_rectScreen.GetWidth() - m_rectSlider.GetWidth()), m_renderQuad.m_rectScreen.top);
    }
    else
    {
        if (m_pSliderBtn)
        {
            m_pSliderBtn->MoveTo(m_renderQuad.m_rectScreen.left, m_renderQuad.m_rectScreen.top + m_dCurValue * (m_renderQuad.m_rectScreen.GetHeight() - m_pSliderBtn->GetHeight()));
        }
        else
        {
            
            //int nCenterY = m_rectCenter.top + m_dCurValue / 1.0 * m_rectCenter.GetHeight();
            //dy = nCenterY - (m_rectSlider.top + m_rectSlider.bottom) / 2;
        }
        m_rectSlider.MoveTo(m_renderQuad.m_rectScreen.left, m_renderQuad.m_rectScreen.top + m_dCurValue * (m_renderQuad.m_rectScreen.GetHeight() - m_rectSlider.GetHeight()));
    }

    //m_pSliderBtn->Move(dx, dy);
    //m_rectSlider.Move(dx, dy);

    //if (m_pSliderBtn)
    //{
    //    m_pSliderBtn->Move(dx, dy);
    //}
}

double CGUISlider::CaculateCurValue()
{
    //水平方向
	
    if (m_byOrient == eORIENT_HORZ)
    {
		if (m_pSliderBtn)
		{
			m_dCurValue = ((m_pSliderBtn->GetLeft() + m_pSliderBtn->GetRight()) * 0.5 - m_rectCenter.left) / m_rectCenter.GetWidth();
		}
		else
			m_dCurValue = ((m_rectSlider.left + m_rectSlider.right) * 0.5 - m_rectCenter.left) / m_rectCenter.GetWidth();
    }
    else
    {
        if (m_pSliderBtn)
        {
            m_dCurValue = ((m_pSliderBtn->GetTop() + m_pSliderBtn->GetBottom()) * 0.5 - m_rectCenter.top) / m_rectCenter.GetHeight();
        }
        else
            m_dCurValue = ((m_rectSlider.top + m_rectSlider.bottom) * 0.5 - m_rectCenter.top) / m_rectCenter.GetHeight();
    }

    m_dCurValue = m_dCurValue > 1.0 ? 1.0 : m_dCurValue;
    m_dCurValue = m_dCurValue < 0.0 ? 0.0 : m_dCurValue;

    return m_dCurValue;
}

void CGUISlider::AdjustSliderPosition()
{
    int dx = 0; 
    int dy = 0;
    if (m_byOrient == eORIENT_HORZ)   //水平
    {
        if (m_rectSlider.left < m_renderQuad.m_rectScreen.left)
        {
            dx = m_rectBar.left - m_rectSlider.left;
            m_rectSlider.Move(dx, 0);
            
        }
        else if (m_rectSlider.right > m_rectBar.right)
        {
            dx = m_rectBar.right - m_rectSlider.right;
            m_rectSlider.Move(dx, 0);
        }
        
    }
    else
    {
        if (m_rectSlider.top < m_rectBar.top)
        {
            dy = m_rectBar.top - m_rectSlider.top;
            m_rectSlider.Move(0, dy);
        }
        else if (m_rectSlider.bottom > m_rectBar.bottom)
        {
            dy = m_rectBar.bottom - m_rectSlider.bottom;
            m_rectSlider.Move(0, dy);
        }
    }

    if (m_pSliderBtn)
    {
        m_pSliderBtn->SetRect(m_rectSlider);
    }
}

void CGUISlider::AdjustSliderBtnPosition()
{
    //int dx = 0; 
    //int dy = 0;
    if (m_byOrient == eORIENT_HORZ)   //水平
    {
        if (m_pSliderBtn->GetLeft() < m_renderQuad.m_rectScreen.left)
        {
            //dx = m_rectBar.left - m_rectSlider.left;
            m_pSliderBtn->MoveTo(m_renderQuad.m_rectScreen.left, m_pSliderBtn->GetTop());

        }
        if (m_pSliderBtn->GetRight() > m_rectBar.right)
        {
            //dx = m_rectBar.right - m_rectSlider.right;
            //m_rectSlider.Move(dx, 0);
            m_pSliderBtn->MoveTo(m_renderQuad.m_rectScreen.right - m_pSliderBtn->GetWidth(), m_pSliderBtn->GetTop());
        }
        if (m_pSliderBtn->GetTop() < m_renderQuad.m_rectScreen.top)
        {
            //dy = m_rectBar.top - m_rectSlider.top;
            m_rectSlider.MoveTo(m_rectSlider.left, m_renderQuad.m_rectScreen.top);
            m_pSliderBtn->MoveTo(m_pSliderBtn->GetLeft(), m_renderQuad.m_rectScreen.top);
        }
        if (m_pSliderBtn->GetBottom() > m_rectBar.bottom)
        {
            //dy = m_rectBar.bottom - m_rectSlider.bottom;
            m_rectSlider.Move(m_rectSlider.left, m_renderQuad.m_rectScreen.bottom - m_pSliderBtn->GetHeight());
            m_pSliderBtn->MoveTo(m_pSliderBtn->GetLeft(), m_renderQuad.m_rectScreen.bottom - m_pSliderBtn->GetHeight());
        }
    }
    else
    {
        if (m_pSliderBtn->GetTop() < m_renderQuad.m_rectScreen.top)
        {
            //dy = m_rectBar.top - m_rectSlider.top;
            m_rectSlider.MoveTo(m_rectSlider.left, m_renderQuad.m_rectScreen.top);
            m_pSliderBtn->MoveTo(m_renderQuad.m_rectScreen.left, m_renderQuad.m_rectScreen.top);
        }
        if (m_pSliderBtn->GetBottom() > m_renderQuad.m_rectScreen.bottom)
        {
            //dy = m_rectBar.bottom - m_rectSlider.bottom;
            m_rectSlider.Move(m_rectSlider.left, m_renderQuad.m_rectScreen.bottom - m_pSliderBtn->GetHeight());
            m_pSliderBtn->MoveTo(m_renderQuad.m_rectScreen.left, m_renderQuad.m_rectScreen.bottom - m_pSliderBtn->GetHeight());
        }

        if (m_pSliderBtn->GetLeft() < m_renderQuad.m_rectScreen.left)
        {
            m_pSliderBtn->MoveTo(m_renderQuad.m_rectScreen.left, m_pSliderBtn->GetTop());
        }

        if (m_pSliderBtn->GetRight() > m_renderQuad.m_rectScreen.right)
        {
            m_pSliderBtn->MoveTo(m_renderQuad.m_rectScreen.right - m_pSliderBtn->GetWidth(), m_pSliderBtn->GetTop());
        }
    }
}

void CGUISlider::SetOrientation(eGUIOrientation orient)
{
    m_byOrient = orient;
}

/*
void CGUISlider::SetRelatedCtrl(CGUIElement* pCtrl)
{
    m_pRelatedCtrl = pCtrl;
}
*/


void CGUISlider::SetSliderBtnImageSrc(const AIRString& strImageSrc)
{
    if (m_szImgSrcSliderBtn == strImageSrc)
    {
        return;
    }

    AIRString strImageSet;
    AIRString strImageUnit;
    CStringUtil::SplitFilename(strImageSrc, strImageSet, strImageUnit);
    CImageSet*  pImageSet = m_pScheme->GetImageSet(strImageSet);
    if (pImageSet)
    {
        if (m_renderQuad.m_pTexture == NULL)
        {
            m_renderQuad.m_pTexture = pImageSet->GetTexture();
        }
    }
    CImageUnit* pUnit = m_pScheme->GetImageUnit(strImageSet, strImageUnit);
    if (pUnit)
    {
        m_sliderUV = pUnit->m_fUV;
        m_szImgSrcSliderBtn = strImageSrc;
    }

    if (m_pSliderBtn)
    {
        m_pSliderBtn->SetImageSrc(strImageSrc);
    }
}

void CGUISlider::SetBarImageSrc(const AIRString& strImageSrc)
{
    if (m_szImgSrcBar == strImageSrc)
    {
        return;
    }

    AIRString strImageSet;
    AIRString strImageUnit;
    CStringUtil::SplitFilename(strImageSrc, strImageSet, strImageUnit);
    CImageSet*  pImageSet = m_pScheme->GetImageSet(strImageSet);
    if (pImageSet)
    {
        if (m_renderQuad.m_pTexture == NULL)
        {
            m_renderQuad.m_pTexture = pImageSet->GetTexture();
        }
    }
    CImageUnit* pUnit = m_pScheme->GetImageUnit(strImageSet, strImageUnit);
    if (pUnit)
    {
        m_barUV = pUnit->m_fUV;
        m_szImgSrcBar = strImageSrc;
    }
}

void CGUISlider::SetBarWidth(int nWidth)
{
    m_nBarWidth = nWidth;
    CaculateSliderRects();
}

void CGUISlider::SetBarHeight(int nHeight)
{
    m_nBarHeight = nHeight;
    CaculateSliderRects();
}

void CGUISlider::SetSliderWidth(int nWidth)
{
    m_nSliderWidth = nWidth;
    //if (m_pSliderBtn)
    //{
    //    m_pSliderBtn->SetWidth
    //}
    CaculateSliderRects();
}

void CGUISlider::SetSliderHeight(int nHeight)
{
    m_nSliderHeight = nHeight;
    CaculateSliderRects();
}

bool CGUISlider::OnMouseMove(int x, int y)
{
    CGUIElement::OnMouseMove(x, y);
    CaculateCurValue();
    AdjustSliderBtnPosition();
    //OutputDebugString(GetString("slider mouse move, curvalue %f\n", m_dCurValue));
    return true;
}
