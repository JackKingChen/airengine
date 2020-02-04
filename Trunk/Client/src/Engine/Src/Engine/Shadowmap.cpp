//#include "stdafx.h"
#include "Shadowmap.h"
#include "Engine.h"
#include "ISceneManager.h"
#include "ThirdPersonCamera.h"
//#include "ShadowManager.h"
#include "MaterialManager.h"
//#include "RenderControl.h"
//#include "SpeedTreeInstManager.h"
#include "TextureMgr.h"
//#include "Technique.h"
#include "Terrain.h"
//#include "SpeedTreeEngine.h"
#include "MathUtil.h"


Matrix4f g_matTest;   //测试矩阵

Shadowmap::Shadowmap() : m_bInitialize(false)//, m_pGPUShadowmapSource(NULL)
, m_fDisLightToTarget(0)
, m_pTarget(NULL)
, m_bPerspective(false)
, m_bDebug(false)
, m_bHardwareShadowmap(true)
//, m_pDepthTexture(NULL)
//, m_pMaterial(NULL)
, m_pJitterTexture(NULL)
, m_fShadowDepthBias(0)
, m_fShadowScoleScaleBias(0)
, m_nShadowmapSize(1024)
//, m_pVirtualCamera(NULL)
, m_type(SMT_TSM)
, m_fMinInfinityZ(2.5f)
, m_fSlideBack(0.0f)
, m_fPPNear(0.0f)
, m_fPPFar(0.0f)
, m_fAspect(0)
, m_zNear(0)
, m_zFar(0)
, m_bShadowTestInverted(false)
, m_fSoftNess(1.0f)
, m_fJitterScale(1.0f)
, m_fShadowRangeRadius(800.0f)
, m_fTSM_Delta(0.7f)
, m_nSplitNum(2)
, m_csmMethod(CSM_DX9)
, m_bUnitClip(true)
, m_pShadowSamplerState(NULL)
, m_pJitterSamplerState(NULL)
, m_pRasterizerStateObject(NULL)
, m_pShadowmap(NULL)
, m_pDepthStencil(NULL)
, m_bEnable(false)
//, m_pCamera(NULL)
{
    m_strName = "Shadowmap";
    m_RenderFlow = FLOW_SHADOWMAP;
}

Shadowmap::~Shadowmap()
{
	SafeDelete(m_pCamera);
}

bool Shadowmap::Initialize(int nWidth, int nHeight)
{
    if (m_type == SMT_CSM && m_csmMethod == CSM_DX9)
    {
        nWidth = 2048;
        nHeight = 512;
    }
    IRenderer* pRenderer = ENGINE_INST->GetRenderer();
    //if (m_pRenderTexture)
    //{
    //    TEXTURE_MANAGER->ClearTexture(m_pRenderTexture);
    //}
    if (m_pShadowmap)
    {
        //TEXTURE_MANAGER->ClearTexture(m_pShadowmap);
    }

    if (m_pJitterTexture)
    {
        TEXTURE_MANAGER->ClearTexture(m_pJitterTexture);
    }

    if (m_bHardwareShadowmap)
    {
        m_bHardwareShadowmap = pRenderer->CheckCurrentDeviceFormat(eRSU_DepthStencil, eRSRT_Texture, RPF_D24S8);
    }

    if (m_bHardwareShadowmap)
    {
        
        //m_pRenderTexture = TEXTURE_MANAGER->CreateTexture(m_strName.c_str(), TextureParam::GetRenderTarget(nWidth, nHeight, RPF_A8R8G8B8)/*nWidth, nHeight, 1, true*/);
        AIRString strName = m_strName + "_depthTexture";
        TextureParam param(nWidth, nHeight, 0, 1, eRSU_DepthStencil,
            RPF_D24S8, TT_TEXTURE, true);
        //m_pDepthTexture = TEXTURE_MANAGER->CreateTexture(strName.c_str(), param);
        //pRenderer->CreateDepthTexture(m_strName + "_depthTexture", nWidth, nHeight, 1);

		m_pShadowmap = TextureRenderTarget::CreateRenderTarget("shadowmap", nWidth, nHeight, RPF_A8R8G8B8); //pRenderer->CreateTextureRenderTarget("shadowmap", nWidth, nHeight, RPF_A8R8G8B8);
		m_pShadowmap->SetFullScreen(false);
		m_pDepthStencil = pRenderer->CreateDepthStencil(nWidth, nHeight, RPF_D24S8, true);
    }
    else
    {
		/*
        if (m_bDebug)
        {
            m_pRenderTexture = TEXTURE_MANAGER->CreateTexture(m_strName.c_str(), TextureParam::GetRenderTarget(nWidth, nHeight, RPF_A8R8G8B8));
        }
        else
        {
            m_pRenderTexture = TEXTURE_MANAGER->CreateTexture(m_strName.c_str(), TextureParam::GetRenderTargetFloat32(nWidth, nHeight));
        }
		*/
    }
    

    if (m_pCamera)
    {
        SafeDelete(m_pCamera);
    }

    m_pCamera = new CCamera(Vector3Df::UNIT_Y * 200, Vector3Df::ZERO);
    m_pCamera->SetViewport(100.0f, 10000.0f, AIR::PI / 4, nWidth, nHeight, false);
    //m_bDeleteCamera = true;

    m_bInitialize = true;

    m_matR = Matrix4f::ZERO;
	f32 fScreenOffset = RENDERER->GetPixelOffset();
    m_matR[0] = 0.5; m_matR[12] = 0.5 + fScreenOffset / nWidth;
    m_matR[5] = -0.5; m_matR[13] = 0.5 + fScreenOffset / nHeight;
    m_matR[10] = 1; m_matR[14] = 0;
    m_matR[15] = 1;

    //m_pMaterial = MATERIAL_MANAGER->CreateMaterial("TemplateMaterial.xml/RenderToShadow");

    //if (m_pGPUShadowmapSource == NULL)
    //{
    //    m_pGPUShadowmapSource = static_cast<ShadowmapManualGPUDataParamSource*>(CREATE_MANUAL_GPU_PARAM_SOURCE(ShadowmapManualGPUDataParamSource));
    //}

	m_pShadowSamplerState = RENDERER->CreateSamplerStateObject(8);
	m_pJitterSamplerState = RENDERER->CreateSamplerStateObject(9);

	m_pShadowSamplerState->SetTextureAddressU(TAV_Border);
	m_pShadowSamplerState->SetTextureAddressV(TAV_Border);
	m_pShadowSamplerState->SetTextureAddressW(TAV_Border);
	m_pShadowSamplerState->SetTextureBorderColor(CColorValue());
	m_pShadowSamplerState->SetTextureFilterOp(TFT_Comparison_Point);
	m_pShadowSamplerState->SetCompareFunc(eCF_LESS);

	m_pJitterSamplerState->SetTextureAddressU(TAV_Warp);
	m_pJitterSamplerState->SetTextureAddressV(TAV_Warp);
	m_pJitterSamplerState->SetTextureAddressW(TAV_Warp);
	m_pJitterSamplerState->SetTextureFilterOp(TFT_Point);

	m_pRasterizerStateObject = RENDERER->CreateRasterizerStateObject();

    //RENDERER->SetTextureFilter(8, TFT_Point);
    //RENDERER->SetTextureFilter(9, TFT_Point);

    CreateSoftShadowJitterTexture();

    return true;
}

//void Shadowmap::UpdateCamera()
//{
//
//}

void Shadowmap::UpdateCamera()
{
    ISceneManager* pScene = ENGINE_INST->GetSceneMgr();
    //获得当前场景的view矩阵
    CCamera* pCameraScene = pScene->GetCamera();
    if (m_type == SMT_ORTHO)
    {
        if (m_pTarget)
        {
            m_pCamera->SetLookAt(m_pTarget->GetDerivedPosition());
        }

        m_pCamera->SetPosition(m_pCamera->GetLookAt() - m_directionLight * m_fDisLightToTarget);
		m_pCamera->SetUserFrustumEnable(false);

        //m_pCamera->SetPosition(m_positionLight);
        m_pCamera->SetViewport(100.0f, m_fDisLightToTarget * 2, AIR::PI / 4, m_pDepthStencil->GetWidth(), m_pDepthStencil->GetHeight(), m_bPerspective);
        m_pCamera->Update(); 

        CHeightMap* pTerrain = TERRAIN->GetCurHeightmap();
        if (pTerrain)
        {
            pTerrain->SetVisable(false);
        }

        //pScene->GetVisableRenderables(m_pCamera, m_renderQueue);

        if (pTerrain)
        {
            pTerrain->SetVisable(true);
        }

        

        m_matLightViewProj = m_pCamera->GetViewMatrix() * m_pCamera->GetProjectionMatrix() * m_matR;
        //m_pGPUShadowmapSource->SetLightViewProjMatrix(m_matLightViewProj);
        IRenderer* pRenderer = ENGINE_INST->GetRenderer();
        //CGPUDataParamSource* pGPUDataParam = pRenderer->GetGPUDataParam();
        //pGPUDataParam->SetLightViewProjMatrix(m_matLightViewProj);
        //更新speedtree
        //SPEEDTREE_ENGINE->UpdateCamera(m_pCamera);
    }
    else
    {
        //m_renderQueue.Clear();    //这里画的是不在场景可视范围内，但又会被渲染到shadowmap的对象
        pScene->Cull(m_pCamera, m_lstVisableNodes, m_RenderFlow);
        
        switch (m_type)
        {
        case SMT_PSM:
            BuildPSMMatrices();
            break;
        case SMT_TSM:
            BuildTSMMatrices();
            //BuildOrthoShadowProjectionMatrix(*(pCameraScene->GetFrustum()), m_matLightViewProj, m_renderQueue);
            break;
        case SMT_CSM:
            UpdateCSMData();
            break;
        }

        //m_pGPUShadowmapSource->SetLightViewProjMatrix(m_matLightViewProj * m_matR);
        IRenderer* pRenderer = ENGINE_INST->GetRenderer();
        CGPUDataParamSource* pGPUDataParam = pRenderer->GetGPUDataParam();
        pGPUDataParam->SetLightViewProjMatrix(m_matLightViewProj * m_matR);

        //更新speedtree
        //SPEEDTREE_ENGINE->UpdateCamera(pCameraScene->GetPosition(), m_bShadowTestInverted ? -pScene->GetSunLight()->GetDirection() : pScene->GetSunLight()->GetDirection());
    }

    //ENGINE_INST->TriggerEvent(Event_RenderTargetUpdate, this);

    ENGINE_INST->GetFPSCounter()->AddTextInfo(GetString("Near=%f, Far=%f, ppNear=%f, ppFar=%f, slideBack=%f", 
        m_zNear, m_zFar, m_fPPNear, m_fPPFar, m_fSlideBack), 10, 90);
    
}

void Shadowmap::SetLightPosition(const Vector3Df& position)
{
    m_positionLight = position;
    //if (m_pCamera)
    //{
    //    m_pCamera->SetPosition(m_positionLight);
    //}
}

void Shadowmap::SetLightDirection(const Vector3Df& direction)
{
    m_directionLight = direction;
    m_directionLight.Normalize();
    //if (m_pCamera)
    //{
    //    m_pCamera->SetLookAt(m_positionLight + m_directionLight * m_pCamera->GetViewFar());
    //}
}

