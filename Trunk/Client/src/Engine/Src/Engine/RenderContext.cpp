#include "RenderContext.h"
#include "MeshInstance.h"
#include "IMaterial.h"
#include "IRenderer.h"
#include "Camera.h"
#include "Primitive.h"
#include "Engine.h"
#include "ShaderObject.h"

static bool ContextLessThan(const RenderContext* pRenderContext1, const RenderContext* pRenderContext2)
{
    bool bC1Transport = pRenderContext1->IsTransparent();
    bool bC2Transport = pRenderContext2->IsTransparent();

    if (bC1Transport)
    {
        if (bC2Transport)     //两者都是透明物
        {
            if (pRenderContext1->m_pRenderPass->depthStencilState.zWriteEnable != pRenderContext2->m_pRenderPass->depthStencilState.zWriteEnable)
            {
                return pRenderContext1->m_pRenderPass->depthStencilState.zWriteEnable , pRenderContext2->m_pRenderPass->depthStencilState.zWriteEnable;
            }

            return pRenderContext1->m_fDisToCamera > pRenderContext2->m_fDisToCamera;
        }
        else
        {
            //C1透明，C2不透明
            return false;
        }
    }
    else              
    {
        if (bC2Transport)
        {
            return true;
        }
        else
        {
            //两者都不透明
            return pRenderContext1->m_fDisToCamera < pRenderContext2->m_fDisToCamera;
        }
    }
    return true;
}

RenderContext::RenderContext() : m_pMaterial(NULL), m_pNode(NULL), m_pPrimitive(NULL), m_pRenderPass(NULL)
, m_fDisToCamera(0.0f)
, m_nBonesNum(0)
, m_pBoneMatrices(NULL)
, m_pRenderable(NULL)
, m_bSetProjMat(false)
, m_bSetViewMat(false)
//, m_pDcl(NULL)
{

}

RenderContext::~RenderContext()
{

}

bool RenderContext::IsTransparent() const
{
    return m_pRenderPass ? m_pRenderPass->blendState.blendEnable : false;
}

RenderContextQueue::RenderContextQueue()
{
    m_RenderContexPool.Resize(1000);
}

RenderContextQueue::~RenderContextQueue()
{

}

void RenderContextQueue::DrawRenderableToContext(Renderable* pRenderable, CCamera* pCamera, eRenderFlow flow)
{

    IMaterial* pMaterial = pRenderable->GetMaterial();

    ShaderMarco macro = pMaterial->GetCurShaderMarco();

    if (flow == FLOW_SHADOWMAP)
    {
        macro.RenderShadowmap = 1;
    }
    else
    {
        if (ENGINE_INST->IsShadowmapEnable() && pRenderable->IsAcceptShadowMap())
        {
            macro.ShadowmapOn = 1;
        }
    }

    //获得材质当前使用的technique
    size_t nTechIndex = pMaterial->GetTechniqueIndexByFlow(flow);
    if (nTechIndex != -1)
    {
        RenderTechnique* pTechnique = pMaterial->GetRenderTechnique(macro, nTechIndex);

        if (pTechnique)
        {
            for (size_t i = 0; i < pTechnique->m_vtRenderPass.size(); ++i)
            {
                //m_pMaterial->UpdateMaterialParams(&(pTechnique->m_vtRenderPass[i]));
                RenderContext* pContext = m_RenderContexPool.AllocObject();

                if (pContext == 0)
                {
                    return;
                }

                pContext->m_pPrimitive = pRenderable->GetPrimitive();

                pContext->m_matWorld = pRenderable->GetWorldMatrix();

				pContext->m_matView = pCamera->GetViewMatrix();
				pContext->m_bSetViewMat = true;
				pContext->m_matProj = pCamera->GetProjectionMatrix();
				pContext->m_bSetProjMat = true;

                pContext->m_pMaterial = pMaterial;
                pContext->m_pRenderPass = &pTechnique->m_vtRenderPass[i];

				IGPUProgram* pVertexShader = pContext->m_pRenderPass->pShaderObject->GetGPUProgram(eGST_Vertex);

				if (pVertexShader->GetVertexDeclaration() == NULL)
				{
					pVertexShader->CreateVertexDeclaration(pRenderable->GetVertexDeclName());
				}

                pContext->m_pNode = pRenderable->GetNodeParent();
                pContext->m_pBoneMatrices = NULL;
                pContext->m_nBonesNum = 0;

				//pContext->m_pDcl = pRenderable->CreateVertexDecl(pContext->m_pRenderPass->pShaderObject->GetGPUProgram(eGST_Vertex));

                pContext->m_fDisToCamera = (pRenderable->GetWorldPosition() - pCamera->GetPosition()).GetLength();
                pContext->m_pRenderable = pRenderable;
                m_vtContexts.push_back(pContext);
            }

            //pRenderer->DrawPrimitive(m_pPrimitive);
        }
    }

    
}

