//#include "stdafx.h"
#include "GUIButton.h"
//#include "ImageSetManager.h"
//#include "TextureManager.h"
//#include "RenderSystem.h"
//#include "InputSystem.h"
//#include "UILuaScriptMgr.h"
#include "Util.h"
#include "StringUtil.h"
#include "GUISystem.h"
#include "DebugTools.h"
#include "ImageSet.h"

CGUIButton::CGUIButton()
{
    //m_nCurBtnState = UI_NORMAL;
    //m_ctrlType = eCtrl_Button;
    m_strType = "CGUIButton";

	m_szImgSrcUp.clear();
	m_szImgSrcDown.clear();
	m_szImgSrcHover.clear();
	m_szImgSrcInactive.clear();

	m_ColorUp.Set(0xffffffff);              // ��ť����ʱ����ɫ
	m_ColorDown.Set(0xffffffff);            // ��ť����ʱ����ɫ
	m_ColorHover.Set(0xffffffff);           // ��������ڰ�ť��ʱ����ɫ
	m_ColorInactive.Set(0xffffffff);        // ��ť������ʱ����ɫ

	m_bMoveable = false;
}

CGUIButton::~CGUIButton()
{

}

CGUIElement* CGUIButton::Clone()
{
    return NULL;
}

void CGUIButton::CleanUp()
{
    CGUIElement::CleanUp();
}

bool CGUIButton::CreateSelf(CGUIElement* pParent, CMarkupSTL& xml)
{
    if (!CGUIElement::CreateSelf(pParent, xml))
        return false;

    
    const char* szAttributeValue = NULL;
    AIRString strImgSet;
    AIRString strImgUnit;
    AIRString strValue;
    CImageSet* pImageSet = NULL;
    CImageUnit* pIU = NULL;

    strValue = xml.GetAttrib("ColorUp").c_str();
	if ( strValue.length() > 0 )
	{
		m_ColorUp = ATOL(strValue.c_str(), 16);
	}

    strValue = xml.GetAttrib("ColorDown").c_str();
	if ( strValue.length() > 0 )
	{
		m_ColorDown = ATOL(strValue.c_str(), 16);
	}

    // ��������ڰ�ť����ʱ����ɫ
    strValue = xml.GetAttrib("ColorHover").c_str();
	if ( strValue.length() > 0 )
	{
		m_ColorHover = ATOL(strValue.c_str(), 16);
	}

    // ��ť������ʱ����ɫ
    strValue = xml.GetAttrib("ColorInactive").c_str();
	if ( strValue.length() > 0 )
	{
		m_ColorInactive = ATOL(strValue.c_str(), 16);
	}

    // ���밴ť����ʱ������
    strValue = xml.GetAttrib("ImgSrcUp").c_str();
	m_szImgSrcUp = strValue;
    CStringUtil::SplitFilename( strValue, strImgSet, strImgUnit );
    if( strImgSet.size() == 0 && strImgUnit.size() == 0 )
        strImgSet = strValue;

    CImageSet* pImgSet = GetScheme()->GetImageSetByName(strImgSet.c_str());
    if ( !pImgSet && strImgSet.length() > 0 )
    {
        DT_TO_MSGBOX_FMT( LT_WARN, GetString("�Ҳ���UIͼ��: [set:%s]",strImgSet.c_str()) );
    }
    if (m_renderQuad.m_pTexture == NULL  && pImgSet)
    {
        m_renderQuad.m_pTexture = pImgSet->GetTexture();
    }

    pIU = pImgSet->GetImgUnit(strImgUnit); //TextureMgr::GetInstancePtr()->GetImageUnit( strImgSet, strImgUnit );
    if( pIU ) 
    {
        m_UVUp = pIU->m_fUV;
    }
    else if ( strImgUnit.length() > 0 )
    {
        DT_TO_MSGBOX_FMT( LT_WARN, GetString("�Ҳ���UIͼ��: [set:%s], [unit:%s]",strImgSet.c_str(),strImgUnit.c_str()) );
    }


    //���밴ť����ʱ������
    strValue = xml.GetAttrib("ImgSrcDown").c_str();
	m_szImgSrcDown = strValue;
    CStringUtil::SplitFilename( strValue, strImgSet, strImgUnit );
    if( strImgSet.size() == 0 && strImgUnit.size() == 0 )
        strImgSet = strValue;

    pIU = GetScheme()->GetImageUnit(strImgSet, strImgUnit); //TextureMgr::GetInstancePtr()->GetImageUnit( strImgSet, strImgUnit );
    if( pIU ) 
    {
        m_UVDown = pIU->m_fUV;
        
    }
    else
        m_UVDown = m_UVUp;

    // ������������ڰ�ť����ʱ������
    strValue = xml.GetAttrib("ImgSrcHover").c_str();
	m_szImgSrcHover = strValue;
    CStringUtil::SplitFilename( strValue, strImgSet, strImgUnit );
    if( strImgSet.size() == 0 && strImgUnit.size() == 0 )
        strImgSet = strValue;

    pIU = GetScheme()->GetImageUnit(strImgSet, strImgUnit); //TextureMgr::GetInstancePtr()->GetImageUnit( strImgSet, strImgUnit );
    if( pIU ) 
    {
        m_UVHover = pIU->m_fUV;
    }
    else
    {
        m_UVHover = m_UVUp;
    }


    // ���밴ť������ʱ������
    strValue = xml.GetAttrib("ImgSrcInactive").c_str();
	m_szImgSrcInactive = strValue;
    CStringUtil::SplitFilename( strValue, strImgSet, strImgUnit );
    if( strImgSet.size() == 0 && strImgUnit.size() == 0 )
        strImgSet = strValue;

    pIU = GetScheme()->GetImageUnit(strImgSet, strImgUnit); //TextureMgr::GetInstancePtr()->GetImageUnit( strImgSet, strImgUnit );
    if( pIU ) 
    {
        m_UVInactive = pIU->m_fUV;
    }
    else
        m_UVInactive = m_UVUp;

    m_renderQuad.m_uv = m_UVUp;
    
    return true;
}



