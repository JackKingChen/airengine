#include "stdafx.h"
#include "Water.h"
#include "Engine.h"
#include "IRenderer.h"
#include "SceneNode.h"
#include "MaterialManager.h"
#include "ISceneManager.h"
//#include "PostScreenCompositor.h"
#include "Terrain.h"
#include "HeightMap.h"
//#include "PostProcessManager.h"
#include "Primitive.h"
//#include "TextureUnit.h"
//#include "Technique.h"
#include "TextureMgr.h"
#include "LightingManager.h"
#include "NormalSceneRenderProcess.h"
#include "RenderTarget.h"

void ReflectProcessUpdateFunc(CustomRenderProcess* pProcess)
{
    NormalSceneRenderProcess* pNormalProcess = static_cast<NormalSceneRenderProcess*>(pProcess);

    CCamera* pCamera = pNormalProcess->GetCamera();

    if (pCamera == NULL)
    {
        CCamera* pReflectCamera = new CCamera(Vector3Df(0, 150, 250), Vector3Df(0, 100, 0));
        pNormalProcess->SetCamera(pReflectCamera, true);
        TextureRenderTarget* pRTT = pNormalProcess->GetRenderTarget();
        pReflectCamera->SetViewport(1.0f, 5000.0f, AIR::PI / 4, pRTT->GetWidth(), pRTT->GetHeight(), true);
        pReflectCamera->SetNeedAutoUpdate( false );
    }
    
    pCamera = pNormalProcess->GetCamera();

    if (pCamera)
    {
        CCamera* pSceneCamera = ENGINE_INST->GetSceneMgr()->GetCamera();
        TextureRenderTarget* pRTT = pNormalProcess->GetRenderTarget();
        pCamera->SetFarPlane( pSceneCamera->GetViewFar() );
        pCamera->SetViewport(pRTT->GetWidth(), pRTT->GetHeight(), pSceneCamera->GetIsPerspectProj());
        pCamera->SetUserClipViewFar( pSceneCamera->GetUserClipViewFar() );
        pSceneCamera->BuildReflectCamara( pNormalProcess->GetClipPlane(0), pCamera );

        //pCamera->Update();
    }

    RENDERER->SetTexture(1, NULL);
}

void RefractProcessUpdateFunc(CustomRenderProcess* pProcess)
{
    NormalSceneRenderProcess* pNormalProcess = static_cast<NormalSceneRenderProcess*>(pProcess);

    CCamera* pCamera = pNormalProcess->GetCamera();

    if (pCamera == NULL)
    {
        CCamera* pRefractCamera = new CCamera(Vector3Df(0, 150, 250), Vector3Df(0, 100, 0));
        pNormalProcess->SetCamera(pRefractCamera, true);
        TextureRenderTarget* pRTT = pNormalProcess->GetRenderTarget();
        pRefractCamera->SetViewport(1.0f, 5000.0f, AIR::PI / 4, pRTT->GetWidth(), pRTT->GetHeight(), true);
        pRefractCamera->SetNeedAutoUpdate(false);
    }

    pCamera = pNormalProcess->GetCamera();

    if (pCamera)
    {
        TextureRenderTarget* pRTT = pNormalProcess->GetRenderTarget();
        CCamera* pSceneCamera = ENGINE_INST->GetSceneMgr()->GetCamera();
        pCamera->SetFarPlane( pSceneCamera->GetViewFar() );
        pCamera->SetPosition( pSceneCamera->GetPosition() );
        pCamera->SetLookAt( pSceneCamera->GetLookAt() );
        pCamera->SetViewport(pRTT->GetWidth(), pRTT->GetHeight(), pSceneCamera->GetIsPerspectProj());
        pCamera->SetUserClipViewFar( pSceneCamera->GetUserClipViewFar() );
        pCamera->RecalculateByPositionAndLookAt();

        //pCamera->Update();
    }

    RENDERER->SetTexture(2, NULL);
}


CWater::CWater() : m_fogColorUnderWater(CColorValue::White)
,m_fogModeUnderWater(eFOG_LINEAR)
,m_fDensityUnderWater(1.0f)
,m_fFogStartUnderWater(200.0f)
,m_fFogEndUnderWater(2000.0f)
, m_nGridCountX(1)
, m_nGridCountZ(1)
, m_fGridWidthX(100)
, m_fGridWidthZ(100)
, m_fDepth(0)
, m_fHeight(100)
, m_fWidth(100)
, m_pRTTReflect(NULL)
, m_pRTTRefract(NULL)
, m_clrWater(0xffffffff)
, m_fUScale(1)
, m_fVScale(1)
, m_bUnderWater(false)
, m_fWaveDistorsion(0.02f)
, m_vWaveDirection(1, -1)
, m_fFlowSpeed(0.02f)
, m_bIsCalByHeightMap(true)
, m_eWaterCmdType(eWCT_HasReflectHasRefractNoSoftEdge)
, m_fShiness(100)
, m_fGradient(100)
, m_pRTTDepthMap(NULL)
, m_pBumpTexture(NULL)
//, m_pSamplerReflect(NULL)
//, m_pSamplerRefract(NULL)
, m_pDepthMapProcess(NULL)
, m_pWaterReflectProcess(NULL)
, m_pWaterRefactProcess(NULL)
//, m_pSamplerBump(NULL)
, m_pRTTDepthStencil(NULL)
{
	//m_pGPUDataParmSource = static_cast<WaterManualGPUDataParamSource*>(CREATE_MANUAL_GPU_PARAM_SOURCE(WaterManualGPUDataParamSource));
	//m_vWaveDirection.Set( 1, -1 );
	m_vWaveDirection.Normalize();
	m_szType = "water";
	m_strVertexDecl = "E_VB_STANDARD";
}

CWater::~CWater( void )
{
	//Clear();
}

void CWater::Clear()
{
    if (m_pBumpTexture)
    {
        TEXTURE_MANAGER->ClearTexture(m_pBumpTexture);
        m_pBumpTexture = NULL;
    }
	Renderable::Clear();
	ClearWaterRTT();
	ClearUnderWaterCompositor();

	//SafeDelete(m_pSamplerReflect);
	//SafeDelete(m_pSamplerRefract);
 //   SafeDelete(m_pSamplerBump);
	//m_pGPUDataParmSource = NULL;
}

// 清除水体RTT
void CWater::ClearWaterRTT( void )
{
    IRenderer* pRenderer = ENGINE_INST->GetRenderer();
	if (m_pRTTReflect)
	{
		//pRenderer->DestroyTextureRenderTarget( m_pRTTReflect );
		m_pRTTReflect->Clear();
		SafeDelete(m_pRTTReflect);
		//m_pRTTReflect = NULL;
	}

	if (m_pRTTRefract)
	{
		//pRenderer->DestroyTextureRenderTarget( m_pRTTRefract );
		m_pRTTRefract->Clear();
		m_pRTTRefract = NULL;
	}	

	if (m_pRTTDepthMap)
	{
		//pRenderer->DestroyTextureRenderTarget( m_pRTTDepthMap );
		//m_pRTTDepthMap = NULL;
		m_pRTTDepthMap->Clear();
		SafeDelete(m_pRTTDepthMap);
	}

	if (m_pDepthMapProcess)
	{
		//ENGINE_INST->RemoveCustomRenderProcess(m_pDepthMapProcess);
		SafeDelete(m_pDepthMapProcess);
	}

	if (m_pWaterRefactProcess)
	{
        //ENGINE_INST->RemoveCustomRenderProcess(m_pWaterRefactProcess);
		SafeDelete(m_pWaterRefactProcess);
	}

	if (m_pWaterReflectProcess)
	{
        //ENGINE_INST->RemoveCustomRenderProcess(m_pWaterReflectProcess);
		SafeDelete(m_pWaterReflectProcess);
	}

    if (m_pRTTDepthStencil)
    {
        pRenderer->DestroyDepthStencil(m_pRTTDepthStencil);
        m_pRTTDepthStencil = NULL;
    }
}