void Shadowmap::Render()
{
	//UpdateCamera();   //这个版本用最普通的shadowmap
	IRenderer* pRenderer = RENDERER;
	pRenderer->SetTexture(8, NULL);

    ISceneManager* pScene = ENGINE_INST->GetSceneMgr();

    //首先取得当前pScene的frustum，然后转成世界坐标的AABB
    AABBBox3Df frustumInViewAABB;
    GetAABBFromProjectMatrix(frustumInViewAABB, pScene->GetCamera()->GetProjectionMatrix());
    
    AABBBox3Df shadowArea = m_casterAABB;
    m_casterAABB.Intersection(shadowArea, frustumInViewAABB);

    Vector3Df centerPos = shadowArea.GetCenter();
    Matrix4f matInvView;
    
    pScene->GetCamera()->GetViewMatrix().GetInverse(matInvView);
    Vector3Df vecLookAt;
    matInvView.TransformCoord(centerPos, vecLookAt);     //这个时候centerPos是world空间的坐标
    m_pCamera->SetLookAt(vecLookAt);
    m_pCamera->SetPosition(vecLookAt - m_directionLight * m_fDisLightToTarget);
    m_pCamera->BuildViewMatrix();

    const Matrix4f& cameraLightView = m_pCamera->GetViewMatrix();

    Matrix4f matCameraToLight = matInvView * cameraLightView;
    matCameraToLight.TransformBox(shadowArea);
    f32 fWidth = shadowArea.MaxEdge.x - shadowArea.MinEdge.x;
    f32 fheight = shadowArea.MaxEdge.y - shadowArea.MinEdge.y;
    f32 fDis = shadowArea.MaxEdge.z - shadowArea.MinEdge.z;

    matCameraToLight.TransformBox(m_casterAABB);

    m_pCamera->SetViewport(m_casterAABB.MinEdge.z, shadowArea.MaxEdge.z, AIR::PI / 4, fWidth, fheight, false);
    //m_pCamera->Update();
    m_pCamera->BuildViewMatrix();
    m_pCamera->BuildProjectionMatrix();
    m_pCamera->CaculateFrustum();

    m_matLightViewProj = m_pCamera->GetViewMatrix() * m_pCamera->GetProjectionMatrix() * m_matR;
    //m_pGPUShadowmapSource->SetLightViewProjMatrix(m_matLightViewProj);
    CGPUDataParamSource* pGPUDataParam = pRenderer->GetGPUDataParam();
    pGPUDataParam->SetLightViewProjMatrix(m_matLightViewProj);
    pRenderer->SetViewTransform(&(m_pCamera->GetViewMatrix()));
    pRenderer->SetProjectionTransform(&(m_pCamera->GetProjectionMatrix()));
    //====以下代码测试用=====
    Vector4f vCameraLookAt = Vector4f(m_pCamera->GetLookAt().x, m_pCamera->GetLookAt().y, m_pCamera->GetLookAt().z, 1.0f);
	Vector4f vLookAtInLightPos;

	m_matLightViewProj.TransformVector4(vCameraLookAt, vLookAtInLightPos);
    //=================

    m_lstVisableNodes.clear();

	ENGINE_INST->TriggerEvent(Event_RenderShadowmap, this, 0, 0);

    pScene->Cull(m_pCamera, m_lstVisableNodes, m_RenderFlow);
	//pRenderer->SetDepthBias(m_fShadowDepthBias, m_fShadowScoleScaleBias);
	//m_pRasterizerStateObject->SetDepthBiasClamp(m_fShadowDepthBias);
	//m_pRasterizerStateObject->SetDepthBias(m_fShadowDepthBias);
	//m_pRasterizerStateObject->SetSlopeScaledDepthBias(m_fShadowScoleScaleBias);
	//m_pRasterizerStateObject->Apply(pRenderer);

    AIRList<CSceneNode*>::iterator itEnd = m_lstVisableNodes.end();
    //for (LIST_SCENENODE_ITER it = lstVisableNodes.begin(); it != itEnd; ++it)
    //{
    //    (*it)->AddAttachObjToRenderQueue(&renderQueue);
    //}

    for (LIST_SCENENODE_ITER it = m_lstVisableNodes.begin(); it != itEnd; ++it)
    {
        (*it)->FillRenderContext(m_pCamera, &m_renderQueue, m_RenderFlow);
    }
    
    if (m_renderQueue.GetRenderContextsNum() == 0)
    {
        return;
    }
    TextureRenderTarget* pTargets[1] = { m_pShadowmap };
    pRenderer->SetRenderTargetsEx(1, pTargets, m_pDepthStencil);
    pRenderer->BeginScene();
    //pRenderer->ClearScene( 0xffffffff );
    pRenderer->SetDepthBiasEnable(true, m_fShadowDepthBias, m_fShadowScoleScaleBias);
	m_renderQueue.DrawContexts();
    /*    先屏蔽shadowmap的渲染代码，2014.10.28 往后改成cascade shadowmap
	switch (m_type)
	{
	case SMT_ORTHO:
		{
			RenderQueueGroup* pMeshGroup = m_renderQueue.GetRenderQueueGroup(RenderGroup_Type::eRGT_Solid);
			if (pMeshGroup)
			{
				ListIterator<RenderQueueGroup::RenderableList> iter = pMeshGroup->GetRenderableListItorObject();
				while (!iter.IsEnd())
				{
					Renderable* pRenderable = iter.GetNext();
					if (pRenderable->IsRenderToShadow())
					{
						pRenderer->SetWorldTransform(&(pRenderable->GetWorldMatrix()));
						pRenderable->RenderToShadowMap(m_pCamera);
					}
				}
			}
		}
		break;
	case SMT_PSM:
	case SMT_TSM:
		{
			//pRenderer->SetProjectionTransform(&Matrix4f::IDENTITY);
			//pRenderer->SetViewTransform(&m_matLightViewProj);
			pRenderer->GetGPUDataParam()->SetViewProjectMatrix(m_matLightViewProj);
			CRenderQueue* pRenderQueue = NULL;//ENGINE_INST->GetSceneMgr()->GetRenderQueue();
			RenderQueueGroup* pGroup = pRenderQueue->GetRenderQueueGroup(RenderGroup_Type::eRGT_Solid);
			ListIterator<RenderQueueGroup::RenderableList> iter = pGroup->GetRenderableListItorObject();

			while (!iter.IsEnd())
			{
				Renderable* pRenderable = iter.GetNext();

				if (pRenderable->IsRenderToShadow())
				{
					pRenderer->GetGPUDataParam()->SetWorldViewProjectMatrix(pRenderable->GetWorldMatrix() * m_matLightViewProj);
					//pRenderer->GetGPUDataParam()->SetWorldMatrix(pRenderable->GetWorldMatrix());
					//pRenderer->GetGPUDataParam()->SetViewMatrix(m_pCamera->GetViewMatrix());
					//pRenderer->GetGPUDataParam()->SetProjectionMatrix(m_pCamera->GetProjectionMatrix() * m_matR);
					pRenderable->RenderToShadowMap(m_pCamera);
				}
			}

			pGroup = m_renderQueue.GetRenderQueueGroup(RenderGroup_Type::eRGT_Solid);
			iter = pGroup->GetRenderableListItorObject();
			while (!iter.IsEnd())
			{
				Renderable* pRenderable = iter.GetNext();

				if (pRenderable->IsRenderToShadow())
				{
					pRenderer->GetGPUDataParam()->SetWorldViewProjectMatrix(pRenderable->GetWorldMatrix() * m_matLightViewProj);
					//pRenderer->GetGPUDataParam()->SetWorldMatrix(pRenderable->GetWorldMatrix());
					//pRenderer->GetGPUDataParam()->SetViewMatrix(m_pCamera->GetViewMatrix());
					//pRenderer->GetGPUDataParam()->SetProjectionMatrix(m_pCamera->GetProjectionMatrix() * m_matR);
					pRenderable->RenderToShadowMap(m_pCamera);
				}
			}
		}
		break;
	case SMT_CSM:
		{
			RenderCascadeShadowmap();
		}
		break;
	}
	*/
    pRenderer->SetDepthBiasEnable(false, 0, 0);

	pRenderer->EndScene();

	pRenderer->SetRenderTargets(1, 0);

	//m_pGPUShadowmapSource->SetFilterSize(Vector4f(0, 0, 0, m_fSoftNess / m_nShadowmapSize));
	//m_pGPUShadowmapSource->SetJitterScale(Vector4f(m_fJitterScale / JITTER_SIZE, m_fJitterScale / JITTER_SIZE, 0, 0));

	pRenderer->GetGPUDataParam()->SetFilterSize(Vector4f(0, 0, 0, m_fSoftNess / m_nShadowmapSize));
	pRenderer->GetGPUDataParam()->SetJitterScale(Vector4f(m_fJitterScale / JITTER_SIZE, m_fJitterScale / JITTER_SIZE, 0, 0));

	//IRenderer* pRenderer = RENDERER;
	m_pShadowSamplerState->Apply(pRenderer, NULL);
	pRenderer->SetDepthStencilAsTexture(8, m_pDepthStencil);
	//pRenderer->SetTexture(8, m_pShadowmap);
	//pRenderer->SetTextureBorderColor(8, 0xffffffff);
	//pRenderer->SetTextureAddressMode(8, TAV_Border);
	//pRenderer->SetTextureFilter(8, TFT_Point);
	//pRenderer->SetTextureFilter(9, TFT_Point);

	m_pJitterSamplerState->Apply(pRenderer, m_pJitterTexture);
	pRenderer->SetTexture(9, m_pJitterTexture/*SHADOWMAP->GetJitterTexture()*/);

	//pRenderer->SetDepthBias(0, 0);
	//m_pRasterizerStateObject->SetDepthBiasClamp(0);
	//m_pRasterizerStateObject->SetSlopeScaledDepthBias(0);
	//m_pRasterizerStateObject->Apply(pRenderer);
}

void Shadowmap::SetTarget(CSceneNode* pTarget)
{
    m_pTarget = pTarget;
}

void Shadowmap::SetDistanceLightToTarget(float fDistance)
{
    m_fDisLightToTarget = fDistance;
}

//void* Shadowmap::GetDepthSurface()
//{
//	return m_pDepthTexture ? m_pDepthTexture->GetPixelBuffer() : 0;
//}

void Shadowmap::Clear()
{
 
    if (m_pShadowmap)
    {
        //TEXTURE_MANAGER->ClearTexture(m_pShadowmap);
    }

    if (m_pJitterTexture)
    {
        TEXTURE_MANAGER->ClearTexture(m_pJitterTexture);
    }

	SafeDelete(m_pShadowSamplerState);
	SafeDelete(m_pJitterSamplerState);
	SafeDelete(m_pRasterizerStateObject);
}

