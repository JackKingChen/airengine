#include "stdafx.h"
#include "PostProcessManager.h"
#include "Engine.h"
#include "NormalSceneRenderProcess.h"
//#include "Technique.h"
#include "IMaterial.h"

PostEffect::PostEffect()
{

}

PostEffect::~PostEffect()
{

}

void PostEffect::Clear()
{
    for (CompositortList_Iter it = m_lstCompositors.begin(); it != m_lstCompositors.end(); ++it)
    {
        (*it)->Clear();
        SafeDelete(*it);
    }

    m_lstCompositors.clear();
}

void PostEffect::SetActive(bool bActive)
{
    m_bActive = bActive;
}

void PostEffect::Create(int nPosX, int nPosY, int nWidth, int nHeight, PostEffectType type)
{
    m_postEffectType = type;
	PostScreenCompositor* pPostScreen = NULL;
    switch (m_postEffectType)
    {
	case ePET_Monochrome:
		{
			pPostScreen = CreatePostScreenCompositor(nPosX, nPosY, nWidth, nHeight, "monochrome", ePPT_COLOR_Monochrome);
			m_lstCompositors.push_back(pPostScreen);
		}
		break;

	case ePET_Bloom:
		{
			pPostScreen = CreatePostScreenCompositor(nPosX, nPosY, nWidth, nHeight, "toneMap", ePPT_COLOR_ToneMap);
			m_lstCompositors.push_back(pPostScreen);

			pPostScreen = CreatePostScreenCompositor(nPosX, nPosY, nWidth, nHeight, "brightPass", ePPT_COLOR_BrightPass);
			m_lstCompositors.push_back(pPostScreen);

            pPostScreen = CreatePostScreenCompositor(nPosX, nPosY, nWidth, nHeight, "downFilter4", ePPT_COLOR_DownFilter4);
            m_lstCompositors.push_back(pPostScreen);
			
			pPostScreen = CreatePostScreenCompositor(nPosX, nPosY, nWidth, nHeight, "bloomBlurH", ePPT_COLOR_BloomBlurH);
			m_lstCompositors.push_back(pPostScreen);

			pPostScreen = CreatePostScreenCompositor(nPosX, nPosY, nWidth, nHeight, "bloomBlurV", ePPT_COLOR_BloomBlurV);
			m_lstCompositors.push_back(pPostScreen);

            pPostScreen = CreatePostScreenCompositor(nPosX, nPosY, nWidth, nHeight, "upFilter4", ePPT_COLOR_UpFilter4);
            m_lstCompositors.push_back(pPostScreen);

			pPostScreen = CreatePostScreenCompositor(nPosX, nPosY, nWidth, nHeight, "NormalCombine", ePPT_COLOR_NormalCombine);
			m_lstCompositors.push_back(pPostScreen);
		}
		break;

	case ePET_Distortion:
		{
			pPostScreen = CreatePostScreenCompositor(nPosX, nPosY, nWidth, nHeight, "Distortion", ePPT_Distortion);
			m_lstCompositors.push_back(pPostScreen);
		}
		break;

	case ePET_Blur:
		{
			pPostScreen = CreatePostScreenCompositor(nPosX, nPosY, nWidth, nHeight, "downFilter4", ePPT_COLOR_DownFilter4);
			m_lstCompositors.push_back(pPostScreen);

			pPostScreen = CreatePostScreenCompositor(nPosX, nPosY, nWidth, nHeight, "blurH", ePPT_COLOR_BlurH);
			m_lstCompositors.push_back(pPostScreen);

			pPostScreen = CreatePostScreenCompositor(nPosX, nPosY, nWidth, nHeight, "blurV", ePPT_COLOR_BlurV);
			m_lstCompositors.push_back(pPostScreen);

			pPostScreen = CreatePostScreenCompositor(nPosX, nPosY, nWidth, nHeight, "upFilter4", ePPT_COLOR_UpFilter4);
			m_lstCompositors.push_back(pPostScreen);
		}
		break;

    case ePET_SoftLight:
        {
			pPostScreen = CreatePostScreenCompositor(nPosX, nPosY, nWidth, nHeight, "downFilter4", ePPT_COLOR_DownFilter4);
			m_lstCompositors.push_back(pPostScreen);

            pPostScreen = CreatePostScreenCompositor(nPosX, nPosY, nWidth, nHeight, "blurH", ePPT_COLOR_BlurH);
            m_lstCompositors.push_back(pPostScreen);

            pPostScreen = CreatePostScreenCompositor(nPosX, nPosY, nWidth, nHeight, "blurV", ePPT_COLOR_BlurV);
            m_lstCompositors.push_back(pPostScreen); 
            
			pPostScreen = CreatePostScreenCompositor(nPosX, nPosY, nWidth, nHeight, "upFilter4", ePPT_COLOR_UpFilter4);
			m_lstCompositors.push_back(pPostScreen);

            pPostScreen = CreatePostScreenCompositor(nPosX, nPosY, nWidth, nHeight, "screenCombine", ePPT_COLOR_ScreenCombine);
            m_lstCompositors.push_back(pPostScreen);

            pPostScreen = CreatePostScreenCompositor(nPosX, nPosY, nWidth, nHeight, "softLightCombine", ePPT_COLOR_SoftLightCombine);
            m_lstCompositors.push_back(pPostScreen);
        }
        break;

    case ePET_Glow:
        {          
            pPostScreen = CreatePostScreenCompositor(nPosX, nPosY, nWidth, nHeight, "colorBurnCombine", ePPT_COLOR_ColorBurn);
            m_lstCompositors.push_back(pPostScreen);

			pPostScreen = CreatePostScreenCompositor(nPosX, nPosY, nWidth, nHeight, "downFilter4", ePPT_COLOR_DownFilter4);
			m_lstCompositors.push_back(pPostScreen);

            pPostScreen = CreatePostScreenCompositor(nPosX, nPosY, nWidth, nHeight, "blurH", ePPT_COLOR_BlurH);
            m_lstCompositors.push_back(pPostScreen);

            pPostScreen = CreatePostScreenCompositor(nPosX, nPosY, nWidth, nHeight, "blurV", ePPT_COLOR_BlurV);
            m_lstCompositors.push_back(pPostScreen); 

			pPostScreen = CreatePostScreenCompositor(nPosX, nPosY, nWidth, nHeight, "upFilter4", ePPT_COLOR_UpFilter4);
			m_lstCompositors.push_back(pPostScreen);

            pPostScreen = CreatePostScreenCompositor(nPosX, nPosY, nWidth, nHeight, "screenCombine", ePPT_COLOR_ScreenCombine);
            m_lstCompositors.push_back(pPostScreen);

        }
        break;

    }
}

