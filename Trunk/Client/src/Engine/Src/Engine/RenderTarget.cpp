#include "stdafx.h"
#include "RenderTarget.h"
#include "Engine.h"
#include "ISceneManager.h"
#include "Camera.h"
#include "TextureMgr.h"

IMPLEMENT_ROOT_RTTI(RenderTarget);
IMPLEMENT_RTTI(TextureRenderTarget, RenderTarget);

RenderTarget::RenderTarget() : 
m_nIndex(0), 
m_clrBackGround(0xff, 0, 0, 0),
m_funcRender(NULL),
m_bUseDepthSurface(false)
{

}

RenderTarget::~RenderTarget()
{

}

bool RenderTarget::Initialize(int nWidth, int nHeight)
{
    return true;
}


void RenderTarget::Clear()
{

}


void RenderTarget::Resize(u32 nWidth, u32 nHeight)
{
	Clear();
	Initialize(nWidth, nHeight);
}

TextureRenderTarget::TextureRenderTarget() : m_clrBackGround(0), m_bFullScreen(false), m_fScreenRatio(1.0f), m_nIndex(0)
, m_pTexture(NULL)
{

}

TextureRenderTarget::~TextureRenderTarget()
{
	Clear();
}

bool TextureRenderTarget::Create(const char* szName, u32 nWidth, u32 nHeight, RendererPixelFormat pixelFormat)
{
	if (m_pTexture)
	{
		m_pTexture->Clear();
		SafeDelete(m_pTexture);
	}

	IRenderer* pRenderer = RENDERER;
	m_pTexture = pRenderer->CreateTextureRenderTarget(szName, nWidth, nHeight, pixelFormat);

	return m_pTexture != NULL;
}

void TextureRenderTarget::Clear()
{
	if (m_pTexture)
	{
		RENDERER->DestroyTextureRenderTarget(m_pTexture);
		m_pTexture = NULL;
	}
}

bool TextureRenderTarget::SaveToFile(const char* szFile)
{
	if (m_pTexture)
	{
		return m_pTexture->SaveToFile(szFile);
	}

	return false;
}

TextureRenderTarget* TextureRenderTarget::CreateRenderTarget(const char* szName, u32 nWidth, u32 nHeight, RendererPixelFormat pixelFormat)
{
	TextureRenderTarget* pTarget = new TextureRenderTarget;
	if (!pTarget->Create(szName, nWidth, nHeight, pixelFormat))
	{
		SafeDelete(pTarget);
	}

	return pTarget;
}