bool CWater::CreateWaterSuface( int nGridCountX, int nGridCountZ, float fGridScaleX, float fGridScaleZ, Vector3Df& vWaterSufaceCenterPos )
{
	m_nGridCountX = nGridCountX;
	m_nGridCountZ = nGridCountZ;
	m_fGridWidthX = fGridScaleX;
	m_fGridWidthZ = fGridScaleZ;
	m_fWidth	= m_nGridCountX * m_fGridWidthX;
	m_fHeight	= m_nGridCountZ * m_fGridWidthZ;
	m_vCenter	= vWaterSufaceCenterPos;
	m_fDepth	= m_vCenter.y;
	RebuildRenderData( true );
	return true;
}

void CWater::InitDefault( void )
{
	if ( m_pMaterial )
	{
		MATERIAL_MANAGER->ClearMaterial( m_pMaterial );
		m_pMaterial = NULL;
	}
	m_pMaterial = MATERIAL_MANAGER->CreateMaterial( "water_material.xml/WaterEffect" );

	if (m_pMaterial->IsReady())
	{
		AutoSelectMaterial();
	}
	
}

bool CWater::CreateWaterRenderTarget( int nTargetWidth, int nTargetHeight )
{
    /*
	m_pRTTReflect = new RenderTargetReflect(this);
	m_pRTTReflect->SetName( (m_strName + "_RTTReflect") );
	m_pRTTReflect->Initialize( nTargetWidth, nTargetHeight );
	m_pRTTReflect->SetReflectPlane( Plane3Df( m_vCenter,Vector3Df::UNIT_Y ) );
	m_pRTTReflect->SetBackgroundColor( SColor(0x0) );
	m_pRTTReflect->SetIsWorkOnCurFrame( true );
	//m_pRTTReflect->AddRTTOwner( this );
	m_pRTTReflect->SetRTTResetOperation( true, 0.5f );
	//ENGINE_INST->AddRenderTarget( m_pRTTReflect );

	m_pRTTRefract = new RenderTargetRefract(this);
	m_pRTTReflect->SetName( (m_strName + "_RTTRefract") );
	m_pRTTRefract->Initialize( nTargetWidth, nTargetHeight );
	m_pRTTRefract->SetRefractPlane( Plane3Df( m_vCenter,Vector3Df::NEGATIVE_UNIT_Y ) );
	m_pRTTRefract->SetBackgroundColor( SColor(0x0) );
	m_pRTTRefract->SetIsWorkOnCurFrame( true );
	m_pRTTRefract->SetRTTResetOperation( true, 0.5f );
    */

    IRenderer* pRenderer = ENGINE_INST->GetRenderer();

	ClearWaterRTT();

    //if (m_pRTTReflect)
    //{
    //    pRenderer->DestroyTextureRenderTarget(m_pRTTReflect);
    //}
    m_pRTTReflect = TextureRenderTarget::CreateRenderTarget((m_strName + "_RTTReflect").c_str(), nTargetWidth, nTargetHeight, RPF_A8R8G8B8);//pRenderer->CreateTextureRenderTarget((m_strName + "_RTTReflect").c_str(), nTargetWidth, nTargetHeight, RPF_A8R8G8B8);

    //if (m_pRTTRefract)
    //{
    //    pRenderer->DestroyTextureRenderTarget(m_pRTTRefract);
    //}
    m_pRTTRefract = TextureRenderTarget::CreateRenderTarget((m_strName + "_RTTRefract").c_str(), nTargetWidth, nTargetHeight, RPF_A8R8G8B8);//pRenderer->CreateTextureRenderTarget((m_strName + "_RTTRefract").c_str(), nTargetWidth, nTargetHeight, RPF_A8R8G8B8);

	if (m_pWaterReflectProcess == 0)
	{
		m_pWaterReflectProcess = new NormalSceneRenderProcess(FLOW_REFLECTION);
		static_cast<NormalSceneRenderProcess*>(m_pWaterReflectProcess)->AddClipPlane(Plane3Df( m_vCenter,Vector3Df::UNIT_Y ));
        m_pWaterReflectProcess->AttachRenderTarget(m_pRTTReflect);
        m_pWaterReflectProcess->SetProcessUpdateCallBack(&ReflectProcessUpdateFunc);
	}

	if (m_pWaterRefactProcess == 0)
	{
		m_pWaterRefactProcess = new NormalSceneRenderProcess(FLOW_REFLECTION);
		static_cast<NormalSceneRenderProcess*>(m_pWaterRefactProcess)->AddClipPlane(Plane3Df( m_vCenter,Vector3Df::NEGATIVE_UNIT_Y ));
        m_pWaterRefactProcess->AttachRenderTarget(m_pRTTRefract);
        m_pWaterRefactProcess->SetProcessUpdateCallBack(&RefractProcessUpdateFunc);
        //m_pWaterRefactProcess->SetCamera(ENGINE_INST->GetSceneMgr()->GetCamera(), false);
	}

    if (m_pRTTDepthStencil == NULL)
    {
        m_pRTTDepthStencil = pRenderer->CreateDepthStencil(nTargetWidth, nTargetHeight, RPF_D24S8, false);
        m_pWaterReflectProcess->SetDepthStencil(m_pRTTDepthStencil);
        m_pWaterRefactProcess->SetDepthStencil(m_pRTTDepthStencil);
    }

	return true;
}

void CWater::SetSize( f32 fWidth, f32 fHeight )
{
	m_fGridWidthX = fWidth / m_nGridCountX;
	m_fGridWidthZ = fHeight / m_nGridCountZ;
	m_fWidth = fWidth;
	m_fHeight = fHeight;
	RebuildRenderData( false );
}

void CWater::SetGridCount( int nCountX, int nCountZ )
{
	m_nGridCountX = nCountX;
	m_nGridCountZ = nCountZ;
	m_fWidth = m_nGridCountX * m_fGridWidthX;
	m_fHeight = m_nGridCountZ * m_fGridWidthZ;
	RebuildRenderData( true );
}

void CWater::SetGridScale( float fScaleX, float fScaleZ )
{
	m_fGridWidthX = fScaleX;
	m_fGridWidthZ = fScaleZ;
	m_fWidth = m_nGridCountX * m_fGridWidthX;
	m_fHeight = m_nGridCountZ * m_fGridWidthZ;
	RebuildRenderData( false );
}

void CWater::SetDepth( f32 fDepth )
{
	m_vCenter.y = fDepth;
	if ( m_pNode )
	{
		m_pNode->SetPosition( m_vCenter );
	}
}

void CWater::SetWaterSufaceCenterPos( Vector3Df& vPos )
{
	m_vCenter = vPos;

	if ( m_pWaterReflectProcess )
	{
		//m_pRTTReflect->SetReflectPlane( Plane3Df( m_vCenter,Vector3Df::UNIT_Y ) );
        m_pWaterReflectProcess->ClearClipPlanes();
        m_pWaterReflectProcess->AddClipPlane(Plane3Df( m_vCenter,Vector3Df::UNIT_Y ));
	}

	if ( m_pWaterRefactProcess )
	{
		//m_pRTTRefract->SetRefractPlane( Plane3Df( m_vCenter,Vector3Df::NEGATIVE_UNIT_Y ) );
        m_pWaterRefactProcess->ClearClipPlanes();
        m_pWaterRefactProcess->AddClipPlane(Plane3Df( m_vCenter,Vector3Df::NEGATIVE_UNIT_Y ));
	}
}

void CWater::SetUseReflect( bool bUseReflect )
{
	if (m_pRTTReflect)
	{
		//m_pRTTReflect->SetIsWorkOnCurFrame(bUseReflect);
	}
}

void CWater::SetUseRefract( bool bUseRefract )
{
	if (m_pRTTRefract)
	{
		//m_pRTTRefract->SetIsWorkOnCurFrame(bUseRefract);
	}
}

/*
bool CWater::GetUseReflect( void )
{
	if (m_pRTTReflect)
	{
		return m_pRTTReflect->GetIsWorkOnCurFrame();
	}
	return false;
}

bool CWater::GetUseRefract( void )
{
	if (m_pRTTRefract)
	{
		return m_pRTTRefract->GetIsWorkOnCurFrame();
	}
	return false;
}
*/

//void CWater::SetUseDepthMap( bool bUseDepthMap )
//{
//	//if (m_pRTTDepthMap)
//	//{
//	//	m_pRTTDepthMap->SetIsWorkOnCurFrame(bUseDepthMap);
//	//}
//	CEngine::GetInstance()->EnableDeferredLighting( true );
//}