void PostEffect::OnResetDevice()
{
    int nWidth, nHeight;
    ENGINE_INST->GetBackBufferSize(nWidth, nHeight);
    for (CompositortList_Iter it = m_lstCompositors.begin(); it != m_lstCompositors.end(); ++it)
    {
        (*it)->SetWindowPosition(0, 0, nWidth, nHeight);
    }
}

PostScreenCompositor* PostEffect::CreatePostScreenCompositor(int nPosX, int nPosY, int nWidth, int nHeight, const char* szName, ScreenCompositorType pscType)
{
    PostScreenCompositor* pPostScreen = new PostScreenCompositor;
	pPostScreen->SetCompositorType(pscType);
    //pPostScreen->SetName(szName);
    pPostScreen->Create(nPosX, nPosY, nWidth, nHeight);
    pPostScreen->SetPostEffect(this);
   
	return pPostScreen;
}

PostProcess::PostProcess() : m_pOutputTarget(NULL), m_fScale(1.0f), m_fUVScale(1.0f)
{

}

PostProcess::~PostProcess()
{

}

void PostProcess::Process(ITexture* pInputTex)
{

}

ITexture* PostProcess::GetOutputTexture()
{
	if (m_pOutputTarget)
	{
		return m_pOutputTarget->GetTexture();
	}
	return NULL;
}