void Shadowmap::CreateSoftShadowJitterTexture()
{
    if (m_pJitterTexture == NULL)
    {
        IRenderer* pRenderer = ENGINE_INST->GetRenderer();
        TextureParam param(JITTER_SIZE, JITTER_SIZE, JITTER_SAMPLES * JITTER_SAMPLES / 2, 1, eRSU_Default, RPF_A8R8G8B8, TT_VOLUMN);
        m_pJitterTexture = TEXTURE_MANAGER->CreateTexture("JitterTexture", param/*TT_VOLUMN, JITTER_SIZE, JITTER_SIZE, JITTER_SAMPLES * JITTER_SAMPLES / 2,
                                                                                1, false, RPF_A8R8G8G8*/);

        int nRowPitch, nSlicePitch;
        unsigned char *data = (unsigned char*)m_pJitterTexture->LockVolumn(nRowPitch, nSlicePitch);
        for (int i = 0; i < JITTER_SIZE; i++) 
        {
            for (int j = 0; j < JITTER_SIZE; j++) 
            {
                float rot_offset = ((float)rand() / RAND_MAX - 1) * 2 * AIR::PI;

                for (int k = 0; k < JITTER_SAMPLES * JITTER_SAMPLES / 2; k++) 
                {

                    int x, y;
                    float v[4];

                    x = k % (JITTER_SAMPLES / 2);
                    y = (JITTER_SAMPLES - 1) - k / (JITTER_SAMPLES / 2);

                    v[0] = (float)(x * 2 + 0.5f) / JITTER_SAMPLES;
                    v[1] = (float)(y + 0.5f) / JITTER_SAMPLES;
                    v[2] = (float)(x * 2 + 1 + 0.5f) / JITTER_SAMPLES;
                    v[3] = v[1];

                    // jitter
                    v[0] += ((float)rand() * 2 / RAND_MAX - 1) / JITTER_SAMPLES;
                    v[1] += ((float)rand() * 2 / RAND_MAX - 1) / JITTER_SAMPLES;
                    v[2] += ((float)rand() * 2 / RAND_MAX - 1) / JITTER_SAMPLES;
                    v[3] += ((float)rand() * 2 / RAND_MAX - 1) / JITTER_SAMPLES;

                    // warp to disk
                    float d[4];
                    d[0] = sqrtf(v[1]) * cosf(2 * AIR::PI * v[0] + rot_offset);
                    d[1] = sqrtf(v[1]) * sinf(2 * AIR::PI * v[0] + rot_offset);
                    d[2] = sqrtf(v[3]) * cosf(2 * AIR::PI * v[2] + rot_offset);
                    d[3] = sqrtf(v[3]) * sinf(2 * AIR::PI * v[2] + rot_offset);

                    d[0] = (d[0] + 1.0) / 2.0;
                    data[k * nSlicePitch + j * nRowPitch + i * 4 + 0] = (unsigned char)(d[0] * 255);
                    d[1] = (d[1] + 1.0) / 2.0;
                    data[k * nSlicePitch + j * nRowPitch + i * 4 + 1] = (unsigned char)(d[1] * 255);
                    d[2] = (d[2] + 1.0) / 2.0;
                    data[k * nSlicePitch + j * nRowPitch + i * 4 + 2] = (unsigned char)(d[2] * 255);
                    d[3] = (d[3] + 1.0) / 2.0;
                    data[k * nSlicePitch + j * nRowPitch + i * 4 + 3] = (unsigned char)(d[3] * 255);
                }
            }
        }
        m_pJitterTexture->Unlock();
    }
}

