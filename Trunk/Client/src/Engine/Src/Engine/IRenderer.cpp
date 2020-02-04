#include "stdafx.h"
#include "IRenderer.h"
#include "RenderTarget.h"
#include "DepthStencil.h"
#include "ShaderObject.h"
#include "FileSystem.h"
#include "MaterialTemplate.h"
#include "RenderWindow.h"
#include "Primitive.h"
#include "Engine.h"

IMPLEMENT_ROOT_RTTI(IRenderer);

class ScreenQuad
{
	EngineAllocator(ScreenQuad);
public:
	ScreenQuad() : m_pPrimitive(NULL), m_pVB(NULL), m_pIB(NULL) {}
	virtual ~ScreenQuad() 
	{
		IRenderer* pRenderer = ENGINE_INST->GetRenderer();
		pRenderer->ClearVertexBuffer(m_pVB);
		pRenderer->ClearIndexBuffer(m_pIB);
		SafeDelete(m_pPrimitive);
	}
	
	Primitive* m_pPrimitive;
	IVertexBuffer* m_pVB;
	IIndexBuffer*  m_pIB;
};

IRenderer::IRenderer() : //m_pCurrentPass(NULL)
m_bClipPlanesDirty(true), m_bVertexProgramBound(false)
, m_nCurrentTextures(0)
, m_bDepthBiasEnable(false)
, m_fDepthBias(0)
, m_fSlopeScaledDepthBias(0)
, m_pCurInputLayout(NULL)
, m_pRenderWindow(NULL)
, m_pScreenQuad(NULL)
{
	m_vtClipPlanes.clear();
}


IRenderer::~IRenderer() 
{

}


void IRenderer::AddClipPlane( const Plane3Df& clipPlane )
{
	m_vtClipPlanes.push_back( clipPlane );
	m_bClipPlanesDirty = true;
}

void IRenderer::SetClipPlanes( const VEC_CLIPPLANE& vtPlanes )
{
	if ( vtPlanes != m_vtClipPlanes )
	{
		m_vtClipPlanes = vtPlanes;
		m_bClipPlanesDirty = true;
	}
	m_gpuDataParam.SetClipPlane(vtPlanes[0]);
}

void IRenderer::ResetClipPlanes( void )
{
	if ( !m_vtClipPlanes.empty() )
	{
		m_vtClipPlanes.clear();
		m_bClipPlanesDirty = true;
	}
}

