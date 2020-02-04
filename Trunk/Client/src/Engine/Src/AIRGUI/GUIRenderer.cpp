#include "GUIRenderer.h"
#include "GUIElement.h"
//#include "ImageSetManager.h"
#include "GUIBuilderMgr.h"
#include "GUISystem.h"
#include "StringUtil.h"
#include "Engine.h"
#include "GUISchemeManager.h"
#include "IGUIRenderer.h"
#include <assert.h>
#include "Primitive.h"
#include "VertexDeclarationManager.h"
//#include "Technique.h"
#include "GPUProgramManager.h"
#include "ShaderObject.h"
#include "MaterialManager.h"

//GUIRenderer*	GUIRenderer::GetSingletonPtr( void )
//{
//	static GUIRenderer m_Instance;
//	return &m_Instance;
//}


GUIRenderer::GUIRenderer() : 
m_nMaxQuadCountPerDP(200), 
m_nMaxVertexCountPerDP(800), 
m_nMaxIndexCountPerDP(1200), 
m_nCurTriangleCountToRender(0),
m_nCurIndexCountToRender(0),
m_nCurVertexCountToRender(0),
m_pGUIVB(NULL), 
m_pGUIIB(NULL), 
m_pCurTexture(NULL), 
m_pRenderer(NULL),
m_pVertexArray(NULL)
, m_pPrimitive(NULL)
//, m_pVertexDecl(NULL)
, m_fMinZ(1.0f)
, m_fMaxZ(10.0f)
//, m_pGUITechnique(NULL)
//, m_pSamplerState(NULL)
//, m_pUIMaterialTemplate(NULL)
, m_pMaterial(NULL)
//, m_pVertexDecl(NULL)
{

}

GUIRenderer::~GUIRenderer()
{
	Clear();
    SafeDelete(m_pPrimitive);
}

void GUIRenderer::SetRenderer( IRenderer* pRenderer )
{
	m_pRenderer = pRenderer;
}

void GUIRenderer::InitGUIRenderer( int nMaxQuadCountPerDP /*= 10000*/ )
{
	if ( !m_pRenderer )
	{
		DT_TO_MSGBOX_FMT( LT_ERROR, "渲染器未初始化,请先调用SetRenderer()指定渲染器!" );
		PrintDebugString( "Renderer not initialized, should call SetRenderer() at first.\n" );
		return;
	}

	// 一次DP所画的最大矩形数
	m_nMaxQuadCountPerDP = nMaxQuadCountPerDP;
	// 一次DP所画的最大顶点数
	m_nMaxVertexCountPerDP = m_nMaxQuadCountPerDP * 4;
	// 一次DP所画的最大索引数
	m_nMaxIndexCountPerDP = m_nMaxQuadCountPerDP * 2 * 3;
	// 当前要渲染的三角形数
	m_nCurTriangleCountToRender = 0;
	// 当前要渲染的索引数
	m_nCurIndexCountToRender = 0;	
	// 当前要渲染的顶点数
	m_nCurVertexCountToRender = 0;	

	// 释放顶点数组
	if ( m_pVertexArray )
	{
		AIR_DELETE_ARRAY_T(m_pVertexArray, RHWVERTEX, m_nMaxVertexCountPerDP);
	}
	// 创建顶点数组
	m_pVertexArray = AIR_NEW_ARRAY_T(RHWVERTEX, m_nMaxVertexCountPerDP); // RHWVERTEX[m_nMaxVertexCountPerDP]();
	memset( m_pVertexArray, 0x0, sizeof(RHWVERTEX) * m_nMaxVertexCountPerDP );

	// 释放索引缓冲
	if ( m_pGUIIB )
	{
		SafeDelete( m_pGUIIB );
	}
	// 释放顶点缓冲
	if ( m_pGUIVB )
	{
		SafeDelete( m_pGUIVB );
	}

	// 重新创建顶点缓冲
    m_pGUIVB = m_pRenderer->CreateVertexBuffer( m_pVertexArray, m_nMaxVertexCountPerDP, sizeof(RHWVERTEX), true );
	if ( !m_pGUIVB )
	{
		DT_TO_MSGBOX_FMT( LT_ERROR, "创建GUI顶点缓冲失败!" );
		PrintDebugString( "create GUI vertex buffer failed!\n" );
		return;
	}

	// 重新创建索引缓冲
	m_pGUIIB = m_pRenderer->CreateIndexBuffer( m_nMaxIndexCountPerDP, false );
	if ( !m_pGUIIB )
	{
		DT_TO_MSGBOX_FMT( LT_ERROR, "创建GUI索引缓冲失败!" );
		PrintDebugString( "create GUI index buffer failed!\n" );
		return;
	}

	// 锁定索引缓冲,并填充数据
	u16* pData = static_cast<u16*>(m_pGUIIB->Lock( 0, 0, true ));
	int nCurIndex = 0;
	// 遍历所有的矩形
	for ( int i = 0; i < m_nMaxQuadCountPerDP; ++i )
	{
		// 将矩形的6个索引值加到缓冲里面
		AddQuadIndex( i, &pData, nCurIndex );
	}
	// 解锁索引缓冲
	m_pGUIIB->Unlock();

    if (m_pMaterial == NULL)
    {
        m_pMaterial = MATERIAL_MANAGER->CreateMaterial("TemplateMaterial.xml/UI");
    }
}


