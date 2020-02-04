#include "stdafx.h"
#include "MeshInstance.h"
#include "Engine.h"
#include "MaterialManager.h"
#include "Shadowmap.h"
#include "ModelInstance.h"
#include "LightingManager.h"
//#include "TechniqueSelector.h"
#include "Primitive.h"
#include "LightingManager.h"
//#include "GBufferRenderProcess.h"
#include "IMaterial.h"
//#include "Pass.h"
//#include "Technique.h"
#include "InstancingObjectManager.h"
#include "GPUProgramManager.h"
#include "GPUVariant.h"
#include "RenderContext.h"
#include "VertexDeclarationManager.h"

CMeshInstance::CMeshInstance() : m_pSubMesh(NULL)
, m_pOwnerModelInst(NULL)
//, m_pTechniqueRenderToShadow(NULL)
//, m_pTechniqueAcceptShadow(NULL)
//, m_pTechniqueDeferredLighting(NULL)
//, m_bVerticeCaculated(false)
//, m_pTechniqueRenderToGBuffer(NULL)
//, m_pTechniqueRenderToDepthMap(NULL)
, m_bUseTexLuminance(false)
, m_bNeedUpdateVertices(false)
{

}

CMeshInstance::CMeshInstance(CAIRSubMesh* pSubMesh, CModelInstance* pOwner) : m_pSubMesh(pSubMesh)
, m_pOwnerModelInst(pOwner)
//, m_pTechniqueRenderToShadow(NULL)
//, m_pTechniqueAcceptShadow(NULL)
//, m_pTechniqueDeferredLighting(NULL)
//, m_bVerticeCaculated(false)
//, m_pTechniqueRenderToGBuffer(NULL)
//, m_pTechniqueRenderToDepthMap(NULL)
, m_bUseTexLuminance(false)
, m_bNeedUpdateVertices(false)
{
    //创建材质
    m_szType = "MeshInstance";
    m_pMaterial = MATERIAL_MANAGER->CreateMaterial();
    m_pMaterial->DeepCopy(pSubMesh->GetMaterial());
    //if (pOwner->IsStatic() && m_pMaterial)
    //{
    //    if (m_pMaterial->IsReady())
    //    {
    //        CTechnique* pTechnique = m_pMaterial->GetTechnique(0);
    //        if (pTechnique)
    //        {
    //            CPass* pPass = pTechnique->GetPass(0);
    //            //IShader* pShader = pPass->GetShader();
    //            IRenderer* pRenderer = ENGINE_INST->GetRenderer();
    //        }
    //    }
    //}
    
    
    m_strName = pSubMesh->GetName();
    m_boundingBox = m_pSubMesh->GetBoundingBox();
    m_bAcceptLighting = true;
	//this->m_bUseTexLuminance = false;
	//m_pGPUDataParmSource = NULL;

    m_renderGroupType = eRGT_Solid;

	if (m_pOwnerModelInst->IsStatic())
	{
		m_strVertexDecl = "VS_MeshInput";
	}
	else
	{
		m_strVertexDecl = ENGINE_INST->IsHardwareSkeleton() ? "E_VB_NORMAL_BONE_WEIGHT" : "VS_MeshInput";
	}
}

CMeshInstance::~CMeshInstance()
{

}

const Matrix4f& CMeshInstance::GetWorldMatrix() const
{
    //if (m_pOwnerModelInst->GetAnimationType() == eMAT_SkeletionAnimation)
    //{
    //    return Matrix4f::IDENTITY;
    //}
    //else
    {
        CSceneNode* pNodeOwner = m_pOwnerModelInst->GetNodeParent();
        return pNodeOwner->GetFullTransform();
    }
}

