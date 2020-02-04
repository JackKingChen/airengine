#include "stdafx.h"
#include "ScreenCompositor.h"
#include "Engine.h"
//#include "PostScreenCompositor.h"
#include "MaterialManager.h"
#include "VertexDeclarationManager.h"
#include "Primitive.h"
#include "MathUtil.h"
#include "ShaderObject.h"

ScreenCompositor::ScreenCompositor( void ) : m_bActive(false)
, m_bFullWindow(false)
//, m_pVertexDecl(NULL)
, m_funcAfterRender(NULL)
, m_pscType(ePPT_None)
, m_fScale(1.0f)
, m_fUVScale(1.0f)
, m_pTexture(NULL)
, m_pSceneTexture(NULL)
, m_pMaterial(NULL)
{
	m_color.Set( 0xffffffff );
}

ScreenCompositor::~ScreenCompositor( void )
{

}

/*
void					ScreenCompositor::Render(CCamera* pCamera)
{
	if ( !this->m_bVisable )
	{
		return;
	}
	PreRender(pCamera);

	IRenderer* pRenderer = ENGINE_INST->GetRenderer();
	pRenderer->SetWorldTransform( &Matrix4f::IDENTITY );

	if (m_pMaterial)
	{
		//--------------------------------------------------
		//! add by hjj 2010-6-12
		//pRenderer->SetRenderState( D3DRS_FOGENABLE, FALSE );
		//--------------------------------------------------
		CTechnique* pTechnique = m_pMaterial->GetCurrentTechnique();
		if (pTechnique)
		{
			//pRenderer->SetVertexBuffer( 0, m_renderData.m_pVB );
            pRenderer->SetVertexDeclaration(m_pVertexDecl);

			size_t nPassNum = pTechnique->GetPassesNum();
			for (size_t i = 0; i < nPassNum; ++i)
			{
				CPass* pPass = pTechnique->GetPass(i);
				pPass->ApplyPass(pRenderer);
                pRenderer->DrawTriangleStrip(0, 2, &m_renderQuad[0], E_VB_RHW);
				//pRenderer->DrawTriangleStrip( 0, m_renderData.m_nPrimCount );
			}
		}
		//--------------------------------------------------
		//! add by hjj 2010-6-12
		//pRenderer->SetRenderState( D3DRS_FOGENABLE, TRUE );
		//--------------------------------------------------
	}

	if ( m_funcAfterRender )
	{
		m_funcAfterRender();
	}

	Renderable::AfterRender();
}


void ScreenCompositor::_GetPrimitive()
{
    if (m_pPrimitive == NULL)
    {
        m_pPrimitive = new Primitive(Primitive::Primitive_TriangleStrip);
        m_pPrimitive->SetUseUserPrimitive(true);
        m_pPrimitive->SetPrimitiveCount(2);
        m_pPrimitive->m_pVertexData = &m_renderQuad[0];
        m_pPrimitive->m_nVerticesStride = sizeof(RHWVERTEX);
        m_pPrimitive->m_pSrcRenderable = this;
		m_pPrimitive->SetVerticesNum(4);
    }
}
*/
void ScreenCompositor::Clear()
{
	//Renderable::Clear();
	if (m_pMaterial)
	{
		MATERIAL_MANAGER->ClearMaterial(m_pMaterial);
	}
}

void ScreenCompositor::Create( ScreenCompositorType type, int nPosX, int nPosY, int nWidth, int nHeight )
{
	m_pscType = type;
	m_rcScreen.SetRect( nPosX, nPosY, nPosX+nWidth, nPosY+nHeight );
    if (m_pMaterial != NULL)
    {
		MATERIAL_MANAGER->ClearMaterial(m_pMaterial);
        
    }
	RebuildRenderData();
	switch (m_pscType)
	{
	case ePPT_None:
		m_pMaterial = MATERIAL_MANAGER->CreateMaterial("CompositorMaterial.xml/Compositor");
		break;
	case ePPT_COLOR_UpFilter4:
		m_pMaterial = MATERIAL_MANAGER->CreateMaterial("CompositorMaterial.xml/UpFilter4");
		break;
	case ePPT_COLOR_DownFilter4:
		m_pMaterial = MATERIAL_MANAGER->CreateMaterial("CompositorMaterial.xml/DownFilter4");
		break;
	case ePPT_COLOR_BlurH:
		m_pMaterial = MATERIAL_MANAGER->CreateMaterial("CompositorMaterial.xml/Blur");
		break;
	case ePPT_COLOR_BlurV:
		m_pMaterial = MATERIAL_MANAGER->CreateMaterial("CompositorMaterial.xml/Blur");
		break;
	case ePPT_COLOR_ColorBurn:
		m_pMaterial = MATERIAL_MANAGER->CreateMaterial("CompositorMaterial.xml/ColorBurnCombine");
		break;
	case ePPT_COLOR_ScreenCombine:
		m_pMaterial = MATERIAL_MANAGER->CreateMaterial("CompositorMaterial.xml/ScreenCombine");
		break;
	case ePPT_COLOR_SoftLightCombine:
		m_pMaterial = MATERIAL_MANAGER->CreateMaterial("CompositorMaterial.xml/SoftLightCombine");
		break;
	}
}