//bool CWater::GetUseDepthMap( void )
//{
//	//if (m_pRTTDepthMap)
//	//{
//	//	return m_pRTTDepthMap->GetIsWorkOnCurFrame();
//	//}
//	//return false;
//
//	RenderTarget* pDepthMap = LightingManager::GetInstance()->GetRenderTargetGBuffer();
//	if ( !pDepthMap )
//	{
//		return false;
//	}
//	return pDepthMap->GetIsWorkOnCurFrame();
//}


void CWater::PreRender(CCamera* pCamera)
{
	// 传入GPU参数
    IRenderer* pRenderer = RENDERER;

    /*
    if (m_pRTTDepthMap)
    {
        m_pRTTDepthMap->SetIsWorkOnCurFrame(false);
    }

    if (m_pRTTRefract)
    {
        //m_pRTTRefract->SetIsWorkOnCurFrame(false);
		//m_pSamplerRefract->Apply(pRenderer, m_pRTTRefract->GetRenderTexture());
        //m_pSamplerRefract->Apply(pRenderer, m_pRTTRefract);
    }
    */
    
    if (m_pBumpTexture)
    {        
        //pRenderer->SetTextureFilter(3, TFT_Linear);
        //pRenderer->SetTextureAddressMode(3, TAV_Warp);
        //pRenderer->SetTexture(3, m_pBumpTexture);
        //m_pSamplerBump->Apply(pRenderer, m_pBumpTexture);
    }
    

    ShaderMarco marco;
    marco.Reset();

    switch (m_eWaterCmdType)
    {
    case eWCT_NoReflectNoRefractNoSoftEdge:
        break;
        // 无反射,有折射
    case eWCT_NoReflectHasRefractNoSoftEdge:
        {
            //pRenderer->SetTexture(2, m_pRTTRefract->GetRenderTexture());
            marco.RefractionOn = true;
            //pRenderer->SetTexture(2, m_pRTTRefract);
        }
        break;
        // 有反射,无折射
    case eWCT_HasReflectNoRefractNoSoftEdge:
        {
            //pRenderer->SetTexture(1, m_pRTTReflect->GetRenderTexture());
            marco.ReflectionOn = true;
            //pRenderer->SetTexture(1, m_pRTTReflect);
        }
        break;

        // 有反射,无折射,带高度图边缘柔化
    case eWCT_HasReflectNoRefractWithHightMapSoftEdge:
        {
            //pRenderer->SetTexture(1, m_pRTTReflect);
            //ITexture* pTexture = TERRAIN->GetCurHeightmap()->GetHeightmapTexture();
            //pRenderer->SetTextureFilter(4, TFT_Linear);
            //pRenderer->SetTextureAddressMode(4, TAV_Warp);
            //pRenderer->SetTexture(4, pTexture);
            marco.ReflectionOn = true;
        }
        break;

        // 有反射,有折射
    case eWCT_HasReflectHasRefractNoSoftEdge:
        {
            //pRenderer->SetTexture(1, m_pRTTReflect);
            //pRenderer->SetTexture(2, m_pRTTRefract);

            marco.ReflectionOn = true;
            marco.RefractionOn = true;
        }
        break;
    case eWCT_NoReflectHasRefractWithHightMapSoftEdge:
        {
            //pRenderer->SetTexture(2, m_pRTTRefract);
            //ITexture* pTexture = TERRAIN->GetCurHeightmap()->GetHeightmapTexture();
            //pRenderer->SetTextureFilter(4, TFT_Linear);
            //pRenderer->SetTextureAddressMode(4, TAV_Warp);
            //pRenderer->SetTexture(4, pTexture);
            marco.RefractionOn = true;
        }
        
        break;
    case eWCT_HasReflectHasRefractWithHightMapSoftEdge:
        {
            //pRenderer->SetTexture(1, m_pRTTReflect);
            //pRenderer->SetTexture(2, m_pRTTRefract);
            //ITexture* pTexture = TERRAIN->GetCurHeightmap()->GetHeightmapTexture();
            //pRenderer->SetTextureFilter(4, TFT_Linear);
            //pRenderer->SetTextureAddressMode(4, TAV_Warp);
            //pRenderer->SetTexture(4, pTexture);

            marco.ReflectionOn = true;
            marco.RefractionOn = true;
        }
        break;

	case eWCT_HasReflectHasRefractWithDepthMapSoftEdge:
		{			
   //         pRenderer->SetTexture(1, m_pRTTReflect);
   //         pRenderer->SetTexture(2, m_pRTTRefract);
			//ENGINE_INST->EnableDeferredLighting( true );
            marco.ReflectionOn = true;
            marco.RefractionOn = true;
		}
		break;
    }

    m_pMaterial->SetCurShaderMarco(marco);

    if (m_pWaterReflectProcess)
    {
        m_pWaterReflectProcess->SetActive(false);
    }

    if (m_pWaterRefactProcess)
    {
        m_pWaterRefactProcess->SetActive(false);
    }
}



void CWater::RebuildRenderData( bool bNeedRebuildRenderData )
{
	size_t nVertexCount = (m_nGridCountX + 1) * (m_nGridCountZ + 1);
	size_t nIndexCount = (m_nGridCountX * m_nGridCountZ) * 6;
	size_t nFaceCount = m_nGridCountX * m_nGridCountZ * 2;

	if ( bNeedRebuildRenderData )
	{
		m_renderData.Clear();
	}
	
	if ( !m_renderData.m_pVB )
	{
		//m_renderData.m_pVB = ENGINE_INST->GetRenderer()->CreateVertexBuffer( E_VB_NORMAL_TANGENT, nVertexCount, false );
		m_renderData.m_pVB = ENGINE_INST->GetRenderer()->CreateVertexBuffer( nVertexCount, sizeof(VERTEX_STANDARD), false );   //改回不用tangent的，因为tangetn暂时没用
	}

	if ( !m_renderData.m_pIB)
	{
		m_renderData.m_pIB = ENGINE_INST->GetRenderer()->CreateIndexBuffer( nIndexCount, false );
		u16* pIndex = (u16*)(m_renderData.m_pIB->Lock(0,0));	
		int nCurIndex = 0;
		for ( int z = 0; z < m_nGridCountZ; ++z )
		{
			for ( int x = 0; x < m_nGridCountX; ++x )
			{
				size_t lt_id = z * (m_nGridCountX+1) + x;
				pIndex[nCurIndex++] = lt_id;
				pIndex[nCurIndex++] = lt_id + 1;
				pIndex[nCurIndex++] = lt_id + m_nGridCountX + 1;

				pIndex[nCurIndex++] = lt_id + m_nGridCountX + 1;
				pIndex[nCurIndex++] = lt_id + 1;
				pIndex[nCurIndex++] = lt_id + m_nGridCountX + 2;
			}
		}
		m_renderData.m_pIB->Unlock();
	}

	//LPVERTEX_NORMAL_TANGENT pVertex = (LPVERTEX_NORMAL_TANGENT)(m_renderData.m_pVB->Lock());
	LPVERTEX_STANDARD pVertex = (LPVERTEX_STANDARD)(m_renderData.m_pVB->Lock());
	
	int   nCurVertex = 0;
	float fUOffset = 1.0f / m_nGridCountX;
	float fVOffset = 1.0f / m_nGridCountZ;
	for ( int z = 0; z <= m_nGridCountZ; ++z )
	{
		float cur_z_pos = z * m_fGridWidthZ - m_fHeight / 2.0f;
		float cur_v_pos = z * fVOffset;
		float cur_x_pos = -m_fWidth / 2.0f;
		float cur_u_pos = 0.0f;
		for ( int x = 0; x <= m_nGridCountX; ++x )
		{
			pVertex[nCurVertex].color = m_clrWater;
			//pVertex[nCurVertex].normal = Vector3Df::UNIT_Y;
			pVertex[nCurVertex].position = Vector3Df( cur_x_pos, 0.0f, cur_z_pos );
			//pVertex[nCurVertex].vPos = Vector3Df( cur_x_pos, 0.0f, cur_z_pos );
			pVertex[nCurVertex].tu = cur_u_pos;
			pVertex[nCurVertex++].tv = cur_v_pos;
			cur_u_pos += fUOffset;
			cur_x_pos += m_fGridWidthX;
		}
	}

	//WORD* pIndex = (WORD*)(m_renderData.m_pIB->Lock(0,0));	
	//LPVERTEX_NORMAL_TANGENT pA, pB, pC;
	//Vector3Df pEdgeA, pEdgeB, pTangent;
	//for(int t=0; t<nFaceCount; t++)
	//{
	//	int a=pIndex[t*3], b=pIndex[t*3+1], c=pIndex[t*3+2];
	//	pA=&(pVertex[a]);
	//	pB=&(pVertex[b]);
	//	pC=&(pVertex[c]);

	//	// compute edges
	//	pEdgeA=pC->position - pB->position; 
	//	pEdgeB=pA->position - pB->position;

	//	// compute gradients
	//	float fDeltaU1=pC->tu - pB->tu;
	//	float fDeltaU2=pA->tu - pB->tu;

	//	// scale edges
	//	pEdgeA*=fDeltaU1;
	//	pEdgeB*=fDeltaU2;

	//	// compute tangent vector
	//	pTangent=pEdgeB-pEdgeA;
	//	pTangent.Normalize();

	//	// sum up tangent vectors
	//	pA->tangent = pTangent;
	//	pB->tangent = pTangent;            
	//	pC->tangent = pTangent;
	//}  
	//m_renderData.m_pIB->Unlock();

	m_renderData.m_pVB->Unlock();
	
	m_renderData.m_nPrimCount = nFaceCount;
	m_renderData.m_renderType = RT_TRIANGLE_LIST;

	m_boundingBox.Reset( Vector3Df(-m_fWidth/2, 0, -m_fHeight/2), Vector3Df( m_fWidth/2, 0,  m_fHeight/2) );

	if ( m_pNode )
	{
		m_pNode->NeedUpdate();
		m_pNode->Update();
	}
}