/*
void CMeshInstance::Render(CCamera* pCamera)
{
    IRenderer* pRenderer = ENGINE_INST->GetRenderer();
#ifdef TEXTURE_RENDER_GROUP
    PreRender(pCamera);   //后期添加
    
    IShader* pShader = NULL;

    CSceneNode* pNodeOwner = m_pOwnerModelInst->GetNodeParent();
    if ((pNodeOwner && m_pOwnerModelInst->IsStatic()) || m_pOwnerModelInst->GetVertexAnimationRes())
    {
        pRenderer->SetWorldTransform(&m_pOwnerModelInst->GetNodeParent()->GetFullTransform());
    }
    else
    {
        if (m_pOwnerModelInst->GetCurrentAnimation() == NULL && pNodeOwner)
        {
            pRenderer->SetWorldTransform(&pNodeOwner->GetFullTransform());
        }
        else
            pRenderer->SetWorldTransform(&Matrix4f::IDENTITY);

    }
#endif
    //根据场景、引擎的一些参数来选择technique

    PreRender(pCamera);
    CSceneNode* pNodeOwner = m_pOwnerModelInst->GetNodeParent();
    if ((pNodeOwner && m_pOwnerModelInst->IsStatic()) || m_pOwnerModelInst->GetVertexAnimationRes())
    {
        pRenderer->SetWorldTransform(&m_pOwnerModelInst->GetNodeParent()->GetFullTransform());
    }
    else
    {
        if (m_pOwnerModelInst->GetCurrentAnimation() == NULL && pNodeOwner)
        {
            pRenderer->SetWorldTransform(&pNodeOwner->GetFullTransform());
        }
        else
            pRenderer->SetWorldTransform(&Matrix4f::IDENTITY);

    }
    if (m_pMaterial)
    {
        CTechnique* pTechnique = m_pMaterial->GetCurrentTechnique();
        if (pTechnique)
        {

			LodData* pCurLodData = m_pSubMesh->GetCurLodData();
			if ( !pCurLodData )
			{
				return;
			}
			pRenderer->SetVertexBuffer( 0, pCurLodData->m_pMeshVB );
			pRenderer->SetIndexBuffer( pCurLodData->m_pMeshIB );
            pRenderer->GetGPUDataParam()->SetColorDiffuse(m_pOwnerModelInst->GetColorDiffuse());
            size_t nPassNum = pTechnique->GetPassesNum();
            for (size_t i = 0; i < nPassNum; ++i)
            {
                CPass* pPass = pTechnique->GetPass(i);
                pPass->ApplyPass(pRenderer);
                ProcessLights(pPass);

                if (m_pSubMesh)
                {
                    m_pSubMesh->Render(pCamera);
                }
            }
        }
    }

	Renderable::AfterRender();
}
*/

bool CMeshInstance::IsPick(const Vector3Df& vOrg, const Vector3Df& vPickDir, const Matrix4f& matTransform)
{
    if (m_pSubMesh)
    {
        return m_pSubMesh->IsPick(vOrg, vPickDir, matTransform);
    }
    f32 fDis = 0;
    return m_WorldAABB.IsRayIntersect(vOrg, vPickDir, fDis);
}

bool CMeshInstance::IsPick(const Vector3Df& vOrg, const Vector3Df& vPickDir, const Matrix4f& matTransform, float& distFromEyeToObject )
{
	if (m_pSubMesh)
	{
		return m_pSubMesh->IsPick(vOrg, vPickDir, matTransform, distFromEyeToObject);
	}
    return m_WorldAABB.IsRayIntersect(vOrg, vPickDir, distFromEyeToObject);
}

void CMeshInstance::SetTransparent(bool bAlpha /* = true */, u8 nAlpha /* = 128 */)
{
    if (m_pMaterial)
    {
        m_pMaterial->SetTransparent(bAlpha);
        m_pMaterial->SetBlendSrc(RSRV_SrcColor);
        m_pMaterial->SetBlendDest(RSRV_InvSrcColor);
        //m_pMaterial->SetZWriteEnable(!bAlpha);
    }
    //m_pSubMesh->SetTransparent(bAlpha, nAlpha);
}

void CMeshInstance::GenRenderToShadowTechnique()
{
    //if (m_pOwnerModelInst->IsStatic() || !(ENGINE_INST->IsHardwareSkeleton()))
    //{
    //    m_pTechniqueRenderToShadow = SHADOWMAP->GenShadowTechnique(m_pMaterial);
    //}
    //else
    /*
    {
        m_pTechniqueRenderToShadow = new CTechnique(m_pMaterial);
        //*m_pTechniqueRenderToShadow = *(m_pMaterial->GetTechnique(0));
        m_pTechniqueRenderToShadow->SetName(SHADOWMAP->GetName().c_str());

        //m_pTechniqueRenderToShadow->SetVSVersion("vs_2_0");
        //m_pTechniqueRenderToShadow->SetPSVersion("ps_2_0");
        CPass* pPass = m_pTechniqueRenderToShadow->GetPass(0);
        //把第1阶段以后的纹理都清除
        for (size_t i = 1; i < 8; ++i)
        {
            //pPass->RemoveTextureUnit(i);

        }
        //IShader* pShader = pPass->GetShader();

        IRenderer* pRenderer = ENGINE_INST->GetRenderer();
        //if (pShader)
        {
        //    pShader->Clear();
            pPass->SetFogEnable(false);
            pPass->SetAlphaBlend(false);
            IGPUProgram* pVSShadowmap = NULL;
            if (m_pOwnerModelInst->IsStatic()  || !(ENGINE_INST->IsHardwareSkeleton()))
            {
                pVSShadowmap = GPUPROGRAM_MANAGER->GetGPUProgram("VSMeshRenderToShadow");
            }
            else
            {
                pVSShadowmap = GPUPROGRAM_MANAGER->GetGPUProgram("VSSkinAnimRenderToShadow"); //pRenderer->CreateVertexShader("VSSkinAnimRenderToShadow.txt", m_pTechniqueRenderToShadow->GetVSVersion().c_str());
            }
            
            //pPass->SetVertexShader(pVSShadowmap);
            IGPUProgram* pPSShadowmap = GPUPROGRAM_MANAGER->GetGPUProgram("PSRenderToShadowmap"); //pRenderer->CreatePixelShader("PSShadowmap.txt", m_pTechniqueRenderToShadow->GetPSVersion().c_str());
            //pPass->SetPixelShader(pPSShadowmap);

			IGPUProgram* pGpuPrograms[4] = { 0 };
			pGpuPrograms[0] = pVSShadowmap;
			pGpuPrograms[1] = pPSShadowmap;
			ShaderObject* pShaderObject = pRenderer->CreateShaderObject(pGpuPrograms, 2);
			pPass->SetShaderObject(pShaderObject);

			pPass->SetRasterizerStateObject(0);
        }

        //pPass->SetDepthBias(SHADOWMAP->GetShadowDepthBias());
        //pPass->SetSlopeScaleDepthBias(SHADOWMAP->GetShadowScoleScaleBias());

        //m_pMaterial->AddTechnique(m_pTechniqueRenderToShadow);
    }
    */
}