void GUIRenderer::Clear( void )
{
    if (m_pMaterial)
    {
        MATERIAL_MANAGER->ClearMaterial(m_pMaterial);
    }
	if ( m_pGUIIB )
	{
		m_pRenderer->ClearIndexBuffer(m_pGUIIB);
        m_pGUIIB = 0;
	}
	if ( m_pGUIVB )
	{
        m_pRenderer->ClearVertexBuffer(m_pGUIVB);
        m_pGUIVB = NULL;
		//SafeDelete( m_pGUIVB );
	}
	if ( m_pVertexArray )
	{
        AIR_DELETE_ARRAY_T(m_pVertexArray, RHWVERTEX, m_nMaxVertexCountPerDP);
        m_pVertexArray = NULL;
		//SafeDeleteArray( m_pVertexArray );
	}


 //   SafeDelete(m_pSamplerState);

	//if (m_pGUITechnique)
	//{
	//	m_pGUITechnique->Clear();
	//	SafeDelete(m_pGUITechnique);
	//}
}

void GUIRenderer::BeginGUIRender()
{
    /*
    Matrix4f matProj;
    int nWidth, nHeight;
    m_pRenderer->GetBackBufferSize(nWidth, nHeight);
#ifdef LEFT_HAND_COORDINATE
    matProj.BuildProjectionMatrixPerspectiveFovLH(AIR::PI / 4, (f32)nWidth / nHeight, m_fMinZ, m_fMaxZ);
#else
    matProj.BuildProjectionMatrixPerspectiveFovRH(AIR::PI / 4,(f32) nWidth / nHeight, m_fMinZ, m_fMaxZ);
#endif
    m_pRenderer->SetProjectionTransform(&matProj);
    */
	//m_pRenderer->BeginGUIRenderStates();
	m_pCurTexture = NULL;
	m_nCurTriangleCountToRender = 0;
	m_nCurIndexCountToRender = 0;
	m_nCurVertexCountToRender = 0;
}

void GUIRenderer::EndGUIRender()
{
	// 如果缓冲还有数据,就渲染出来
	if ( m_nCurVertexCountToRender > 0 )
	{
		RenderImmediately();
	}
	//m_pRenderer->EndGUIRenderStates();
}

void GUIRenderer::Render( const AIR::Rectf& rectScreen, const AIR::Rectf& rectUV, f32 fDepth, ITexture* pTex, const CColorValue& color)
{
	// 如果当前是从顶点数组的起始位置填充
    if (m_pMaterial)
    {
        if (!m_pMaterial->IsReady())
        {
            if (!m_pMaterial->Cache())
            {
                return;
            }
        }
    }
	if ( 0 == m_nCurVertexCountToRender )
	{
		// 记录当前纹理
		m_pCurTexture = pTex;
		// 填充顶点
		AddQuadVertex( rectScreen, rectUV, color, fDepth );
	}
	// 如果当前不是从顶点数组的起始位置填充
	else if ( m_nCurVertexCountToRender < m_nMaxVertexCountPerDP )
	{
		// 如果当前的纹理不一致,现将缓冲渲染出来
		if ( m_pCurTexture != pTex )
		{
			RenderImmediately();
			m_pCurTexture = pTex;
		}
		// 填充顶点数据
		AddQuadVertex( rectScreen, rectUV, color, fDepth );
	}

	// 如果当前缓冲已经满了,就将缓冲区绘制出来,并清空
	if ( m_nCurVertexCountToRender == m_nMaxVertexCountPerDP )
	{
		RenderImmediately();
	}
}

