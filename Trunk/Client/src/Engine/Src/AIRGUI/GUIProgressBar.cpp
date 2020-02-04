//#include "StdAfx.h"
#include "GUIProgressBar.h"
#include "ImageSet.h"
#include "GUISystem.h"
//#include "RenderSystem.h"
//#include "ImageSetManager.h"
#include "StringUtil.h"

CGUIProgressBar::CGUIProgressBar(void)
{
    m_fCurValue = 0.0f;
    m_byOrient = eORIENT_HORZ; 
    m_bCanBeSet = false; 
    m_dwColorBar = 0xffffffff;
    //m_barQuad.blend = BLEND_COLORMUL | BLEND_ALPHABLEND;
	m_strType = "CGUIProgressBar";
	m_bMoveable = false;
}

CGUIProgressBar::~CGUIProgressBar(void)
{
}

CGUIElement* CGUIProgressBar::Clone()
{
    return NULL;
}


void CGUIProgressBar::RenderSelf()
{
    CGUIElement::RenderSelf();

    GUISYSTEM->GetGUIRenderer()->Render(m_RectBar, m_RectBarUV, m_renderQuad.m_fDepth, m_renderQuad.m_pTexture);

    //UpdateBarQuad();
    //GUISYSTEM->GetHGE()->Gfx_RenderQuad(&m_barQuad);
    //LPDIRECT3DDEVICE9 pDevice = RENDERSYSTEM->GetDevice();
    //m_renderQuad.m_pTexture->Use(0);
 
    //pDevice->SetFVF(RHWVERTEX::FVF);

    //pDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, (void*)m_vtVertices, sizeof(RHWVERTEX));

    // 绘制背景
    //if( m_bDrawBG )
    //    RENDERSYSTEM->DrawRectWithImage( m_renderQuad.m_rectScreen, m_UV, m_renderQuad.m_clrDiffuse, m_renderQuad.m_pTexture );
    // 绘制进度条
    //RENDERSYSTEM->DrawRectWithImage( m_RectTemp, m_RectTempUV, m_dwColorBar, m_renderQuad.m_pTexture );
}

bool CGUIProgressBar::CreateSelf(CGUIElement* pParent, CMarkupSTL& xml)
{
    CGUIElement::CreateSelf(pParent, xml);

    AIRString strValue = xml.GetAttrib("Orient").c_str();

    if (strValue == "eORIENT_HORZ")
    {
        m_byOrient = eORIENT_HORZ;
    }
    else
    {
        m_byOrient = eORIENT_VERT;
    }

    strValue = xml.GetAttrib("Value").c_str();
    m_fCurValue = (float)atof( strValue.c_str() );

    strValue = xml.GetAttrib("BarPos").c_str();
    //char szTmp[256] = { 0 };
    float topLeftX, topLeftY, bottomRightX, bottomRightY;
    sscanf(strValue.c_str(), "(%f, %f), (%f, %f)", &topLeftX, &topLeftY, &bottomRightX, &bottomRightY);

    int nParentWidth = GetWidth();
    int nParentHeight = GetHeight();

    m_RectBar.SetRect(nParentWidth * topLeftX, nParentHeight * topLeftY, nParentWidth * bottomRightX, nParentHeight * bottomRightY);
    m_RectBar.left += m_renderQuad.m_rectScreen.left;
    m_RectBar.top += m_renderQuad.m_rectScreen.top;
    m_RectBar.right += m_renderQuad.m_rectScreen.left;
    m_RectBar.bottom += m_renderQuad.m_rectScreen.top;

    // 控件滑条纹理
    strValue = xml.GetAttrib("BarImgSrc").c_str();
	m_szImgSrcBar = strValue;
    AIRString szImgSet, szImgUnitBG;
    CStringUtil::SplitFilename( strValue, szImgSet, szImgUnitBG );
    CImageSet* is = GetScheme()->GetImageSet( szImgSet );
    //m_barQuad.tex = is->GetTexture();
    CImageUnit* iu = GetScheme()->GetImageUnit( szImgSet, szImgUnitBG );
    if( iu ) 
    {
        m_RectBarUV = iu->m_fUV;
        m_renderQuad.m_pTexture = is->GetTexture();
    }
    
    // 根据当前值初始化进度条的长度
    m_RectTemp = m_RectBar;
    m_RectTempUV = m_RectBarUV;
    if( m_byOrient ==  eORIENT_HORZ )
    {
        m_RectTemp.right = m_RectTemp.left + m_fCurValue * m_RectBar.GetWidth();
        m_RectTempUV.right = m_RectTempUV.left + m_fCurValue * m_RectTempUV.GetWidth();;
    }
    else if( m_byOrient ==  eORIENT_VERT )
    {
        m_RectTemp.top = m_RectTemp.bottom - m_fCurValue * m_RectBar.GetHeight();
        m_RectTempUV.top = m_RectTempUV.bottom - m_fCurValue * m_RectTempUV.GetHeight();
    }
   
    return true;
}