/*
void CMeshInstance::RenderToShadowMap(CCamera* pCamera)
{
    //IMaterial* pMaterial = pRenderable->GetMaterial();
    if (m_pTechniqueRenderToShadow == NULL)
    {
        GenRenderToShadowTechnique();
    }

    IRenderer* pRenderer = ENGINE_INST->GetRenderer();
    PreRender(pCamera);
    CSceneNode* pNodeOwner = m_pOwnerModelInst->GetNodeParent();
    if ((pNodeOwner && m_pOwnerModelInst->IsStatic()) || m_pOwnerModelInst->GetVertexAnimationRes())
    {
        pRenderer->SetWorldTransform(&m_pOwnerModelInst->GetNodeParent()->GetFullTransform());
    }
    else
    {
        if (m_pOwnerModelInst->GetCurrentAnimation() == NULL && pNodeOwner)
        {
            pRenderer->SetWorldTransform(&pNodeOwner->GetFullTransform());
        }
        else
            pRenderer->SetWorldTransform(&Matrix4f::IDENTITY);

    }

    if (m_pTechniqueRenderToShadow)
    {

        LodData* pCurLodData = m_pSubMesh->GetCurLodData();
        if ( !pCurLodData )
        {
            return;
        }
        pRenderer->SetVertexBuffer( 0, pCurLodData->m_pMeshVB );
        pRenderer->SetIndexBuffer( pCurLodData->m_pMeshIB );
        size_t nPassNum = m_pTechniqueRenderToShadow->GetPassesNum();
        for (size_t i = 0; i < nPassNum; ++i)
        {
            CPass* pPass = m_pTechniqueRenderToShadow->GetPass(i);
            pPass->ApplyPass(pRenderer);
            ProcessLights(pPass);

            if (m_pSubMesh)
            {
                m_pSubMesh->Render(pCamera);
            }
        }
    }
}
*/

void CMeshInstance::AcceptShadowmap(bool bAccept)
{
    m_bAcceptShadow = bAccept;
    //m_pMaterial->SetCurrentTechnique(TECHNIQUE_SELECTOR->SelectTechnique(this));
}

/*
bool CMeshInstance::CreateHardwareSkinAnimMaterial( void )
{
	IRenderer* pRenderer = ENGINE_INST->GetRenderer();
    if (m_pMaterial && m_pMaterial->IsReady())
    {
        CTechnique* pTechniqueHardwareSkinAnim = m_pMaterial->GetTechnique( "ModelHardwareSkinAnim" );
        CTechnique* pTechniqueDefaulf = m_pMaterial->GetTechnique( 0 );

        if ( !pTechniqueHardwareSkinAnim )
        {
            pTechniqueHardwareSkinAnim = new CTechnique(m_pMaterial);
            *pTechniqueHardwareSkinAnim = *pTechniqueDefaulf;
            pTechniqueHardwareSkinAnim->SetName("ModelHardwareSkinAnim");
            //pTechniqueHardwareSkinAnim->SetVSVersion("vs_2_0");
            //pTechniqueHardwareSkinAnim->SetPSVersion("ps_3_0");
            //获得最后一个pass
            CPass* pLastPass = pTechniqueHardwareSkinAnim->GetPass(pTechniqueHardwareSkinAnim->GetPassesNum() - 1);
            //IShader* pShader = pLastPass->GetShader();
            //pShader->Clear();
            AIRString strVertexShader = "VSSkinAnim";
            IGPUProgram* pVertexShader = GPUPROGRAM_MANAGER->GetGPUProgram(strVertexShader); //pRenderer->CreateVertexShader(strVertexShader.c_str(), pTechniqueHardwareSkinAnim->GetVSVersion().c_str());
            //pLastPass->SetVertexShader(pVertexShader);
			AIRString strPixelShader = "PSMesh";
			IGPUProgram* pPixelShader = GPUPROGRAM_MANAGER->GetGPUProgram(strPixelShader);
            //pLastPass->SetPixelShader(pPixelShader);

			IGPUProgram* pGpuPrograms[4] = { 0 };
			pGpuPrograms[0] = pVertexShader;
			pGpuPrograms[1] = pPixelShader;
			ShaderObject* pShaderObject = pRenderer->CreateShaderObject(pGpuPrograms, 2);
			pLastPass->SetShaderObject(pShaderObject);

            m_pMaterial->AddTechnique( pTechniqueHardwareSkinAnim );
        }

        if ( ENGINE_INST->IsHardwareSkeleton() )
        {
            m_pMaterial->SetCurrentTechnique( pTechniqueHardwareSkinAnim );
        }
    }
	

	return true;
}
*/

