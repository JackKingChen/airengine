#include "PostRenderProcess.h"
//#include "PostProcessManager.h"
#include "NormalSceneRenderProcess.h"
#include "Engine.h"
#include "RenderTarget.h"
#include "ScreenCompositor.h"

PostRenderProcess::PostRenderProcess() : m_type(ePP_MAX)
	, m_pInputTexture(NULL)
	, m_pOutputTexture(NULL)
	, m_pSceneTexture(NULL)// : m_pSceneProcess(NULL)
{

}

PostRenderProcess::~PostRenderProcess()
{

}


void PostRenderProcess::Reset()
{
	//m_vtPostEffects.clear();
}

void PostRenderProcess::Clear()
{
	for (size_t i = 0; i < m_vtPostEffects.size(); ++i)
	{
		m_vtPostEffects[i]->Clear();
		SafeDelete(m_vtPostEffects[i]);
	}

	m_vtPostEffects.clear();

	for (size_t i = 0; i < m_vtRenderTargets.size(); ++i)
	{
		m_vtRenderTargets[i]->Clear();
		SafeDelete(m_vtRenderTargets[i]);
	}

	m_vtRenderTargets.clear();
}

void PostRenderProcess::Render()
{
	IRenderer* pRenderer = ENGINE_INST->GetRenderer();
	ITexture* pInputTexture = m_pInputTexture;
	for (size_t i = 0; i < m_vtPostEffects.size(); ++i)
	{
		m_vtPostEffects[i]->SetInputTexture(pInputTexture);
		m_vtPostEffects[i]->SetSceneTexture(m_pSceneTexture);
		TextureRenderTarget* pTargets[1];
		pTargets[0] = m_vtRenderTargets[i];
		pRenderer->SetRenderTargetsEx(1, pTargets, NULL);
		//pRenderer->ClearScene( 0x00000000 );
		pRenderer->BeginScene();
		m_vtPostEffects[i]->Draw();
		pRenderer->EndScene();

		pInputTexture = pTargets[0]->GetTexture();
	}
}

void PostRenderProcess::SetSceneTexture(ITexture* pSceneTexture)
{
	m_pSceneTexture = pSceneTexture;
}