void PostProcess::BuildScreenSizeData()
{
	LPRHWVERTEX pVertex = &m_renderQuad[0];
	int nWidth, nHeight;
	ENGINE_INST->GetBackBufferSize(nWidth, nHeight);
	float fPixelOffset = RENDERER->GetPixelOffset();
	float fWidth = nWidth * m_fScale;
	float fHeight = nHeight * m_fScale;
	pVertex[0].color = SColor::White;
	pVertex[0].rhw = 1.0f;
	pVertex[0].tu = 0.0f;
	pVertex[0].tv = 0.0f;
	pVertex[0].x =  -fPixelOffset;
	pVertex[0].y =  -fPixelOffset;
	pVertex[0].z = 1.0f;

	pVertex[1].color = SColor::White;
	pVertex[1].rhw = 1.0f;
	pVertex[1].tu = m_fUVScale;
	pVertex[1].tv = 0.0f;
	pVertex[1].x = fWidth - fPixelOffset;
	pVertex[1].y =  -fPixelOffset;
	pVertex[1].z = 1.0f;

	pVertex[2].color = SColor::White;
	pVertex[2].rhw = 1.0f;
	pVertex[2].tu = 0.0f;
	pVertex[2].tv = m_fUVScale;
	pVertex[2].x =  -fPixelOffset;
	pVertex[2].y = fHeight - fPixelOffset;
	pVertex[2].z = 1.0f;

	pVertex[3].color = SColor::White;
	pVertex[3].rhw = 1.0f;
	pVertex[3].tu = m_fUVScale;
	pVertex[3].tv = m_fUVScale;
	pVertex[3].x = fWidth - fPixelOffset;
	pVertex[3].y = fHeight - fPixelOffset;
	pVertex[3].z = 1.0f;
}

PostProcessManager::PostProcessManager() : m_pSceneRenderProcess(NULL)
, m_pSceneRenderTarget(NULL)
//, m_pPostManualGPUDataSource(NULL)
, m_pCompositor(NULL)
, m_pSamplerState(NULL)
{
    memset(m_PostEffects, 0, sizeof(m_PostEffects));
}

PostProcessManager::~PostProcessManager()
{

}

PostEffect* PostProcessManager::CreatePostEffect(PostEffectType type, int nWidth, int nHeight)
{
    //if (m_pPostManualGPUDataSource == NULL)
    //{
    //    m_pPostManualGPUDataSource = static_cast<PostCompositorManualGPUDataSource*>(CREATE_MANUAL_GPU_PARAM_SOURCE(PostCompositorManualGPUDataSource));
    //}

    PostEffect* pPostEffect = GetPostEffect(type);
    if (pPostEffect == NULL)
    {
        pPostEffect = new PostEffect;
        int nWidth, nHeight;
        ENGINE_INST->GetBackBufferSize(nWidth, nHeight);
        pPostEffect->Create(0, 0, nWidth, nHeight, type);
        pPostEffect->SetActive(true);
        m_PostEffects[type] = pPostEffect;
        
    }

    return pPostEffect;
}

void PostProcessManager::Clear()
{
    if (m_pSceneRenderTarget)
    {
		SafeDelete(m_pSceneRenderTarget);
        //RENDERER->DestroyTextureRenderTarget(m_pSceneRenderTarget);
    }

	if (m_pSceneRenderProcess)
	{
		//ENGINE_INST->RemoveCustomRenderProcess(m_pSceneRenderProcess);
		m_pSceneRenderProcess->Clear();
		SafeDelete(m_pSceneRenderProcess);
	}
    for (int i = 0; i < ePET_MAX; ++i)
    {
        if (m_PostEffects[i])
        {
            m_PostEffects[i]->Clear();
            SafeDelete(m_PostEffects[i]);
        }
    }
    m_lstActiveCompositors.clear();

    if (m_pCompositor)
    {
        m_pCompositor->Clear();
        SafeDelete(m_pCompositor);
    }
}