void Shadowmap::BuildPSMMatrices()
{
    //第一步，获取所有在可视范围内的渲染物 
    ISceneManager* pScene = ENGINE_INST->GetSceneMgr();
    CCamera* pSceneCamera = pScene->GetCamera();

    //ListIterator<LIST_SCENENODE> iter = pScene->GetVisibleSceneNodes();
    CRenderQueue* pRenderQueue = NULL;//pScene->GetRenderQueue();

    m_zNear = 1.0f;
    m_zFar = 1000.0f;
    m_fSlideBack = 0;
    //f32 fFar = 100.0f;

    //计算虚拟摄像机的far和near
    AABBBox3Df boundingInView;

    //AIRList<AABBBox3Df> boxList;
    m_lstShadowReceivers.clear();

    
    //ListIterator<LIST_SCENENODE> nodeIter = pScene->GetVisibleSceneNodes();
    //MapIterator<ISceneManager::MAP_SCENENODE> nodeIter = pScene->GetMapItorSceneNode();
    //AIRList<CSceneNode*>::iterator nodeIter = m_lstVisableNodes.begin();
    
    //while (!nodeIter.IsEnd())
    for (AIRList<CSceneNode*>::iterator it = m_lstVisableNodes.begin(); it != m_lstVisableNodes.end(); ++it)
    {
        CSceneNode* pNode = *it;//nodeIter.GetNext();
        ListIterator<CSceneNode::RenderableList> renderableIter = pNode->GetMapItorRenderable();
        while (!renderableIter.IsEnd())
        {
            Renderable* pRenderable = renderableIter.GetNext();
            boundingInView = pRenderable->GetWorldBoundingBox(true);
            //boundingInView.MaxEdge += boundingInView.GetHalfSize();
            //boundingInView.MinEdge -= boundingInView.GetHalfSize();
            if (pSceneCamera->IsVisable(boundingInView))
            {
                if (pRenderable->IsAcceptShadowMap() || pRenderable->IsRenderToShadow())
                {
                    if (pRenderable->GetCenter().GetDistanceFrom(pSceneCamera->GetLookAt()) <= m_fShadowRangeRadius)
                    {
                        boundingInView.MaxEdge += boundingInView.GetHalfSize() * 0.5f;
                        boundingInView.MinEdge -= boundingInView.GetHalfSize() * 0.5f;
                        //pSceneCamera->GetViewMatrix().TransformBox(boundingInView);
                        CMathUtil::XFormBoundingBox(boundingInView, boundingInView, pSceneCamera->GetViewMatrix());

                        m_zNear = Min(m_zNear, boundingInView.MinEdge.z);
                        m_zFar = Max(m_zFar, boundingInView.MaxEdge.z);

                        m_lstShadowReceivers.push_back(boundingInView);
                    }
                    
                }
                
            }
        }
    }

    //地形boundingbox
    
    const Vector3Df& camLookAt = pSceneCamera->GetLookAt();
    boundingInView.MaxEdge = camLookAt;
    boundingInView.MinEdge = camLookAt;
    //CHeightMap* pTerrain = TERRAIN->GetCurHeightmap();
    //f32 fW = pSceneCamera->GetDistance() < pTerrain->GetBlockWidth() / 16 ? 40 : 20;
    boundingInView.MaxEdge.x += m_fShadowRangeRadius; //pTerrain->GetBlockWidth() / fW;
    boundingInView.MaxEdge.z += m_fShadowRangeRadius; //pTerrain->GetBlockWidth() / fW;
    //boundingInView.MaxEdge.y += pTerrain->GetBlockWidth() / 200;
    //boundingInView.MinEdge = camLookAt;
    boundingInView.MinEdge.x -= m_fShadowRangeRadius; //pTerrain->GetBlockWidth() / fW;
    boundingInView.MinEdge.z -= m_fShadowRangeRadius; //pTerrain->GetBlockWidth() / fW;
    //boundingInView.MinEdge.y -= pTerrain->GetBlockWidth() / 100;
    //pSceneCamera->GetViewMatrix().TransformBox(boundingInView);
    CMathUtil::XFormBoundingBox(boundingInView, boundingInView, pSceneCamera->GetViewMatrix());
    m_zNear = Min(m_zNear, boundingInView.MinEdge.z);
    m_zFar = Max(m_zFar, boundingInView.MaxEdge.z);

    m_lstShadowReceivers.push_back(boundingInView);
    
    /*
    RenderQueueGroup* pGroup = pRenderQueue->GetRenderQueueGroup(RenderGroup_Type::eRGT_Terrain);
    ListIterator<RenderableList> iter = pGroup->GetRenderableListItorObject();
    while (!iter.IsEnd())
    {
        Renderable* pRenderable = iter.GetNext();

        boundingInView = pRenderable->GetWorldBoundingBox(true);
        pSceneCamera->GetViewMatrix().TransformBox(boundingInView);
        m_zNear = Min(m_zNear, boundingInView.MinEdge.z);
        m_zFar = Max(m_zFar, boundingInView.MaxEdge.z);

        m_lstShadowReceivers.push_back(boundingInView);
    }
    */

    f32 infinity = m_zFar / (m_zFar - m_zNear);
    f32 fInfinityZ = m_fMinInfinityZ;
    if (infinity <= fInfinityZ)
    {
        float additionalSlide = fInfinityZ * (m_zFar - m_zNear) - m_zFar + 0.0001f;
        m_fSlideBack = additionalSlide;
        m_zFar += additionalSlide;
        m_zNear += additionalSlide;
    }

    const Vector3Df posEye(0, 0, 0);
    const Vector3Df dirEye(0, 0, 1);

    Matrix4f matVirtualCameraView;     
    Matrix4f matVirtualCameraProj;
    matVirtualCameraView.Identity();
    matVirtualCameraProj.Identity();
    matVirtualCameraView.SetTranslation(0, 0, m_fSlideBack);

    BoundingPerspectiveData boundingCone;
    ComputeBoundPerpespectiveData(m_lstShadowReceivers, matVirtualCameraView, posEye, dirEye, boundingCone);

    
    matVirtualCameraProj.BuildProjectionMatrixPerspectiveLH(2.f * tanf(boundingCone.fFovX)  * m_zNear, 
        2.f * tanf(boundingCone.fFovY) * m_zNear, m_zNear, m_zFar);

    //CCamera* pSceneCamera = pScene->GetCamera();
    m_matVirtualCameraViewProj = pSceneCamera->GetViewMatrix() * matVirtualCameraView;
    m_matVirtualCameraViewProj = m_matVirtualCameraViewProj * matVirtualCameraProj;

    Matrix4f matEyeToPostProjectiveVirtualCamera;
    Matrix4f matEyeToPostProjectiveView;
    matEyeToPostProjectiveVirtualCamera = matVirtualCameraView * matVirtualCameraProj;
    //D3DXMATRIX eyeToPostProjectiveVirtualCamera;
    //D3DXMatrixMultiply(&eyeToPostProjectiveVirtualCamera, &virtualCameraView, &virtualCameraProj);

    Vector3Df dirLightInView = -pScene->GetSunLight()->GetDirection();
    pSceneCamera->GetViewMatrix().TransformNormal(dirLightInView);
    //dirLightInView.Normalize();
    //D3DXVECTOR3 eyeLightDir;  
    //D3DXVec3TransformNormal(&eyeLightDir, &m_lightDir, &m_View);

    //  directional light becomes a point on infinity plane in post-projective space
    Vector4f lightDirW(dirLightInView.x, dirLightInView.y, dirLightInView.z, 0.f);   
    Vector4f   ppLight;
    matVirtualCameraProj.TransformVector4(lightDirW, ppLight);

    //D3DXVec4Transform(&ppLight, &lightDirW, &virtualCameraProj);

    Matrix4f matLightView;
    Matrix4f matLightProj;
    m_bShadowTestInverted = (ppLight.w < 0.f); // the light is coming from behind the eye

    //if (bShadowTestInverted)
    //{
    //    OutputDebugString("the light is coming from behind the eye!\n");
    //}
    //else
    //{
    //    OutputDebugString("the light is not behind the eye!\n");
    //}
    

    if ( (fabsf(ppLight.w) <= 0.001f) )  // orthographic matrix; uniform shadow mapping
    {
        Vector3Df ppLightDirection(ppLight.x, ppLight.y, ppLight.z);
        ppLightDirection.Normalize();
        //D3DXVec3Normalize(&ppLightDirection, &ppLightDirection);

        AABBBox3Df ppUnitBox; 
        ppUnitBox.MaxEdge = Vector3Df(1, 1, 1); 
        ppUnitBox.MinEdge = Vector3Df(-1, -1, 0);
        Vector3Df cubeCenter = ppUnitBox.GetCenter();
        //ppUnitBox.Centroid( &cubeCenter );
        float t = 0;

        ppUnitBox.Intersect(t, cubeCenter, ppLightDirection);
        //ppUnitBox.Intersect( &t, &cubeCenter, &ppLightDirection );
        if (t == 0)
        {
            t = 1;
        }
        Vector3Df lightPos = cubeCenter + 2.f * t * ppLightDirection;
        Vector3Df axis = Vector3Df::UNIT_Y;

        //  if the yAxis and the view direction are aligned, choose a different up vector, to avoid singularity
        //  artifacts
        if ( fabsf(ppLightDirection.DotProduct(Vector3Df::UNIT_Y)) > 0.99f )
            axis = Vector3Df::UNIT_Z;

        //D3DXMatrixLookAtLH(&lightView, &lightPos, &cubeCenter, &axis);
        matLightView.BuildCameraLookAtMatrixLH(lightPos, cubeCenter, axis);
        //XFormBoundingBox(&ppUnitBox, &ppUnitBox, &matLightView);
        //matLightView.TransformBox(ppUnitBox);
        CMathUtil::XFormBoundingBox(ppUnitBox, ppUnitBox, matLightView);
        matLightProj.BuildProjectionMatrixOrthoOffCenterLH(ppUnitBox.MinEdge.x, ppUnitBox.MaxEdge.x, ppUnitBox.MinEdge.y, ppUnitBox.MaxEdge.y, ppUnitBox.MinEdge.z, ppUnitBox.MaxEdge.z);
        //D3DXMatrixOrthoOffCenterLH(&lightProj, ppUnitBox.minPt.x, ppUnitBox.maxPt.x, ppUnitBox.minPt.y, ppUnitBox.maxPt.y, ppUnitBox.minPt.z, ppUnitBox.maxPt.z);
        m_fPPNear = ppUnitBox.MinEdge.z;
        m_fPPFar  = ppUnitBox.MaxEdge.z;

        m_fSlideBack = 0.f;
    }
    else  // otherwise, use perspective shadow mapping
    {
        Vector3Df ppLightPos;
        float wRecip = 1.0f / ppLight.w;
        ppLightPos.x = ppLight.x * wRecip;
        ppLightPos.y = ppLight.y * wRecip;
        ppLightPos.z = ppLight.z * wRecip;

        //Matrix4f eyeToPostProjectiveLightView;

        const float ppCubeRadius = 1.5f;  // the post-projective view box is [-1,-1,0]..[1,1,1] in DirectX, so its radius is 1.5
        const Vector3Df ppCubeCenter(0.f, 0.f, 0.5f);

        if (m_bShadowTestInverted)  // use the inverse projection matrix
        {
            /*
            BoundingCone viewCone;
            if (!m_bUnitCubeClip)
            {
                //  project the entire unit cube into the shadow map  
                AIRVector<BoundingBox> justOneBox;
                BoundingBox unitCube;
                unitCube.minPt = D3DXVECTOR3(-1.f, -1.f, 0.f);
                unitCube.maxPt = D3DXVECTOR3( 1.f, 1.f, 1.f );
                justOneBox.push_back(unitCube);
                D3DXMATRIX tmpIdentity;
                D3DXMatrixIdentity(&tmpIdentity);
                viewCone = BoundingCone(&justOneBox, &tmpIdentity, &ppLightPos);               
            }
            else
            */
            {
                //  clip the shadow map to just the used portions of the unit box.
                //viewCone = BoundingCone(&m_ShadowReceiverPoints, &eyeToPostProjectiveVirtualCamera, &ppLightPos);

                ComputeBoundPerpespectiveData(m_lstShadowReceivers, matEyeToPostProjectiveVirtualCamera, ppLightPos, boundingCone);
                /*
                if (m_lstShadowReceivers.size() == 0)
                {
                    dirEye = Vector3Df::NEGATIVE_UNIT_Z;
                    fFovX = 0;
                    fFovY = 0;
                    matLookAt.Identity();
                }
                else
                {
                    boxPoints.clear();
                    size_t boxSize = m_lstShadowReceivers.size();
                    boxPoints.resize(boxSize * 8);

                    int i = 0;
                    for (AIRList<AABBBox3Df>::iterator it = m_lstShadowReceivers.begin(); it != itBoxEnd; ++it, ++i)
                    {
                        (*it).GetEdges(&boxPoints[i * 8]);
                    }

                    Vector3Df boundingCenter;
                    f32 fRadius = 0;
                    CaculateBoundingSphere(boxPoints, boundingCenter, fRadius);

                    float min_cosTheta = 1.f;

                    dirEye = boundingCenter - posEye;
                    dirEye.Normalize();
                    //D3DXVec3Normalize(&direction, &direction);

                    Vector3Df axis = Vector3Df::UNIT_Y;

                    if ( fabsf(axis.DotProduct(dirEye)) > 0.99f )
                    {
                        axis = Vector3Df::UNIT_Z;
                    }

                    matLookAt.BuildCameraLookAtMatrixLH(posEye, posEye + dirEye, axis);
                    //D3DXMatrixLookAtLH(&m_LookAt, &posEye, &(posEye + dirEye), &axis);

                    fNear = 1e32f;
                    fFar = 0.f;

                    fMaxX = 0;
                    fMaxY = 0;
                    Vector3Df tmp;
                    for (size_t i = 0; i < boxPoints.size(); i++)
                    {
                        //D3DXVECTOR3 tmp;
                        //D3DXVec3TransformCoord(&tmp, &ppPts[i], &m_LookAt);
                        matLookAt.TransformCoord(boxPoints[i], tmp);
                        fMaxX = Max(fMaxX, fabsf(tmp.x / tmp.z));
                        fMaxY = Max(fMaxY, fabsf(tmp.y / tmp.z));
                        fNear = Min(fNear, tmp.z);
                        fFar  = Max(fFar, tmp.z);
                    }

                    fFovX = atanf(fMaxX);
                    fFovY = atanf(fMaxY);
                }
                */
            }

            //  construct the inverse projection matrix -- clamp the fNear value for sanity (clamping at too low
            //  a value causes significant underflow in a 24-bit depth buffer)
            //  the multiplication is necessary since I'm not checking shadow casters
            boundingCone.fNear = Max(0.001f, boundingCone.fNear * 0.3f);
            m_fPPNear = -boundingCone.fNear;
            m_fPPFar  = boundingCone.fNear;
            matLightView = boundingCone.matView;
            matLightProj.BuildProjectionMatrixPerspectiveLH( 2.f * tanf(boundingCone.fFovX) * m_fPPNear, 2.f * tanf(boundingCone.fFovY) * m_fPPNear, m_fPPNear, m_fPPFar);
            //lightView = viewCone.m_LookAt;
            //D3DXMatrixPerspectiveLH( &lightProj, 2.f*tanf(viewCone.fovx)*m_ppNear, 2.f*tanf(viewCone.fovy)*m_ppNear, m_ppNear, m_ppFar );
            
        }
        else  // regular projection matrix
        {
            float fFovy, fAspect, fFar, fNear;
            /*
            if (!m_bUnitCubeClip)
            {
                D3DXVECTOR3 lookAt = ppCubeCenter - ppLightPos;

                float distance = D3DXVec3Length(&lookAt);
                lookAt = lookAt / distance;

                D3DXVECTOR3 axis = yAxis;
                //  if the yAxis and the view direction are aligned, choose a different up vector, to avoid singularity
                //  artifacts
                if ( fabsf(D3DXVec3Dot(&yAxis, &lookAt))>0.99f )
                    axis = zAxis;

                //  this code is super-cheese.  treats the unit-box as a sphere
                //  lots of problems, looks like hell, and requires that MinInfinityZ >= 2
                D3DXMatrixLookAtLH(&lightView, &ppLightPos, &ppCubeCenter, &axis);
                fFovy = 2.f*atanf(ppCubeRadius/distance);
                fAspect = 1.f;
                fNear = max(0.001f, distance - 2.f*ppCubeRadius);
                fFar = distance + 2.f*ppCubeRadius;
                BoundingBox ppView;
                D3DXMatrixMultiply(&eyeToPostProjectiveLightView, &eyeToPostProjectiveVirtualCamera, &lightView);
            }
            else
            */
            {
                //  unit cube clipping
                //  fit a cone to the bounding geometries of all shadow receivers (incl. terrain) in the scene

                /*
                if (m_lstShadowReceivers.size() == 0)
                {
                    dirEye = Vector3Df::NEGATIVE_UNIT_Z;
                    fFovX = 0;
                    fFovY = 0;
                    matLookAt.Identity();
                }
                else
                {
                    boxPoints.clear();
                    size_t boxSize = m_lstShadowReceivers.size();
                    boxPoints.resize(boxSize * 8);

                    int i = 0;
                    for (AIRList<AABBBox3Df>::iterator it = m_lstShadowReceivers.begin(); it != itBoxEnd; ++it, ++i)
                    {
                        (*it).GetEdges(&boxPoints[i * 8]);
                    }

                    Vector3Df boundingCenter;
                    f32 fRadius = 0;
                    CaculateBoundingSphere(boxPoints, boundingCenter, fRadius);

                    float min_cosTheta = 1.f;

                    dirEye = boundingCenter - posEye;
                    dirEye.Normalize();
                    //D3DXVec3Normalize(&direction, &direction);

                    Vector3Df axis = Vector3Df::UNIT_Y;

                    if ( fabsf(axis.DotProduct(dirEye)) > 0.99f )
                    {
                        axis = Vector3Df::UNIT_Z;
                    }

                    matLookAt.BuildCameraLookAtMatrixLH(posEye, posEye + dirEye, axis);
                    //D3DXMatrixLookAtLH(&m_LookAt, &posEye, &(posEye + dirEye), &axis);

                    fNear = 1e32f;
                    fFar = 0.f;

                    fMaxX = 0;
                    fMaxY = 0;
                    Vector3Df tmp;
                    for (size_t i = 0; i < boxPoints.size(); i++)
                    {
                        //D3DXVECTOR3 tmp;
                        //D3DXVec3TransformCoord(&tmp, &ppPts[i], &m_LookAt);
                        matLookAt.TransformCoord(boxPoints[i], tmp);
                        fMaxX = Max(fMaxX, fabsf(tmp.x / tmp.z));
                        fMaxY = Max(fMaxY, fabsf(tmp.y / tmp.z));
                        fNear = Min(fNear, tmp.z);
                        fFar  = Max(fFar, tmp.z);
                    }

                    fFovX = atanf(fMaxX);
                    fFovY = atanf(fMaxY);
                }
                */
                ComputeBoundPerpespectiveData(m_lstShadowReceivers, matEyeToPostProjectiveVirtualCamera, ppLightPos, boundingCone);
                matLightView = boundingCone.matView;
                matEyeToPostProjectiveView = matEyeToPostProjectiveVirtualCamera * matLightView;
                //D3DXMatrixMultiply(&eyeToPostProjectiveLightView, &eyeToPostProjectiveVirtualCamera, &lightView);
                float fDistance = (ppLightPos - ppCubeCenter).GetLength();
                fFovy = 2.f * boundingCone.fFovY;
                fAspect = boundingCone.fFovX / boundingCone.fFovY;
                fFar = boundingCone.fFar;
                fNear = boundingCone.fNear;
                //  hack alert!  adjust the near-plane value a little bit, to avoid clamping problems
                fNear = fNear * 0.6f;
            }

            fNear = max(0.001f, fNear);
            m_fPPNear = fNear;
            m_fPPFar = fFar;
            matLightProj.BuildProjectionMatrixPerspectiveFovLH(fFovy, fAspect, m_fPPNear, m_fPPFar);
            //D3DXMatrixPerspectiveFovLH(&lightProj, fFovy, fAspect, m_ppNear, m_ppFar);
        }
    }

    //  build the composite matrix that transforms from world space into post-projective light space
    m_matLightViewProj = matLightView * matLightProj;   
    m_matLightViewProj = m_matVirtualCameraViewProj * m_matLightViewProj;
    //D3DXMatrixMultiply(&m_LightViewProj, &lightView, &lightProj);
    //D3DXMatrixMultiply(&m_LightViewProj, &virtualCameraViewProj, &m_LightViewProj);
}

