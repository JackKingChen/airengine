#include "GUIAutoSizeFrame.h"


void CGUIAutoSizeFrame::SetImgSrcTopLeft( const AIRString& szImgSrc )
{
	if ( !m_pTopLeftImg )
	{
		m_pTopLeftImg = new CGUIImage;
		m_pTopLeftImg->SetName( "AutoSizeFrame_GUIImgTopLeft" );
		AddChild( m_pTopLeftImg );
        m_pTopLeftImg->SetParentLayer(true);
	}
	m_pTopLeftImg->SetImageSrc( szImgSrc );
	m_pTopLeftImg->AutoSize();
	m_pTopLeftImg->Show();
    m_strImgSrcTopLeft = szImgSrc;
}

void CGUIAutoSizeFrame::SetImgSrcTop( const AIRString& szImgSrc )
{
	if ( !m_pTopImg )
	{
		m_pTopImg = new CGUIImage;
		m_pTopImg->SetName( "AutoSizeFrame_GUIImgTop" );
		AddChild( m_pTopImg );
        m_pTopImg->SetParentLayer(true);
	}
	m_pTopImg->SetImageSrc( szImgSrc );
	m_pTopImg->AutoSize();
	m_pTopImg->Show();
    m_strImgSrcTop = szImgSrc;
}

void CGUIAutoSizeFrame::SetImgSrcTopRight( const AIRString& szImgSrc )
{
	if ( !m_pTopRightImg )
	{
		m_pTopRightImg = new CGUIImage;
		m_pTopRightImg->SetName( "AutoSizeFrame_GUIImgTopRight" );
		AddChild( m_pTopRightImg );
        m_pTopRightImg->SetParentLayer(true);
	}
	m_pTopRightImg->SetImageSrc( szImgSrc );
	m_pTopRightImg->AutoSize();
	m_pTopRightImg->Show();
    m_strImgSrcTopRight = szImgSrc;
}

void CGUIAutoSizeFrame::SetImgSrcRight( const AIRString& szImgSrc )
{
	if ( !m_pRightImg )
	{
		m_pRightImg = new CGUIImage;
		m_pRightImg->SetName( "AutoSizeFrame_GUIImgRight" );
		AddChild( m_pRightImg );
        m_pRightImg->SetParentLayer(true);
	}
	m_pRightImg->SetImageSrc( szImgSrc );
	m_pRightImg->AutoSize();
	m_pRightImg->Show();
    m_strImgSrcRight = szImgSrc;
}

void CGUIAutoSizeFrame::SetImgSrcBottomRight( const AIRString& szImgSrc )
{
	if ( !m_pBottomRightImg )
	{
		m_pBottomRightImg = new CGUIImage;
		m_pBottomRightImg->SetName( "AutoSizeFrame_GUIImgBottomRight" );
		AddChild( m_pBottomRightImg );
        m_pBottomRightImg->SetParentLayer(true);
	}
	m_pBottomRightImg->SetImageSrc( szImgSrc );
	m_pBottomRightImg->AutoSize();
	m_pBottomRightImg->Show();
    m_strImgSrcBottomRight = szImgSrc;
}

void CGUIAutoSizeFrame::SetImgSrcBottom( const AIRString& szImgSrc )
{
	if ( !m_pBottomImg )
	{
		m_pBottomImg = new CGUIImage;
		m_pBottomImg->SetName( "AutoSizeFrame_GUIImgBottom" );
		AddChild( m_pBottomImg );
        m_pBottomImg->SetParentLayer(true);
	}
	m_pBottomImg->SetImageSrc( szImgSrc );
	m_pBottomImg->AutoSize();
	m_pBottomImg->Show();
    m_strImgSrcBottom = szImgSrc;
}

void CGUIAutoSizeFrame::SetImgSrcBottomLeft( const AIRString& szImgSrc )
{
	if ( !m_pBottomLeftImg )
	{
		m_pBottomLeftImg = new CGUIImage;
		m_pBottomLeftImg->SetName( "AutoSizeFrame_GUIImgBottomLeft" );
		AddChild( m_pBottomLeftImg );
        m_pBottomLeftImg->SetParentLayer(true);
	}
	m_pBottomLeftImg->SetImageSrc( szImgSrc );
	m_pBottomLeftImg->AutoSize();
	m_pBottomLeftImg->Show();
    m_strImgSrcBottomLeft = szImgSrc;
}

