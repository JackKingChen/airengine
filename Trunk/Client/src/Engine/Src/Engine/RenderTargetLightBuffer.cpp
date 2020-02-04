#include "stdafx.h"
#include "RenderTargetLightBuffer.h"
#include "Engine.h"
#include "MaterialManager.h"
#include "LightingManager.h"
#include "GBufferRenderProcess.h"
#include "ISceneManager.h"
#include "VertexDeclarationManager.h"
#include "FullScreenQuad.h"
//#include "Technique.h"

RenderTargetLightBuffer::RenderTargetLightBuffer() : m_pMaterial(NULL)
//, m_pPointLightTechnique(NULL)
//, m_pDirectionalLightTechnique(NULL)
, m_pFullScreenQuad(NULL)
//, m_billBoard("RenderTargetLightBuffer")
{
    m_strName = "RenderTargetLightBuffer";
    m_quad.SetFullWindow(true);
    m_clrBackGround = 0;
    m_billBoard.SetBBSize(2.0f, 2.0f);
}

RenderTargetLightBuffer::~RenderTargetLightBuffer()
{

}

void RenderTargetLightBuffer::Clear()
{
    RenderTarget::Clear();
    m_billBoard.Clear();
    if (m_pFullScreenQuad)
    {
        m_pFullScreenQuad->Clear();
        SafeDelete(m_pFullScreenQuad);
    }
    if (m_pMaterial)
    {
        MATERIAL_MANAGER->ClearMaterial(m_pMaterial);
    }
}

bool RenderTargetLightBuffer::Initialize(int nWidth, int nHeight)
{
    RenderTarget::Initialize(nWidth, nHeight);
    //for test 主要测试一下位置是否正确
    //if (m_pRenderTexture == NULL)
    //{
    //    m_pRenderTexture = RENDERER->CreateFloatTexture(m_strName.c_str(), nWidth, nHeight, true, 1, true);
    //}
    //------
    m_pRenderTexture->SetResetOperation(true, 1.0f);

    m_pMaterial = MATERIAL_MANAGER->CreateMaterial("TemplateMaterial.xml/LightPrePass");
    if (m_pMaterial && m_pMaterial->IsReady())
    {
        //m_pPointLightTechnique = m_pMaterial->GetTechnique("LPPPointLightRenderToLightBuffer");
        //m_pDirectionalLightTechnique = m_pMaterial->GetTechnique("LightPrePassDirectionalLightRenderToLightBuffer");
        //m_pTechniqueClearGBuffer = m_pMaterial->GetTechnique("ClearGBuffer");
    }

    m_quad.Create(0, 0, nWidth, nHeight);

    m_pFullScreenQuad = new FullScreenQuad;

    RENDERER->SetTextureFilter(10, TFT_None);

    return true;
}

void RenderTargetLightBuffer::RenderImpl()
{
    //Update();
	RENDERER->SetTexture(10, 0);

	RENDERER->SetRenderTargets(1, 0);

    RENDERER->SetTexture(10, m_pRenderTexture);
}

void RenderTargetLightBuffer::RenderToLightBuffer(CLight* pLight)
{
    IRenderer* pRenderer = ENGINE_INST->GetRenderer();

    pRenderer->GetGPUDataParam();
}