PostGlowRenderProcess::PostGlowRenderProcess()
{
	m_type = ePP_Glow;
	ScreenCompositor* pCompositor = new ScreenCompositor;
	int nScreenWidth = 0;
	int nScreenHeight = 0;
	ENGINE_INST->GetBackBufferSize(nScreenWidth, nScreenHeight);
	pCompositor->Create(ePPT_COLOR_ColorBurn, 0, 0, nScreenWidth, nScreenHeight);
	m_vtPostEffects.push_back(pCompositor);
	TextureRenderTarget* pRTT = TextureRenderTarget::CreateRenderTarget("ePPT_COLOR_ColorBurn", nScreenWidth, nScreenHeight, RPF_A8R8G8B8);
	//pRTT->SetUseDepthSurface(true);
	m_vtRenderTargets.push_back(pRTT);
	
	pCompositor = new ScreenCompositor;
	pCompositor->Create(ePPT_COLOR_DownFilter4, 0, 0, nScreenWidth * 0.25f, nScreenHeight * 0.25f);
	m_vtPostEffects.push_back(pCompositor);
	pRTT = TextureRenderTarget::CreateRenderTarget("ePPT_COLOR_DownFilter4", nScreenWidth * 0.25f, nScreenHeight * 0.25f, RPF_A8R8G8B8);
	//pRTT->SetUseDepthSurface(true);
	m_vtRenderTargets.push_back(pRTT);

	pCompositor = new ScreenCompositor;
	pCompositor->Create(ePPT_COLOR_BlurH, 0, 0, nScreenWidth, nScreenHeight);
	m_vtPostEffects.push_back(pCompositor);
	pRTT = TextureRenderTarget::CreateRenderTarget("ePPT_COLOR_BlurH", nScreenWidth, nScreenHeight, RPF_A8R8G8B8);
	//pRTT->SetUseDepthSurface(true);
	m_vtRenderTargets.push_back(pRTT);

	pCompositor = new ScreenCompositor;
	pCompositor->Create(ePPT_COLOR_BlurV, 0, 0, nScreenWidth, nScreenHeight);
	m_vtPostEffects.push_back(pCompositor);
	pRTT = TextureRenderTarget::CreateRenderTarget("ePPT_COLOR_BlurV", nScreenWidth, nScreenHeight, RPF_A8R8G8B8);
	//pRTT->SetUseDepthSurface(true);
	m_vtRenderTargets.push_back(pRTT);

	pCompositor = new ScreenCompositor;
	pCompositor->Create(ePPT_COLOR_UpFilter4, 0, 0, nScreenWidth, nScreenHeight);
	m_vtPostEffects.push_back(pCompositor);
	pRTT = TextureRenderTarget::CreateRenderTarget("ePPT_COLOR_UpFilter4", nScreenWidth, nScreenHeight, RPF_A8R8G8B8);
	//pRTT->SetUseDepthSurface(true);
	m_vtRenderTargets.push_back(pRTT);

	pCompositor = new ScreenCompositor;
	pCompositor->Create(ePPT_COLOR_ScreenCombine, 0, 0, nScreenWidth, nScreenHeight);
	m_vtPostEffects.push_back(pCompositor);
	pRTT = TextureRenderTarget::CreateRenderTarget("ePPT_COLOR_ScreenCombine", nScreenWidth, nScreenHeight, RPF_A8R8G8B8);
	//pRTT->SetUseDepthSurface(true);
	m_vtRenderTargets.push_back(pRTT);
}

PostGlowRenderProcess::~PostGlowRenderProcess()
{

}

void PostGlowRenderProcess::Render()
{
	IRenderer* pRenderer = ENGINE_INST->GetRenderer();
	ITexture* pInputTexture = m_pInputTexture;
	for (size_t i = 0; i < m_vtPostEffects.size(); ++i)
	{
		m_vtPostEffects[i]->SetInputTexture(pInputTexture);
		m_vtPostEffects[i]->SetSceneTexture(m_pSceneTexture);
		RenderTarget* pTargets[1];
		pTargets[0] = m_vtRenderTargets[i];
		//pRenderer->SetRenderTargetsEx(1, pTargets, NULL);
		pRenderer->SetRenderTargets(1, pTargets);
		//pRenderer->ClearScene( 0x00000000 );
		pRenderer->BeginScene();
		m_vtPostEffects[i]->Draw();
		pRenderer->EndScene();

		pInputTexture = pTargets[0]->GetTexture();
		//AIRString strFile = AIRString("D:/") + pInputTexture->GetName() + ".png";
		//pInputTexture->SaveToFile(strFile.c_str());
	}

	m_pOutputTexture = pInputTexture;
}