bool CGUIProgressBar::SaveSelfToXML(CMarkupSTL& xml )
{
	CGUIElement::SaveSelfToXML(xml);

	if (m_byOrient == eORIENT_HORZ)
	{
		xml.AddAttrib("Orient","eORIENT_HORZ");
	}
	else
	{
		xml.AddAttrib("Orient","ORIENT_VERT");
	}

	xml.AddAttrib( "Value", GetString("%.4f",m_fCurValue) );

	if ( m_szImgSrcBar.size() > 0 )
	{
		xml.AddAttrib( "BarImgSrc", m_szImgSrcBar.c_str() );
	}

	return true;
}



void CGUIProgressBar::UpdateSelf()
{
    CGUIElement::UpdateSelf();
    //m_RectTemp = m_RectBar;
    //m_RectTempUV = m_RectBarUV;
    if( m_byOrient ==  eORIENT_HORZ )
    {
        m_RectTemp.right = m_RectTemp.left + m_fCurValue * m_RectBar.GetWidth();
        m_RectTempUV.right = m_RectTempUV.left + m_fCurValue * m_RectTempUV.GetWidth();
    }
    else if( m_byOrient ==  eORIENT_VERT )
    {
        m_RectTemp.top = m_RectTemp.bottom - m_fCurValue * m_RectBar.GetHeight();
        m_RectTempUV.top = m_RectTempUV.bottom - m_fCurValue * m_RectTempUV.GetHeight();
    }
}

/*
void CGUIProgressBar::UpdateBarQuad()
{
    m_barQuad.v[0].x = (float)m_RectBar.left;
    m_barQuad.v[0].y = (float)m_RectBar.top;
    m_barQuad.v[0].z = 0;

    m_barQuad.v[0].tx = m_RectTempUV.left;
    m_barQuad.v[0].ty = m_RectTempUV.top;
    m_barQuad.v[0].col = m_renderQuad.m_clrDiffuse;


    //右上角
    m_barQuad.v[1].x = (float)m_RectBar.right;
    m_barQuad.v[1].y = (float)m_RectBar.top;
    m_barQuad.v[1].z = 0;
    //m_barQuad.v[1].rhw = 1.0;
    m_barQuad.v[1].tx = m_RectTempUV.right;
    m_barQuad.v[1].ty = m_RectTempUV.top;
    m_barQuad.v[1].col = m_renderQuad.m_clrDiffuse;


    //右下角
    m_barQuad.v[2].x = (float)m_RectBar.right;
    m_barQuad.v[2].y = (float)m_RectBar.bottom;
    m_barQuad.v[2].z = 0.0;
    //m_barQuad.v[3].rhw = 1.0;
    m_barQuad.v[2].tx = m_RectTempUV.right;
    m_barQuad.v[2].ty = m_RectTempUV.bottom;
    m_barQuad.v[2].col = m_renderQuad.m_clrDiffuse;

    //左下角
    m_barQuad.v[3].x = (float)m_RectBar.left;
    m_barQuad.v[3].y = (float)m_RectBar.bottom;
    m_barQuad.v[3].z = 0.0;
    //m_barQuad.v[2].rhw = 1.0;
    m_barQuad.v[3].tx = m_RectTempUV.left;
    m_barQuad.v[3].ty = m_RectTempUV.bottom;
    m_barQuad.v[3].col = m_renderQuad.m_clrDiffuse;
}
*/

void CGUIProgressBar::Move(int dx, int dy)
{
    CGUIElement::Move(dx, dy);
    m_RectBar.Move(dx, dy);
}

void CGUIProgressBar::SetValue(float fValue)
{
    m_fCurValue = fValue;
    CaculateBarPosition();
}