void CWater::GetWorldRect( Rectf& rc )
{
	rc.left = m_WorldAABB.MinEdge.x;
	rc.top = m_WorldAABB.MinEdge.z;
	rc.right = m_WorldAABB.MaxEdge.x;
	rc.bottom = m_WorldAABB.MaxEdge.z;
}


/*
// 设置反射RTT对象
bool CWater::SetRenderTargetReflect( RenderTargetReflect* prtt )
{
	if ( m_pRTTReflect )
	{
		DT_TO_MSGBOX_FMT( LT_WARN, GetString( "Water [%s] Reflect RTT has been exist!", m_strName.c_str() ) );
		return false;
	}
	m_pRTTReflect = prtt;
	return true;
}

// 设置折射RTT对象
bool CWater::SetRenderTargetRefract( RenderTargetRefract* prtt )
{
	if ( m_pRTTRefract )
	{
		DT_TO_MSGBOX_FMT( LT_WARN, GetString( "Water [%s] Refract RTT has been exist!", m_strName.c_str() ) );
		return false;
	}
	m_pRTTRefract = prtt;
	return true;
}
*/

// 设置水体的颜色
void CWater::SetWaterColor( const SColor& color )
{
	m_clrWater = color;
	RebuildRenderData();
}

// 获得反射RTT对象
/*
RenderTargetReflect* CWater::GetRenderTargetReflect( void )
{
	return m_pRTTReflect;
}

// 获得折射RTT]对象
RenderTargetRefract* CWater::GetRenderTargetRefract( void )
{
	return m_pRTTRefract;
}
*/

void CWater::AutoSelectMaterial( void )
{
	//如果材质为空
	if ( !m_pMaterial )
	{
		return;
	}

	//当前不显示,关闭折射,反射
	if ( !m_bVisable )
	{
		SetUseReflect(false);
		SetUseRefract(false);
		//SetUseDepthMap(false);
		return;
	}
	//bool bUseReflect = GetUseReflect();
	//bool bUseRefract = GetUseRefract();

	ITexture* pTexture = TERRAIN->GetCurHeightmap()->GetHeightmapTexture();

	switch( m_eWaterCmdType )
	{
		// 无反射,无折射
	case eWCT_NoReflectNoRefractNoSoftEdge:
		{
			//m_pMaterial->SetCurrentTechnique( CSTR_WATER_TECHNIQUES[ eWCT_NoReflectNoRefractNoSoftEdge ] );
			SetUseReflect(false);
			SetUseRefract(false);
			//SetUseDepthMap(false);
		}
		break;

		// 无反射,有折射
	case eWCT_NoReflectHasRefractNoSoftEdge:
		{
			SetUseReflect(false);
			SetUseRefract(true);
			//SetUseDepthMap(false);
			//m_pMaterial->SetCurrentTechnique( CSTR_WATER_TECHNIQUES[ eWCT_NoReflectHasRefractNoSoftEdge ] );
			// 折射纹理
			//m_pMaterial->GetCurrentTechnique()->SetTextureUnitTex( 1, m_pRTTRefract->GetRenderTexture() );
		}
		break;

		// 无反射,有折射, 带高度图边缘柔化
	case eWCT_NoReflectHasRefractWithHightMapSoftEdge:
		{
			SetUseReflect(false);
			SetUseRefract(true);
			//SetUseDepthMap(false);
			//m_pMaterial->SetCurrentTechnique( CSTR_WATER_TECHNIQUES[ eWCT_NoReflectHasRefractWithHightMapSoftEdge ] );
			// 折射纹理
            /*
			m_pMaterial->GetCurrentTechnique()->SetTextureUnitTex( 1, m_pRTTRefract->GetRenderTexture() );
			// 地形高度图
			CTextureUnit* pUnit = m_pMaterial->GetCurrentTechnique()->GetPass(0)->GetTextureUnit(3);
			if (pUnit)
			{
				if (pUnit->GetTexture() != pTexture)
				{
					pTexture->AddRef();
					pUnit->SetTexture(pTexture);
				}
			}
            */
		}
		break;

		// 有反射,无折射
	case eWCT_HasReflectNoRefractNoSoftEdge:
		{
			if ( m_bUnderWater )
			{
				SetUseReflect(false);
				SetUseRefract(false);
				//SetUseDepthMap(false);
				//m_pMaterial->SetCurrentTechnique( CSTR_WATER_TECHNIQUES[ eWCT_NoReflectNoRefractNoSoftEdge ] );
			}
			else
			{
				SetUseReflect(true);
				SetUseRefract(false);
				//SetUseDepthMap(false);
				// 反射纹理
				//m_pMaterial->SetCurrentTechnique( CSTR_WATER_TECHNIQUES[ eWCT_HasReflectNoRefractNoSoftEdge ] );
				//m_pMaterial->GetCurrentTechnique()->SetTextureUnitTex( 1, m_pRTTReflect->GetRenderTexture() );
			}
		}
		break;

		// 有反射,无折射, 带深度柔化
	case eWCT_HasReflectNoRefractWithHightMapSoftEdge:
		{
			if ( m_bUnderWater )
			{
				SetUseReflect(false);
				SetUseRefract(false);
				//SetUseDepthMap(false);
				//m_pMaterial->SetCurrentTechnique( CSTR_WATER_TECHNIQUES[ eWCT_NoReflectNoRefractNoSoftEdge ] );
			}
			else
			{
				SetUseReflect(true);
				SetUseRefract(false);
				//SetUseDepthMap(false);
				//m_pMaterial->SetCurrentTechnique( CSTR_WATER_TECHNIQUES[ eWCT_HasReflectNoRefractWithHightMapSoftEdge ] );
				// 反射纹理
				//m_pMaterial->GetCurrentTechnique()->SetTextureUnitTex( 1, m_pRTTReflect->GetRenderTexture() );
				// 地形高度图纹理
				//CTextureUnit* pUnit = m_pMaterial->GetCurrentTechnique()->GetPass(0)->GetTextureUnit(3);
				//if (pUnit)
				//{
				//	if (pUnit->GetTexture() != pTexture)
				//	{
				//		pTexture->AddRef();
				//		pUnit->SetTexture(pTexture);
				//	}
				//}
			}
		}
		break;

		// 有反射,有折射
	case eWCT_HasReflectHasRefractNoSoftEdge:
		{
			// 在水底中
			if ( m_bUnderWater )
			{
				SetUseReflect(false);
				SetUseRefract(true);
				//SetUseDepthMap(false);
				//m_pMaterial->SetCurrentTechnique( CSTR_WATER_TECHNIQUES[ eWCT_NoReflectHasRefractNoSoftEdge ] );
				// 折射纹理
				//m_pMaterial->GetCurrentTechnique()->SetTextureUnitTex( 1, m_pRTTRefract->GetRenderTexture() );
			}
			// 在水面上
			else
			{
				SetUseReflect(true);
				SetUseRefract(true);
				//SetUseDepthMap(false);
				//m_pMaterial->SetCurrentTechnique( CSTR_WATER_TECHNIQUES[ eWCT_HasReflectHasRefractNoSoftEdge ] );
				// 反射纹理
				//m_pMaterial->GetCurrentTechnique()->SetTextureUnitTex( 1, m_pRTTReflect->GetRenderTexture() );
				// 折射纹理
				//m_pMaterial->GetCurrentTechnique()->SetTextureUnitTex( 2, m_pRTTRefract->GetRenderTexture() );
			}
		}
		break;

		// 有反射,有折射,带高度图边缘柔化
	case eWCT_HasReflectHasRefractWithHightMapSoftEdge:
		{
			// 在水底中
			if ( m_bUnderWater )
			{
				SetUseReflect(false);
				SetUseRefract(true);
				//SetUseDepthMap(false);
				//m_pMaterial->SetCurrentTechnique( CSTR_WATER_TECHNIQUES[ eWCT_NoReflectHasRefractNoSoftEdge ] );
				// 折射纹理
				//m_pMaterial->GetCurrentTechnique()->SetTextureUnitTex( 1, m_pRTTRefract->GetRenderTexture() );
                //m_pMaterial->GetCurrentTechnique()->SetTextureUnitTex( 1, m_pRTTRefract );
			}
			// 在水面上
			else
			{
				SetUseReflect(true);
				SetUseRefract(true);
				//SetUseDepthMap(false);
				//m_pMaterial->SetCurrentTechnique( CSTR_WATER_TECHNIQUES[ eWCT_HasReflectHasRefractWithHightMapSoftEdge ] );
				
			}
		}
		break;

		// 有反射,有折射,带深度图边缘柔化
	case eWCT_HasReflectHasRefractWithDepthMapSoftEdge:
		{
			// 在水底中
			if ( m_bUnderWater )
			{
				SetUseReflect(false);
				SetUseRefract(true);
				//SetUseDepthMap(false);
				//m_pMaterial->SetCurrentTechnique( CSTR_WATER_TECHNIQUES[ eWCT_NoReflectHasRefractNoSoftEdge ] );
				// 折射纹理
				//m_pMaterial->GetCurrentTechnique()->SetTextureUnitTex( 1, m_pRTTRefract->GetRenderTexture() );
			}
			// 在水面上
			else
			{
				SetUseReflect(true);
				SetUseRefract(true);
				//SetUseDepthMap(true);
				//m_pMaterial->SetCurrentTechnique( CSTR_WATER_TECHNIQUES[ eWCT_HasReflectHasRefractWithDepthMapSoftEdge ] );
				// 反射纹理
				//m_pMaterial->GetCurrentTechnique()->SetTextureUnitTex( 1, m_pRTTReflect->GetRenderTexture() );
				// 折射纹理
				//m_pMaterial->GetCurrentTechnique()->SetTextureUnitTex( 2, m_pRTTRefract->GetRenderTexture() );
				// 深度图纹理
				//m_pMaterial->GetCurrentTechnique()->SetTextureUnitTex( 4, m_pRTTDepthMap->GetRenderTexture() );
			}
		}
		break;
	}
}