void PostProcessManager::PostProcess()
{
    m_lstActiveCompositors.clear();

    for (int i = 0; i < ePET_MAX; ++i)
    {
        if (m_PostEffects[i] && m_PostEffects[i]->IsActive())
        {
            //连接两个compositor的队列
            //m_lstActiveCompositors.push_back(m_PostScreenCompositors[i]);
            m_lstActiveCompositors.insert(m_lstActiveCompositors.end(), m_PostEffects[i]->GetCompositorList().begin(),
                m_PostEffects[i]->GetCompositorList().end());
        }
    }

    if (m_lstActiveCompositors.size() == 0)
    {
        return;
    }

    if (m_pSceneRenderTarget == NULL)
    {
		s32 nWidth = 0; 
		s32 nHeight = 0;
		RENDERER->GetBackBufferSize(nWidth, nHeight);
        m_pSceneRenderTarget = TextureRenderTarget::CreateRenderTarget("SceneRenderTarget", nWidth, nHeight, RPF_A8R8G8B8); //RENDERER->CreateTextureRenderTarget("SceneRenderTarget", nWidth, nHeight, RPF_A8R8G8B8);
    }

	if (m_pSamplerState == 0)
	{
		m_pSamplerState = RENDERER->CreateSamplerStateObject(0);
		m_pSamplerState->SetTextureAddressU(TAV_Clamp);
		m_pSamplerState->SetTextureAddressV(TAV_Clamp);
		m_pSamplerState->SetTextureAddressW(TAV_Clamp);
		m_pSamplerState->SetTextureFilterOp(TFT_Linear);
	}

	RENDERER->SetTexture(0, 0);
	RENDERER->SetTexture(1, 0);
    //m_pSceneRenderTarget->Render();
    m_fScreenScale = 1.0f;
	m_fUVScale = 1.0f;

	Matrix4f matTemp;
	Matrix4f matProj;
	matTemp.Identity();
	matTemp.SetScale(Vector3Df(1.0f, -1.0f, 1.0f));
	int nScreenWidth = 0;
	int nScreenHeight = 0;
	ENGINE_INST->GetBackBufferSize(nScreenWidth, nScreenHeight);
	matProj.BuildProjectionMatrixOrthoOffCenterLH(0, nScreenWidth, 0, nScreenHeight, 0.1f, 1000.0f);
	//matProj.BuildProjectionMatrixOrthoLH(dimensionScreen.Width, dimensionScreen.Height, 0.0f, 1000.0f);
	matProj = matProj * matTemp;
	RENDERER->SetProjectionTransform(&matProj);

    CompositortList_Iter itEnd = m_lstActiveCompositors.end();
    int i = 0;
    PostScreenCompositor* pPrePost = NULL;
	m_pSamplerState->Apply(RENDERER, NULL);
    for (CompositortList_Iter it = m_lstActiveCompositors.begin(); it != itEnd; ++it, ++i)
    {
        if ( (*it)->GetCompositorType() == ePPT_COLOR_DownFilter4 )
        {
            m_fScreenScale *= 0.25;
        }
        else if ( (*it)->GetCompositorType() == ePPT_COLOR_UpFilter4 )
        {
            m_fScreenScale *= 4;
        }
        
        if (i == 0)
        {
           // m_pSceneRenderTarget->GetRenderTexture()->SaveToFile("D:/screen.png");
            (*it)->Process(m_pSceneRenderTarget, m_fScreenScale, m_fUVScale);
            
        }
        else
        {
            (*it)->Process(pPrePost->GetRenderTarget(), m_fScreenScale, m_fUVScale);
        }
        pPrePost = *it;
		m_fUVScale = m_fScreenScale;
    }
}