bool			CGUIButton::SaveSelfToXML(CMarkupSTL& xml )
{
	CGUIElement::SaveSelfToXML(xml);

	xml.AddAttrib( "ColorUp", m_ColorUp.GetAsHexString().c_str() );
	xml.AddAttrib( "ColorDown", m_ColorDown.GetAsHexString().c_str() );
	xml.AddAttrib( "ColorHover", m_ColorHover.GetAsHexString().c_str() );
	xml.AddAttrib( "ColorInactive", m_ColorInactive.GetAsHexString().c_str() );

	if ( !m_szImgSrcUp.empty() )
	{
		xml.AddAttrib( "ImgSrcUp", m_szImgSrcUp.c_str() );
	}

	if ( !m_szImgSrcDown.empty() )
	{
		xml.AddAttrib( "ImgSrcDown", m_szImgSrcDown.c_str() );
	}
	
	if ( !m_szImgSrcHover.empty() )
	{
		xml.AddAttrib( "ImgSrcHover", m_szImgSrcHover.c_str() );
	}
	
	if ( !m_szImgSrcInactive.empty() )
	{
		xml.AddAttrib( "ImgSrcInactive", m_szImgSrcInactive.c_str() );
	}

	return true;
}

void CGUIButton::RenderSelf()
{
    /*
    LPDIRECT3DDEVICE9 pDevice = RENDERSYSTEM->GetDevice();
    
    m_renderQuad.m_pTexture->Use(0);
    
    pDevice->SetFVF(RHWVERTEX::FVF);
    

    pDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, (void*)m_vtVertices, sizeof(RHWVERTEX));
    */

    //AIRString name = m_strName;
    CGUIElement::RenderSelf();
    
}