void CGUIAutoSizeFrame::SetImgSrcLeft( const AIRString& szImgSrc )
{
	if ( !m_pLeftImg )
	{
		m_pLeftImg = new CGUIImage;
		m_pLeftImg->SetName( "AutoSizeFrame_GUIImgLeft" );
		AddChild( m_pLeftImg );
        m_pLeftImg->SetParentLayer(true);
	}
	m_pLeftImg->SetImageSrc( szImgSrc );
	m_pLeftImg->AutoSize();
	m_pLeftImg->Show();
    m_strImgSrcLeft = szImgSrc;
}

void CGUIAutoSizeFrame::SetImgSrcCenter( const AIRString& szImgSrc )
{
	if ( !m_pCenterImg )
	{
		m_pCenterImg = new CGUIImage;
		m_pCenterImg->SetName( "AutoSizeFrame_GUIImgCenter" );
		AddChild( m_pCenterImg );
        m_pCenterImg->SetParentLayer(true);
	}
	m_pCenterImg->SetImageSrc( szImgSrc );
	m_pCenterImg->AutoSize();
	m_pCenterImg->Show();
    m_strImgSrcCenter = szImgSrc;
}

void CGUIAutoSizeFrame::Resize( void )
{
	//if ( !m_pTextContent )
	//{
	//	return;
	//}
	//Rectf rc;
	//m_pTextContent->CalWholeRealSize( rc );
	if ( m_pTopLeftImg )
	{
		m_pTopLeftImg->AutoSize();
		//m_pTopLeftImg->MoveTo( rc.GetLeft() - m_pTopLeftImg->GetWidth(), rc.GetTop() - m_pTopLeftImg->GetHeight());
	}
    s32 nWidth = 0;
    s32 nHeight = 0;
    if (m_pTopLeftImg && m_pTopRightImg)
    {
        nWidth = m_renderQuad.m_rectScreen.GetWidth() - m_pTopLeftImg->GetWidth() - m_pTopRightImg->GetWidth();
    }
    
    if (m_pTopLeftImg && m_pBottomLeftImg)
    {
        nHeight = m_renderQuad.m_rectScreen.GetHeight() - m_pTopLeftImg->GetHeight() - m_pBottomLeftImg->GetHeight();
    }

    //定义中间矩形
    Rectf rc;
    rc.left = m_pTopLeftImg ? m_pTopLeftImg->GetRight() : m_renderQuad.m_rectScreen.left;
    rc.top = m_pTopLeftImg ? m_pTopLeftImg->GetBottom() : m_renderQuad.m_rectScreen.top;
    rc.right = m_pTopLeftImg ? m_pTopLeftImg->GetRight() + m_renderQuad.m_rectScreen.GetWidth() : m_renderQuad.m_rectScreen.right;
    rc.bottom = m_pTopLeftImg ? m_pTopLeftImg->GetBottom() + m_renderQuad.m_rectScreen.GetHeight() : m_renderQuad.m_rectScreen.bottom;
    
	if ( m_pTopImg )
	{
		m_pTopImg->AutoSize();
        
		m_pTopImg->SetWidth(nWidth);
		m_pTopImg->MoveTo(rc.left, rc.top - m_pTopImg->GetHeight());
	}
	if ( m_pTopRightImg )
	{
		m_pTopRightImg->AutoSize();
		m_pTopRightImg->MoveTo(rc.right, rc.top - m_pTopRightImg->GetHeight());
	}
	if ( m_pRightImg )
	{
		m_pRightImg->AutoSize();
		m_pRightImg->SetHeight(nHeight);
		m_pRightImg->MoveTo(rc.right, rc.top);
	}
	if ( m_pBottomRightImg )
	{
		m_pBottomRightImg->AutoSize();
		m_pBottomRightImg->MoveTo( rc.right, rc.bottom );
	}
	if ( m_pBottomImg )
	{
		m_pBottomImg->AutoSize();
		m_pBottomImg->SetWidth( nWidth );
		m_pBottomImg->MoveTo( m_pBottomLeftImg->GetRight(), m_pBottomLeftImg->GetTop() );
	}
	if ( m_pBottomLeftImg )
	{
		m_pBottomLeftImg->AutoSize();
		m_pBottomLeftImg->MoveTo( rc.left, rc.bottom);
	}
	if ( m_pLeftImg )
	{
		m_pLeftImg->AutoSize();
		m_pLeftImg->SetHeight( rc.GetHeight() );
		m_pLeftImg->MoveTo( rc.left - m_pLeftImg->GetWidth(), rc.top );
	}
	if ( m_pCenterImg )
	{
		m_pCenterImg->SetWidth( rc.GetWidth() );
		m_pCenterImg->SetHeight( rc.GetHeight() );
		m_pCenterImg->MoveTo( rc.GetLeft(), rc.GetTop() );
	}
	SetRect( Rectf( m_pLeftImg->GetLeft(), m_pTopImg->GetTop(), m_pRightImg->GetRight(), m_pBottomImg->GetBottom() ) );
}