void RenderContextQueue::DrawSkinmeshToContext(CMeshInstance* pMesh, CCamera* pCamera, Matrix4f* pBoneMatrices, u32 nBonesNum, eRenderFlow flow)
{
    IMaterial* pMaterial = pMesh->GetMaterial();
    ShaderMarco macro = pMaterial->GetCurShaderMarco();
    if (flow == FLOW_SHADOWMAP)
    {
        macro.RenderShadowmap = 1;
    }
    else
    {
        if (ENGINE_INST->IsShadowmapEnable() && pMesh->IsAcceptShadowMap())
        {
            macro.ShadowmapOn = 1;
        }
    }
    

    //获得材质当前使用的technique
    size_t nTechIndex = pMaterial->GetTechniqueIndexByFlow(flow);
    if (nTechIndex != -1)
    {
		if (ENGINE_INST->IsHardwareSkeleton())
		{
			macro.HardwareSkin = 1;
		}
		else
		{
			macro.HardwareSkin = 0;
		}
        RenderTechnique* pTechnique = pMaterial->GetRenderTechnique(macro, nTechIndex);

        if (pTechnique)
        {
            for (size_t i = 0; i < pTechnique->m_vtRenderPass.size(); ++i)
            {
                //m_pMaterial->UpdateMaterialParams(&(pTechnique->m_vtRenderPass[i]));
                RenderContext* pContext = m_RenderContexPool.AllocObject();

                if (pContext == 0)
                {
                    return;
                }

                pContext->m_pPrimitive = pMesh->GetPrimitive();

                pContext->m_matWorld = pMesh->GetWorldMatrix();

				pContext->m_matView = pCamera->GetViewMatrix();
				pContext->m_bSetViewMat = true;
				pContext->m_matProj = pCamera->GetProjectionMatrix();
				pContext->m_bSetProjMat = true;

                pContext->m_pMaterial = pMaterial;
                pContext->m_pRenderPass = &pTechnique->m_vtRenderPass[i];

				IGPUProgram* pVertexShader = pContext->m_pRenderPass->pShaderObject->GetGPUProgram(eGST_Vertex);
				
				if (pVertexShader->GetVertexDeclaration() == NULL)
				{
					pVertexShader->CreateVertexDeclaration(pMesh->GetVertexDeclName());
				}

				//pContext->m_pDcl = pVertexShader->GetVertexDeclaration();

                pContext->m_pNode = pMesh->GetNodeParent();
                pContext->m_pBoneMatrices = pBoneMatrices;
                pContext->m_nBonesNum = nBonesNum;

                pContext->m_fDisToCamera = (pMesh->GetWorldPosition() - pCamera->GetPosition()).GetLength();

                pContext->m_pRenderable = pMesh;

                m_vtContexts.push_back(pContext);
            }

            //pRenderer->DrawPrimitive(m_pPrimitive);
        }
    }
}

void RenderContextQueue::DrawEntityToContext(Entity* pEntiy, CCamera* pCamera, eRenderFlow flow)
{
	IMaterial* pMaterial = pEntiy->GetMaterial();

	ShaderMarco macro = pMaterial->GetCurShaderMarco();

	if (flow == FLOW_SHADOWMAP)
	{
		macro.RenderShadowmap = 1;
	}
	else
	{
		if (ENGINE_INST->IsShadowmapEnable() && pEntiy->IsAcceptShadowMap())
		{
			macro.ShadowmapOn = 1;
		}
	}

	//获得材质当前使用的technique
	size_t nTechIndex = pMaterial->GetTechniqueIndexByFlow(flow);
	if (nTechIndex != -1)
	{
		RenderTechnique* pTechnique = pMaterial->GetRenderTechnique(macro, nTechIndex);

		if (pTechnique)
		{
			for (size_t i = 0; i < pTechnique->m_vtRenderPass.size(); ++i)
			{
				//m_pMaterial->UpdateMaterialParams(&(pTechnique->m_vtRenderPass[i]));
				RenderContext* pContext = m_RenderContexPool.AllocObject();

				if (pContext == 0)
				{
					return;
				}

				pContext->m_pPrimitive = pRenderable->GetPrimitive();

				pContext->m_matWorld = pRenderable->GetWorldMatrix();

				pContext->m_matView = pCamera->GetViewMatrix();
				pContext->m_bSetViewMat = true;
				pContext->m_matProj = pCamera->GetProjectionMatrix();
				pContext->m_bSetProjMat = true;

				pContext->m_pMaterial = pMaterial;
				pContext->m_pRenderPass = &pTechnique->m_vtRenderPass[i];

				IGPUProgram* pVertexShader = pContext->m_pRenderPass->pShaderObject->GetGPUProgram(eGST_Vertex);

				if (pVertexShader->GetVertexDeclaration() == NULL)
				{
					pVertexShader->CreateVertexDeclaration(pRenderable->GetVertexDeclName());
				}

				pContext->m_pNode = pRenderable->GetNodeParent();
				pContext->m_pBoneMatrices = NULL;
				pContext->m_nBonesNum = 0;

				//pContext->m_pDcl = pRenderable->CreateVertexDecl(pContext->m_pRenderPass->pShaderObject->GetGPUProgram(eGST_Vertex));

				pContext->m_fDisToCamera = (pRenderable->GetWorldPosition() - pCamera->GetPosition()).GetLength();
				pContext->m_pRenderable = pRenderable;
				m_vtContexts.push_back(pContext);
			}

			//pRenderer->DrawPrimitive(m_pPrimitive);
		}
	}
}