void GUIRenderer::Render(GUIRenderQuad* pQuad)
{
    AddQuadVertex(pQuad);
}

void GUIRenderer::EnableScissor( const RECT& scissorrect )
{
    RenderImmediately();
    m_pRenderer->SetScissorEnable(true);
    m_pRenderer->SetScissorRect(&scissorrect);
}

void GUIRenderer::DisableScissor( void )
{
    RenderImmediately();
    m_pRenderer->SetScissorEnable( false );
}


void GUIRenderer::RenderImmediately( void )
{
	// 上传顶点数据到显存
    if (m_nCurVertexCountToRender == 0)
    {
        return;
    }
	//void* pData = m_pGUIVB->Lock( true );
	//memcpy( pData, m_pVertexArray, sizeof(RHWVERTEX) * m_nCurVertexCountToRender );
	//m_pGUIVB->Unlock();
	m_pGUIVB->WriteData(0, sizeof(RHWVERTEX) * m_nCurVertexCountToRender, m_pVertexArray, true);

	// 设置纹理
    //m_pRenderer->SetTextureBlendMode(0, m_colorBlend);
    //m_pRenderer->SetTextureBlendMode(0, m_alphaBlend);
	//m_pRenderer->SetTexture( 0, m_pCurTexture );
    //m_pSamplerState->Apply(m_pRenderer, m_pCurTexture);
    GPUVariant var;
    var.SetTexture(m_pCurTexture);
    m_pMaterial->SetMaterialParam("SampUI", var);

	GPUVariant var2;
	var2.SetInt( (m_pCurTexture != NULL) ? 1 : 0 );
	m_pMaterial->SetMaterialParam("UseTex", var2);

    if (m_pPrimitive == NULL)
    {
        m_pPrimitive = new Primitive(Primitive::Primitive_TriangleList/*, m_pGUIVB->GetVertexDeclaration()*/);
        m_pPrimitive->m_pVertexBuffer = m_pGUIVB;
        m_pPrimitive->m_pIndexBuffer = m_pGUIIB;
        m_pPrimitive->SetUseUserPrimitive(true);
    }

	//if (m_pVertexDecl == NULL)
	//{
	//	m_pVertexDecl = VERTEXDECL_MANAGER->CreateVertexDeclaration("VS_Pos4_color_texcoord");
	//}

	Matrix4f matProj;
	Vector2D dimensionScreen;
	m_pRenderer->GetBackBufferSize(dimensionScreen);
	Matrix4f matTemp;
	matTemp.Identity();
	matTemp.SetScale(Vector3Df(1.0f, -1.0f, 1.0f));
	//matProj.BuildProjectionMatrixOrthoOffCenterRH(0, dimensionScreen.Width, 0, dimensionScreen.Height, 1, -1);
	matProj.BuildProjectionMatrixOrthoOffCenterLH(0, dimensionScreen.x, 0, dimensionScreen.y, 0.0f, 1000.0f);
	matProj = matProj * matTemp;
	m_pRenderer->SetProjectionTransform(&matProj);

	Matrix4f matView;
	matView.BuildCameraLookAtMatrixLH(Vector3Df(0, 0, 10), Vector3Df(0, 0, 1), Vector3Df(0, 1, 0));
	m_pRenderer->SetViewTransform(&matView);
    
    //m_pRenderer->SetVertexDeclaration(m_pVertexDecl);
    m_pPrimitive->SetPrimitiveCount(m_nCurTriangleCountToRender);

    /*
    m_pGUITechnique->GetPass(0)->GetShaderObject()->BeginUse();
    m_pGUITechnique->GetPass(0)->ApplyPass(m_pRenderer);
    m_pPrimitive->Draw(m_pGUITechnique);
    */

    /*
    GetRenderPass();

    m_renderPass.pShaderObject->BeginUse();


    m_renderPass.pShaderObject->Apply(m_pRenderer);

    //pPass->ApplyPass(pRenderer);
    //设置渲染状态
    m_pRenderer->SetRenderState(m_renderPass.blendState, m_renderPass.rasterizerState, m_renderPass.depthStencilState, m_renderPass.fixedPipeState);

    //下面是更新sampler，主要是提交samplerstate
    m_pRenderer->SetSamplerState(0, m_samplerState);

    m_pMaterial->UpdateMaterialParams(&m_renderPass);
    */

	if( ENGINE_INST->GetUseVAO() == false )
	{
		m_pRenderer->SetVertexBuffer(0, m_pGUIVB);
		m_pRenderer->SetIndexBuffer(m_pGUIIB);
	}

    ShaderMarco macro = m_pMaterial->GetCurShaderMarco();

    //获得材质当前使用的technique
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
				if( ENGINE_INST->GetUseVAO() )
				{
					IVertexArrayObject* vao = pTechnique->m_vtRenderPass[i].vertexArrayObj;
					if( !vao )
						ENGINE_INST->GetRenderer()->CreateVAO(pPass,m_pGUIVB, m_pGUIIB);
					if( pTechnique->m_vtRenderPass[i].vertexArrayObj )
						ENGINE_INST->GetRenderer()->SetVAO( pPass->vertexArrayObj );
				}

                m_pMaterial->UpdateMaterialParams(pPass);

				m_pRenderer->DrawPrimitive(m_pPrimitive);
            }
        }
    }

	// 画缓冲
	//m_pRenderer->DrawTriangleListByIndex( 0, 0, m_nCurVertexCountToRender, 0, /*m_pGUIVB, m_pGUIIB, */m_nCurTriangleCountToRender );
	m_nCurVertexCountToRender = 0;
	m_nCurTriangleCountToRender = 0;
	m_nCurIndexCountToRender = 0;
}