void CGUIButton::OnStatusChange()
{
	switch ( m_nCurUIState )
	{
	case UI_NORMAL:
		{
			m_renderQuad.m_uv = m_UVUp;
		}
		break;
	case UI_HOVER:
		{
			m_renderQuad.m_uv = m_UVHover;
		}
		break;
	case UI_LBUTTONDOWN:
		{
			m_renderQuad.m_uv = m_UVDown;
		}
		break;
	case UI_GRAY:
		{
			m_renderQuad.m_uv = m_UVInactive;
		}
		break;
	}
	//// ��ǰ��״̬
	//switch(m_nCurUIState)
	//{
	//	// ��ǰ��һ��״̬
	//case UI_NORMAL:
	//	// �����һ�ε�״̬����갴��
	//	if (m_nLastUIState == UI_LBUTTONDOWN)
	//	{
	//		// ������굯��Ľű�
	//		if (m_pScriptObject)
	//		{
	//			m_pScriptObject->RunOnLMouseUpScript();
	//		}
	//		m_renderQuad.m_uv = m_UVHover;
	//	}
	//	// �����һ�ε�״̬���������
	//	else if (m_nLastUIState == UI_HOVER)
	//	{
	//		// ��������뿪�ؼ��Ľű�
	//		if (m_pScriptObject)
	//		{
	//			m_pScriptObject->RunOnMouseLeaveScript();
	//		}
	//		m_renderQuad.m_uv = m_UVUp;
	//	}
	//	break;
	//	// ��ǰ�ǰ���������
	//case UI_LBUTTONDOWN:
	//	// �����һ�ε�״̬���ǰ���������
	//	if (m_nLastUIState != UI_LBUTTONDOWN)
	//	{
	//		if (m_pScriptObject)
	//		{
	//			m_pScriptObject->RunOnLMouseDownScript();
	//		}
	//		m_renderQuad.m_uv = m_UVDown;
	//	}
	//	break;
	//	// ��ǰ���������״̬
	//case UI_HOVER:
	//	// �����һ�ε�״̬�����������
	//	if (m_nLastUIState != UI_HOVER)
	//	{
	//		if (m_pScriptObject)
	//		{
	//			m_pScriptObject->RunOnMouseHoverScript();
	//		}
	//		m_renderQuad.m_uv = m_UVHover;
	//	}
	//	break;
	//	// ��ǰ�����״̬�ǲ�����״̬
	//case UI_GRAY:
	//	if (m_nLastUIState != UI_GRAY)
	//	{
	//		m_renderQuad.m_uv = m_UVInactive;
	//	}
	//	break;
	//}    
}


void CGUIButton::RegisterToScript()
{
    //UILUASCRIPTMGR->Declare(this->m_strName.c_str(), this);
}


void CGUIButton::SetImgSrcUp( const AIRString& strImgSrcUp )
{
	if (strImgSrcUp == m_szImgSrcUp)
	{
		return;
	}

	AIRString strImgSet;
	AIRString strImgUnit;
	CStringUtil::SplitFilename(strImgSrcUp, strImgSet, strImgUnit);

	CImageSet* pImgSet = m_pScheme->GetImageSet(strImgSet);
	CImageUnit* pUnit = m_pScheme->GetImageUnit(strImgSet, strImgUnit);
	if (!pUnit)
	{
		return;
	}
	m_UVUp = pUnit->m_fUV;

    if (m_szImgSrcDown.length() == 0)
    {
        m_UVDown = m_UVUp;
    }

    if (m_szImgSrcHover.length() == 0)
    {
        m_UVHover = m_UVUp;
    }

    if (m_szImgSrcInactive.length() == 0)
    {
        m_UVInactive = m_UVUp;
    }

	m_szImgSrcUp = strImgSrcUp;
	if (m_renderQuad.m_pTexture && m_renderQuad.m_pTexture != pImgSet->GetTexture())
	{
		GUISYSTEM->ClearTexture(m_renderQuad.m_pTexture);
	}
	m_renderQuad.m_pTexture = pImgSet->GetTexture();

    m_renderQuad.m_uv = m_UVUp;
    m_nCurUIState = UI_NORMAL;
    //OnStatusChange();
}