//
//bool CMeshInstance::CreateTexLuminanceMaterial( void )
//{
//	if ( !m_pGPUDataParmSource )
//	{
//		m_pGPUDataParmSource = static_cast<TexLuminanceManualGPUDataSource*>(CREATE_MANUAL_GPU_PARAM_SOURCE(TexLuminanceManualGPUDataSource));
//	}
//	IRenderer* pRenderer = ENGINE_INST->GetRenderer();
//	CTechnique* pTechniqueTexLuminance = m_pMaterial->GetTechnique( "TexLuminance" );
//	CTechnique* pTechniqueDefaulf = m_pMaterial->GetTechnique( 0 );
//
//	if ( !pTechniqueTexLuminance )
//	{
//		pTechniqueTexLuminance = new CTechnique(m_pMaterial);
//		*pTechniqueTexLuminance = *pTechniqueDefaulf;
//		pTechniqueTexLuminance->SetName("TexLuminance");
//		pTechniqueTexLuminance->SetVSVersion("vs_2_0");
//		pTechniqueTexLuminance->SetPSVersion("ps_2_0");
//		//获得最后一个pass
//		CPass* pLastPass = pTechniqueTexLuminance->GetPass(pTechniqueTexLuminance->GetPassesNum() - 1);
//		IShader* pShader = pLastPass->GetShader();
//		//pShader->SetAlphaBlend( true );
//		//pShader->SetBlendSrc( D3DBLEND_SRCALPHA );
//		//pShader->SetBlendDest( D3DBLEND_DESTALPHA );
//		AIRString strVertexShader = "VSTexLuminance.txt";
//		if ( !m_pOwnerModelInst->IsStatic() )
//		{
//			strVertexShader = "VSSkinAnim.txt";
//		}
//		IGPUProgram* pVertexShader = pRenderer->CreateVertexShader(strVertexShader.c_str(), pTechniqueTexLuminance->GetVSVersion().c_str());
//		pShader->SetVertexShader(pVertexShader);
//		pVertexShader->SetManualGPUDataParamSource( m_pGPUDataParmSource );
//		AIRString strPixelShader = "PSTexLuminance.txt";
//		IGPUProgram* pPixelShader = pRenderer->CreatePixelShader(strPixelShader.c_str(), pTechniqueTexLuminance->GetPSVersion().c_str());
//		pShader->SetPixelShader(pPixelShader);
//		pPixelShader->SetManualGPUDataParamSource( m_pGPUDataParmSource );
//		m_pMaterial->AddTechnique( pTechniqueTexLuminance );
//	}
//
//	return true;
//}


void CMeshInstance::PreRender(CCamera* pCamera)
{
    if (m_pOwnerModelInst->GetCurrentAnimation() && m_pOwnerModelInst->GetModel())
    {
        //如果带骨骼动画的
        if (ENGINE_INST->IsHardwareSkeleton())
        {
            IRenderer* pRenderer = ENGINE_INST->GetRenderer();
            pRenderer->GetGPUDataParam()->SetBoneMatrices(m_pOwnerModelInst->GetBoneMatrices(), 
                m_pOwnerModelInst->GetModel()->GetBoneInstanceCount());
            //这里要检查有没顶点动画，有的话要计算
            if (m_pSubMesh->GetAnimationType() & eMAT_VertexAnimation)
            {
                m_pSubMesh->CaculateKeyframeVertices(m_pOwnerModelInst->GetCurrentAnimation()->GetTimePos());
            }
        }
        else
        {
            m_pSubMesh->CaculateVertices(m_pOwnerModelInst->GetColorDiffuse(), m_pOwnerModelInst->GetBoneMatrices(), /*m_pOwnerModelInst->GetCurrentAnimation()->GetTimePos()*/0);
        }
    }
    //else if (m_pOwnerModelInst->GetSkeletonRes())
    //{
    //    if (ENGINE_INST->IsHardwareSkeleton())
    //    {
    //        IRenderer* pRenderer = ENGINE_INST->GetRenderer();
    //        pRenderer->GetGPUDataParam()->SetBoneMatrices(m_pOwnerModelInst->GetBoneMatrices(), 
    //            m_pOwnerModelInst->GetSkeletonRes()->GetBoneInstanceCount());
    //    }
    //}

	if ( m_bUseTexLuminance )
	{
		UpdateTexLuminance();
	}
}