void IRenderer::ApplyClipPlanes( bool bVertexProgramBound )
{
	if ( bVertexProgramBound != m_bVertexProgramBound )
	{
		m_bClipPlanesDirty = true;
		m_bVertexProgramBound = bVertexProgramBound;
	}
	if ( m_bClipPlanesDirty )
	{
		SetClipPlanesImpl( m_vtClipPlanes, bVertexProgramBound );
		m_bClipPlanesDirty = false;
	}
}
/*
RendererPixelFormat IRenderer::GetPixelFormatByName(const char* szName)
{
    if (strcmp(szName, "FMT_R8G8B8") == 0)
    {
        return RPF_R8G8B8;
    }
    else if (strcmp(szName, "FMT_A8R8G8B8") == 0)
    {
        return RPF_A8R8G8B8;
    }
    else if (strcmp(szName, "FMT_X8R8G8B8") == 0)
    {
        return RPF_X8R8G8B8;
    }
    else if (strcmp(szName, "FMT_D24S8") == 0)
    {
        return RPF_D24S8;
    }
    else if (strcmp(szName, "FMT_L8") == 0)
    {
        return RPF_L8;
    }
    else if (strcmp(szName, "FMT_L16") == 0)
    {
        return RPF_L16;
    }
    else if (strcmp(szName, "FMT_R16F") == 0)
    {
        return RPF_R16F;
    }
    else if (strcmp(szName, "FMT_A16B16G16R16F") == 0)
    {
        return RPF_A16B16G16R16F;
    }
    else if (strcmp(szName, "FMT_R32F") == 0)
    {
        return RPF_R32F;
    }
    else if (strcmp(szName, "FMT_G32R32F") == 0)
    {
        return RPF_G32R32F;
    }
    else if (strcmp(szName, "FMT_A32B32G32R32F") == 0)
    {
        return RPF_A32B32G32R32F;
    }

    return RPF_A8R8G8B8;
}

void IRenderer::RenderAllRenderTargets()
{
    PriorityRenderTargetsIter itEnd = m_priorityRenderTargets.end();
    RenderTarget* pRenderTarget = NULL;
    for (PriorityRenderTargetsIter it = m_priorityRenderTargets.begin(); it != itEnd; ++it)
    {
        pRenderTarget = it->second;
        //if (pRenderTarget->GetIsWorkOnCurFrame())
        {
            pRenderTarget->Render();
        }
    }
}

void IRenderer::AttachRenderTarget(RenderTarget* pRenderTarget)
{
	if ( !pRenderTarget )
	{
		return;
	}
    RenderTargetMapIter it = m_mapRenderTargets.find(pRenderTarget->GetName());
    if (it == m_mapRenderTargets.end())
    {
        m_mapRenderTargets.insert(std::make_pair(pRenderTarget->GetName(), pRenderTarget));
        m_priorityRenderTargets.insert(std::make_pair(pRenderTarget->GetPriority(), pRenderTarget));
    }
    
}

void IRenderer::DetachRenderTarget(RenderTarget* pRenderTarget)
{
	if ( !pRenderTarget )
	{
		return;
	}
    RenderTargetMapIter itFind = m_mapRenderTargets.find(pRenderTarget->GetName());

    if (itFind != m_mapRenderTargets.end())
    {
        m_mapRenderTargets.erase(itFind);
        PriorityRenderTargetsIter itEnd = m_priorityRenderTargets.end();

        for (PriorityRenderTargetsIter it = m_priorityRenderTargets.begin(); it != itEnd; ++it)
        {
            if (it->second == pRenderTarget)
            {
                m_priorityRenderTargets.erase(it);
                break;
            }
        }
    }
}

void IRenderer::DestroyRenderTarget(RenderTarget* pRenderTarget)
{
	if ( !pRenderTarget )
	{
		return;
	}
    RenderTargetMapIter itFind = m_mapRenderTargets.find(pRenderTarget->GetName());

    if (itFind != m_mapRenderTargets.end())
    {
        m_mapRenderTargets.erase(itFind);
        PriorityRenderTargetsIter itEnd = m_priorityRenderTargets.end();

        for (PriorityRenderTargetsIter it = m_priorityRenderTargets.begin(); it != itEnd; ++it)
        {
            if (it->second == pRenderTarget)
            {
                m_priorityRenderTargets.erase(it);
                break;
            }
        }

        pRenderTarget->Clear();
        SafeDelete(pRenderTarget);
    }
}

void IRenderer::DestroyAllRenderTargets()
{
    //if (m_mapRenderTargets.size() > 0)
    //{
    //    RenderTargetMapIter itEnd = m_mapRenderTargets.end();
    //    for (RenderTargetMapIter it = m_mapRenderTargets.begin(); it != itEnd; ++it)
    //    {
    //        it->second->Clear();
    //        SafeDelete(it->second);
    //    }
    //}

    for (RenderTargetMapIter it = m_mapRenderTargets.begin(); it != m_mapRenderTargets.end(); ++it)
    {
        RenderTarget* pRenderTarget = it->second;
        pRenderTarget->Clear();
        SafeDelete(pRenderTarget);
    }
    
    m_mapRenderTargets.clear();
    m_priorityRenderTargets.clear();

	for (TextureRenderTargetMap::iterator it = m_mapTexRenderTargets.begin(); it != m_mapTexRenderTargets.end(); ++it)
	{
		ITexture* pRT = it->second;
		pRT->Clear();
		SafeDelete(pRT);
	}

	m_mapTexRenderTargets.clear();
}

RenderTarget* IRenderer::GetRenderTarget(const AIRString& strName)
{
    RenderTargetMapIter it = m_mapRenderTargets.find(strName);
    if (it != m_mapRenderTargets.end())
    {
        return it->second;
    }

    return NULL;
}

void IRenderer::DetachAllRenderTargets()
{
    m_priorityRenderTargets.clear();
}
*/
void IRenderer::DestroyAllDepthStencil()
{
	for (DepthStencilMap::iterator it = m_mapDepthStencils.begin(); it != m_mapDepthStencils.end(); ++it)
	{
		it->second->Clear();
		SafeDelete(it->second);
	}

	m_mapDepthStencils.clear();
}

void IRenderer::DestroyTextureRenderTarget(ITexture* pRTT)
{
	if (pRTT)
	{
		TextureRenderTargetMap::iterator it = m_mapTexRenderTargets.find(pRTT->GetName());
		if (it != m_mapTexRenderTargets.end())
		{
			m_mapTexRenderTargets.erase(it);

			pRTT->Clear();
			SafeDelete(pRTT);
		}
	}
}

void IRenderer::DestroyDepthStencil(DepthStencil* pDepthStencil)
{
	if (pDepthStencil)
	{
		DepthStencilMap::iterator it = m_mapDepthStencils.find(pDepthStencil->GetKey());

		if (it != m_mapDepthStencils.end())
		{
			m_mapDepthStencils.erase(it);

			pDepthStencil->Clear();
			SafeDelete(pDepthStencil);
		}
	}
}