void Shadowmap::ComputeFitCameraParams(CCamera* pCamera)
{
    
}

void Shadowmap::ComputeFOVByBounds(AIRList<AABBBox3Df>& boxList, const Matrix4f& matProj, const Vector3Df& posEye, const Vector3Df& _direction, f32& fFovX, f32& fFovY)
{
    Vector3Df axis = Vector3Df::UNIT_Y;

    if ( fabsf(Vector3Df::UNIT_Y.DotProduct(_direction)) > 0.99f )
        axis = Vector3Df::UNIT_Z;

    Matrix4f matLookAt, matConcat;   //这个matconcat不知道具体的作用是什么
    matLookAt.BuildCameraLookAtMatrixLH(posEye, posEye + _direction, axis);
    matConcat = matProj * matLookAt;
    //计算摄像机的fov角度
    AIRList<AABBBox3Df>::iterator itBoxEnd = boxList.end();
    AIRVector<Vector3Df> boxPoints;
    boxPoints.resize(8);
    f32 fMaxX = 0;
    f32 fMaxY = 0;
    for (AIRList<AABBBox3Df>::iterator it = boxList.begin(); it != itBoxEnd; ++it)
    {
        (*it).GetEdges(&boxPoints[0]);
        for (size_t i = 0; i < 8; ++i)
        {
            matConcat.TransformCoord(boxPoints[i], boxPoints[i]);
            fMaxX = Max(fMaxX, fabsf(boxPoints[i].x / boxPoints[i].z));
            fMaxY = Max(fMaxY, fabsf(boxPoints[i].y / boxPoints[i].z));
        }
    }

    fFovX = atanf(fMaxX);
    fFovY = atanf(fMaxY);
}

void Shadowmap::CaculateBoundingSphere(const AIRVector<Vector3Df>& points, Vector3Df& center, f32& fRadius)
{
    AIRVector<Vector3Df>::const_iterator ptIt = points.begin();

    fRadius = 0.f;
    center = *ptIt++;

    Vector3Df cVec;
    while ( ptIt != points.end() )
    {
        const Vector3Df& tmp = *ptIt++;
        cVec = tmp - center;
        float d = cVec.DotProduct(cVec); //D3DXVec3Dot( &cVec, &cVec );
        if ( d > fRadius * fRadius )
        {
            d = sqrtf(d);
            float r = 0.5f * (d + fRadius);
            float scale = (r - fRadius) / d;
            center = center + scale*cVec;
            fRadius = r;
        }
    }
}

void Shadowmap::ComputeBoundPerpespectiveData(AIRList<AABBBox3Df>& boxList, const Matrix4f& matProj, const Vector3Df& posEye, const Vector3Df& _direction, BoundingPerspectiveData& out)
{
    Vector3Df axis = Vector3Df::UNIT_Y;

    if ( fabsf(Vector3Df::UNIT_Y.DotProduct(_direction)) > 0.99f )
        axis = Vector3Df::UNIT_Z;

    Matrix4f matConcat;   //这个matconcat不知道具体的作用是什么
    out.matView.BuildCameraLookAtMatrixLH(posEye, posEye + _direction, axis);
    matConcat = matProj * out.matView;
    //计算摄像机的fov角度
    AIRList<AABBBox3Df>::iterator itBoxEnd = boxList.end();
    //AIRVector<Vector3Df, CMemorySTLAllocator<Vector3Df, CMemoryNedAlloc>> boxPoints;
    //boxPoints.resize(8);
    f32 fMaxX = 0;
    f32 fMaxY = 0;
    out.fNear = 1e32f;
    out.fFar = 0;
    Vector3Df boxPoint;
    for (AIRList<AABBBox3Df>::iterator it = boxList.begin(); it != itBoxEnd; ++it)
    {
        //(*it).GetEdges(&boxPoints[0]);
        for (size_t i = 0; i < 8; ++i)
        {
            boxPoint = (*it).Point(i);
            matConcat.TransformCoord(boxPoint, boxPoint);
            fMaxX = Max(fMaxX, fabsf(boxPoint.x / boxPoint.z));
            fMaxY = Max(fMaxY, fabsf(boxPoint.y / boxPoint.z));
            out.fNear = Min(out.fNear, boxPoint.z);
            out.fFar = Max(out.fFar, boxPoint.z);
        }
    }

    out.fFovX = atanf(fMaxX);
    out.fFovY = atanf(fMaxY);
}

void Shadowmap::ComputeBoundPerpespectiveData(AIRList<AABBBox3Df>& boxList, const Matrix4f& matProj, const Vector3Df& posEye, BoundingPerspectiveData& out)
{
    if (boxList.size() == 0)
    {
        out.direction = Vector3Df::NEGATIVE_UNIT_Z;
        out.fFovX = 0;
        out.fFovY = 0;
        out.matView.Identity();
    }
    else
    {
        AIRVector<Vector3Df> boxPoints;
        size_t boxSize = m_lstShadowReceivers.size();
        boxPoints.reserve(boxSize * 8);

        //int i = 0;
        Vector3Df point;
        for (AIRList<AABBBox3Df>::iterator it = boxList.begin(); it != boxList.end(); ++it)
        {
            //(*it).GetEdges(&boxPoints[i * 8]);
            for (u32 i = 0; i < 8; ++i)
            {
                point = (*it).Point(i);
                matProj.TransformCoord(point, point);
                boxPoints.push_back(point);
            }
        }

        Vector3Df boundingCenter;
        f32 fRadius = 0;
        CaculateBoundingSphere(boxPoints, boundingCenter, fRadius);

        float min_cosTheta = 1.f;

        out.direction = boundingCenter - posEye;
        out.direction.Normalize();
        //D3DXVec3Normalize(&direction, &direction);

        Vector3Df axis = Vector3Df::UNIT_Y;

        if ( fabsf(axis.DotProduct(out.direction)) > 0.99f )
        {
            axis = Vector3Df::UNIT_Z;
        }

        out.matView.BuildCameraLookAtMatrixLH(posEye, posEye + out.direction, axis);
        //D3DXMatrixLookAtLH(&m_LookAt, &posEye, &(posEye + dirEye), &axis);

        f32 fMaxX = 0;
        f32 fMaxY = 0;

        out.fNear = 1e32f;
        out.fFar = 0.f;

        Vector3Df tmp;
        for (size_t i = 0; i < boxPoints.size(); i++)
        {
            //D3DXVECTOR3 tmp;
            //D3DXVec3TransformCoord(&tmp, &ppPts[i], &m_LookAt);
            out.matView.TransformCoord(boxPoints[i], tmp);
            fMaxX = Max(fMaxX, fabsf(tmp.x / tmp.z));
            fMaxY = Max(fMaxY, fabsf(tmp.y / tmp.z));
            out.fNear = Min(out.fNear, tmp.z);
            out.fFar  = Max(out.fFar, tmp.z);
        }

        out.fFovX = atanf(fMaxX);
        out.fFovY = atanf(fMaxY);
    }
}

bool Shadowmap::SaveShadowmap(const char* szFilename)
{
    if (m_pShadowmap)
    {
        return m_pShadowmap->SaveToFile(szFilename);
    }
    return false;
}