//void CMeshInstance::CommitRenderBuffers()
//{
//    IRenderer* pRenderer = ENGINE_INST->GetRenderer();
//
//    pRenderer->SetVertexBuffer(0, m_pSubMesh->GetVertexBuffer());
//    pRenderer->SetIndexBuffer(m_pSubMesh->GetIndexBuffer());
//}

void CMeshInstance::CommitTransform()
{
    IRenderer* pRenderer = ENGINE_INST->GetRenderer();
    CSceneNode* pNodeOwner = m_pOwnerModelInst->GetNodeParent();
    if ((pNodeOwner && m_pOwnerModelInst->IsStatic()) || m_pOwnerModelInst->GetVertexAnimationRes())
    {
        pRenderer->SetWorldTransform(&m_pOwnerModelInst->GetNodeParent()->GetFullTransform());
    }
    else
    {
        if (m_pOwnerModelInst->GetCurrentAnimation() == NULL && pNodeOwner)
        {
            pRenderer->SetWorldTransform(&pNodeOwner->GetFullTransform());
        }
        else
            pRenderer->SetWorldTransform(&Matrix4f::IDENTITY);

    }
}

void CMeshInstance::PostRender(CCamera* pCamera)
{
    PreRender(pCamera);
    //m_pSubMesh->Render(pCamera);
    _GetPrimitive();
    //m_pPrimitive->Draw(NULL);
}

const AABBBox3Df& CMeshInstance::GetWorldBoundingBox(bool bTransform)
{
    if (bTransform)
    {
        m_WorldAABB = GetBoundingBox();
        CSceneNode* pNode = m_pOwnerModelInst->GetNodeParent();
        if (pNode)
        {
            m_WorldAABB.TransformAffine(pNode->GetFullTransform());
        }
        m_vtCenter = m_WorldAABB.GetCenter();
    }
    return m_WorldAABB;
}


void CMeshInstance::SetUseTexLiminance( bool b )
{
	m_bUseTexLuminance = b;
	//m_pMaterial->SetCurrentTechnique(TECHNIQUE_SELECTOR->SelectTechnique(this));
}

void CMeshInstance::SetAcceptLighting(bool bAccept)
{
    Renderable::SetAcceptLighting(bAccept);

    /*
    m_pMaterial->SetCurrentTechnique(TECHNIQUE_SELECTOR->SelectTechnique(this));
    */
}


/*
void CMeshInstance::ResetCurrentTechnique()
{
    m_pMaterial->SetCurrentTechnique(TECHNIQUE_SELECTOR->SelectTechnique(this));
}
*/


void CMeshInstance::UpdateTexLuminance( void )
{
	//if ( !m_pGPUDataParmSource )
	//{
	//	return;
	//}

	static float fTimeElapse = 0.0f;
	fTimeElapse += ENGINE_INST->GetTimeSinceLastFrame();

    GPUVariant var;

	if ( m_mapTexLuminanceInfo.size() )
	{
		if ( m_mapTexLuminanceInfo.size() == 1 )
		{
			TEX_LUMINANCE_INFO_MAP_ITOR itc = m_mapTexLuminanceInfo.begin();
			//m_pGPUDataParmSource->SetColor( itc->second.m_Color );
			//m_pGPUDataParmSource->SetStrength( itc->second.m_vStrength );

            
            var.SetColor(itc->second.m_Color);
            m_pMaterial->SetMaterialParam("TLColor", var);

            var.SetVector4(itc->second.m_vStrength);
            m_pMaterial->SetMaterialParam("TLStrength", var);
		}
		else
		{
			TEX_LUMINANCE_INFO_MAP_ITOR it = m_mapTexLuminanceInfo.end(), its, ite;
			--it;
			float fMaxTimeLen = it->first;

			if ( fTimeElapse > fMaxTimeLen )
			{
				fTimeElapse = 0.0f;
			}

			for ( ite = m_mapTexLuminanceInfo.begin(); ite != m_mapTexLuminanceInfo.end(); ++ite )
			{
				if ( ite->first > fTimeElapse )
				{
					break;
				}
			}
			if ( ite == m_mapTexLuminanceInfo.begin() )
			{
				its = it;
			}
			else
			{
				its = ite;
				--its;
			}
			CColorValue cs = its->second.m_Color, ce = ite->second.m_Color, c;
			float rate = (fTimeElapse - its->first) / (ite->first - its->first);
			c = Lerp( cs, ce, rate );
			Vector4f ss = its->second.m_vStrength, se = ite->second.m_vStrength, s;
			s = AIR::Lerp<Vector4f>( ss, se, rate );
			//m_pGPUDataParmSource->SetColor( c );
			//m_pGPUDataParmSource->SetStrength( s );

            var.SetColor(c);
            m_pMaterial->SetMaterialParam("TLColor", var);

            var.SetVector4(s);
            m_pMaterial->SetMaterialParam("TLStrength", var);
		}
	}

	//m_pGPUDataParmSource->SetTexLuminanceWholeMesh( m_nTexLuminanceWholeMesh );

    var.SetFloat((f32)m_nTexLuminanceWholeMesh);
    m_pMaterial->SetMaterialParam("TLWholeMesh", var);
}