ShaderObject* IRenderer::CreateShaderObject(IGPUProgram* pGPUPrograms[], u32 nGPUProgramsNum)
{
	AIRString strName;
	for (u32 i = 0; i < nGPUProgramsNum; ++i)
	{
		strName += pGPUPrograms[i]->GetName();
	}

	ShaderObjectMap::iterator it = m_mapShaderObjects.find(strName);

	if (it != m_mapShaderObjects.end())
	{
		return it->second;
	}

	ShaderObject* pShaderObject = CreateShaderObjectImpl();
	if (pShaderObject->Create(pGPUPrograms, nGPUProgramsNum))
	{
		m_mapShaderObjects.insert(std::make_pair(strName, pShaderObject));
	}
	else
	{
		SafeDelete(pShaderObject);
	}

	return pShaderObject;
}

void IRenderer::DestroyAllShaderObjects()
{
	for (ShaderObjectMap::iterator it = m_mapShaderObjects.begin(); it != m_mapShaderObjects.end(); ++it)
	{
		SafeDelete(it->second);
	}

	m_mapShaderObjects.clear();
}

void IRenderer::Release()
{
	SafeDelete(m_pScreenQuad);
	DestroyAllShaderObjects();

    for (MaterialTemplateMap::iterator it = m_mapMaterialTemplates.begin(); it != m_mapMaterialTemplates.end(); ++it)
    {
        MaterialTemplate* pTemplate = it->second;
        pTemplate->Clear();
        SafeDelete(pTemplate);
    }

    m_mapMaterialTemplates.clear();

	for (AIRMap<FixedString, RenderWindow*>::iterator it = m_mapRenderWindows.begin(); it != m_mapRenderWindows.end(); ++it)
	{
		RenderWindow* pWindow = it->second;
		pWindow->Clear();
		SafeDelete(pWindow);
	}

	m_mapRenderWindows.clear();
}

ShaderObject* IRenderer::CreateShaderObjectImpl()
{
    return new ShaderObject;
}

ShaderObject* IRenderer::CreateShaderObject()
{
    return new ShaderObject;
}

MaterialTemplate* IRenderer::CreateMaterialTemplate(const FixedString& strFile)
{
    MaterialTemplateMap::iterator it = m_mapMaterialTemplates.find(strFile);

    if (it != m_mapMaterialTemplates.end())
    {
        return it->second;
    }

    MaterialTemplate* pMaterialTemplate = new MaterialTemplate();

    CDataStream* pStream = FILESYSTEM->GetFileStream( strFile.AsCharPtr() );
    if (pMaterialTemplate->Create(pStream))
    {
        m_mapMaterialTemplates.insert(std::make_pair(strFile, pMaterialTemplate));
    }
    else
    {
        SafeDelete(pMaterialTemplate);
    }

    SafeDelete(pStream);

    return pMaterialTemplate;
}

MaterialTemplate* IRenderer::CreateMaterialTemplate(const FixedString& strFile, CDataStream* pDataStream)
{
	AIRString strKey = pDataStream->GetName() + AIRString("/") + strFile.AsCharPtr();
	MaterialTemplateMap::iterator it = m_mapMaterialTemplates.find(strKey.c_str());

	if (it != m_mapMaterialTemplates.end())
	{
		return it->second;
	}

	MaterialTemplate* pMaterialTemplate = new MaterialTemplate();

	if (pMaterialTemplate->LoadFromStream(pDataStream, strFile.AsCharPtr()))
	{
		m_mapMaterialTemplates.insert(std::make_pair(strKey.c_str(), pMaterialTemplate));
	}
	else
	{
		SafeDelete(pMaterialTemplate);
	}

	return pMaterialTemplate;
}

RenderWindow* IRenderer::GetRenderWindow(const char* szName)
{
	AIRMap<FixedString, RenderWindow*>::iterator it = m_mapRenderWindows.find(szName);

	if (it != m_mapRenderWindows.end())
	{
		return it->second;
	}

	return NULL;
}