void Shadowmap::BuildTSMMatrices()
{
    //第一步，获取所有在可视范围内的渲染物 
    ISceneManager* pScene = ENGINE_INST->GetSceneMgr();
    CCamera* pSceneCamera = pScene->GetCamera();

    //ListIterator<LIST_SCENENODE> iter = pScene->GetVisibleSceneNodes();
    CRenderQueue* pRenderQueue = NULL;//pScene->GetRenderQueue();

    m_zNear = 1.0f;
    m_zFar = 1000.0f;
    m_fSlideBack = 0;
    //f32 fFar = 100.0f;

    //计算虚拟摄像机的far和near
    AABBBox3Df boundingInView;

    //AIRList<AABBBox3Df> boxList;
    m_lstShadowReceivers.clear();
    m_lstShadowCasters.clear();

    //ListIterator<LIST_SCENENODE> nodeIter = pScene->GetVisibleSceneNodes();
    /*
    MapIterator<ISceneManager::MAP_SCENENODE> nodeIter = pScene->GetMapItorSceneNode();
    while (!nodeIter.IsEnd())
    {
        CSceneNode* pNode = nodeIter.GetNext();
        ListIterator<CSceneNode::RenderableList> renderableIter = pNode->GetMapItorRenderable();
        while (!renderableIter.IsEnd())
        {
            Renderable* pRenderable = renderableIter.GetNext();
            boundingInView = pRenderable->GetWorldBoundingBox(true);
            //boundingInView.MaxEdge += boundingInView.GetHalfSize();
            //boundingInView.MinEdge -= boundingInView.GetHalfSize();
            if (pSceneCamera->IsVisable(boundingInView))
            {
                if (pRenderable->IsAcceptShadowMap() || pRenderable->IsRenderToShadow())
                {
                    boundingInView.MaxEdge += boundingInView.GetHalfSize() * 0.5f;
                    boundingInView.MinEdge -= boundingInView.GetHalfSize() * 0.5f;
                    CMathUtil::XFormBoundingBox(boundingInView, boundingInView, pSceneCamera->GetViewMatrix());

                    m_zNear = Min(m_zNear, boundingInView.MinEdge.z);
                    m_zFar = Max(m_zFar, boundingInView.MaxEdge.z);

                    m_lstShadowReceivers.push_back(boundingInView);

                    m_lstShadowCasters.push_back(boundingInView);
                    
                }

            }
            else
            {
                if (pRenderable->IsRenderToShadow())
                {
                    f32 fRadius = boundingInView.GetRadius();
                    if (pSceneCamera->IsVisable(boundingInView.GetCenter(), fRadius * 1.5f))
                    {
                        pRenderable->UpdateToRenderQueue(pSceneCamera, &m_renderQueue);

                        //pSceneCamera->GetViewMatrix().TransformBox(boundingInView);
                        CMathUtil::XFormBoundingBox(boundingInView, boundingInView, pSceneCamera->GetViewMatrix());

                        m_zNear = Min(m_zNear, boundingInView.MinEdge.z);
                        m_zFar = Max(m_zFar, boundingInView.MaxEdge.z);
                        m_lstShadowReceivers.push_back(boundingInView);
                        m_lstShadowCasters.push_back(boundingInView);
                    }
                }
            }
        }
    }
    */

    //地形boundingbox

    const Vector3Df& camLookAt = pSceneCamera->GetLookAt();
    boundingInView.MaxEdge = camLookAt;
    boundingInView.MinEdge = camLookAt;
    CHeightMap* pTerrain = TERRAIN->GetCurHeightmap();
    //f32 fW = pSceneCamera->GetDistance() < pTerrain->GetBlockWidth() / 16 ? 40 : 20;
    boundingInView.MaxEdge.x += m_fShadowRangeRadius; //pTerrain->GetBlockWidth() / fW;
    boundingInView.MaxEdge.z += m_fShadowRangeRadius; //pTerrain->GetBlockWidth() / fW;
    //boundingInView.MaxEdge.y += pTerrain->GetBlockWidth() / 200;
    //boundingInView.MinEdge = camLookAt;
    boundingInView.MinEdge.x -= m_fShadowRangeRadius; //pTerrain->GetBlockWidth() / fW;
    boundingInView.MinEdge.z -= m_fShadowRangeRadius;  //pTerrain->GetBlockWidth() / fW;
    //boundingInView.MinEdge.y -= pTerrain->GetBlockWidth() / 100;
    //pSceneCamera->GetViewMatrix().TransformBox(boundingInView);
    CMathUtil::XFormBoundingBox(boundingInView, boundingInView, pSceneCamera->GetViewMatrix());
    m_zNear = Min(m_zNear, boundingInView.MinEdge.z);
    m_zFar = Max(m_zFar, boundingInView.MaxEdge.z);

    //m_lstShadowReceivers.push_back(boundingInView);

    //f32 fCosGamma = -m_directionLight.x * pSceneCamera->GetViewMatrix()[2] +
    //    -m_directionLight.y * pSceneCamera->GetViewMatrix()[6] +
    //    -m_directionLight.z * pSceneCamera->GetViewMatrix()[10];

    //if ( fabsf(fCosGamma) >= 0.999f )
    //{
    //    //BuildOrthoShadowProjectionMatrix();
    //    int a = 0;
    //}

    //下面开始计算TSM矩阵
    //  get the near and the far plane (points) in eye space.
    Vector3Df frustumPntEye[8];
    Vector3Df frustumPnts[8];

    IRenderer* pRenderer = ENGINE_INST->GetRenderer();
    //m_matTSMProj.BuildProjectionMatrixPerspectiveFovLH(AIR::PI / 3.0f, (f32)pRenderer->GetRenderTargetWidth() / pRenderer->GetRenderTargetHeight(), 1.0f, 1000.0f);
    CFrustum eyeFrustum; //(/*pSceneCamera->GetProjectionMatrix()*/m_matTSMProj);
    eyeFrustum.SetPerspective(1.0f, pSceneCamera->GetViewFar() / 4, AIR::PI / 3.0f, (f32)pRenderer->GetRenderTargetWidth(), pRenderer->GetRenderTargetHeight());
    eyeFrustum.CalculateFrustum(Matrix4f::IDENTITY/*pSceneCamera->GetViewMatrix()*/);

    eyeFrustum.GetViewSpaceCorners(frustumPntEye);

    //下面是测试代码，测试frustum数据是否正确
    //eyeFrustum.CalculateFrustum(pSceneCamera->GetViewMatrix());
    //Vector3Df frustumPntWorld[8];
    //memcpy(frustumPntWorld, eyeFrustum.GetWorldSpaceCorners(), sizeof(Vector3Df) * 8);
    //for (int i = 0; i < 8; ++i)
    //{
    //    pSceneCamera->GetViewMatrix().TransformCoord(frustumPntWorld[i], frustumPntEye[i]);
    //}
    //-----测试代码end--------
    for ( int i = 0; i < 4; i++ )
    {
        frustumPnts[i] = frustumPntEye[(i << 1)];       // far plane
        frustumPnts[i + 4] = frustumPntEye[(i << 1) | 0x1]; // near plane
        
    }

    //   we need to transform the eye into the light's post-projective space.
    //   however, the sun is a directional light, so we first need to find an appropriate
    //   rotate/translate matrix, before constructing an ortho projection.
    //   this matrix is a variant of "light space" from LSPSMs, with the Y and Z axes permuted

    Vector3Df leftVector, upVector, viewVector;
    const Vector3Df eyeVector( 0.f, 0.f, -1.f );  //  eye is always -Z in eye space

    //  code copied straight from BuildLSPSMProjectionMatrix
    pSceneCamera->GetViewMatrix().TransformNormal(upVector, -m_directionLight);
    upVector.Normalize();
    leftVector = upVector.CrossProduct(eyeVector);
    leftVector.Normalize();
    viewVector = upVector.CrossProduct(leftVector);
    //D3DXVec3TransformNormal( &upVector, &m_lightDir, &m_View );  // lightDir is defined in eye space, so xform it
    //D3DXVec3Cross( &leftVector, &upVector, &eyeVector );
    //D3DXVec3Normalize( &leftVector, &leftVector );
    //D3DXVec3Cross( &viewVector, &upVector, &leftVector );

    Matrix4f lightSpaceBasis;  
    lightSpaceBasis[0] = leftVector.x; lightSpaceBasis[1] = viewVector.x; lightSpaceBasis[2] = -upVector.x; lightSpaceBasis[3] = 0.f;
    lightSpaceBasis[4] = leftVector.y; lightSpaceBasis[5] = viewVector.y; lightSpaceBasis[6] = -upVector.y; lightSpaceBasis[7] = 0.f;
    lightSpaceBasis[8] = leftVector.z; lightSpaceBasis[9] = viewVector.z; lightSpaceBasis[10] = -upVector.z; lightSpaceBasis[11] = 0.f;
    lightSpaceBasis[12] = 0.f;          lightSpaceBasis[13] = 0.f;          lightSpaceBasis[14] = 0.f;        lightSpaceBasis[15] = 1.f;

    //  rotate the view frustum into light space
    //D3DXVec3TransformCoordArray( frustumPnts, sizeof(D3DXVECTOR3), frustumPnts, sizeof(D3DXVECTOR3), &lightSpaceBasis, sizeof(frustumPnts)/sizeof(D3DXVECTOR3) );
    for (int i = 0; i < 8; ++i)
    {
        lightSpaceBasis.TransformCoord(frustumPnts[i], frustumPnts[i]);
    }

    //  build an off-center ortho projection that translates and scales the eye frustum's 3D AABB to the unit cube
    //BoundingBox frustumBox( frustumPnts, sizeof(frustumPnts) / sizeof(D3DXVECTOR3) );
    AABBBox3Df frustumBox;
    frustumBox.MaxEdge = Vector3Df(-100000, -100000, -100000);
    frustumBox.MinEdge = Vector3Df(100000, 100000, 100000);

    for (int i = 0; i < 8; ++i)
    {
        frustumBox.AddInternalPoint(frustumPnts[i]);
    }
    
    AIRVector<Vector3Df> boxCasterPoints;
    size_t nCasterCount = m_lstShadowCasters.size();
    boxCasterPoints.resize(nCasterCount * 8);
    for (size_t i = 0; i < nCasterCount; ++i)
    {
        for (int j = 0; j < 8; ++j)
        {
            boxCasterPoints[i * 8 + j] = m_lstShadowCasters[i].Point(j);
        }
    }

    for (size_t i = 0; i < nCasterCount * 8; ++i)
    {
        lightSpaceBasis.TransformCoord(boxCasterPoints[i], boxCasterPoints[i]);
    }

    AABBBox3Df boxCaster;
    boxCaster.MinEdge = Vector3Df(10000, 10000, 10000);
    boxCaster.MaxEdge = Vector3Df(-10000, -10000, -10000);

    for (size_t i = 0; i < nCasterCount * 8; ++i)
    {
        boxCaster.AddInternalPoint(boxCasterPoints[i]);
    }

    float min_z = min( boxCaster.MinEdge.z, frustumBox.MinEdge.z );
    float max_z = max( boxCaster.MaxEdge.z, frustumBox.MaxEdge.z );

    if ( min_z <= 0.f )
    {
        Matrix4f lightSpaceTranslate = Matrix4f::IDENTITY;
        lightSpaceTranslate.SetTranslation( 0.f, 0.f, -min_z + 1.f );
        max_z = -min_z + max_z + 1.f;
        min_z = 1.f;
        lightSpaceBasis = lightSpaceBasis * lightSpaceTranslate;
        //D3DXVec3TransformCoordArray( frustumPnts, sizeof(D3DXVECTOR3), frustumPnts, sizeof(D3DXVECTOR3), &lightSpaceTranslate, sizeof(frustumPnts)/sizeof(D3DXVECTOR3) );
        //frustumBox = BoundingBox( frustumPnts, sizeof(frustumPnts)/sizeof(D3DXVECTOR3) );
        for (size_t i = 0; i < 8; ++i)
        {
            lightSpaceTranslate.TransformCoord(frustumPnts[i], frustumPnts[i]);
        }

        frustumBox.MinEdge = Vector3Df(100000, 100000, 100000);
        frustumBox.MaxEdge = Vector3Df(-100000, -100000, -100000);
        for (size_t i = 0; i < 8; ++i)
        {
            frustumBox.AddInternalPoint(frustumPnts[i]);
        }
    }

    Matrix4f lightSpaceOrtho;
    //D3DXMatrixOrthoOffCenterLH( &lightSpaceOrtho, frustumBox.minPt.x, frustumBox.maxPt.x, frustumBox.minPt.y, frustumBox.maxPt.y, min_z, max_z );
    lightSpaceOrtho.BuildProjectionMatrixOrthoOffCenterLH(frustumBox.MinEdge.x, frustumBox.MaxEdge.x, frustumBox.MinEdge.y, frustumBox.MaxEdge.y, min_z, max_z);

    //  transform the view frustum by the new matrix
    //D3DXVec3TransformCoordArray( frustumPnts, sizeof(D3DXVECTOR3), frustumPnts, sizeof(D3DXVECTOR3), &lightSpaceOrtho, sizeof(frustumPnts)/sizeof(D3DXVECTOR3) );
    for (int i = 0; i < 8; ++i)
    {
        lightSpaceOrtho.TransformCoord(frustumPnts[i], frustumPnts[i]);
    }

    Vector2Df centerPts[2];
    //  near plane
    centerPts[0].x = 0.25f * (frustumPnts[4].x + frustumPnts[5].x + frustumPnts[6].x + frustumPnts[7].x);
    centerPts[0].y = 0.25f * (frustumPnts[4].y + frustumPnts[5].y + frustumPnts[6].y + frustumPnts[7].y);
    //  far plane
    centerPts[1].x = 0.25f * (frustumPnts[0].x + frustumPnts[1].x + frustumPnts[2].x + frustumPnts[3].x);
    centerPts[1].y = 0.25f * (frustumPnts[0].y + frustumPnts[1].y + frustumPnts[2].y + frustumPnts[3].y);

    Vector2Df centerOrig = (centerPts[0] + centerPts[1]) * 0.5f;

    Matrix4f trapezoid_space;

    Matrix4f xlate_center( 1.f, 0.f, 0.f, 0.f,
        0.f,           1.f, 0.f, 0.f,
        0.f,           0.f, 1.f, 0.f,
        -centerOrig.x, -centerOrig.y, 0.f, 1.f );

    float half_center_len = Vector2Df(centerPts[1] - centerOrig).GetLength();
    float x_len = centerPts[1].x - centerOrig.x;
    float y_len = centerPts[1].y - centerOrig.y;

    float cos_theta = x_len / half_center_len;
    float sin_theta = y_len / half_center_len;

    Matrix4f rot_center( cos_theta, -sin_theta, 0.f, 0.f,
        sin_theta,  cos_theta, 0.f, 0.f,
        0.f,        0.f, 1.f, 0.f,
        0.f,        0.f, 0.f, 1.f );

    //--------------
    //  this matrix transforms the center line to y=0.
    //  since Top and Base are orthogonal to Center, we can skip computing the convex hull, and instead
    //  just find the view frustum X-axis extrema.  The most negative is Top, the most positive is Base
    //  Point Q (trapezoid projection point) will be a point on the y=0 line.
    trapezoid_space = xlate_center * rot_center;
    for (int i = 0; i < 8; ++i)
    {
        trapezoid_space.TransformCoord(frustumPnts[i], frustumPnts[i]);
    }

    AABBBox3Df frustumAABB2D;
    frustumAABB2D.MinEdge = Vector3Df(10000, 10000, 10000);
    frustumAABB2D.MaxEdge = Vector3Df(-10000, -10000, -10000);
    for (int i = 0; i < 8; ++i)
    {
        frustumAABB2D.AddInternalPoint(frustumPnts[i]);
    }

    float x_scale = max( fabsf(frustumAABB2D.MaxEdge.x), fabsf(frustumAABB2D.MinEdge.x) );
    float y_scale = max( fabsf(frustumAABB2D.MaxEdge.y), fabsf(frustumAABB2D.MinEdge.y) );
    x_scale = 1.f / x_scale;
    y_scale = 1.f / y_scale;

    //  maximize the area occupied by the bounding box
    Matrix4f scale_center( x_scale, 0.f, 0.f, 0.f,
        0.f, y_scale, 0.f, 0.f,
        0.f,     0.f, 1.f, 0.f,
        0.f,     0.f, 0.f, 1.f );

    trapezoid_space = trapezoid_space * scale_center;

    //  scale the frustum AABB up by these amounts (keep all values in the same space)
    frustumAABB2D.MinEdge.x *= x_scale;
    frustumAABB2D.MaxEdge.x *= x_scale;
    frustumAABB2D.MinEdge.y *= y_scale;
    frustumAABB2D.MaxEdge.y *= y_scale;

    //  compute eta.
    float lambda = frustumAABB2D.MaxEdge.x - frustumAABB2D.MinEdge.x;
    float delta_proj = m_fTSM_Delta * lambda; //focusPt.x - frustumAABB2D.minPt.x;

    const float xi = -0.6f;  // 80% line

    float eta = (lambda * delta_proj * (1.f + xi)) / (lambda * (1.f - xi) - 2.f * delta_proj);

    //  compute the projection point a distance eta from the top line.  this point is on the center line, y=0
    Vector2Df projectionPtQ( frustumAABB2D.MaxEdge.x + eta, 0.f );

    //  find the maximum slope from the projection point to any point in the frustum.  this will be the
    //  projection field-of-view
    float max_slope = -1e32f;
    float min_slope =  1e32f;

    for ( int i = 0; i < 8; i++ )
    {
        Vector2Df tmp( frustumPnts[i].x * x_scale, frustumPnts[i].y * y_scale );
        float x_dist = tmp.x - projectionPtQ.x;
        if ( !(iszero(tmp.y) || iszero(x_dist)))
        {
            max_slope = max(max_slope, tmp.y / x_dist);
            min_slope = min(min_slope, tmp.y / x_dist);
        }
    }

    float xn = eta;
    float xf = lambda + eta;

    Matrix4f ptQ_xlate(-1.f, 0.f, 0.f, 0.f,
        0.f, 1.f, 0.f, 0.f,
        0.f, 0.f, 1.f, 0.f,
        projectionPtQ.x, 0.f, 0.f, 1.f );
    trapezoid_space = trapezoid_space * ptQ_xlate;

    //  this shear balances the "trapezoid" around the y=0 axis (no change to the projection pt position)
    //  since we are redistributing the trapezoid, this affects the projection field of view (shear_amt)
    float shear_amt = (max_slope + fabsf(min_slope)) * 0.5f - max_slope;
    max_slope = max_slope + shear_amt;

    Matrix4f trapezoid_shear( 1.f, shear_amt, 0.f, 0.f,
        0.f,       1.f, 0.f, 0.f,
        0.f,       0.f, 1.f, 0.f,
        0.f,       0.f, 0.f, 1.f );

    trapezoid_space = trapezoid_space * trapezoid_shear;


    float z_aspect = (frustumBox.MaxEdge.z - frustumBox.MinEdge.z) / (frustumAABB2D.MaxEdge.y - frustumAABB2D.MinEdge.y);

    //  perform a 2DH projection to 'unsqueeze' the top line.
    Matrix4f trapezoid_projection(  xf / (xf - xn), 0.f, 0.f, 1.f,
        0.f, 1.f / max_slope, 0.f, 0.f,
        0.f,           0.f, 1.f / (z_aspect * max_slope), 0.f,
        -xn*xf / (xf - xn),           0.f, 0.f, 0.f );

    trapezoid_space = trapezoid_space * trapezoid_projection;

    //  the x axis is compressed to [0..1] as a result of the projection, so expand it to [-1,1]
    Matrix4f biasedScaleX( 2.f, 0.f, 0.f, 0.f,
        0.f, 1.f, 0.f, 0.f,
        0.f, 0.f, 1.f, 0.f,
        -1.f, 0.f, 0.f, 1.f );
    trapezoid_space = trapezoid_space * biasedScaleX;

    trapezoid_space = lightSpaceOrtho * trapezoid_space;
    trapezoid_space = lightSpaceBasis * trapezoid_space;

    // now, focus on shadow receivers.
    if ( m_bUnitClip )
    {
        //D3DXVECTOR3* shadowReceiverPnts = NULL;
        //shadowReceiverPnts = new D3DXVECTOR3[8*m_ShadowReceiverPoints.size()];
        AIRVector<Vector3Df> shadowReceiverPnts;
        //shadowReceiverPnts.resize(m_lstShadowReceivers.size() * 8)
        for (AIRList<AABBBox3Df>::iterator it = m_lstShadowReceivers.begin(); it != m_lstShadowReceivers.end(); ++it)
        {
            //(*it).GetEdges(&boxPoints[0]);
            for (size_t i = 0; i < 8; ++i)
            {
                shadowReceiverPnts.push_back((*it).Point(i));
            }
        }
        //for ( UINT i=0; i<m_ShadowReceiverPoints.size(); i++ )
        //{
        //    for ( int j=0; j<8; j++ ) shadowReceiverPnts[i*8+j] = m_ShadowReceiverPoints[i].Point(j);
        //}

        //D3DXVec3TransformCoordArray( shadowReceiverPnts, sizeof(D3DXVECTOR3), shadowReceiverPnts, sizeof(D3DXVECTOR3), &trapezoid_space, m_ShadowReceiverPoints.size()*8 );
        size_t nShadowReceiverNum = shadowReceiverPnts.size();
        for (size_t i = 0; i < nShadowReceiverNum; ++i)
        {
            trapezoid_space.TransformCoord(shadowReceiverPnts[i], shadowReceiverPnts[i]);
        }
        //BoundingBox rcvrBox( shadowReceiverPnts, m_ShadowReceiverPoints.size()*8 );
        //delete [] shadowReceiverPnts;

        AABBBox3Df rcvrBox;
        rcvrBox.MaxEdge = Vector3Df(-100000, -100000, -100000);
        rcvrBox.MinEdge = Vector3Df(100000, 100000, 100000);
        for (size_t i = 0; i < nShadowReceiverNum; ++i)
        {
            rcvrBox.AddInternalPoint(shadowReceiverPnts[i]);
        }
        //  never shrink the box, only expand it.
        rcvrBox.MaxEdge.x = Min( 1.f, rcvrBox.MaxEdge.x );
        rcvrBox.MinEdge.x = Max(-1.f, rcvrBox.MinEdge.x );
        rcvrBox.MaxEdge.y = Min( 1.f, rcvrBox.MaxEdge.y );
        rcvrBox.MinEdge.y = Max(-1.f, rcvrBox.MinEdge.y );
        float boxWidth  = rcvrBox.MaxEdge.x - rcvrBox.MinEdge.x;
        float boxHeight = rcvrBox.MaxEdge.y - rcvrBox.MinEdge.y;

        //  the receiver box is degenerate, this will generate specials (and there shouldn't be any shadows, anyway).
        if ( !(iszero(boxWidth) || iszero(boxHeight)) )
        {
            //  the divide by two's cancel out in the translation, but included for clarity
            float boxX = (rcvrBox.MaxEdge.x + rcvrBox.MinEdge.x) / 2.f;
            float boxY = (rcvrBox.MaxEdge.y + rcvrBox.MinEdge.y) / 2.f;
            Matrix4f trapezoidUnitCube( 2.f / boxWidth,                 0.f, 0.f, 0.f,
                0.f,       2.f / boxHeight, 0.f, 0.f,
                0.f,                 0.f, 1.f, 0.f,
                -2.f * boxX / boxWidth, -2.f * boxY / boxHeight, 0.f, 1.f );
            //D3DXMatrixMultiply( &trapezoid_space, &trapezoid_space, &trapezoidUnitCube );
            trapezoid_space = trapezoid_space * trapezoidUnitCube;
        }
    }

    //D3DXMatrixMultiply( &m_LightViewProj, &m_View, &trapezoid_space );
    m_matLightViewProj = pSceneCamera->GetViewMatrix() * trapezoid_space;
}