bool CMeshInstance::GetUseTexLiminance( void )
{
	return m_bUseTexLuminance;
}

void CMeshInstance::InsertTexLuminanceColorPair( float time, CColorValue& col, Vector4f& strength )
{
	TexLuminanceInfo info;
	info.m_Color = col;
	info.m_vStrength = strength;
	m_mapTexLuminanceInfo.insert( std::make_pair(time, info) );
}

void CMeshInstance::DeleteTexLuminanceColorPair( float time )
{
	TEX_LUMINANCE_INFO_MAP_ITOR it = m_mapTexLuminanceInfo.find( time );
	if ( it != m_mapTexLuminanceInfo.end() )
	{
		m_mapTexLuminanceInfo.erase( it );
	}
}


void CMeshInstance::ClearTexLuminanceInfo( void )
{
	m_mapTexLuminanceInfo.clear();
}

void CMeshInstance::SetTexLuminanceWholeMesh( bool b )
{
	m_nTexLuminanceWholeMesh = int(b);
}

//ManualGPUDataParamSource*	CMeshInstance::GetTexLuminanceGPUData( void )
//{
//	return m_pGPUDataParmSource;
//}

//void CMeshInstance::CreateTexLuminanceGPUData( void )
//{
//	if ( !m_pGPUDataParmSource )
//	{
//		m_pGPUDataParmSource = static_cast<TexLuminanceManualGPUDataSource*>(CREATE_MANUAL_GPU_PARAM_SOURCE(TexLuminanceManualGPUDataSource));
//	}
//}