/*
void CGUIProgressBar::Update()
{
    if( m_bCanBeSet )
    {
        
        CInputSystem* pInputSystem = INPUTSYSTEM;
        if (!pInputSystem->IsMouseInRect(m_renderQuad.m_rectScreen.top, m_renderQuad.m_rectScreen.left, m_renderQuad.m_rectScreen.bottom, m_renderQuad.m_rectScreen.right))
        {
            return;
        }
        if (!pInputSystem->LButtonDown())
            return;

        int x, y;
        pInputSystem->GetMousePosInWindow(x, y);
        //if( !( IsInClipRect(x,y) && INPUTSYSTEM->LButtonDown() ) )
        //    return;
        m_RectTemp = m_RectBar;
        m_RectTempUV = m_RectBarUV;
        if( m_byOrient == ORIENT_HORZ )
        {
            m_RectTemp.right = ( x > m_RectBar.right ? m_RectBar.right : ( x < m_RectBar.left ? m_RectBar.left : x ) );
            m_RectTempUV.right = m_RectTempUV.left + m_RectBarUV.GetWidth() * m_RectTemp.GetWidth() / m_RectBar.GetWidth();
        }
        else if( m_byOrient == ORIENT_VERT )
        {
            m_RectTemp.top = ( y > m_RectBar.bottom ? m_RectBar.bottom : ( y < m_RectBar.top ? m_RectBar.top : y ) );
            m_RectTempUV.top = m_RectTempUV.bottom - m_RectBarUV.GetHeight() * m_RectTemp.GetHeight() / m_RectBar.GetHeight();
        }
    }
    else
    {
        m_RectTemp = m_RectBar;
        m_RectTempUV = m_RectBarUV;
        if( m_byOrient ==  ORIENT_HORZ )
        {
            m_RectTemp.right = m_RectTemp.left + m_fCurValue * m_RectBar.GetWidth();
            m_RectTempUV.right = m_RectTempUV.left + m_fCurValue * m_RectTempUV.GetWidth();
        }
        else if( m_byOrient ==  ORIENT_VERT )
        {
            m_RectTemp.top = m_RectTemp.bottom - m_fCurValue * m_RectBar.GetHeight();
            m_RectTempUV.top = m_RectTempUV.bottom - m_fCurValue * m_RectTempUV.GetHeight();
        }
    }
    CGUIElement::Update();
}
*/

void CGUIProgressBar::SetBarImageSrc(const AIRString& strImageSrc)
{
    if (m_szImgSrcBar == strImageSrc)
    {
        return;
    }

    AIRString strImageSet;
    AIRString strImageUnit;
    CStringUtil::SplitFilename(strImageSrc, strImageSet, strImageUnit);

    CImageUnit* pUnit = m_pScheme->GetImageUnit(strImageSet, strImageUnit);
    CImageSet*  pImgSet = m_pScheme->GetImageSet(strImageSet);
    if (pUnit)
    {
        m_RectBarUV = pUnit->m_fUV;
        m_szImgSrcBar = strImageSrc;
        if (m_renderQuad.m_pTexture == NULL && pImgSet != NULL)
        {
            m_renderQuad.m_pTexture = pImgSet->GetTexture();
        }
    }
}

void CGUIProgressBar::SetOrientation(eGUIOrientation orient)
{
    m_byOrient = orient;
}

void CGUIProgressBar::CaculateBarPosition()
{
    if (m_byOrient == eORIENT_HORZ)
    {
        m_RectBar.left = m_renderQuad.m_rectScreen.left;
        m_RectBar.right = m_RectBar.left + m_renderQuad.m_rectScreen.GetWidth() * m_fCurValue;
        m_RectBar.top = m_renderQuad.m_rectScreen.top;
        m_RectBar.bottom = m_renderQuad.m_rectScreen.bottom;
    }
    else
    {
        m_RectBar.top = m_renderQuad.m_rectScreen.top;
        m_RectBar.bottom = m_RectBar.top + m_renderQuad.m_rectScreen.GetHeight() * m_fCurValue;
        m_RectBar.left = m_renderQuad.m_rectScreen.left;
        m_RectBar.right = m_renderQuad.m_rectScreen.right;
    }
}

void CGUIProgressBar::SetRect(const Rectf& rect)
{
    CGUIElement::SetRect(rect);
    CaculateBarPosition();
}

void CGUIProgressBar::SetWidth(int nWidth)
{
    CGUIElement::SetWidth(nWidth);
    CaculateBarPosition();
}

void CGUIProgressBar::SetHeight(int nHeight)
{
    CGUIElement::SetHeight(nHeight);
    CaculateBarPosition();
}
