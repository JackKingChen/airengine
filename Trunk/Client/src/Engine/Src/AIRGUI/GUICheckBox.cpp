#include "GUICheckBox.h"
#include "Util.h"
#include "GUISystem.h"
//#include "ImageSetManager.h"
#include "StringUtil.h"
#include "ImageSet.h"

CGUICheckBox::CGUICheckBox() : m_bChecked(true)
{
	m_szImgSrcUnChecked.clear();
	m_szImgSrcChecked.clear();
	m_szImgSrcInactive.clear();
	m_strType = "CGUICheckBox";
	m_bMoveable = false;
}

CGUICheckBox::~CGUICheckBox()
{

}

void CGUICheckBox::SetCheck(bool bCheck)
{
    m_bChecked = bCheck;
    if (m_bChecked)
    {
        m_renderQuad.m_uv = m_UVChecked;
    }
    else
    {
        m_renderQuad.m_uv = m_UVUnChecked;
    }
}

bool    CGUICheckBox::IsChecked() 
{
    return m_bChecked; 
}

bool CGUICheckBox::CreateSelf(CGUIElement* pParent, CMarkupSTL& xml)
{
    if (!CGUIElement::CreateSelf(pParent, xml))
        return false;

    AIRString strImgSet;
    AIRString strImgUnit;
    AIRString strValue;
    CImageSet* pImageSet = NULL;
    CImageUnit* pIU = NULL;

    // 读入按钮弹起时的纹理
    strValue = xml.GetAttrib("ImgSrcUnChecked").c_str();
	m_szImgSrcUnChecked = strValue;
    CStringUtil::SplitFilename( strValue, strImgSet, strImgUnit );
    if( strImgSet.empty() && strImgUnit.empty() )
        strImgSet = strValue;

    pIU = GetScheme()->GetImageUnit(strImgSet, strImgUnit); //TextureMgr::GetInstancePtr()->GetImageUnit( strImgSet, strImgUnit );
    if( pIU ) 
    {
        m_UVUnChecked = pIU->m_fUV;
    }


    //读入按钮按下时的纹理
    strValue = xml.GetAttrib("ImgSrcChecked").c_str();
	m_szImgSrcChecked = strValue;
    CStringUtil::SplitFilename( strValue, strImgSet, strImgUnit );
    if( strImgSet.empty() && strImgUnit.empty() )
        strImgSet = strValue;

    pIU = GetScheme()->GetImageUnit(strImgSet, strImgUnit); //TextureMgr::GetInstancePtr()->GetImageUnit( strImgSet, strImgUnit );
    if( pIU ) 
    {
        m_UVChecked = pIU->m_fUV;
    }


    // 读入按钮不可用时的纹理
    strValue = xml.GetAttrib("ImgSrcInactive").c_str();
	m_szImgSrcInactive = strValue;
    CStringUtil::SplitFilename( strValue, strImgSet, strImgUnit );
    if( strImgSet.empty() && strImgUnit.empty() )
        strImgSet = strValue;

    pIU = GetScheme()->GetImageUnit(strImgSet, strImgUnit); //TextureMgr::GetInstancePtr()->GetImageUnit( strImgSet, strImgUnit );
    if( pIU ) 
    {
        m_UVInactive = pIU->m_fUV;
    }

    strValue = xml.GetAttrib("Checked").c_str();
    if (strValue == "TRUE")
    {
        m_bChecked = true;
    }
    else
    {
        m_bChecked = false;
    }

    if (m_bChecked)
    {
        m_renderQuad.m_uv = m_UVChecked;
    }
    else
        m_renderQuad.m_uv = m_UVUnChecked;

	return true;
}


bool CGUICheckBox::SaveSelfToXML(CMarkupSTL& xml )
{
	CGUIElement::SaveSelfToXML(xml);

	if ( !m_szImgSrcUnChecked.empty() )
	{
		xml.AddAttrib( "ImgSrcUnChecked", m_szImgSrcUnChecked.c_str() );
	}

	if ( m_szImgSrcChecked.size() > 0 )
	{
		xml.AddAttrib( "ImgSrcChecked", m_szImgSrcChecked.c_str() );
	}

	if ( m_szImgSrcInactive.size() > 0 )
	{
		xml.AddAttrib( "ImgSrcInactive", m_szImgSrcInactive.c_str() );
	}

	if ( m_bChecked )
	{
		xml.AddAttrib( "Checked", "TRUE" );
	}
	else
	{
		xml.AddAttrib( "Checked", "FALSE" );
	}

	return true;
}


void CGUICheckBox::RenderSelf()
{
	CGUIElement::RenderSelf();
}