void CMeshInstance::_GetPrimitive()
{
    LodData* pCurLodData = m_pSubMesh->GetCurLodData();
    if (m_pPrimitive == NULL)
    {
        m_pPrimitive = new Primitive(Primitive::Primitive_TriangleList/*, pCurLodData->m_VertexData.m_pMeshVB->GetVertexDeclaration()*/);
        m_pPrimitive->m_pSrcRenderable = this;
    }
    m_pPrimitive->m_pVertexBuffer = pCurLodData->m_VertexData.m_pMeshVB;
    m_pPrimitive->m_pIndexBuffer = pCurLodData->m_VertexData.m_pMeshIB;
}
/*
void CMeshInstance::GenRenderToGBufferTechnique()
{
    if (m_pOwnerModelInst->IsStatic() || !ENGINE_INST->IsHardwareSkeleton())
    {
        //GBufferRenderProcess* pRTTGBuffer = static_cast<GBufferRenderProcess*>(LIGHTING_MANAGER->GetRenderTargetGBuffer());
        //m_pTechniqueRenderToGBuffer = pRTTGBuffer->GetTechniqueRenderToGBuffer();
        m_pTechniqueRenderToGBuffer = m_pMaterial->GetTechnique("StaticMeshRenderToGBuffer");
        if (m_pTechniqueRenderToGBuffer == NULL)
        {
            m_pTechniqueRenderToGBuffer = new CTechnique(m_pMaterial);
            *m_pTechniqueRenderToGBuffer = *(m_pMaterial->GetTechnique(0));
            m_pTechniqueRenderToGBuffer->SetName("StaticMeshRenderToGBuffer");
            //m_pTechniqueRenderToGBuffer->SetVSVersion("vs_2_0");
            //m_pTechniqueRenderToGBuffer->SetPSVersion("ps_2_0");

            CPass* pPass = m_pTechniqueRenderToGBuffer->GetPass(0);
            //把第1阶段以后的纹理都清除
            for (size_t i = 1; i < 8; ++i)
            {
                pPass->RemoveTextureUnit(i);
            }
            //IShader* pShader = pPass->GetShader();
            //pShader->Clear();
            pPass->SetFogEnable(false);
            pPass->SetAlphaBlend(false);

            AIRString strPixelShader = "PSLPPRenderToGBuffer";
            AIRString strVertexShader = "VSMeshRenderToGBuffer";

            IRenderer* pRenderer = ENGINE_INST->GetRenderer();
            IGPUProgram* pVertexShader = GPUPROGRAM_MANAGER->GetGPUProgram(strVertexShader); //pRenderer->CreateVertexShader(strVertexShader.c_str(), m_pTechniqueRenderToGBuffer->GetVSVersion().c_str());
            IGPUProgram* pPixelShader = GPUPROGRAM_MANAGER->GetGPUProgram(strPixelShader); //pRenderer->CreatePixelShader(strPixelShader.c_str(), m_pTechniqueRenderToGBuffer->GetPSVersion().c_str());
            //pPass->SetVertexShader(pVertexShader);
            //pPass->SetPixelShader(pPixelShader);
			IGPUProgram* pGpuPrograms[4] = { 0 };
			pGpuPrograms[0] = pVertexShader;
			pGpuPrograms[1] = pPixelShader;
			ShaderObject* pShaderObject = pRenderer->CreateShaderObject(pGpuPrograms, 2);
			pPass->SetShaderObject(pShaderObject);

            m_pMaterial->AddTechnique(m_pTechniqueRenderToGBuffer);
        }
    }
    else
    {
        m_pTechniqueRenderToGBuffer = m_pMaterial->GetTechnique("SkinAnimMeshRenderToGBuffer");
        if (m_pTechniqueRenderToGBuffer == NULL)
        {
            m_pTechniqueRenderToGBuffer = new CTechnique(m_pMaterial);
            *m_pTechniqueRenderToGBuffer = *(m_pMaterial->GetTechnique(0));
            m_pTechniqueRenderToGBuffer->SetName("SkinAnimMeshRenderToGBuffer");
            //m_pTechniqueRenderToGBuffer->SetVSVersion("vs_2_0");
            //m_pTechniqueRenderToGBuffer->SetPSVersion("ps_2_0");

            CPass* pPass = m_pTechniqueRenderToGBuffer->GetPass(0);
            //把第1阶段以后的纹理都清除
            for (size_t i = 1; i < 8; ++i)
            {
                pPass->RemoveTextureUnit(i);
            }
            //IShader* pShader = pPass->GetShader();
            //pShader->Clear();
            pPass->SetFogEnable(false);
            pPass->SetAlphaBlend(false);

            AIRString strPixelShader = "PSLPPRenderToGBuffer";
            AIRString strVertexShader = "VSSkinAnimRenderToGBuffer";

            IRenderer* pRenderer = ENGINE_INST->GetRenderer();
            IGPUProgram* pVertexShader = GPUPROGRAM_MANAGER->GetGPUProgram(strVertexShader); //pRenderer->CreateVertexShader(strVertexShader.c_str(), m_pTechniqueRenderToGBuffer->GetVSVersion().c_str());
            IGPUProgram* pPixelShader = GPUPROGRAM_MANAGER->GetGPUProgram(strPixelShader); //pRenderer->CreatePixelShader(strPixelShader.c_str(), m_pTechniqueRenderToGBuffer->GetPSVersion().c_str());
            //pPass->SetVertexShader(pVertexShader);
            //pPass->SetPixelShader(pPixelShader);
			IGPUProgram* pGpuPrograms[4] = { 0 };
			pGpuPrograms[0] = pVertexShader;
			pGpuPrograms[1] = pPixelShader;
			ShaderObject* pShaderObject = pRenderer->CreateShaderObject(pGpuPrograms, 2);
			pPass->SetShaderObject(pShaderObject);

            m_pMaterial->AddTechnique(m_pTechniqueRenderToGBuffer);
        }
    }
}

void CMeshInstance::GenRenderToDepthMapTechnique()
{
    if (m_pOwnerModelInst->IsStatic() || !ENGINE_INST->IsHardwareSkeleton())
    {
        //GBufferRenderProcess* pRTTGBuffer = static_cast<GBufferRenderProcess*>(LIGHTING_MANAGER->GetRenderTargetGBuffer());
        //m_pTechniqueRenderToGBuffer = pRTTGBuffer->GetTechniqueRenderToGBuffer();
        Renderable::GenRenderToDepthMapTechnique();
    }
    else
    {
        m_pTechniqueRenderToDepthMap = m_pMaterial->GetTechnique("SkinAnimMeshRenderToDepthmap");
        if (m_pTechniqueRenderToDepthMap == NULL)
        {
            m_pTechniqueRenderToDepthMap = new CTechnique(m_pMaterial);
            *m_pTechniqueRenderToDepthMap = *(m_pMaterial->GetTechnique(0));
            m_pTechniqueRenderToDepthMap->SetName("SkinAnimMeshRenderToDepthmap");
            //m_pTechniqueRenderToDepthMap->SetVSVersion("vs_2_0");
            //m_pTechniqueRenderToDepthMap->SetPSVersion("ps_2_0");

            CPass* pPass = m_pTechniqueRenderToDepthMap->GetPass(0);
            //把第1阶段以后的纹理都清除
            for (size_t i = 1; i < 8; ++i)
            {
                pPass->RemoveTextureUnit(i);
            }
            //IShader* pShader = pPass->GetShader();
            //pShader->Clear();
            pPass->SetFogEnable(false);
            pPass->SetAlphaBlend(false);

            AIRString strPixelShader = "PSRenderDepthMap.txt";
            AIRString strVertexShader = "VSSkinAnimRenderDepthMap.txt";

            IRenderer* pRenderer = ENGINE_INST->GetRenderer();
            IGPUProgram* pVertexShader = GPUPROGRAM_MANAGER->GetGPUProgram(strVertexShader); //pRenderer->CreateVertexShader(strVertexShader.c_str(), m_pTechniqueRenderToDepthMap->GetVSVersion().c_str());
            IGPUProgram* pPixelShader = GPUPROGRAM_MANAGER->GetGPUProgram(strPixelShader); //pRenderer->CreatePixelShader(strPixelShader.c_str(), m_pTechniqueRenderToDepthMap->GetPSVersion().c_str());
            //pPass->SetVertexShader(pVertexShader);
            //pPass->SetPixelShader(pPixelShader);
			IGPUProgram* pGpuPrograms[4] = { 0 };
			pGpuPrograms[0] = pVertexShader;
			pGpuPrograms[1] = pPixelShader;
			ShaderObject* pShaderObject = pRenderer->CreateShaderObject(pGpuPrograms, 2);
			pPass->SetShaderObject(pShaderObject);

            m_pMaterial->AddTechnique(m_pTechniqueRenderToDepthMap);
        }
    }
}
*/
void CMeshInstance::OnUpdateToRenderQueue(CCamera* pCamera, CRenderQueue* pQueue)
{
    if (m_pOwnerModelInst->IsInstancingEnable())
    {
        InstancingObject* pObject = INSTANCING_MANAGER->GetInstancingObject(GetMeshRes());
        if (pObject && pObject->GetMaxInstancesNum() > 3)
        {
            MeshInstancingObject* pMeshInstancing = pObject->GetMeshInstancingObject(pQueue, m_pOwnerModelInst->GetCurLodLevel());
            pMeshInstancing->AddModelInstance(this);
            if (!pMeshInstancing->IsInRenderQueue(/*pQueue*/))
            {
                pQueue->AddRenderable(pMeshInstancing);
                pMeshInstancing->SetIsInRenderQueue(true);
                //    pMeshInstancing->AddRenderQueue(pQueue);
            }
        }
        else
        {
            //m_vtMeshInst[i]->UpdateToRenderQueue(pCamera, pQueue);
            pQueue->AddRenderable(this);
        }
    }
    else
    {
        pQueue->AddRenderable(this);
    }
}