bool CGUIAutoSizeFrame::CreateSelf(CGUIElement* pParent, CMarkupSTL& xml)
{
    CGUIElement::CreateSelf(pParent, xml);

    m_pTopLeftImg = new CGUIImage;
    m_pTopImg = new CGUIImage;
    m_pTopRightImg = new CGUIImage;
    m_pRightImg = new CGUIImage;
    m_pBottomRightImg = new CGUIImage;
    m_pBottomImg = new CGUIImage;
    m_pBottomLeftImg = new CGUIImage;
    m_pLeftImg = new CGUIImage;
    m_pCenterImg = new CGUIImage;

    AIRString strValue = xml.GetAttrib("ImgSrcTopLeft").c_str();
    //m_pTopLeftImg->SetImageSrc(strValue);
    //m_pTopLeftImg->AutoSize();
    AddChild(m_pTopLeftImg);
    SetImgSrcTopLeft(strValue);

    m_strImgSrcTop = xml.GetAttrib("ImgSrcTop").c_str();
    AddChild(m_pTopImg);
    m_pTopImg->SetImageSrc(m_strImgSrcTop);
    m_pTopImg->AutoSize();

    AddChild(m_pTopRightImg);
    m_strImgSrcTopRight = xml.GetAttrib("ImgSrcTopRight").c_str();
    m_pTopRightImg->SetImageSrc(m_strImgSrcTopRight);
    m_pTopRightImg->AutoSize();

    AddChild(m_pRightImg);
    m_strImgSrcRight = xml.GetAttrib("ImgSrcRight").c_str();
    m_pRightImg->SetImageSrc(m_strImgSrcRight);
    m_pRightImg->AutoSize();

    AddChild(m_pBottomRightImg);
    m_strImgSrcBottomRight = xml.GetAttrib("ImgSrcBottomRight").c_str();
    m_pBottomRightImg->SetImageSrc(m_strImgSrcBottomRight);
    m_pBottomRightImg->AutoSize();

    AddChild(m_pBottomImg);
    m_strImgSrcBottom = xml.GetAttrib("ImgSrcBottom").c_str();
    m_pBottomImg->SetImageSrc(m_strImgSrcBottom);
    m_pBottomImg->AutoSize();

    AddChild(m_pBottomLeftImg);
    m_strImgSrcBottomLeft = xml.GetAttrib("ImgSrcBottomLeft").c_str();
    m_pBottomLeftImg->SetImageSrc(m_strImgSrcBottomLeft);
    m_pBottomLeftImg->AutoSize();

    AddChild(m_pLeftImg);
    m_strImgSrcLeft = xml.GetAttrib("ImgSrcLeft").c_str();
    m_pLeftImg->SetImageSrc(m_strImgSrcLeft);
    m_pLeftImg->AutoSize();

    AddChild(m_pCenterImg);
    m_strImgSrcCenter = xml.GetAttrib("ImgSrcCenter").c_str();
    m_pCenterImg->SetImageSrc(m_strImgSrcCenter);
    m_pCenterImg->AutoSize();

    return true;
}

bool CGUIAutoSizeFrame::SaveSelfToXML(CMarkupSTL& xml)
{
    CGUIElement::SaveSelfToXML(xml);
    xml.AddAttrib("ImgSrcTopLeft", m_strImgSrcTopLeft.c_str());
    xml.AddAttrib("ImgSrcTop", m_strImgSrcTop.c_str());
    xml.AddAttrib("ImgSrcTopRight", m_strImgSrcTopRight.c_str());
    xml.AddAttrib("ImgSrcRight", m_strImgSrcRight.c_str());
    xml.AddAttrib("ImgSrcBottomRight", m_strImgSrcBottomRight.c_str());
    xml.AddAttrib("ImgSrcBottom", m_strImgSrcBottom.c_str());
    xml.AddAttrib("ImgSrcBottomLeft", m_strImgSrcBottomLeft.c_str());
    xml.AddAttrib("ImgSrcLeft", m_strImgSrcLeft.c_str());
    xml.AddAttrib("ImgSrcCenter", m_strImgSrcCenter.c_str());

    return true;
}