void ScreenCompositor::SetWindowPosition(int nPosX, int nPosY, int nWidth, int nHeight)
{
    //m_strName = szName;
    m_rcScreen.SetRect( nPosX, nPosY, nPosX + nWidth, nPosY + nHeight );
    RebuildRenderData();
}
/*
void ScreenCompositor::PreRender(CCamera* pCamera)
{
    //if (m_pVertexDecl == NULL)
    //{
    //    CreateVertexDeclaration();
    //}
    if (m_bFullWindow)
    {
        int nWidth, nHeight;
        ENGINE_INST->GetBackBufferSize(nWidth, nHeight);

        if (nWidth != (int)m_rcScreen.GetWidth() || nHeight != (int)m_rcScreen.GetHeight())
        {
            m_rcScreen.SetRect(0, 0, nWidth, nHeight);
            RebuildRenderData();
        }
    }

	if (m_pTexture)
	{
		m_pTexture->Apply(0);
	}
}
*/
void ScreenCompositor::RebuildRenderData( void )
{
	f32 fOffset = RENDERER->GetPixelOffset();

    LPRHWVERTEX pVertex = &m_renderQuad[0];
    pVertex[0].color = m_color;
    pVertex[0].rhw = 1.0f;
    pVertex[0].tu = 0.0f;
    pVertex[0].tv = 0.0f;
    pVertex[0].x = m_rcScreen.left - fOffset;
    pVertex[0].y = m_rcScreen.top - fOffset;
    pVertex[0].z = 1.0f;

    pVertex[1].color = m_color;
    pVertex[1].rhw = 1.0f;
    pVertex[1].tu = 1.0f;
    pVertex[1].tv = 0.0f;
    pVertex[1].x = m_rcScreen.right - fOffset;
    pVertex[1].y = m_rcScreen.top - fOffset;
    pVertex[1].z = 1.0f;

    pVertex[2].color = m_color;
    pVertex[2].rhw = 1.0f;
    pVertex[2].tu = 0.0f;
    pVertex[2].tv = 1.0f;
    pVertex[2].x = m_rcScreen.left - fOffset;
    pVertex[2].y = m_rcScreen.bottom - fOffset;
    pVertex[2].z = 1.0f;

    pVertex[3].color = m_color;
    pVertex[3].rhw = 1.0f;
    pVertex[3].tu = 1.0f;
    pVertex[3].tv = 1.0f;
    pVertex[3].x = m_rcScreen.right - fOffset;
    pVertex[3].y = m_rcScreen.bottom - fOffset;
    pVertex[3].z = 1.0f;
}