void IRenderer::DrawScreenQuad(const AIR::Rectf& rectScreen, const AIR::Rectf& rectUV, f32 fUVScale, const CColorValue& color, IMaterial* pMaterial)
{
	if (m_pScreenQuad == NULL)
	{
		m_pScreenQuad = new ScreenQuad;
	}

	if (m_pScreenQuad->m_pPrimitive == NULL)
	{
		m_pScreenQuad->m_pPrimitive = new Primitive(Primitive::Primitive_TriangleList);
		m_pScreenQuad->m_pPrimitive->SetPrimitiveCount(2);
	}

	if (m_pScreenQuad->m_pVB == NULL)
	{
		m_pScreenQuad->m_pVB = CreateVertexBuffer(4, sizeof(RHWVERTEX));
	}
	f32 fOffset = GetPixelOffset();

	LPRHWVERTEX pVertex = (LPRHWVERTEX)m_pScreenQuad->m_pVB->Lock();

	//左上角
	pVertex[0].color = color;
	pVertex[0].rhw = 1.0f;
	pVertex[0].tu = 0.0f;
	pVertex[0].tv = 0.0f;
	pVertex[0].x = rectScreen.left - fOffset;
	pVertex[0].y = rectScreen.top - fOffset;
	pVertex[0].z = 1.0f;

	//右上角
	pVertex[1].color = color;
	pVertex[1].rhw = 1.0f;
	pVertex[1].tu = fUVScale;
	pVertex[1].tv = 0.0f;
	pVertex[1].x = rectScreen.right - fOffset;
	pVertex[1].y = rectScreen.top - fOffset;
	pVertex[1].z = 1.0f;

	//右下角
	pVertex[2].color = color;
	pVertex[2].rhw = 1.0f;
	pVertex[2].tu = fUVScale;
	pVertex[2].tv = fUVScale;
	pVertex[2].x = rectScreen.right - fOffset;
	pVertex[2].y = rectScreen.bottom - fOffset;
	pVertex[2].z = 1.0f;

	//左下角
	pVertex[3].color = color;
	pVertex[3].rhw = 1.0f;
	pVertex[3].tu = 0.0f;
	pVertex[3].tv = fUVScale;
	pVertex[3].x = rectScreen.left - fOffset;
	pVertex[3].y = rectScreen.bottom - fOffset;
	pVertex[3].z = 1.0f;
	m_pScreenQuad->m_pVB->Unlock();

	if (m_pScreenQuad->m_pIB == NULL)
	{
		m_pScreenQuad->m_pIB = CreateIndexBuffer(6);

		u16* pData = static_cast<u16*>(m_pScreenQuad->m_pIB->Lock( 0, 0, true ));
		int nCurIndex = 0;
		int nLT = 0;
		// 右上角的索引ID
		int nRT = nLT + 1;
		// 右下角的索引ID
		int nRB = nRT + 1;
		// 左下角的索引ID
		int nLB = nRB + 1;

		// 三角形0-1-3
		int nIndexPos = 0;
		pData[nIndexPos++] = nLT;
		pData[nIndexPos++] = nRT;
		pData[nIndexPos++] = nLB;

		// 三角形3-1-2
		pData[nIndexPos++] = nLB;
		pData[nIndexPos++] = nRT;
		pData[nIndexPos++] = nRB;
		
		// 解锁索引缓冲
		m_pScreenQuad->m_pIB->Unlock();
	}

	if (m_pScreenQuad->m_pPrimitive->m_pVertexBuffer == NULL)
	{
		m_pScreenQuad->m_pPrimitive->m_pVertexBuffer = m_pScreenQuad->m_pVB;
	}

	if (m_pScreenQuad->m_pPrimitive->m_pIndexBuffer == NULL)
	{
		m_pScreenQuad->m_pPrimitive->m_pIndexBuffer = m_pScreenQuad->m_pIB;
	}

	ShaderMarco macro = pMaterial->GetCurShaderMarco();
	size_t nTechIndex = pMaterial->GetTechniqueIndexByFlow(FLOW_SCENE);
	if (nTechIndex != -1)
	{
		RenderTechnique* pTechnique = pMaterial->GetRenderTechnique(macro, nTechIndex);

		//m_pRenderer->SetVertexDeclaration(m_pVertexDecl);

		if (pTechnique)
		{
			for (size_t i = 0; i < pTechnique->m_vtRenderPass.size(); ++i)
			{
				RenderPass* pPass = &pTechnique->m_vtRenderPass[i];
				pPass->pShaderObject->GetGPUProgram(eGST_Vertex)->CreateVertexDeclaration("VS_Pos4_color_texcoord");

				pMaterial->UpdateMaterialParams(pPass);

				if (!ENGINE_INST->GetUseVAO())
				{
					if (m_pScreenQuad->m_pPrimitive->m_pVertexBuffer)
						SetVertexBuffer(0, m_pScreenQuad->m_pPrimitive->m_pVertexBuffer);

					if (m_pScreenQuad->m_pPrimitive->m_pIndexBuffer)
						SetIndexBuffer(m_pScreenQuad->m_pPrimitive->m_pIndexBuffer);
				}
				else
				{
					IVertexArrayObject* vao = pTechnique->m_vtRenderPass[i].vertexArrayObj;
					if (!vao)
						CreateVAO(pPass, m_pScreenQuad->m_pVB, m_pScreenQuad->m_pIB);
					if (pTechnique->m_vtRenderPass[i].vertexArrayObj)
						SetVAO(pPass->vertexArrayObj);
				}

				DrawPrimitive(m_pScreenQuad->m_pPrimitive);
			}
		}
	}

	

	
}