void Shadowmap::BuildOrthoShadowProjectionMatrix(const CFrustum& frustum, Matrix4f& matOut/*, CRenderQueue& renderQueue*/)
{
    //第一步，获取所有在可视范围内的渲染物 
    ISceneManager* pScene = ENGINE_INST->GetSceneMgr();
    CCamera* pSceneCamera = pScene->GetCamera();

    //ListIterator<LIST_SCENENODE> iter = pScene->GetVisibleSceneNodes();
    //CRenderQueue* pRenderQueue = pScene->GetRenderQueue();

    m_zNear = 1.0f;
    m_zFar = 10000.0f;
    m_fSlideBack = 0;
    //f32 fFar = 100.0f;

    //计算虚拟摄像机的far和near
    AABBBox3Df boundingInView;

    //AIRList<AABBBox3Df> boxList;
    m_lstShadowReceivers.clear();
    m_lstShadowCasters.clear();

    //ListIterator<LIST_SCENENODE> nodeIter = pScene->GetVisibleSceneNodes();
    /*
    MapIterator<ISceneManager::MAP_SCENENODE> nodeIter = pScene->GetMapItorSceneNode();
    while (!nodeIter.IsEnd())
    {
        CSceneNode* pNode = nodeIter.GetNext();
        ListIterator<CSceneNode::RenderableList> renderableIter = pNode->GetMapItorRenderable();
        while (!renderableIter.IsEnd())
        {
            Renderable* pRenderable = renderableIter.GetNext();
            boundingInView = pRenderable->GetWorldBoundingBox(true);
            //boundingInView.MaxEdge += boundingInView.GetHalfSize();
            //boundingInView.MinEdge -= boundingInView.GetHalfSize();
            if (frustum.IsAABBIn(boundingInView))
            {
                if (pRenderable->IsAcceptShadowMap() || pRenderable->IsRenderToShadow())
                {
                    boundingInView.MaxEdge += boundingInView.GetHalfSize() * 0.5f;
                    boundingInView.MinEdge -= boundingInView.GetHalfSize() * 0.5f;
                    CMathUtil::XFormBoundingBox(boundingInView, boundingInView, pSceneCamera->GetViewMatrix());

                    m_zNear = Min(m_zNear, boundingInView.MinEdge.z);
                    m_zFar = Max(m_zFar, boundingInView.MaxEdge.z);

                    m_lstShadowReceivers.push_back(boundingInView);

                    m_lstShadowCasters.push_back(boundingInView);

                    pRenderable->UpdateToRenderQueue(pSceneCamera, &renderQueue);

                }

            }
            else
            {
                if (pRenderable->IsRenderToShadow())
                {
                    f32 fRadius = boundingInView.GetRadius();
                    if (frustum.IsSphereIn(boundingInView.GetCenter(), fRadius * 1.5f))
                    {
                        pRenderable->UpdateToRenderQueue(pSceneCamera, &renderQueue);

                        //pSceneCamera->GetViewMatrix().TransformBox(boundingInView);
                        CMathUtil::XFormBoundingBox(boundingInView, boundingInView, pSceneCamera->GetViewMatrix());

                        m_zNear = Min(m_zNear, boundingInView.MinEdge.z);
                        m_zFar = Max(m_zFar, boundingInView.MaxEdge.z);
                        m_lstShadowReceivers.push_back(boundingInView);
                        m_lstShadowCasters.push_back(boundingInView);
                    }
                }
            }
        }
    }
    */

    //地形boundingbox

    const Vector3Df& camLookAt = pSceneCamera->GetLookAt();
    boundingInView.MaxEdge = camLookAt;
    boundingInView.MinEdge = camLookAt;
    CHeightMap* pTerrain = TERRAIN->GetCurHeightmap();
    //f32 fW = pSceneCamera->GetDistance() < pTerrain->GetBlockWidth() / 16 ? 40 : 20;
    boundingInView.MaxEdge.x += m_fShadowRangeRadius; //pTerrain->GetBlockWidth() / fW;
    boundingInView.MaxEdge.z += m_fShadowRangeRadius; //pTerrain->GetBlockWidth() / fW;
    //boundingInView.MaxEdge.y += pTerrain->GetBlockWidth() / 200;
    //boundingInView.MinEdge = camLookAt;
    boundingInView.MinEdge.x -= m_fShadowRangeRadius; //pTerrain->GetBlockWidth() / fW;
    boundingInView.MinEdge.z -= m_fShadowRangeRadius;  //pTerrain->GetBlockWidth() / fW;
    //boundingInView.MinEdge.y -= pTerrain->GetBlockWidth() / 100;
    //pSceneCamera->GetViewMatrix().TransformBox(boundingInView);
    CMathUtil::XFormBoundingBox(boundingInView, boundingInView, pSceneCamera->GetViewMatrix());
    m_zNear = Min(m_zNear, boundingInView.MinEdge.z);
    m_zFar = Max(m_zFar, boundingInView.MaxEdge.z);

    m_lstShadowReceivers.push_back(boundingInView);


    Matrix4f lightView, lightProj;
    //const Vector3Df zAxis(0.f, 0.f, 1.f);
    //const Vector3Df yAxis(0.f, 1.f, 0.f);
    Vector3Df eyeLightDir;

    //D3DXVec3TransformNormal(&eyeLightDir, &m_lightDir, &m_View);
    pSceneCamera->GetViewMatrix().TransformNormal(eyeLightDir, -m_directionLight);

    float fHeight = AIR::PI / 3; //D3DXToRadian(60.f);
    f32 fAspect = (f32)RENDERER->GetRenderTargetWidth() / RENDERER->GetRenderTargetHeight();
    float fWidth = fAspect * fHeight;

    AABBBox3Df frustumAABB(99999, 99999, 99999, -99999, -99999, -99999);
    if ( true )
    {
        //frustumAABB = BoundingBox( &m_ShadowReceiverPoints );
        //AIRVector<Vector3Df, CMemorySTLAllocator<AABBBox3Df, CMemoryNedAlloc>> shadowReceiverPnts;
        //shadowReceiverPnts.resize(m_lstShadowReceivers.size() * 8)
        for (AIRList<AABBBox3Df>::iterator it = m_lstShadowReceivers.begin(); it != m_lstShadowReceivers.end(); ++it)
        {
            //(*it).GetEdges(&boxPoints[0]);
            //for (size_t i = 0; i < 8; ++i)
            //{
            //    frustumAABB.AddInternalPoint((*it).Point(i));
            //}
            frustumAABB.AddInternalBox(*it);
        }
    }
    else
    {
        //frustumAABB.minPt = D3DXVECTOR3(-fWidth*ZFAR_MAX, -fHeight*ZFAR_MAX, ZNEAR_MIN);
        //frustumAABB.maxPt = D3DXVECTOR3( fWidth*ZFAR_MAX,  fHeight*ZFAR_MAX, ZFAR_MAX);
    }

    //  light pt is "infinitely" far away from the view frustum.
    //  however, all that's really needed is to place it just outside of all shadow casters

    AABBBox3Df casterAABB(99999, 99999, 99999, -99999, -99999, -99999);   //( &m_ShadowCasterPoints );
    for (size_t i = 0; i < m_lstShadowCasters.size(); ++i)
    {
        casterAABB.AddInternalBox(m_lstShadowCasters[i]);
    }
    Vector3Df frustumCenter = frustumAABB.GetCenter();
    //frustumAABB.Centroid( &frustumCenter );
    float t = 0;
    casterAABB.Intersect( t, frustumCenter, eyeLightDir );

    Vector3Df lightPt = frustumCenter + 2.f * t * eyeLightDir;
    Vector3Df axis;

    if ( fabsf(eyeLightDir.DotProduct(Vector3Df::UNIT_Y)) > 0.99f )
        axis = Vector3Df::UNIT_Z;
    else
        axis = Vector3Df::UNIT_Y;

    //D3DXMatrixLookAtLH( &lightView, &lightPt, &frustumCenter, &axis );
    lightView.BuildCameraLookAtMatrixLH(lightPt, frustumCenter, axis);

    CMathUtil::XFormBoundingBox( frustumAABB, frustumAABB, lightView );
    CMathUtil::XFormBoundingBox( casterAABB,  casterAABB,  lightView );

    //  use a small fudge factor for the near plane, to avoid some minor clipping artifacts
    //D3DXMatrixOrthoOffCenterLH( &lightProj, frustumAABB.minPt.x, frustumAABB.maxPt.x,
    //    frustumAABB.minPt.y, frustumAABB.maxPt.y,
    //    casterAABB.minPt.z, frustumAABB.maxPt.z );
    lightProj.BuildProjectionMatrixOrthoOffCenterLH(frustumAABB.MinEdge.x, frustumAABB.MaxEdge.x,
            frustumAABB.MinEdge.y, frustumAABB.MaxEdge.y,
            casterAABB.MinEdge.z, frustumAABB.MaxEdge.z );

    //D3DXMatrixMultiply( &lightView, &m_View, &lightView );
    //D3DXMatrixMultiply( &m_LightViewProj, &lightView, &lightProj );
    //CCamera* pSceneCamera = ENGINE_INST->GetSceneMgr()->GetCamera();
    lightView = pSceneCamera->GetViewMatrix() * lightView;
    matOut = lightView * lightProj;
}