void ScreenCompositor::Draw()
{
	if (m_pMaterial)
	{
		if (!m_pMaterial->IsReady())
		{
			m_pMaterial->Cache();
		}

		if (m_pMaterial->IsReady())
		{
			GPUVariant var(m_pTexture);
			m_pMaterial->SetMaterialParam("SampUI", var);

			switch (m_pscType)
			{
			case ePPT_None:
				break;
			case ePPT_COLOR_BlurH:
				{
					m_pMaterial->SetMaterialParam("srcColor", var);
					GPUVariant varSam(m_pSceneTexture);
					m_pMaterial->SetMaterialParam("sceneColor", varSam);
					UpdateBlurGPUData(true);
					break;
				}
				
			case ePPT_COLOR_BlurV:
				{
					m_pMaterial->SetMaterialParam("srcColor", var);
					GPUVariant varSam(m_pSceneTexture);
					m_pMaterial->SetMaterialParam("sceneColor", varSam);
					UpdateBlurGPUData(false);
				}
				
				break;
			case ePPT_COLOR_DownFilter4:
				m_pMaterial->SetMaterialParam("srcColor", var);
				UpdateDownFilter4GPUData();
				break;
			case ePPT_COLOR_UpFilter4:
				m_pMaterial->SetMaterialParam("srcColor", var);
				break;
			case ePPT_COLOR_ScreenCombine:
			case ePPT_COLOR_ColorBurn:
			case ePPT_COLOR_SoftLightCombine:
				{
					m_pMaterial->SetMaterialParam("srcColor", var);
					GPUVariant varSam(m_pSceneTexture);
					m_pMaterial->SetMaterialParam("sceneColor", varSam);
				}
				break;
			}

			IRenderer* pRenderer = ENGINE_INST->GetRenderer();
			Matrix4f matTemp;
			matTemp.Identity();
			matTemp.SetScale(Vector3Df(1.0f, -1.0f, 1.0f));
			Matrix4f matProj;
			//Dimension2Di dimensionScreen;
			//pRenderer->GetBackBufferSize(dimensionScreen);
			//matProj.BuildProjectionMatrixOrthoOffCenterRH(0, dimensionScreen.Width, 0, dimensionScreen.Height, 1, -1);
			matProj.BuildProjectionMatrixOrthoOffCenterLH(0, m_rcScreen.GetWidth(), 0, m_rcScreen.GetHeight(), 0.0f, 1000.0f);
			matProj = matProj * matTemp;
			pRenderer->SetProjectionTransform(&matProj);

			Matrix4f matView;
			matView.BuildCameraLookAtMatrixLH(Vector3Df(0, 0, 10), Vector3Df(0, 0, 1), Vector3Df(0, 1, 0));
			pRenderer->SetViewTransform(&matView);
			pRenderer->SetWorldTransform( &Matrix4f::IDENTITY );

			Rectf uvRect;
			uvRect.top = 0.0f;
			uvRect.left = 0.0f;
			uvRect.bottom = m_fUVScale;
			uvRect.right = m_fUVScale;
			pRenderer->DrawScreenQuad(m_rcScreen, uvRect, 1.0f, CColorValue::White, m_pMaterial);

			/*
			ShaderMarco macro = m_pMaterial->GetCurShaderMarco();
			size_t nTechIndex = m_pMaterial->GetTechniqueIndexByFlow(FLOW_SCENE);
			if (nTechIndex != -1)
			{
				RenderTechnique* pTechnique = m_pMaterial->GetRenderTechnique(macro, nTechIndex);

				//m_pRenderer->SetVertexDeclaration(m_pVertexDecl);

				if (pTechnique)
				{
					for (size_t i = 0; i < pTechnique->m_vtRenderPass.size(); ++i)
					{
						RenderPass* pPass = &pTechnique->m_vtRenderPass[i];
						pPass->pShaderObject->GetGPUProgram(eGST_Vertex)->CreateVertexDeclaration("VS_Pos4_color_texcoord");

						m_pMaterial->UpdateMaterialParams(pPass);
						Rectf uvRect;
						uvRect.top = 0.0f;
						uvRect.left = 0.0f;
						uvRect.bottom = m_fUVScale;
						uvRect.right = m_fUVScale;
						pRenderer->DrawScreenQuad(m_rcScreen, uvRect, 1.0f);
					}
				}
			}
			*/
		}
	}
}

void ScreenCompositor::UpdateBlurGPUData(bool bHorizontal)
{
	//PostCompositorManualGPUDataSource* pPostManualGPUSource = POSTPROCESS_MANAGER->GetPostManualGPUDataSource();
	//AIRVector<Vector2Df>& vtSamplerOffset = pPostManualGPUSource->GetBlurSamplerOffset();
	//AIRVector<float>& vtBlurWeight = pPostManualGPUSource->GetBlurWeight();
	if (m_vtBlurSamplerOffset.empty())
	{
		m_vtBlurSamplerOffset.resize(7);
		GPUVariant var;
		var.SetPointer(&m_vtBlurSamplerOffset[0]);
		m_pMaterial->SetMaterialParam("TexelKernel", var);
	}

	if (m_vtBlurWeight.empty())
	{
		m_vtBlurWeight.resize(7);
		GPUVariant var;
		var.SetPointer(&m_vtBlurWeight[0]);

		m_pMaterial->SetMaterialParam("BlurWeights", var);
	}
	int nWidth, nHeight;
	ENGINE_INST->GetBackBufferSize(nWidth, nHeight);
	CMathUtil::GetGaussianOffsets(bHorizontal, Vector2Df(1.0f/nWidth, 1.0f/nHeight), m_vtBlurSamplerOffset, m_vtBlurWeight, m_vtBlurWeight.size(), 2.0f);
}