void CWater::SetWaveDistorsion( float fDistorsion )
{
	m_fWaveDistorsion = fDistorsion;
}

void CWater::SetFlowDirection( const Vector2Df& vDir )
{
	m_vWaveDirection = vDir;
}


void CWater::SetFlowSpeed( float fSpeed )
{
	m_fFlowSpeed = fSpeed;
}


void CWater::CreateUnderWaterCompositor( int nTargetWidth, int nTargetHeight )
{
	IRenderer* pRenderer = ENGINE_INST->GetRenderer();
	AIRString szName = m_strName + "_UnderWaterCompositor";
	// 创建一个屏幕合成器
	//m_pUnderWaterCompositor = SCREEN_COMPOSITOR_MANAGER->CreateDefaultScreenCompositor( szName );
    //m_pUnderWaterCompositor = SCREEN_COMPOSITOR_MANAGER->CreateScreenCompositor(szName, "PostScreenCompositor");
	// 设置屏幕合成器的大小与后缓冲一样大
	//m_pUnderWaterCompositor->Create( 0, 0, nTargetWidth + 20, nTargetHeight + 20 );
    //(static_cast<PostScreenCompositor*>(m_pUnderWaterCompositor))->SetPostScreenType(ePSCT_Distortion);

    /*
	// 创建屏幕合成器的RTT
	m_pRTTUnderWaterCompositor = new RenderTargetScreenCompositor();
	// 设置RTT的大小为屏幕大小
	m_pRTTUnderWaterCompositor->Initialize( nTargetWidth, nTargetHeight );
	// 设置RTT每帧都工作
	m_pRTTUnderWaterCompositor->SetIsWorkOnCurFrame( false );
	m_pRTTUnderWaterCompositor->SetRTTResetOperation( true, 1.0f );
	// 将RTT记录到引擎里面
	ENGINE_INST->AddRenderTarget( m_pRTTUnderWaterCompositor );
	// 创建合成器的材质
	IMaterial* pMaterial = MATERIAL_MANAGER->CreateMaterial("CompositerMaterial.xml/UnderWaterScreenCompositor");
	pMaterial->SetTextureUnitTex( 0, m_pRTTUnderWaterCompositor->GetRenderTexture() );
	m_pUnderWaterCompositor->SetMaterial( pMaterial );
	m_pUnderWaterCompositor->SetVisable( false );
    */
}


void CWater::ClearUnderWaterCompositor( void )
{
    /*
	if (m_pUnderWaterCompositor)
	{
		SCREEN_COMPOSITOR_MANAGER->DestroyScreenCompositor(m_pUnderWaterCompositor->GetName());
		m_pUnderWaterCompositor = NULL;
	}
    */

    /*
	if (m_pRTTUnderWaterCompositor)
	{
		ENGINE_INST->DestroyRenderTarget( m_pRTTUnderWaterCompositor );
		m_pRTTUnderWaterCompositor = NULL;
	}
    */
}

