#include "GUIAnimation.h"
#include "GUISystem.h"
//#include "SEFps.h"
#include "Engine.h"
#include "StringUtil.h"
#include "GUIBuilderMgr.h"

CGUIAnimation::CGUIAnimation()
{
    m_bLoop = true;
    m_nFrameRate = 0;
    m_fFrameChangeTime = 1.0f;
    m_fLastRenderTime = 0.0f;
    m_nCurFrame = 0;
    //m_ctrlType = eCtrl_Animation;
    m_strType = "CGUIAnimation";
    m_vtFrames.clear();
	m_bMoveable = false;
}

CGUIAnimation::~CGUIAnimation()
{

}

void CGUIAnimation::Update()
{
    if (m_vtFrames.size() == 0)
    {
        return;
    }
    if ( ENGINE_INST->GetCurTime() - m_fLastRenderTime >= m_fFrameChangeTime)
    {
        m_nCurFrame++;
        if (m_nCurFrame == m_vtFrames.size())
        {
            m_nCurFrame = 0;
        }
        CImageUnit* pUnit = m_vtFrames[m_nCurFrame];
        if ( pUnit )
        {
            m_renderQuad.m_uv = pUnit->m_fUV;
        }
        m_fLastRenderTime = ENGINE_INST->GetCurTime();
    }
}

void CGUIAnimation::RenderSelf()
{
    CGUIElement::RenderSelf();
}

bool CGUIAnimation::CreateSelf(CGUIElement* pParent, CMarkupSTL& xml)
{
    CGUIElement::CreateSelf(pParent, xml);

	AIRString strValue;
	AIRString strImgSet;
	AIRString strImgUnit;

    strValue = xml.GetAttrib("FrameElapseTime").c_str();
    m_fFrameChangeTime = (float)atof(strValue.c_str());

    strValue = xml.GetAttrib("Loop").c_str();
    if (strValue == "TRUE")
    {
        m_bLoop = true;
    }

    strValue = xml.GetAttrib("FramesNum").c_str();
    int nFramesCount = atoi(strValue.c_str());
    m_vtFrames.reserve(nFramesCount);

    m_nFrameRate = 1.0f / m_fFrameChangeTime;
    
    return true;
}


bool CGUIAnimation::CreateChild(CMarkupSTL& parent)
{
	AIRString strValue;
    AIRString strImgSet;
    AIRString strImgUnit;

	while (parent.FindChildElem())
	{
		parent.IntoElem();
		strValue = parent.GetTagName().c_str();
		// 读取帧信息节点
		if (strValue == "Frame")
		{
			strValue = parent.GetAttrib("ImgSrc").c_str();
			CStringUtil::SplitFilename( strValue, strImgSet, strImgUnit );
			CImageUnit* pUnit = GetScheme()->GetImageUnit( strImgSet, strImgUnit );
			if (pUnit)
			{
				m_renderQuad.m_pTexture = pUnit->m_pSet->GetTexture();
				m_vtFrames.push_back(pUnit);
			}
		}
		// 读取脚本信息节点
		else if (strValue == "Script")
		{
			if( !m_pScriptObject )
			{
				m_pScriptObject = new CGUIScriptObject();	
			}
			if( !m_pScriptObject->Load(parent) )
			{
				SafeDelete(m_pScriptObject);
				return false;
			}
			if (m_pScriptObject)
			{
				m_byKeyUp = m_pScriptObject->GetKeyUp();
				m_byKeyDown = m_pScriptObject->GetKeyDown();
			}
		}
		// 读取其他子控件信息节点
		else
		{
			CGUIBuilder* pBuilder = CGUIBuilderMgr::GetInstance()->GetBuilder(strValue.c_str());
			if (!pBuilder)
			{
				continue;
			}
			CGUIElement* pUIElement = pBuilder->BuildElement(this, parent, m_pScheme);
			if (pUIElement)
			{
				AddChild(pUIElement);
			}
		}
		parent.OutOfElem();
	}

	if (m_vtFrames.size() > 0)
	{
		CImageUnit* pUnit = m_vtFrames[0];
		if ( pUnit )
		{
			m_renderQuad.m_uv = pUnit->m_fUV;
		}
	}

	return true;
}