PostSoftlightRenderProcess::PostSoftlightRenderProcess()
{
	m_type = ePP_SoftLight;
	ScreenCompositor* pCompositor = new ScreenCompositor;
	int nScreenWidth = 0;
	int nScreenHeight = 0;
	ENGINE_INST->GetBackBufferSize(nScreenWidth, nScreenHeight);
	pCompositor->Create(ePPT_COLOR_DownFilter4, 0, 0, nScreenWidth * 0.25f, nScreenHeight * 0.25f);
	m_vtPostEffects.push_back(pCompositor);
	TextureRenderTarget* pRTT = TextureRenderTarget::CreateRenderTarget("ePPT_COLOR_DownFilter4", nScreenWidth * 0.25f, nScreenHeight * 0.25f, RPF_A8R8G8B8);
	//pRTT->SetUseDepthSurface(true);
	m_vtRenderTargets.push_back(pRTT);

	pCompositor = new ScreenCompositor;
	pCompositor->Create(ePPT_COLOR_BlurH, 0, 0, nScreenWidth, nScreenHeight);
	m_vtPostEffects.push_back(pCompositor);
	pRTT = TextureRenderTarget::CreateRenderTarget("ePPT_COLOR_BlurH", nScreenWidth, nScreenHeight, RPF_A8R8G8B8);
	//pRTT->SetUseDepthSurface(true);
	m_vtRenderTargets.push_back(pRTT);

	pCompositor = new ScreenCompositor;
	pCompositor->Create(ePPT_COLOR_BlurV, 0, 0, nScreenWidth, nScreenHeight);
	m_vtPostEffects.push_back(pCompositor);
	pRTT = TextureRenderTarget::CreateRenderTarget("ePPT_COLOR_BlurV", nScreenWidth, nScreenHeight, RPF_A8R8G8B8);
	//pRTT->SetUseDepthSurface(true);
	m_vtRenderTargets.push_back(pRTT);

	pCompositor = new ScreenCompositor;
	pCompositor->Create(ePPT_COLOR_UpFilter4, 0, 0, nScreenWidth, nScreenHeight);
	m_vtPostEffects.push_back(pCompositor);
	pRTT = TextureRenderTarget::CreateRenderTarget("ePPT_COLOR_UpFilter4", nScreenWidth, nScreenHeight, RPF_A8R8G8B8);
	//pRTT->SetUseDepthSurface(true);
	m_vtRenderTargets.push_back(pRTT);

	pCompositor = new ScreenCompositor;
	pCompositor->Create(ePPT_COLOR_ScreenCombine, 0, 0, nScreenWidth, nScreenHeight);
	m_vtPostEffects.push_back(pCompositor);
	pRTT = TextureRenderTarget::CreateRenderTarget("ePPT_COLOR_ScreenCombine", nScreenWidth, nScreenHeight, RPF_A8R8G8B8);
	//pRTT->SetUseDepthSurface(true);
	m_vtRenderTargets.push_back(pRTT);

	pCompositor = new ScreenCompositor;
	pCompositor->Create(ePPT_COLOR_SoftLightCombine, 0, 0, nScreenWidth, nScreenHeight);
	m_vtPostEffects.push_back(pCompositor);
	pRTT = TextureRenderTarget::CreateRenderTarget("ePPT_COLOR_SoftLightCombine", nScreenWidth, nScreenHeight, RPF_A8R8G8B8);
	//pRTT->SetUseDepthSurface(true);
	m_vtRenderTargets.push_back(pRTT);
}

PostSoftlightRenderProcess::~PostSoftlightRenderProcess()
{

}

void PostSoftlightRenderProcess::Render()
{
	IRenderer* pRenderer = ENGINE_INST->GetRenderer();
	ITexture* pInputTexture = m_pInputTexture;
	for (size_t i = 0; i < m_vtPostEffects.size(); ++i)
	{
		m_vtPostEffects[i]->SetInputTexture(pInputTexture);
		m_vtPostEffects[i]->SetSceneTexture(m_pSceneTexture);
		RenderTarget* pTargets[1];
		pTargets[0] = m_vtRenderTargets[i];
		//pRenderer->SetRenderTargetsEx(1, pTargets, NULL);
		pRenderer->SetRenderTargets(1, pTargets);
		//pRenderer->ClearScene( 0x00000000 );
		pRenderer->BeginScene();
		m_vtPostEffects[i]->Draw();
		pRenderer->EndScene();

		pInputTexture = pTargets[0]->GetTexture();

		//AIRString strFile = AIRString("D:/") + pInputTexture->GetName() + ".png";
		//pInputTexture->SaveToFile(strFile.c_str());
	}

	m_pOutputTexture = pInputTexture;
}

void PostSoftlightRenderProcess::Init()
{

}