void CWater::Update(float fTimeElapse )
{
	ISceneManager* pSceneMgr = ENGINE_INST->GetSceneMgr();

	// 如果当前场景摄像机在水体中
	CCamera* pCamera = ENGINE_INST->GetSceneMgr()->GetCamera();
	// 获取摄像机位置
	Vector3Df vPosCam = pCamera->GetPosition();
	// 摄像机是否在水体之外
	m_bIsCameraInWater = !( (vPosCam.y > m_vCenter.y) || (vPosCam.x < m_WorldAABB.MinEdge.x) || (vPosCam.x > m_WorldAABB.MaxEdge.x) || (vPosCam.z < m_WorldAABB.MinEdge.z) || (vPosCam.z > m_WorldAABB.MaxEdge.z) );

    if (!m_bVisable)
    {
		//ENGINE_INST->RemoveRenderTarget(m_pRTTDepthMap);
        //ENGINE_INST->RemoveRenderTarget(m_pRTTReflect);
        //ENGINE_INST->RemoveRenderTarget(m_pRTTRefract);
		if ( m_bIsCameraInWater )
		{
			/*POSTPROCESS_MANAGER->EnablePostEffect( false, ePET_Distortion );
			if ( POSTPROCESS_MANAGER->GetActiveCompositorsNum() == 0 )
			{
				ENGINE_INST->EnablePostProcess( false );
			}*/
		}
		if (m_pWaterRefactProcess)
		{
			m_pWaterRefactProcess->SetActive(false);
		}

		if (m_pWaterReflectProcess)
		{
			m_pWaterReflectProcess->SetActive(false);
		}
        return;
    }
    
    if ( m_bIsCameraInWater && !m_bUnderWater )
    {
		// 摄像机进入水体之内
        m_bUnderWater = true;
        // 关闭水面反射
        //SetUseReflect( false );
		// 设置折射
        if (m_pRTTRefract)
        {
            // 设置折射平面
            //m_pRTTRefract->SetRefractPlane( Plane3Df(m_vCenter, Vector3Df::UNIT_Y) );
            m_pWaterRefactProcess->ClearClipPlanes();
            m_pWaterRefactProcess->AddClipPlane(Plane3Df(m_vCenter, Vector3Df::UNIT_Y));
			// 打开后处理全屏特效
            ENGINE_INST->EnablePostProcess(true);

			/*if ( !POSTPROCESS_MANAGER->GetPostEffect(ePET_Distortion) )
			{
				int nWidth, nHeight;
				ENGINE_INST->GetBackBufferSize(nWidth, nHeight);
				POSTPROCESS_MANAGER->CreatePostEffect(ePET_Distortion, nWidth, nHeight);
			}
			POSTPROCESS_MANAGER->EnablePostEffect( true, ePET_Distortion );*/

        }

		AutoSelectMaterial();
    }
    // 如果当前场景摄像机在水体上面
    else if ( !m_bIsCameraInWater && m_bUnderWater )
    {
		// 摄像机离开水面
        m_bUnderWater = false;
        // 打开反射
        //SetUseReflect( true );
		// 设置折射平面
        if (m_pWaterRefactProcess)
        {
            //m_pRTTRefract->SetRefractPlane( Plane3Df(m_vCenter, Vector3Df::NEGATIVE_UNIT_Y) );

            m_pWaterRefactProcess->ClearClipPlanes();
            m_pWaterRefactProcess->AddClipPlane(Plane3Df(m_vCenter, Vector3Df::UNIT_Y));
        }

		/*POSTPROCESS_MANAGER->EnablePostEffect( false, ePET_Distortion );
		if ( POSTPROCESS_MANAGER->GetActiveCompositorsNum() == 0 )
		{
			ENGINE_INST->EnablePostProcess( false );
		}*/

		AutoSelectMaterial();
    }

	// 如果在水底下
    if (m_bIsCameraInWater)
    {
		// 打开后处理全屏特效
        ENGINE_INST->EnablePostProcess(true);
    }

	if ( m_bIsCameraInWater )
	{
		//--------------------------------------------------
		//! add by hjj 2010-9-14
		pSceneMgr->SetFogColor( m_fogColorUnderWater );
		pSceneMgr->SetFogMode( m_fogModeUnderWater );
		pSceneMgr->SetFogDensity( m_fDensityUnderWater );
		pSceneMgr->SetFogStart( m_fFogStartUnderWater );
		pSceneMgr->SetFogEnd( m_fFogEndUnderWater );
		//--------------------------------------------------
	}

    switch( m_eWaterCmdType )
    {
        // 无反射,无折射
    case eWCT_NoReflectNoRefractNoSoftEdge:
        break;
        // 无反射,有折射
    case eWCT_NoReflectHasRefractNoSoftEdge:
        {
   //         ENGINE_INST->AddRenderTarget(m_pRTTRefract);
			//ENGINE_INST->RemoveRenderTarget(m_pRTTReflect);
			//ENGINE_INST->AddCustomRenderProcess(m_pWaterRefactProcess);
			//ENGINE_INST->RemoveCustomRenderProcess(m_pWaterReflectProcess);

			m_pWaterRefactProcess->SetActive(true);
			m_pWaterReflectProcess->SetActive(false);
        }
        break;

        // 无反射,有折射, 带高度图边缘柔化
    case eWCT_NoReflectHasRefractWithHightMapSoftEdge:
        {
   //         ENGINE_INST->AddRenderTarget(m_pRTTRefract);
			//ENGINE_INST->RemoveRenderTarget(m_pRTTReflect);
			//ENGINE_INST->AddCustomRenderProcess(m_pWaterRefactProcess);
			//ENGINE_INST->RemoveCustomRenderProcess(m_pWaterReflectProcess);
			m_pWaterRefactProcess->SetActive(true);
			m_pWaterReflectProcess->SetActive(false);
        }
        break;

        // 有反射,无折射
    case eWCT_HasReflectNoRefractNoSoftEdge:
        {
   //         ENGINE_INST->AddRenderTarget(m_pRTTReflect);
			//ENGINE_INST->RemoveRenderTarget(m_pRTTRefract);
			//ENGINE_INST->AddCustomRenderProcess(m_pWaterReflectProcess);
			//ENGINE_INST->RemoveCustomRenderProcess(m_pWaterReflectProcess);

			m_pWaterRefactProcess->SetActive(false);
			m_pWaterReflectProcess->SetActive(true);
        }
        break;

        // 有反射,无折射,带高度图边缘柔化
    case eWCT_HasReflectNoRefractWithHightMapSoftEdge:
        {
            //ENGINE_INST->AddRenderTarget(m_pRTTReflect);
            //ENGINE_INST->AddRenderTarget(m_pRTTRefract);
			//ENGINE_INST->AddCustomRenderProcess(m_pWaterRefactProcess);
			//ENGINE_INST->AddCustomRenderProcess(m_pWaterReflectProcess);

			m_pWaterRefactProcess->SetActive(false);
			m_pWaterReflectProcess->SetActive(true);
        }
        break;

        // 有反射,有折射
    case eWCT_HasReflectHasRefractNoSoftEdge:
        {
            //ENGINE_INST->AddRenderTarget(m_pRTTReflect);
            //ENGINE_INST->AddRenderTarget(m_pRTTRefract);
			//ENGINE_INST->AddCustomRenderProcess(m_pWaterRefactProcess);
			//ENGINE_INST->AddCustomRenderProcess(m_pWaterReflectProcess);

			m_pWaterRefactProcess->SetActive(true);
			m_pWaterReflectProcess->SetActive(true);
        }
        break;

        // 有反射,有折射,带高度图边缘柔化
    case eWCT_HasReflectHasRefractWithHightMapSoftEdge:
        {
            //ENGINE_INST->AddRenderTarget(m_pRTTReflect);
            //ENGINE_INST->AddRenderTarget(m_pRTTRefract);
   //         ENGINE_INST->AddCustomRenderProcess(m_pWaterReflectProcess);
			//ENGINE_INST->AddCustomRenderProcess(m_pWaterRefactProcess);

			m_pWaterRefactProcess->SetActive(true);
			m_pWaterReflectProcess->SetActive(true);
        }
        break;

		// 有反射,有折射,带深度图边缘柔化
	case eWCT_HasReflectHasRefractWithDepthMapSoftEdge:
		{
			//ENGINE_INST->AddRenderTarget(m_pRTTReflect);
			//ENGINE_INST->AddRenderTarget(m_pRTTRefract);
			//ENGINE_INST->AddCustomRenderProcess(m_pWaterRefactProcess);
			//ENGINE_INST->AddCustomRenderProcess(m_pWaterReflectProcess);

			m_pWaterRefactProcess->SetActive(true);
			m_pWaterReflectProcess->SetActive(true);
			//ENGINE_INST->AddRenderTarget(m_pRTTDepthMap);
			CEngine::GetInstance()->EnableDeferredLighting( true );
		}
		break;
    }
}

void CWater::UpdateToRenderQueue(CCamera* pCamera, CRenderQueue* pQueue)
{

    if (m_bVisable)
    {   
		if (Prepare())
		{
			pQueue->AddRenderable(this);

			Update(ENGINE_INST->GetTimeSinceLastFrame());

            if (m_pWaterReflectProcess)
            {
                m_pWaterReflectProcess->SetActive(true);
            }

            if (m_pWaterRefactProcess)
            {
                m_pWaterRefactProcess->SetActive(true);
                //m_pWaterRefactProcess->SetCamera(ENGINE_INST->GetSceneMgr()->GetCamera(), false);
            }
		}
        
    }
    
}