void GUIRenderer::RenderImmediatelyWithDepth()
{
    ITexture* pTex = NULL;
    GUIRenderQuad* pQuad = NULL;
    for (RenderQuadListMap_Iter it = m_mapRenderQuadList.begin(); it != m_mapRenderQuadList.end(); ++it)
    {
        pTex = it->first;
        m_pRenderer->SetTexture( 0, pTex );
        RenderQuad_List* pList = &it->second;

        //LPRHWVERTEX pVertex = (LPRHWVERTEX)(m_pGUIVB->Lock( true ));
        for (RenderQuadList_Iter it = pList->begin(); it != pList->end(); ++it)
        {
            pQuad = *it;

            Render(pQuad->m_rectScreen, pQuad->m_uv, pQuad->m_fDepth, pQuad->m_pTexture, pQuad->m_clrDiffuse);
        }

        pList->clear();
    }

    RenderImmediately();
    
    m_nCurVertexCountToRender = 0;
    m_nCurTriangleCountToRender = 0;
    m_nCurIndexCountToRender = 0;
}

//
// nLT : 0--------1  nRT
//        |      |
//        |      |
// nLB : 3--------2  nRB
void GUIRenderer::AddQuadIndex( int nQuadID, u16** ppData, int & nIndexPos )
{
	// 左上角的索引ID
	int nLT = nQuadID * 4;
	// 右上角的索引ID
	int nRT = nLT + 1;
	// 右下角的索引ID
	int nRB = nRT + 1;
	// 左下角的索引ID
	int nLB = nRB + 1;

	// 三角形0-1-3
	(*ppData)[nIndexPos++] = nLT;
	(*ppData)[nIndexPos++] = nRT;
	(*ppData)[nIndexPos++] = nLB;

	// 三角形3-1-2
	(*ppData)[nIndexPos++] = nLB;
	(*ppData)[nIndexPos++] = nRT;
	(*ppData)[nIndexPos++] = nRB;
}