void CMeshInstance::FillRenderContext(CCamera* pCamera, RenderContextQueue* pQueue, eRenderFlow flow)
{
    if (m_pOwnerModelInst->IsStatic())
    {
        Renderable::FillRenderContext(pCamera, pQueue, flow);
    }
    else
    {
        PreRender(pCamera);

        _GetPrimitive();

        pQueue->DrawSkinmeshToContext(this, pCamera, m_pOwnerModelInst->GetBoneMatrices(), m_pOwnerModelInst->GetBoneInstanceCount(), flow);
    }
}

//CVertexDeclaration* CMeshInstance::CreateVertexDecl(IGPUProgram* pVertexShader)
//{
//	if (m_pVertexDecl == NULL)
//	{
//		if (m_pOwnerModelInst->IsStatic())
//		{
//			m_pVertexDecl = VERTEXDECL_MANAGER->CreateVertexDeclaration("VS_MeshInput", pVertexShader);
//		}
//		else
//		{
//			FixedString strDecl = ENGINE_INST->IsHardwareSkeleton() ? "E_VB_NORMAL_BONE_WEIGHT" : "VS_MeshInput";
//			m_pVertexDecl = VERTEXDECL_MANAGER->CreateVertexDeclaration(strDecl, pVertexShader);
//		}
//	}
//
//	return m_pVertexDecl;
//}
