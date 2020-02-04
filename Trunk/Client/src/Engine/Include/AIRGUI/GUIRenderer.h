// ***************************************************************
//  文件: GUIRenderer.h
//  日期: 2010-2-1
//  作者: huangjunjie
//  说明: GUI渲染器
// ***************************************************************


#pragma once

#ifndef __GUIRenderer_H__
#define __GUIRenderer_H__


//class	IGUIRenderer;
class	ITexture;
class	SColor;
class	IVertexBuffer;
class	IIndexBuffer;
class	GUIRenderer;
class	IRenderer;
class   Primitive;
class   MaterialTemplate;


#include "IGUIRenderer.h"
#include "Engine.h"
#include "GUIElement.h"
#include "VertexDef.h"
//#include "TextureUnit.h"
#include "IMaterial.h"

class AIRGuiExport GUIRenderer : public IGUIRenderer
{
private:
	

public:
    GUIRenderer();
	//static GUIRenderer* GetSingletonPtr( void );

	virtual ~GUIRenderer();

	// 设置图形渲染器
	void SetRenderer( IRenderer* pRenderer );

	// 初始化GUI渲染器
	void InitGUIRenderer( int nMaxQuadCountPerDP = 100 );

	// 清空
	void Clear( void );

	// 开始GUI渲染
	void BeginGUIRender();

	// 结束GUI渲染
	void EndGUIRender();

	// 渲染一个UI图元
    virtual void Render( const AIR::Rectf& rectScreen, const AIR::Rectf& rectUV, f32 fDepth, ITexture* pTex, const CColorValue& color = CColorValue::White );

    virtual void Render(GUIRenderQuad* pQuad);
    // 打开裁剪
    void EnableScissor( const RECT& scissorrect );

    // 结束裁剪
    void DisableScissor( void );

	// 渲染缓冲区
	void RenderImmediately( void );

    virtual void SetDepths(f32 fMin, f32 fMax)
    {
        m_fMinZ = fMin;
        m_fMaxZ = fMax;
    }

    virtual f32 GetMinZ() const
    {
        return m_fMinZ;
    }

    virtual f32 GetMaxZ() const
    {
        return m_fMaxZ;
    }

    //带深度的渲染
    virtual void RenderImmediatelyWithDepth();
protected:
	//
	// nLT : 0--------1  nRT
	//        |      |
	//        |      |
	// nLB : 3--------2  nRB
	// 添加一个矩形的索引
	void AddQuadIndex( int nQuadID, u16** ppData, int & nIndexPos );

	// 添加一个矩形的顶点
	void AddQuadVertex( const AIR::Rectf& rectScreen, const AIR::Rectf& rectUV, const CColorValue& color, f32 fDepth );

    void AddQuadVertex(GUIRenderQuad* pRenderQuad);

private:
    void GetRenderPass();

protected:
	int				m_nMaxQuadCountPerDP;		// 矩形缓冲区所能容纳的最大的矩形数
	int				m_nMaxVertexCountPerDP;		// 矩形缓冲区所能容纳的最大的顶点数
	int				m_nMaxIndexCountPerDP;		// 矩形缓冲区所能容纳的最大的索引数
	int				m_nCurTriangleCountToRender;// 当前要渲染的三角形数
	int				m_nCurIndexCountToRender;	// 当前要渲染的索引数
	int				m_nCurVertexCountToRender;	// 当前要渲染的顶点数
	IVertexBuffer*	m_pGUIVB;					// GUI顶点缓冲对象
	IIndexBuffer*	m_pGUIIB;					// GUI索引缓冲对象
	ITexture*		m_pCurTexture;				// 当前GUI纹理对象
	IRenderer*		m_pRenderer;				// 渲染器对象
	RHWVERTEX*		m_pVertexArray;				// 顶点数组

    //用于排序，开了深度的接口
    typedef AIRList<GUIRenderQuad*> RenderQuad_List;
    typedef RenderQuad_List::iterator RenderQuadList_Iter;
    typedef AIRMap<ITexture*, RenderQuad_List> RenderQuadList_Map;
    typedef RenderQuadList_Map::iterator RenderQuadListMap_Iter;
    RenderQuadList_Map m_mapRenderQuadList;

    Primitive*  m_pPrimitive;
    //CVertexDeclaration* m_pVertexDecl;
	//CTechnique* m_pGUITechnique;
 //   SamplerStateObject* m_pSamplerState;

    //MaterialTemplate* m_pUIMaterialTemplate;
    IMaterial*        m_pMaterial;
    //SamplerState      m_samplerState;
    //RenderPass        m_renderPass;

    f32 m_fMinZ;
    f32 m_fMaxZ;

    //TextureUnitBlend    m_colorBlend;
    //TextureUnitBlend    m_alphaBlend;

	std::map<RenderPass*, IVertexArrayObject*> renderPassVAOMap;

};//! end class GUIRenderer


//#define GUIRENDERER	(GUIRenderer::GetSingletonPtr())


#endif //! end define __GUIRenderer_H__