void CGUIButton::SetImgSrcDown(const AIRString& strImgSrcDown)
{
    if (strImgSrcDown == m_szImgSrcDown)
    {
        return;
    }

    AIRString strImgSet;
    AIRString strImgUnit;
    CStringUtil::SplitFilename(strImgSrcDown, strImgSet, strImgUnit);

    CImageSet* pImgSet = m_pScheme->GetImageSet(strImgSet);
    CImageUnit* pUnit = m_pScheme->GetImageUnit(strImgSet, strImgUnit);
    if (!pUnit)
    {
        return;
    }
    m_UVDown = pUnit->m_fUV;

    m_szImgSrcDown = strImgSrcDown;
    if (m_renderQuad.m_pTexture && m_renderQuad.m_pTexture != pImgSet->GetTexture())
    {
        GUISYSTEM->ClearTexture(m_renderQuad.m_pTexture);
    }
    m_renderQuad.m_pTexture = pImgSet->GetTexture();

    OnStatusChange();
}

void CGUIButton::SetImgSrcHover(const AIRString& strImgSrcHover)
{
    if (strImgSrcHover == m_szImgSrcHover)
    {
        return;
    }

    AIRString strImgSet;
    AIRString strImgUnit;
    CStringUtil::SplitFilename(strImgSrcHover, strImgSet, strImgUnit);

    CImageSet* pImgSet = m_pScheme->GetImageSet(strImgSet);
    CImageUnit* pUnit = m_pScheme->GetImageUnit(strImgSet, strImgUnit);
    if (!pUnit)
    {
        return;
    }
    m_UVHover = pUnit->m_fUV;

    m_szImgSrcHover = strImgSrcHover;
    if (m_renderQuad.m_pTexture && m_renderQuad.m_pTexture != pImgSet->GetTexture())
    {
        GUISYSTEM->ClearTexture(m_renderQuad.m_pTexture);
    }
    m_renderQuad.m_pTexture = pImgSet->GetTexture();

    OnStatusChange();
}

void CGUIButton::SetImgSrcGray(const AIRString& strImgSrcGray)
{
    if (strImgSrcGray == m_szImgSrcInactive)
    {
        return;
    }

    AIRString strImgSet;
    AIRString strImgUnit;
    CStringUtil::SplitFilename(strImgSrcGray, strImgSet, strImgUnit);

    CImageSet* pImgSet = m_pScheme->GetImageSet(strImgSet);
    CImageUnit* pUnit = m_pScheme->GetImageUnit(strImgSet, strImgUnit);
    if (!pUnit)
    {
        return;
    }
    m_UVInactive = pUnit->m_fUV;

    m_szImgSrcInactive = strImgSrcGray;
    if (m_renderQuad.m_pTexture && m_renderQuad.m_pTexture != pImgSet->GetTexture())
    {
        GUISYSTEM->ClearTexture(m_renderQuad.m_pTexture);
    }
    m_renderQuad.m_pTexture = pImgSet->GetTexture();

    OnStatusChange();
}

void CGUIButton::SetColorDown(const SColor color)
{
    m_ColorDown = color;
}

void CGUIButton::SetColorHover(const SColor color)
{
    m_ColorHover = color;
}

void CGUIButton::SetColorGray(const SColor color)
{
    m_ColorInactive = color;
}

void CGUIButton::AutoSize()
{
    AIRString strImgSet, strImgUnit;
    CStringUtil::SplitFilename(m_szImgSrcUp, strImgSet, strImgUnit);
    CImageUnit* pUnit = m_pScheme->GetImageUnit(strImgSet, strImgUnit);
    if (pUnit)
    {
        SetWidth(pUnit->m_nWidth);
        SetHeight(pUnit->m_nHeight);
    }
}

/*
void CGUIButton::OnLButtonDownSelf(int x, int y)
{
    m_nCurBtnState = UI_LBUTTONDOWN;
}

void CGUIButton::OnLButtonUpSelf(int x, int y)
{
    m_nCurBtnState = UI_NORMAL;
}

void CGUIButton::OnMouseMoveSelf(int x, int y)
{
    //if (!INPUTSYSTEM->LButtonDown(x, y))
    //{
    //    
    //}
    if (m_renderQuad.m_rectScreen.IsInRect(x, y))
    {
        m_nCurBtnState = UI_HOVER;
    }
    else 
        m_nCurBtnState = UI_NORMAL;

}
*/