bool CWater::CreateFromXML( CMarkupSTL& xml )
{
	AIRString strValue;
	m_strName = xml.GetAttrib( "Name" ).c_str();
	m_nGridCountX = atoi(xml.GetAttrib( "GridCountX" ).c_str());
	m_nGridCountZ = atoi(xml.GetAttrib( "GridCountZ" ).c_str());
	m_fGridWidthX = atof(xml.GetAttrib( "GridScaleX" ).c_str());
	m_fGridWidthZ = atof(xml.GetAttrib( "GridScaleZ" ).c_str());
	m_fUScale = atof(xml.GetAttrib( "UScale" ).c_str());
	m_fVScale = atof(xml.GetAttrib( "VScale" ).c_str());
	strValue = xml.GetAttrib( "CenterPos" ).c_str();
	sscanf( strValue.c_str(), "(%f,%f,%f)", &m_vCenter.x, &m_vCenter.y, &m_vCenter.z );
	strValue = xml.GetAttrib( "Color" ).c_str();
	int a,r,g,b;
	sscanf( strValue.c_str(), "(%d,%d,%d,%d)", &a, &r, &g, &b );
	m_clrWater.Set( a, r, g, b );
	m_fWaveDistorsion = atof(xml.GetAttrib( "Distorsion" ).c_str());
	strValue = xml.GetAttrib( "Direction" ).c_str();
	sscanf( strValue.c_str(), "(%f,%f)", &m_vWaveDirection.x, &m_vWaveDirection.y );
	m_fFlowSpeed = atof(xml.GetAttrib( "FlowSpeed" ).c_str());
	if ( xml.GetAttrib("WaterMode").length() )
	{
		m_eWaterCmdType = eWaterCmdType( atoi(xml.GetAttrib("WaterMode").c_str()) );
	}
    m_eWaterCmdType = eWCT_HasReflectHasRefractNoSoftEdge;

	strValue = xml.GetAttrib("FogModeUnderWater").c_str();
	if ( strValue.length() )
	{
		m_fogModeUnderWater = FogMode( atoi(strValue.c_str()) );
	}

	strValue = xml.GetAttrib( "FogColorUnderWater" ).c_str();
	if ( strValue.length() )
	{
		m_fogColorUnderWater.a = 1.0f;
		sscanf( strValue.c_str(), "(%f,%f,%f)", &m_fogColorUnderWater.r, &m_fogColorUnderWater.g, &m_fogColorUnderWater.b );
	}

	strValue = xml.GetAttrib( "FogStartUnderWater" ).c_str();
	if ( strValue.length() )
	{
		m_fFogStartUnderWater = atof( strValue.c_str() );
	}

	strValue = xml.GetAttrib( "FogEndUnderWater" ).c_str();
	if ( strValue.length() )
	{
		m_fFogEndUnderWater = atof( strValue.c_str() );
	}

	strValue = xml.GetAttrib( "FogDensityUnderWater" ).c_str();
	if ( strValue.length() )
	{
		m_fDensityUnderWater = atof( strValue.c_str() );
	}

	strValue = xml.GetAttrib( "Shiness" ).c_str();
	if ( strValue.length() )
	{
		m_fShiness = atof( strValue.c_str() );
	} 

	strValue = xml.GetAttrib( "Gradient" ).c_str();
	if ( strValue.length() )
	{
		m_fGradient = atof( strValue.c_str() );
	} 

	int nWidth, nHeight;
	ENGINE_INST->GetBackBufferSize( nWidth, nHeight );
	CreateWaterSuface( m_nGridCountX, m_nGridCountZ, m_fGridWidthX, m_fGridWidthZ, m_vCenter );
	CreateWaterRenderTarget( nWidth / 2, nHeight / 2 );
	CreateUnderWaterCompositor( nWidth, nHeight );
	SetWaterColor( m_clrWater );
	SetUseReflect( true );
	SetUseRefract( true );
	InitDefault();
	
	return true;
}


bool CWater::SaveToXML( CMarkupSTL& xml )
{
	xml.AddAttrib( "Name", m_strName.c_str() );
	xml.AddAttrib( "GridCountX", m_nGridCountX );
	xml.AddAttrib( "GridCountZ", m_nGridCountZ );
	xml.AddAttrib( "GridScaleX", GetString("%f",m_fGridWidthX) );
	xml.AddAttrib( "GridScaleZ", GetString("%f",m_fGridWidthZ) );
	xml.AddAttrib( "UScale", GetString("%f",m_fUScale) );
	xml.AddAttrib( "VScale", GetString("%f",m_fVScale) );
	xml.AddAttrib( "CenterPos", GetString("(%f,%f,%f)",m_vCenter.x, m_vCenter.y, m_vCenter.z) );
	xml.AddAttrib( "Color", GetString("(%d,%d,%d,%d)", m_clrWater.GetAlpha(), m_clrWater.GetRed(), m_clrWater.GetGreen(), m_clrWater.GetBlue()) );
	xml.AddAttrib( "Distorsion", GetString("%f", m_fWaveDistorsion) );
	xml.AddAttrib( "Direction", GetString("(%f,%f)",m_vWaveDirection.x, m_vWaveDirection.y) );
	xml.AddAttrib( "FlowSpeed", GetString("%f", m_fFlowSpeed) );
	xml.AddAttrib( "WaterMode", int(m_eWaterCmdType) );

	xml.AddAttrib( "FogModeUnderWater", int(m_fogModeUnderWater) );
	xml.AddAttrib( "FogColorUnderWater", GetString("(%f,%f,%f)",m_fogColorUnderWater.r, m_fogColorUnderWater.g, m_fogColorUnderWater.b) );
	xml.AddAttrib( "FogStartUnderWater", GetString("%f",m_fFogStartUnderWater) );
	xml.AddAttrib( "FogEndUnderWater", GetString("%f",m_fFogEndUnderWater) );
	xml.AddAttrib( "FogDensityUnderWater", GetString("%f",m_fDensityUnderWater) );

	xml.AddAttrib( "Shiness", GetString("%f",m_fShiness) );
	xml.AddAttrib( "Gradient", GetString("%f",m_fGradient) );

	return true;
}

/*
void CWater::SetDebugWater( bool b )
{
	if ( m_pRTTReflect )
	{
		m_pRTTReflect->SetDebugRTT( b );
	}
	if ( m_pRTTRefract )
	{
		m_pRTTRefract->SetDebugRTT( b );
	}
	if ( m_pRTTDepthMap )
	{
		m_pRTTDepthMap->SetDebugRTT( b );
	}
	//if ( m_pRTTUnderWaterCompositor )
	//{
	//	m_pRTTUnderWaterCompositor->SetDebugRTT( b );
	//}
}
*/

void CWater::SetUScale( float fUScale )
{
	m_fUScale = fUScale;
}

void CWater::SetVScale( float fVScale )
{
	m_fVScale = fVScale;
}

void CWater::SetIsCalByHeightMap( bool b )
{
	m_bIsCalByHeightMap = b;
}


void CWater::SetWaterCmdType( eWaterCmdType eWCT )
{
	m_eWaterCmdType = eWCT;
}


bool CWater::IsPick(const Vector3Df& vOrg, const Vector3Df& vPickDir, float& distFromEyeToObject)
{
	distFromEyeToObject = float(0xfffffffe);
	Vector3Df vLT( m_WorldAABB.MinEdge.x, m_WorldAABB.MinEdge.y, m_WorldAABB.MaxEdge.z );
	Vector3Df vRT( m_WorldAABB.MaxEdge.x, m_WorldAABB.MinEdge.y, m_WorldAABB.MaxEdge.z );
	Vector3Df vLB( m_WorldAABB.MinEdge.x, m_WorldAABB.MinEdge.y, m_WorldAABB.MinEdge.z );
	Vector3Df vRB( m_WorldAABB.MaxEdge.x, m_WorldAABB.MinEdge.y, m_WorldAABB.MinEdge.z );
	Vector3Df pointIntersect;
	Triangle3Df tri1( vLT, vRT, vLB ), tri2( vRT, vRB, vLB );
	if ( tri1.GetIntersectionWithLine(vOrg, vPickDir, pointIntersect) )
	{
		distFromEyeToObject = vOrg.GetDistanceFrom( pointIntersect );
		return true;
	}
	if ( tri2.GetIntersectionWithLine(vOrg, vPickDir, pointIntersect) )
	{
		distFromEyeToObject = vOrg.GetDistanceFrom( pointIntersect );
		return true;
	}
	return false;
}

void CWater::_GetPrimitive()
{
    if (m_pPrimitive == NULL)
    {
        m_pPrimitive = new Primitive(Primitive::Primitive_TriangleList/*, m_renderData.m_pVB->GetVertexDeclaration()*/);
        m_pPrimitive->m_pSrcRenderable = this;
    }

    m_pPrimitive->m_pVertexBuffer = m_renderData.m_pVB;
    m_pPrimitive->m_pIndexBuffer = m_renderData.m_pIB;
}