bool CGUICheckBox::OnLButtonDownSelf(int x, int y)
{
	//CGUIElement::OnLButtonDownSelf(x,y);
    m_bChecked = !m_bChecked;

    if (m_bChecked)
    {
        m_renderQuad.m_uv = m_UVChecked;
    }
    else
        m_renderQuad.m_uv = m_UVUnChecked;

    GUISYSTEM->SetFocusElement(this);

    if (m_pScriptObject)
    {
        m_pScriptObject->RunOnLMouseDownScript();
    }

    if (m_pLButtonDownFun)
    {
        return m_pLButtonDownFun(this);
    }

    return true;
}


void CGUICheckBox::SetUnCheckedImageSrc( const AIRString& strUnCheckedImgSrc )
{
	if (strUnCheckedImgSrc == m_szImgSrcUnChecked)
	{
		return;
	}

	AIRString strImgSet;
	AIRString strImgUnit;
	CStringUtil::SplitFilename(strUnCheckedImgSrc, strImgSet, strImgUnit);

	CImageSet* pImgSet = m_pScheme->GetImageSet(strImgSet);
	CImageUnit* pUnit = m_pScheme->GetImageUnit(strImgSet, strImgUnit);
	if (!pUnit)
	{
		return;
	}
	m_UVUnChecked = pUnit->m_fUV;

	m_szImgSrcUnChecked = strUnCheckedImgSrc;
	if (m_renderQuad.m_pTexture && m_renderQuad.m_pTexture != pImgSet->GetTexture())
	{
		GUISYSTEM->ClearTexture(m_renderQuad.m_pTexture);
	}
	m_renderQuad.m_pTexture = pImgSet->GetTexture();
    
    if (m_bChecked)
    {
        m_renderQuad.m_uv = m_UVChecked;
    }
    else
        m_renderQuad.m_uv = m_UVUnChecked;
}


void CGUICheckBox::SetCheckedImageSrc(const AIRString& strImageSrc)
{
    if (strImageSrc == m_szImgSrcChecked)
    {
        return;
    }

    AIRString strImgSet;
    AIRString strImgUnit;
    CStringUtil::SplitFilename(strImageSrc, strImgSet, strImgUnit);

    CImageSet* pImgSet = m_pScheme->GetImageSet(strImgSet);
    CImageUnit* pUnit = m_pScheme->GetImageUnit(strImgSet, strImgUnit);
    if (!pUnit)
    {
        return;
    }
    m_UVChecked = pUnit->m_fUV;

    m_szImgSrcChecked = strImageSrc;
    if (m_renderQuad.m_pTexture && m_renderQuad.m_pTexture != pImgSet->GetTexture())
    {
        GUISYSTEM->ClearTexture(m_renderQuad.m_pTexture);
    }
    m_renderQuad.m_pTexture = pImgSet->GetTexture();

    if (m_bChecked)
    {
        m_renderQuad.m_uv = m_UVChecked;
    }
    else
        m_renderQuad.m_uv = m_UVUnChecked;
}

void CGUICheckBox::SetInactiveImageSrc(const AIRString& strInactiveImageSrc)
{
    if (strInactiveImageSrc == m_szImgSrcInactive)
    {
        return;
    }

    AIRString strImgSet;
    AIRString strImgUnit;
    CStringUtil::SplitFilename(strInactiveImageSrc, strImgSet, strImgUnit);

    CImageSet* pImgSet = m_pScheme->GetImageSet(strImgSet);
    CImageUnit* pUnit = m_pScheme->GetImageUnit(strImgSet, strImgUnit);
    if (!pUnit)
    {
        return;
    }
    m_UVInactive = pUnit->m_fUV;

    m_szImgSrcInactive = strInactiveImageSrc;
    if (m_renderQuad.m_pTexture && m_renderQuad.m_pTexture != pImgSet->GetTexture())
    {
        GUISYSTEM->ClearTexture(m_renderQuad.m_pTexture);
    }
    m_renderQuad.m_pTexture = pImgSet->GetTexture();
}

void CGUICheckBox::AutoSize()
{
    if (!m_pScheme)
    {
        return;
    }
    AIRString strImgSet, strImgUnit;
    CStringUtil::SplitFilename(m_szImgSrcChecked, strImgSet, strImgUnit);
    CImageUnit* pUnit = m_pScheme->GetImageUnit(strImgSet, strImgUnit);
    if (pUnit)
    {
        SetWidth(pUnit->m_nWidth);
        SetHeight(pUnit->m_nHeight);
    }
    else
    {
        CStringUtil::SplitFilename(m_szImgSrcUnChecked, strImgSet, strImgUnit);
        CImageUnit* pUnit = m_pScheme->GetImageUnit(strImgSet, strImgUnit);
        if (pUnit)
        {
            SetWidth(pUnit->m_nWidth);
            SetHeight(pUnit->m_nHeight);
        }
    }
}
