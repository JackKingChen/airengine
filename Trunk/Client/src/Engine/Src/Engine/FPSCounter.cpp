#include "stdafx.h"

#include "FPSCounter.h"
#include "IRenderer.h"
#include "Engine.h"
//#include "SEFps.h"
#include "FontManager.h"
#include "VertexDeclarationManager.h"
#include "ISceneManager.h"
#include "MemoryAllocatorConfig.h"
#include "StringUtil.h"
#include "ITexture.h"

CFPSCounter::CFPSCounter() : m_pFont(NULL), m_dCurRenderTime(0.0)//, m_pVertexDecl(NULL)
{
    memset(m_szText, 0, sizeof(m_szText));
    memset(m_szSceneInfo, 0, sizeof(m_szSceneInfo));

	m_fMinFps = 9999.0f;
	m_fMaxFps = 0.0f;
	m_fAvgFps = 0.0f;
	m_fCurFps = 0.0f;
	m_bShow = true;
	m_fFrameTime = 0.0f;
}

CFPSCounter::~CFPSCounter()
{
    CleanUp();
}

void CFPSCounter::Init()
{
    m_dCurRenderTime = ENGINE_INST->GetCurTime();
    //FONTMGR->CreateDefaultFonts();
    FONTTYPE defaultFont;
	defaultFont.m_nFontSize = 12;
	defaultFont.m_strFaceName = "方正兰亭黑_GBK";
	defaultFont.m_strFileName = "方正兰亭黑.ttc";
	defaultFont.m_bBold = true;
	defaultFont.m_bAsPound = true;
	//defaultFont.m_strFaceName = "微软雅黑";
	//defaultFont.m_strFileName = "MSYH.TTF";
	
    //m_pFont = FONTMGR->CreateFontW(defaultFont, L"帧数：");
	m_pFont = FONTMGR->CreateFontW( defaultFont, CStringUtil::s2ws( "数距离主角1当前FPS:执Lag我1023456789+你他测试yghc没有，！" ).c_str() );
    //m_pVertexDecl = VERTEXDECL_MANAGER->CreateVertexDeclaration(E_VB_RHW);
	m_pFont->GetWordInfo(0);
}

void CFPSCounter::Render()
{
	if ( !m_bShow )
	{
		return;
	}
    ISceneManager* pScene = ENGINE_INST->GetSceneMgr();

    if (pScene && pScene->IsReady())
    {
        IRenderer* pRenderer = ENGINE_INST->GetRenderer();
        //pRenderer->SetVertexDeclaration(m_pVertexDecl);

		char buf[256];
		snprintf( buf, 256, "当前帧率: %f", m_fCurFps );
		m_pFont->Render(CStringUtil::s2ws( buf ).c_str(), AIR::Rectf(20, 20, 800, 40), CColorValue::Yellow);
        //m_pFont->Render(m_szText, AIR::Rectf(20, 20, 800, 40), CColorValue::Yellow);

		if (pScene)
		{
			const Vector3Df& vPos = pScene->GetCamera()->GetPosition();
			const Vector3Df& vLookat = pScene->GetCamera()->GetLookAt();
			snprintf( buf, 256, "场景渲染物个数[ %d ] 摄像机位置[ %.4f, %.4f, %.4f ] 摄像机距离[%.4f] 主角位置[ %.4f, %.4f, %.4f ]", 
				pScene->GetSceneNodesNum(), vPos.x, vPos.y, vPos.z, pScene->GetCamera()->GetDistance(),
				vLookat.x, vLookat.y, vLookat.z);
			m_pFont->Render(CStringUtil::s2ws( buf ).c_str(), AIR::Rectf(20, 50, 800, 70), CColorValue::White);

			static bool bSaved = false;
			if (!bSaved)
			{
				char szFilename[256] = { 0 };
				sprintf_s(szFilename, 255, "C:/%s.png", m_pFont->GetTexture()->GetName().c_str());
				m_pFont->SaveFontTextureToFile(szFilename);
				bSaved = true;
			}
			
		}
        size_t infoSize = m_vtInfo.size();
        for ( size_t i = 0; i < infoSize; ++i )
        {
            if ( !m_vtInfo[i]->m_szInfo.empty() )
            {
                m_pFont->Render( m_vtInfo[i]->m_szInfo.c_str(), m_vtInfo[i]->m_RC, m_vtInfo[i]->m_Color );
            }
        }
        m_vtInfo.clear();

		//ITexture* pFontTex = m_pFont->GetTexture();
		//AIR::Rectf rcFont( 20, 80, 20 + pFontTex->GetWidth(), 80 + pFontTex->GetHeight() );
		//AIR::Rectf rcUV( 0, 0, 1, 1 );
		//ENGINE_INST->GetGUIRenderer()->Render(rcFont, rcUV, 0.01f, pFontTex, CColorValue::White);
    }
}

