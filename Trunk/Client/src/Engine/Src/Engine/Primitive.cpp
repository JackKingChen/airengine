
#include "stdafx.h"
#include "Primitive.h"
#include "Engine.h"
#include "IVertexBuffer.h"
#include "IIndexBuffer.h"
#include "MeshInstancingObject.h"
//#include "Technique.h"
#include "IMaterial.h"

Primitive::Primitive(Primitive::Type type/*, CVertexDeclaration* pDecl*/) : m_type(type)
, m_pVertexBuffer(NULL)
, m_pIndexBuffer(NULL)
, m_pSrcRenderable(NULL)
, m_pVertexData(NULL)
, m_pIndexData(NULL)
, m_nVertexStart(0)
, m_nIndexStart(0)
, m_nIndicesNum(0)
, m_nVerticesStride(0)
//, m_pVertexDecl(pDecl)
, m_nVerticesNum(0)
, m_nPrimitiveCount(0)
, m_bUserPrimitiveCount(false)
{

}

Primitive::~Primitive()
{

}
/*
void Primitive::Draw(CTechnique* pTechnique)
{
    IRenderer* pRenderer = ENGINE_INST->GetRenderer();
    //pRenderer->SetVertexDeclaration(m_pVertexDecl);
    if ( m_pVertexBuffer )
    {
        pRenderer->SetVertexBuffer(0, m_pVertexBuffer);
    }
    if ( m_pIndexBuffer )
    {
        pRenderer->SetIndexBuffer(m_pIndexBuffer);
    }    
    //if (pTechnique && m_pSrcRenderable)
    //{
    //    size_t nPassNum = pTechnique->GetPassesNum();
    //    for (size_t i = 0; i < nPassNum; ++i)
    //    {
    //        CPass* pPass = pTechnique->GetPass(i);
    //        pPass->ApplyPass(pRenderer);
    //        m_pSrcRenderable->ProcessLights(pPass);
    //        pRenderer->DrawPrimitive(this);
    //    }
    //}
	if (pTechnique)
	{
	    size_t nPassNum = pTechnique->GetPassesNum();
	    for (size_t i = 0; i < nPassNum; ++i)
	    {
	        CPass* pPass = pTechnique->GetPass(i);
            IMaterial* pMaterial = pTechnique->GetParentMaterial();
            if (pMaterial)
            {
                pMaterial->UpdateMaterialParams(pPass);
            }
	        //pPass->ApplyPass(pRenderer);
            
			if (m_pSrcRenderable)
			{
				//m_pSrcRenderable->ProcessLights(pPass);
			}	        
	        pRenderer->DrawPrimitive(this);
	    }
	}
    else
    {
        pRenderer->DrawPrimitive(this);
    }
    
}
*/

InstancingPrimitive::InstancingPrimitive(Type type/*, CVertexDeclaration* pDecl*/) : Primitive(type/*, pDecl*/), m_pInstanceVB(NULL)
{

}

InstancingPrimitive::~InstancingPrimitive()
{

}
/*
void InstancingPrimitive::Draw(CTechnique* pTechnique)
{
    MeshInstancingObject* pInstancingObject = static_cast<MeshInstancingObject*>(m_pSrcRenderable);
    IRenderer* pRenderer = ENGINE_INST->GetRenderer();
	pRenderer->SetIndexBuffer(m_pIndexBuffer);
    //pRenderer->SetVertexDeclaration(m_pVertexDecl);

	pRenderer->DrawInstances(m_pVertexBuffer, m_pInstanceVB, this, pInstancingObject->GetInstancesNum(), pTechnique, m_pIndexBuffer ? m_pIndexBuffer->GetTotalIndex() : 0);
}
*/

SharePrimitive::SharePrimitive(Type type/*, CVertexDeclaration* pDecl*/) : Primitive(type/*, pDecl*/)
{

}

SharePrimitive::~SharePrimitive()
{
    PrimitiveListIter itEnd = m_lstPrimitive.end();
    for (PrimitiveListIter it = m_lstPrimitive.begin(); it != itEnd; ++it)
    {
        SafeDelete(*it);
    }

    m_lstPrimitive.clear();
    m_vtTextures.clear();
}
/*
void SharePrimitive::Draw(CTechnique* pTechnique)
{
    IRenderer* pRenderer = ENGINE_INST->GetRenderer();
    //pRenderer->SetVertexDeclaration(m_pVertexDecl);
    pRenderer->SetVertexBuffer(0, m_pVertexBuffer);
    pRenderer->SetIndexBuffer(m_pIndexBuffer);
    if (pTechnique)
    {
        size_t nPassNum = pTechnique->GetPassesNum();
        for (size_t i = 0; i < nPassNum; ++i)
        {
            CPass* pPass = pTechnique->GetPass(i);
            pPass->ApplyPass(pRenderer);
            m_pSrcRenderable->ProcessLights(pPass);

            PrimitiveListIter itEnd = m_lstPrimitive.end();
            size_t nTexture = 0;
            for (PrimitiveListIter it = m_lstPrimitive.begin(); it != itEnd; ++it, ++nTexture)
            {
                if (m_vtTextures.size() > nTexture)
                {
                    pRenderer->SetTexture(0, m_vtTextures[nTexture]);
                }
                pRenderer->DrawPrimitive(*it);
            }
        }
    }
    else
    {
        PrimitiveListIter itEnd = m_lstPrimitive.end();
        size_t i = 0;
        for (PrimitiveListIter it = m_lstPrimitive.begin(); it != itEnd; ++it, ++i)
        {
            if (m_vtTextures.size() > i)
            {
                pRenderer->SetTexture(0, m_vtTextures[i]);
            }
            pRenderer->DrawPrimitive(*it);
        }
    }
}
*/