void ScreenCompositor::UpdateDownFilter4GPUData()
{
	float fPixelOffset[ 32 ] = 
	{
		1.5,  -1.5 ,
		1.5,  -0.5 ,
		1.5,   0.5 ,
		1.5,   1.5 ,

		0.5,  -1.5 ,
		0.5,  -0.5 ,
		0.5,   0.5 ,
		0.5,   1.5 ,

		-0.5,  -1.5 ,
		-0.5,  -0.5 ,
		-0.5,   0.5 ,
		-0.5,   1.5 ,

		-1.5,  -1.5 ,
		-1.5,  -0.5 ,
		-1.5,   0.5 ,
		-1.5,   1.5 ,
	};

	//PostCompositorManualGPUDataSource* pPostManualGPUSource = POSTPROCESS_MANAGER->GetPostManualGPUDataSource();
	//AIRVector<Vector2Df>& vtSamplerOffset = pPostManualGPUSource->GetDownFilter4SamplerOffset();
	if (m_vtDownFilter4SamplerOffset.empty())
	{
		m_vtDownFilter4SamplerOffset.resize(16);
		GPUVariant var;
		var.SetPointer(&m_vtDownFilter4SamplerOffset[0]);
		m_pMaterial->SetMaterialParam("TexCoordsDownFilter", var);
	}
	int nWidth, nHeight;
	ENGINE_INST->GetBackBufferSize(nWidth, nHeight);
	for ( int i = 0 ; i < 16 ; i++)
	{
		m_vtDownFilter4SamplerOffset[i] =  Vector4f( fPixelOffset[i*2] / float(nWidth), fPixelOffset[i*2 + 1] / float(nHeight), 0.0f, 0.0f);
	}
}

/*
void ScreenCompositor::PostRender(CCamera* pCamera)
{
    PreRender(pCamera);
    //IRenderer* pRenderer = ENGINE_INST->GetRenderer();
    //pRenderer->SetVertexDeclaration(m_pVertexDecl);
    //pRenderer->DrawTriangleStrip(0, 2, &m_renderQuad[0], E_VB_RHW);
    _GetPrimitive();
    //m_pPrimitive->Draw(NULL);
}
*/


//--------------------------------------------------------

ScreenCompositorManager::ScreenCompositorManager( void )
{
	//m_mapScreenCompositor.clear();
}

ScreenCompositorManager::~ScreenCompositorManager( void )
{
	DestroyAllScreenCompositor();
}

ScreenCompositor* ScreenCompositorManager::CreateDefaultScreenCompositor( const AIRString& szName )
{
	ScreenCompositor* pSC = GetScreenCompositor( szName );
	if ( !pSC )
	{
		pSC = new ScreenCompositor();
		//pSC->SetName( szName );
		m_mapScreenCompositor.insert( std::make_pair(szName, pSC) );
	}
	return pSC;
}

ScreenCompositor* ScreenCompositorManager::GetScreenCompositor( const AIRString& szName )
{
	MAP_SCREEN_COMPOSITOR_ITOR it = m_mapScreenCompositor.find( szName );
	if ( it != m_mapScreenCompositor.end() )
	{
		return it->second;
	}
	return NULL;
}

void ScreenCompositorManager::RenderAllScreenCompositor( void )
{
	MAP_SCREEN_COMPOSITOR_ITOR it = m_mapScreenCompositor.begin();
	for ( ; it != m_mapScreenCompositor.end(); ++it )
	{
		//it->second->Render(NULL);
	}
}

void ScreenCompositorManager::DestroyAllScreenCompositor( void )
{
	MAP_SCREEN_COMPOSITOR_ITOR it = m_mapScreenCompositor.begin();
	for ( ; it != m_mapScreenCompositor.end(); ++it )
	{
		it->second->Clear();
		SafeDelete( it->second );
	}
	m_mapScreenCompositor.clear();
}

void ScreenCompositorManager::DestroyScreenCompositor( const AIRString& szName )
{
	MAP_SCREEN_COMPOSITOR_ITOR it = m_mapScreenCompositor.find( szName );
	if ( it != m_mapScreenCompositor.end() )
	{
		it->second->Clear();
		SafeDelete( it->second );
		m_mapScreenCompositor.erase( it );
	}
}

ScreenCompositor* ScreenCompositorManager::CreateScreenCompositor(const AIRString& strName, const AIRString& strType)
{
    ScreenCompositor* pCompositor = GetScreenCompositor(strName);
    if (pCompositor != NULL)
    {
        return pCompositor;
    }
    //if (strType == "PostScreenCompositor")
    //{
    //    pCompositor = new PostScreenCompositor();
    //}
    //else 
    {
        pCompositor = new ScreenCompositor;
    }

    //pCompositor->SetName(strName);
    m_mapScreenCompositor.insert(make_pair(strName, pCompositor));

    return pCompositor;
}

void ScreenCompositorManager::DestoryScreenCompositor(ScreenCompositor* pCompositor )
{
    //DestroyScreenCompositor(pCompositor->GetName());
}
