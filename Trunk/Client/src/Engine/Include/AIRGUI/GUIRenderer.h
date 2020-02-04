// ***************************************************************
//  �ļ�: GUIRenderer.h
//  ����: 2010-2-1
//  ����: huangjunjie
//  ˵��: GUI��Ⱦ��
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

	// ����ͼ����Ⱦ��
	void SetRenderer( IRenderer* pRenderer );

	// ��ʼ��GUI��Ⱦ��
	void InitGUIRenderer( int nMaxQuadCountPerDP = 100 );

	// ���
	void Clear( void );

	// ��ʼGUI��Ⱦ
	void BeginGUIRender();

	// ����GUI��Ⱦ
	void EndGUIRender();

	// ��Ⱦһ��UIͼԪ
    virtual void Render( const AIR::Rectf& rectScreen, const AIR::Rectf& rectUV, f32 fDepth, ITexture* pTex, const CColorValue& color = CColorValue::White );

    virtual void Render(GUIRenderQuad* pQuad);
    // �򿪲ü�
    void EnableScissor( const RECT& scissorrect );

    // �����ü�
    void DisableScissor( void );

	// ��Ⱦ������
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

    //����ȵ���Ⱦ
    virtual void RenderImmediatelyWithDepth();
protected:
	//
	// nLT : 0--------1  nRT
	//        |      |
	//        |      |
	// nLB : 3--------2  nRB
	// ���һ�����ε�����
	void AddQuadIndex( int nQuadID, u16** ppData, int & nIndexPos );

	// ���һ�����εĶ���
	void AddQuadVertex( const AIR::Rectf& rectScreen, const AIR::Rectf& rectUV, const CColorValue& color, f32 fDepth );

    void AddQuadVertex(GUIRenderQuad* pRenderQuad);

private:
    void GetRenderPass();

protected:
	int				m_nMaxQuadCountPerDP;		// ���λ������������ɵ����ľ�����
	int				m_nMaxVertexCountPerDP;		// ���λ������������ɵ����Ķ�����
	int				m_nMaxIndexCountPerDP;		// ���λ������������ɵ�����������
	int				m_nCurTriangleCountToRender;// ��ǰҪ��Ⱦ����������
	int				m_nCurIndexCountToRender;	// ��ǰҪ��Ⱦ��������
	int				m_nCurVertexCountToRender;	// ��ǰҪ��Ⱦ�Ķ�����
	IVertexBuffer*	m_pGUIVB;					// GUI���㻺�����
	IIndexBuffer*	m_pGUIIB;					// GUI�����������
	ITexture*		m_pCurTexture;				// ��ǰGUI�������
	IRenderer*		m_pRenderer;				// ��Ⱦ������
	RHWVERTEX*		m_pVertexArray;				// ��������

    //�������򣬿�����ȵĽӿ�
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