bool CGUIAnimation::SaveSelfToXML(CMarkupSTL& xml )
{
	CGUIElement::SaveSelfToXML(xml);
	xml.AddAttrib( "FrameElapseTime", GetString("%.2f",m_fFrameChangeTime));

	if ( m_bLoop )
	{
		xml.AddAttrib( "Loop", "TRUE" );
	}
	else
	{
		xml.AddAttrib( "Loop", "FALSE" );
	}

	xml.AddAttrib( "FramesNum", GetString("%d",m_vtFrames.size()) );

	xml.IntoElem();
	for ( size_t i = 0; i < m_vtFrames.size(); ++i)
	{
		xml.AddElem("Frame");
		AIRString szSRC = m_vtFrames[i]->m_szImgSetName;
		szSRC += "\\";
		szSRC += m_vtFrames[i]->m_strName;
		xml.AddAttrib( "ImgSrc", szSRC.c_str() );
	}
	xml.OutOfElem();
	return true;
}

void CGUIAnimation::SetLoop(bool bLoop)
{
    m_bLoop = bLoop;
}

void CGUIAnimation::SetFrameRate(int nFrameRate)
{
    m_nFrameRate = nFrameRate;
    if (m_nFrameRate != 0)
    {
        m_fFrameChangeTime = 1.0f / m_nFrameRate;
    }
}


// for editor 
void CGUIAnimation::SetFrames( AIRList<AIRString>& frameInfo )
{
	m_vtFrames.clear();
	AIRString imgset(""), imgunit("");
	AIRList<AIRString>::iterator it = frameInfo.begin();
	for ( ; it != frameInfo.end(); ++it )
	{
		CStringUtil::SplitFilename((*it).c_str(), imgset, imgunit);
        CImageSet* pSet = GUISYSTEM->GetCurrentScheme()->GetImageSet(imgset);
        if ( pSet )
        {
            m_renderQuad.m_pTexture = pSet->GetTexture();
            CImageUnit* pUnit = pSet->GetImgUnit(imgunit);
            m_vtFrames.push_back( pUnit );
        }
	}
}

void CGUIAnimation::GetFrames( AIRList<AIRString>& frameInfo )
{
	frameInfo.clear();
	CImageUnit* pUnit = NULL;
	for ( size_t i = 0; i < m_vtFrames.size(); ++i )
	{
		pUnit = m_vtFrames[i];
		frameInfo.push_back( pUnit->m_szImgSetName + AIRString( "\\" ) + pUnit->m_strName );
	}
}

/*
CGUIAnimation& CGUIAnimation::operator = (const CGUIAnimation& other)
{
    m_bLoop = other.m_bLoop;
    m_fFrameChangeTime = other.m_fFrameChangeTime;
    m_fLastRenderTime = other.m_fLastRenderTime;
    m_nFrameRate = other.m_nFrameRate;
    m_nCurFrame = other.m_nCurFrame;
    m_renderQuad.m_pTexture = other.m_renderQuad.m_pTexture;
    m_vtFrames = other.m_vtFrames;

    return *this;
}
*/

void CGUIAnimation::CopyFrom(const CGUIElement* pOther)
{
    CGUIElement::CopyFrom(pOther);

    const CGUIAnimation* pAniOther = static_cast<const CGUIAnimation*>(pOther);

    m_bLoop = pAniOther->m_bLoop;
    m_fFrameChangeTime = pAniOther->m_fFrameChangeTime;
    m_fLastRenderTime = pAniOther->m_fLastRenderTime;
    m_nFrameRate = pAniOther->m_nFrameRate;
    m_nCurFrame = pAniOther->m_nCurFrame;
    m_renderQuad.m_pTexture = pAniOther->m_renderQuad.m_pTexture;
    m_vtFrames = pAniOther->m_vtFrames;
}