void RenderContextQueue::DrawPrimitive(Primitive* pPrim, IMaterial* pMaterial, const Matrix4f& matWorld, const Matrix4f& matView, const Matrix4f& matProj, eRenderFlow flow,
	const FixedString& strVertexDecl)
{
	ShaderMarco macro = pMaterial->GetCurShaderMarco();
	size_t nTechIndex = pMaterial->GetTechniqueIndexByFlow(flow);
	if (nTechIndex != -1)
	{
		RenderTechnique* pTechnique = pMaterial->GetRenderTechnique(macro, nTechIndex);

		if (pTechnique)
		{
			for (size_t i = 0; i < pTechnique->m_vtRenderPass.size(); ++i)
			{
				RenderContext* pContext = m_RenderContexPool.AllocObject();

				if (pContext == 0)
				{
					return;
				}

				pContext->m_pPrimitive = pPrim;
				pContext->m_matWorld = matWorld;
				pContext->m_matView = matView;
				pContext->m_bSetViewMat = true;
				pContext->m_matProj = matProj;
				pContext->m_bSetProjMat = true;
				pContext->m_pMaterial = pMaterial;
				pContext->m_pRenderPass = &pTechnique->m_vtRenderPass[i];
				IGPUProgram* pVertexShader = pContext->m_pRenderPass->pShaderObject->GetGPUProgram(eGST_Vertex);
				if (pVertexShader->GetVertexDeclaration() == NULL)
				{
					pVertexShader->CreateVertexDeclaration(strVertexDecl);
				}
				pContext->m_pNode = NULL;
				pContext->m_pBoneMatrices = NULL;
				pContext->m_nBonesNum = 0;
				pContext->m_fDisToCamera = 0;
				pContext->m_pRenderable = NULL;
				m_vtContexts.push_back(pContext);
			}
		}
	}
}

void RenderContextQueue::DrawContexts()
{
    std::stable_sort(m_vtContexts.begin(), m_vtContexts.end(), ContextLessThan);
    IRenderer* pRenderer = ENGINE_INST->GetRenderer();
    RenderContext* pContext = NULL;
    for (size_t i = 0; i < m_vtContexts.size(); ++i)
    {
        pContext = m_vtContexts[i];
        
        pRenderer->SetWorldTransform(&pContext->m_matWorld);
        if( pContext->m_bSetViewMat )
            pRenderer->SetViewTransform(&pContext->m_matView);
        if( pContext->m_bSetProjMat )
            pRenderer->SetProjectionTransform(&pContext->m_matProj);
		//���ö�������
		//pRenderer->SetVertexDeclaration(pContext->m_pDcl);

        if (pContext->m_pBoneMatrices)
            pRenderer->GetGPUDataParam()->SetBoneMatrices(pContext->m_pBoneMatrices, pContext->m_nBonesNum);
        
        bool hasSetVAO = false;
		if( ENGINE_INST->GetUseVAO() )
		{
			if( pContext->m_pRenderPass && pContext->m_pPrimitive->m_pVertexBuffer )
			{
                if( pContext->m_pRenderPass->vertexArrayObj == NULL || pContext->m_pRenderPass->vertexArrayObj->m_vb == NULL )
                    pRenderer->CreateVAO(pContext->m_pRenderPass, pContext->m_pPrimitive->m_pVertexBuffer, pContext->m_pPrimitive->m_pIndexBuffer);
			}
			if( pContext->m_pRenderPass->vertexArrayObj )
			{
				pRenderer->SetVAO(pContext->m_pRenderPass->vertexArrayObj);
				hasSetVAO = true;
			}
		}
        
        if( hasSetVAO == false )
        {
            //首先提交GPU参数
            if (pContext->m_pPrimitive->m_pVertexBuffer)
                pRenderer->SetVertexBuffer(0, pContext->m_pPrimitive->m_pVertexBuffer);

            if (pContext->m_pPrimitive->m_pIndexBuffer)
                pRenderer->SetIndexBuffer(pContext->m_pPrimitive->m_pIndexBuffer);
        }

        pContext->m_pMaterial->UpdateMaterialParams(pContext->m_pRenderPass);

        //再画context
        pRenderer->DrawPrimitive(pContext->m_pPrimitive);

        m_RenderContexPool.FreeObject(pContext);
    }

    m_vtContexts.clear();
}