void PostProcessManager::Render()
{
    if (m_lstActiveCompositors.size() == 0)    
    {
        return;
    }

	IRenderer* pRenderer = ENGINE_INST->GetRenderer();

	if (m_pSamplerState == 0)
	{
		m_pSamplerState = pRenderer->CreateSamplerStateObject(0);
		m_pSamplerState->SetTextureAddressU(TAV_Clamp);
		m_pSamplerState->SetTextureAddressV(TAV_Clamp);
		m_pSamplerState->SetTextureAddressW(TAV_Clamp);
		m_pSamplerState->SetTextureFilterOp(TFT_Linear);
	}

    pRenderer->BeginScene();
    if (m_pCompositor == NULL)
    {
        m_pCompositor = new ScreenCompositor;
        int nWidth, nHeight;
        ENGINE_INST->GetBackBufferSize(nWidth, nHeight);
        m_pCompositor->Create(ePPT_None, 0, 0, nWidth, nHeight);
        
    }

    CompositortList::reverse_iterator it = m_lstActiveCompositors.rbegin();
    PostScreenCompositor* pLastCompositor = *it;
    IMaterial* pMaterial = m_pCompositor->GetMaterial();
    if (pMaterial)
    {
        if (pMaterial->IsReady())
        {
            //CPass* pPass = pMaterial->GetCurrentTechnique()->GetPass(0);
            //pPass->SetTextureUnitTex(0, pLastCompositor->GetRenderTarget()->GetRenderTexture());
			m_pSamplerState->Apply(pRenderer, pLastCompositor->GetRenderTarget()->GetTexture());

			pRenderer->SetTexture(0, pLastCompositor->GetRenderTarget()->GetTexture());
            //if (m_pCompositor->Prepare())
            //{
            //    //pLastCompositor->GetRenderTarget()->GetRenderTexture()->SaveToFile("D:/screencompositor.png");
            //    m_pCompositor->Render(NULL);
            //}
            
            //pPass->SetTextureUnitTex(0, NULL);
			pRenderer->SetTexture(0, 0);
        }
        else
        {
            pMaterial->Cache();
        }
    }
    ENGINE_INST->GetRenderer()->EndScene();
    //CompositortList::reverse_iterator rit = m_lstActiveCompositors.rbegin();
    //(*rit)->Render(NULL);
}

void PostProcessManager::EnablePostEffect(bool bActive, PostEffectType type)
{
    if (m_PostEffects[type])
    {
        m_PostEffects[type]->SetActive(bActive);
    }
    else if (bActive)
    {
        //m_PostScreenCompositors[type] = new PostScreenCompositor;
        int nWidth, nHeight;
        ENGINE_INST->GetBackBufferSize(nWidth, nHeight);
        CreatePostEffect(type, nWidth, nHeight);
    }
}

bool PostProcessManager::IsPostEffectActive(PostEffectType type)
{
    if (m_PostEffects[type])
    {
        return m_PostEffects[type]->IsActive();
    }
    return false;
}

void PostProcessManager::SaveToFile(const char* szFilename)
{
    if (m_pSceneRenderTarget)
    {
        m_pSceneRenderTarget->SaveToFile(szFilename);
    }

    CompositortList::reverse_iterator it = m_lstActiveCompositors.rbegin();
    PostScreenCompositor* pLastCompositor = *it;
    if (pLastCompositor)
    {
        pLastCompositor->GetRenderTarget()->SaveToFile("D:/last_compositor.png");
    }
}

void PostProcessManager::OnResetDevice()
{
    for (int i = 0; i < ePET_MAX; ++i)
    {
        if (m_PostEffects[i])
        {
            m_PostEffects[i]->OnResetDevice();
        }
    }

    if (m_pCompositor)
    {
        int nWidth, nHeight;
        ENGINE_INST->GetBackBufferSize(nWidth, nHeight);
        m_pCompositor->SetWindowPosition(0, 0, nWidth, nHeight);
    }
}


size_t PostProcessManager::GetActiveCompositorsNum() const
{
	size_t nActivePostEffectCount = 0;
	for (int i = 0; i < ePET_MAX; ++i)
	{
		if ( m_PostEffects[i] && m_PostEffects[i]->IsActive() )
		{
			++nActivePostEffectCount;
		}
	}
	return nActivePostEffectCount;
}