bool CWater::Prepare()
{
	if (!m_bPrepared)
	{
		if (m_pMaterial)
		{
			if (!m_pMaterial->IsReady())
			{
				m_pMaterial->Cache();
			}
			else
			{
				//AutoSelectMaterial();
				//m_bPrepared = true;
			}
		}

        if (!m_pBumpTexture)
        {
            TextureParam param;
            param.nMipmaps = 1;
            m_pBumpTexture = TEXTURE_MANAGER->CreateTextureFromFile("wavesbump.dds", param);
        }

        if (m_pBumpTexture && m_pBumpTexture->IsLoaded())
        {
            if (!m_pBumpTexture->IsReady())
            {
                m_pBumpTexture->Cache();
            }

            m_bPrepared = m_pMaterial->IsReady() && m_pBumpTexture->IsReady();

            FixedString strMtlName = "BumpMap";
            GPUVariant var;
            var.SetTexture(m_pBumpTexture);
            m_pMaterial->SetMaterialParam(strMtlName, var);
            SamplerState sampler;
            sampler.stage = 3;
            sampler.texAddr = TAV_Warp;
            sampler.texFilter = TFT_Linear;
            m_pMaterial->AddSamplerState(strMtlName, sampler);
            if (m_bPrepared && m_pMaterial->GetMaterialTemplate() == NULL)
            {
                AutoSelectMaterial();
            }
        }

        /*
		if (m_pSamplerReflect == NULL)
		{
			m_pSamplerReflect = RENDERER->CreateSamplerStateObject(1);
			m_pSamplerReflect->SetTextureAddressU(TAV_Clamp);
			m_pSamplerReflect->SetTextureAddressV(TAV_Clamp);
			m_pSamplerReflect->SetTextureAddressW(TAV_Clamp);
			m_pSamplerReflect->SetTextureFilterOp(TFT_Linear);
		}

		if (m_pSamplerRefract == NULL)
		{
			m_pSamplerRefract = RENDERER->CreateSamplerStateObject(2);
			m_pSamplerRefract->SetTextureAddressU(TAV_Clamp);
			m_pSamplerRefract->SetTextureAddressV(TAV_Clamp);
			m_pSamplerRefract->SetTextureAddressW(TAV_Clamp);
			m_pSamplerRefract->SetTextureFilterOp(TFT_Linear);
		}

        if (m_pSamplerBump == NULL)
        {
            m_pSamplerBump = RENDERER->CreateSamplerStateObject(3);
            m_pSamplerBump->SetTextureAddressU(TAV_Warp);
            m_pSamplerBump->SetTextureAddressV(TAV_Warp);
            m_pSamplerBump->SetTextureAddressW(TAV_Warp);
            m_pSamplerBump->SetTextureFilterOp(TFT_Linear);
        }
        */

        FixedString strMtlName = "ReflectMap";
        GPUVariant var;
        var.SetTexture(m_pRTTReflect->GetTexture());
        m_pMaterial->SetMaterialParam(strMtlName, var);
        SamplerState sampler;
        sampler.stage = 1;
        sampler.texAddr = TAV_Clamp;
        sampler.texFilter = TFT_Linear;
        m_pMaterial->AddSamplerState(strMtlName, sampler);

        strMtlName = "RefractMap";
        var.SetTexture(m_pRTTRefract->GetTexture());
        m_pMaterial->SetMaterialParam(strMtlName, var);
        sampler.stage = 2;
        sampler.texAddr = TAV_Clamp;
        sampler.texFilter = TFT_Linear;
        m_pMaterial->AddSamplerState(strMtlName, sampler);
	}

	return m_bPrepared;
	
}

void CWater::FillRenderContext(CCamera* pCamera, RenderContextQueue* pQueue, eRenderFlow flow)
{
    if (flow == FLOW_SCENE)
    {
        Renderable::FillRenderContext(pCamera, pQueue, flow);

        Update(ENGINE_INST->GetTimeSinceLastFrame());

        if (m_pWaterReflectProcess->IsActive())
        {
            m_pWaterReflectProcess->Render();
        }

        if (m_pWaterRefactProcess->IsActive())
        {
            m_pWaterRefactProcess->Render();
        }
    }
}

WaterManager::WaterManager( void )
{
	//m_mapWater.clear();
}


WaterManager::~WaterManager( void )
{

}


bool WaterManager::AddWater( CWater* pWater )
{
	CWater* pWaterExist = GetWater( pWater->GetName() );
	if ( !pWaterExist )
	{
		m_mapWater.insert( std::make_pair(pWater->GetName(), pWater) );
		return true;
	}
	return false;
}

CWater* WaterManager::GetWater( const AIRString& szName )
{
	MAP_WATER::iterator it = m_mapWater.find( szName );
	if ( it != m_mapWater.end() )
	{
		return it->second;
	}
	return NULL;
}

CWater* WaterManager::CreateDefaultWater( const AIRString& szName )
{
	CWater* pWater = GetWater( szName );
	if ( pWater )
	{
		DT_TO_DBGSTR( LT_WARN, GetString("Water [%s] has exist!\n", szName.c_str()) );
		return pWater;
	}
	pWater = new CWater();
	pWater->SetName( szName );
	m_mapWater.insert( std::make_pair(pWater->GetName(), pWater) );
	return pWater;
}

void WaterManager::DestroyWater( const AIRString& szName )
{
	MAP_WATER::iterator it = m_mapWater.find( szName );
	if ( it != m_mapWater.end() )
	{
		it->second->Clear();
		SafeDelete( it->second );
		m_mapWater.erase( it );
	}
}

void WaterManager::DestroyWater( CWater* pWater )
{
	MAP_WATER::iterator it = m_mapWater.begin();
	for ( ; it != m_mapWater.end(); ++it  )
	{
		if ( it->second == pWater )
		{
			it->second->Clear();
			SafeDelete( it->second );
			m_mapWater.erase( it );
			break;
		}
	}
}

void WaterManager::DestroyAllWater( void )
{
	MAP_WATER::iterator it_cur = m_mapWater.begin(), it_end = m_mapWater.end();
	for ( ; it_cur != it_end; ++it_cur )
	{
		it_cur->second->Clear();
		SafeDelete( it_cur->second );
	}
	m_mapWater.clear();
}


bool WaterManager::ChangeWaterName( const AIRString& szOldName, const AIRString& szNewName )
{
	MAP_WATER::iterator it_old = m_mapWater.find( szOldName );
	if ( it_old == m_mapWater.end() )
	{
		DT_TO_MSGBOX_FMT( LT_WARN, GetString("Can not find old water object [%s]!",szOldName.c_str()) );
		return false;
	}

	MAP_WATER::iterator it_new = m_mapWater.find( szNewName );
	if ( it_new != m_mapWater.end() )
	{
		DT_TO_MSGBOX_FMT( LT_WARN, GetString("The water object [%s] has been exist!",szNewName.c_str()) );
		return false;
	}

	CWater* pWater = it_old->second;
	pWater->SetName( szNewName );
	m_mapWater.erase( it_old );
	m_mapWater.insert( std::make_pair( szNewName, pWater ) );
	return true;
}

void WaterManager::UpdateWater(float fTimeElapsed)
{
    MAP_WATER::iterator itEnd = m_mapWater.end();
    for (MAP_WATER::iterator it = m_mapWater.begin(); it != itEnd; ++it)
    {
        it->second->Update(fTimeElapsed);
    }
}

/*
void WaterManager::RefreshHeightMapToWater( void )
{
	MAP_WATER::iterator itEnd = m_mapWater.end();
	for (MAP_WATER::iterator it = m_mapWater.begin(); it != itEnd; ++it)
	{
		it->second->RefreshHeightMapToWater();
	}
}
*/

CWater* WaterManager::CreateDefaultWaterImpl()
{
	return new CWater();
}


void WaterManager::SetShowAllWater( bool bShow )
{
	MAP_WATER::iterator itEnd = m_mapWater.end();
	for (MAP_WATER::iterator it = m_mapWater.begin(); it != itEnd; ++it)
	{
		it->second->SetVisable( bShow );
	}
}