void CFPSCounter::Update()
{
    if (ENGINE_INST->GetCurTime() - m_dCurRenderTime >= 1.0)
    {
        m_fCurFps = (float)ENGINE_INST->GetFPS();

        if ( m_fMinFps > m_fCurFps )
        {
            m_fMinFps = m_fCurFps;
        }
        if ( m_fMaxFps < m_fCurFps )
        {
            m_fMaxFps = m_fCurFps;
        }
        m_fAvgFps = (m_fMaxFps+m_fMinFps) * 0.5f;
        
        m_fFrameTime = (float)ENGINE_INST->GetTimeSinceLastFrame();

        //swprintf(m_szText, L"当前帧率: %.2f", m_fCurFps );
		char buf[256];
		snprintf( buf, 256, "当前帧率: %.2f", m_fCurFps );
		WAIRString wstr = CStringUtil::s2ws( buf );
		memset( m_szText, 0, sizeof(m_szText) );
		memcpy( m_szText, wstr.c_str(), wstr.size() * sizeof(wchar_t) );
        m_dCurRenderTime = ENGINE_INST->GetCurTime();
    }
    
}

void CFPSCounter::CleanUp()
{
    for ( int i = 0; i < m_vtInfo.size(); ++i )
    {
        SafeDelete( m_vtInfo[i] );
    }
    m_vtInfo.clear();
}


int CFPSCounter::AddTextInfo(const AIRString& buf, int x /* = 10 */, int y /* = 10 */, const SColor& c /* = SColor */)
{
    u32 nCount = buf.length() * 2 + 2;
    wchar_t* ws = AIR_NEW_ARRAY_T(wchar_t, nCount); //new wchar_t[];
    MByteToWChar( buf.c_str(), ws, nCount );
    TEXT_INFO* pTI = new TEXT_INFO( ws, Rectf(x, y, x + 800,y + 600), c );
    m_vtInfo.push_back( pTI );
    //SafeDeleteArray( ws );
    AIR_DELETE_ARRAY_T(ws, wchar_t, nCount);
	return m_vtInfo.size() - 1;
}


void CFPSCounter::ChangeTextInfo( int id, const AIRString& buf )
{    
	if ( m_vtInfo.size() > id && id >= 0 )
	{
		//wchar_t* ws = new wchar_t[buf.length() * 2 + 2];
		u32 nCount = buf.length() * 2 + 2;
		wchar_t* ws = AIR_NEW_ARRAY_T(wchar_t, nCount);
		MByteToWChar( buf.c_str(), ws, nCount );
		m_vtInfo[id]->m_szInfo = ws;
		//SafeDeleteArray( ws );
		AIR_DELETE_ARRAY_T(ws, wchar_t, nCount);
	}
}

void CFPSCounter::ChangeTextInfo( int id, const AIRString& buf, const Rectf& rc )
{
	if ( id >= 0 && id < m_vtInfo.size() )
	{
		//wchar_t* ws = new wchar_t[buf.length() * 2 + 2];
		u32 nCount = buf.length() * 2 + 2;
		wchar_t* ws = AIR_NEW_ARRAY_T(wchar_t, nCount);
		MByteToWChar( buf.c_str(), ws, nCount );
		m_vtInfo[id]->m_szInfo = ws;
		m_vtInfo[id]->m_RC = rc;
		//SafeDeleteArray( ws );
		AIR_DELETE_ARRAY_T(ws, wchar_t, nCount);
	}
}