void Shadowmap::UpdateCSMData()
{
    //首先划分frustum
    ISceneManager* pScene = ENGINE_INST->GetSceneMgr();
    CCamera* pSceneCamera = pScene->GetCamera();

    //计算每个frustum对应的matLightViewProj;
    const CFrustum* pFrustum = pSceneCamera->GetFrustum();
    f32 fNear = pFrustum->GetNear();
    f32 fFar = pFrustum->GetFar();
    f32 fDist = fFar - fNear;
    for (int i = 0; i < m_nSplitNum; ++i)
    {
        if (i == m_nSplitNum - 1)
        {
            fFar = pSceneCamera->GetViewFar();
        }
        else
        {
            fFar = fNear + 0.1f * (i + 1) * fDist;
        }
        m_fFars[i] = fFar;
        m_frustumSplit[i].SetPerspective(fNear, fFar, pSceneCamera->GetViewFOV(), pSceneCamera->GetViewWidth(), pSceneCamera->GetViewHeight());
        m_frustumSplit[i].CalculateFrustum(pSceneCamera->GetViewMatrix());
        fNear = fFar;
    }

    //m_pGPUShadowmapSource->SetFrustumFars(m_fFars);
}

void Shadowmap::RenderCascadeShadowmap()
{
    IRenderer* pRenderer = ENGINE_INST->GetRenderer();
    Matrix4f matLightViewProj;
    for (int i = 0; i < m_nSplitNum; ++i)
    {
        //m_renderQueue.Clear();
        BuildOrthoShadowProjectionMatrix(m_frustumSplit[i], matLightViewProj);
        //RenderQueueGroup* pGroup = m_renderQueue.GetRenderQueueGroup(RenderGroup_Type::eRGT_Solid);
        //ListIterator<RenderQueueGroup::RenderableList> iter = pGroup->GetRenderableListItorObject();
        
        switch (i)
        {
        case 0:
            pRenderer->SetViewPort(0, 0, 1024, 1024);
            break;
        case 1:
            pRenderer->SetViewPort(1024, 0, 1024, 1024);
            break;
        case 2:
            pRenderer->SetViewPort(0, 1024, 1024, 1024);
            break;
        case 3:
            pRenderer->SetViewPort(1024, 1024, 1024, 1024);
            break;
        }
        //m_pGPUShadowmapSource->SetLightViewProjMatrixArray(i, matLightViewProj * m_matR);

        /*
        while (!iter.IsEnd())
        {
            Renderable* pRenderable = iter.GetNext();

            if (pRenderable->IsRenderToShadow())
            {
                //pRenderer->GetGPUDataParam()->SetWorldViewProjectMatrix(pRenderable->GetWorldMatrix() * m_pGPUShadowmapSource->GetLightViewProjMatrix(i));
                pRenderable->RenderToShadowMap(m_pCamera);
            }
        }
        */
    }
}

void Shadowmap::SetShadowmapType(ShadowmapType type)
{
    if (type == SMT_CSM && m_type != type)
    {
        Initialize(2048, 2048);
    }
    m_type = type;
}

void Shadowmap::GetAABBFromProjectMatrix(AABBBox3Df& aabb, const Matrix4f& matProj)
{
    Matrix4f matInv;
    matProj.GetInverse(matInv);

    Vector3Df v[8];
    static const Vector3Df v0(-1.0f, -1.0f, 0.0f);
    static const Vector3Df v1(-1.0f, 1.0f, 0.0f);
    static const Vector3Df v2(1.0f, -1.0f, 0.0f);
    static const Vector3Df v3(1.0f, 1.0f, 0.0f);
    static const Vector3Df v4(-1.0f, -1.0f, 1.0f);
    static const Vector3Df v5(-1.0f, 1.0f, 1.0f);
    static const Vector3Df v6(1.0f, -1.0f, 1.0f);
    static const Vector3Df v7(1.0f, 1.0f, 1.0f);

    matInv.TransformCoord(v0, v[0]);
    matInv.TransformCoord(v1, v[1]);
    matInv.TransformCoord(v2, v[2]);
    matInv.TransformCoord(v3, v[3]);
    matInv.TransformCoord(v4, v[4]);
    matInv.TransformCoord(v5, v[5]);
    matInv.TransformCoord(v6, v[6]);
    matInv.TransformCoord(v7, v[7]);

    //Vector3Df vPosMin = v[0];
    //Vector3Df vPosMax = v[0];
    aabb.Reset(Vector3Df(999999, 999999, 999999), Vector3Df(-999999, -999999, -999999));
    for (int k = 0; k < 8; k++)
    {

        aabb.AddInternalPoint(v[k]);
    }
    

}