void GUIRenderer::AddQuadVertex( const AIR::Rectf& rectScreen, const AIR::Rectf& rectUV, const CColorValue& color, f32 fDepth )
{
	// 顶点数增加4个
	// 左上角的顶点
	float fPixelOffset = RENDERER->GetPixelOffset();
	m_pVertexArray[m_nCurVertexCountToRender].x		= rectScreen.left - fPixelOffset;
	m_pVertexArray[m_nCurVertexCountToRender].y		= rectScreen.top - fPixelOffset;
	m_pVertexArray[m_nCurVertexCountToRender].z		= fDepth;
	m_pVertexArray[m_nCurVertexCountToRender].rhw	= 1.0f;
	m_pVertexArray[m_nCurVertexCountToRender].color = color;
	m_pVertexArray[m_nCurVertexCountToRender].tu	= rectUV.left;
	m_pVertexArray[m_nCurVertexCountToRender++].tv	= rectUV.top;

	// 右上角的顶点
	m_pVertexArray[m_nCurVertexCountToRender].x		= rectScreen.right - fPixelOffset;
	m_pVertexArray[m_nCurVertexCountToRender].y		= rectScreen.top - fPixelOffset;
	m_pVertexArray[m_nCurVertexCountToRender].z		= fDepth;
	m_pVertexArray[m_nCurVertexCountToRender].rhw	= 1.0f;
	m_pVertexArray[m_nCurVertexCountToRender].color = color;
	m_pVertexArray[m_nCurVertexCountToRender].tu	= rectUV.right;
	m_pVertexArray[m_nCurVertexCountToRender++].tv	= rectUV.top;

	// 右下角的顶点
	m_pVertexArray[m_nCurVertexCountToRender].x		= rectScreen.right - fPixelOffset;
	m_pVertexArray[m_nCurVertexCountToRender].y		= rectScreen.bottom - fPixelOffset;
	m_pVertexArray[m_nCurVertexCountToRender].z		= fDepth;
	m_pVertexArray[m_nCurVertexCountToRender].rhw	= 1.0f;
	m_pVertexArray[m_nCurVertexCountToRender].color = color;
	m_pVertexArray[m_nCurVertexCountToRender].tu	= rectUV.right;
	m_pVertexArray[m_nCurVertexCountToRender++].tv	= rectUV.bottom;

	// 左下角的顶点
	m_pVertexArray[m_nCurVertexCountToRender].x		= rectScreen.left - fPixelOffset;
	m_pVertexArray[m_nCurVertexCountToRender].y		= rectScreen.bottom - fPixelOffset;
	m_pVertexArray[m_nCurVertexCountToRender].z		= fDepth;
	m_pVertexArray[m_nCurVertexCountToRender].rhw	= 1.0f;
	m_pVertexArray[m_nCurVertexCountToRender].color = color;
	m_pVertexArray[m_nCurVertexCountToRender].tu	= rectUV.left;
	m_pVertexArray[m_nCurVertexCountToRender++].tv	= rectUV.bottom;

	// 索引数增加6个
	m_nCurIndexCountToRender += 6;

	// 三角形数增加2个
	m_nCurTriangleCountToRender += 2;
}

void GUIRenderer::AddQuadVertex(GUIRenderQuad* pRenderQuad)
{
    RenderQuadListMap_Iter it = m_mapRenderQuadList.find(pRenderQuad->m_pTexture);
    if (it == m_mapRenderQuadList.end())
    {
        RenderQuad_List listQuad;
        listQuad.push_back(pRenderQuad);
        m_mapRenderQuadList.insert(std::make_pair(pRenderQuad->m_pTexture, listQuad));
    }
    else
    {
        RenderQuad_List* pListQuad = &it->second;
        pListQuad->push_back(pRenderQuad);
    }

    // 索引数增加6个
    m_nCurIndexCountToRender += 6;

    // 三角形数增加2个
    m_nCurTriangleCountToRender += 2;
}

void GUIRenderer::GetRenderPass()
{
    /*
    if (m_pUIMaterialTemplate)
    {
        if (m_renderPass.pShaderObject == NULL)
        {
            IGPUProgram* pVertexShader = NULL;
            IGPUProgram* pPixelShader = NULL;

            ShaderMarco marco;
            marco.Reset();

            m_pUIMaterialTemplate->GetShader(marco, 0, 0, &pVertexShader, &pPixelShader);

            ShaderObject* pShaderObject = m_pUIMaterialTemplate->CreateShaderObject(pVertexShader, pPixelShader);//pRenderer->CreateShaderObject(pGpuPrograms, 2);
            m_renderPass.pShaderObject = pShaderObject;

            m_pUIMaterialTemplate->GetRenderState(0, 0, m_renderPass.blendState, m_renderPass.rasterizerState, m_renderPass.depthStencilState, m_renderPass.fixedPipeState);

            m_samplerState.texAddr = TAV_Warp;
            m_samplerState.texFilter = TFT_Point;
        }
    }
    */
}
