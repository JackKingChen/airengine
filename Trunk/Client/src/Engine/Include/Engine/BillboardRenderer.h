/********************************************************************
	created:	2010/10/16
	created:	16:10:2010   11:24
	filename: 	BillboardRenderer.h
	author:		liangairan
	
	purpose:	Billboard渲染器，用于批量渲染billboard
*********************************************************************/

#pragma once 
#include "Billboard.h"
#include "Singleton.h"

class EngineExport BillboardRenderer : public Singleton<BillboardRenderer>
{
    BE_SINGLETON(BillboardRenderer);
public:
    ~BillboardRenderer();

    void Render(CBillboard* pBillboard);

    //立即渲染
    void RenderImmediately();  

    void SetMaxBillboardCount(int nMaxBillboard);

    void Clear();

    void Initialize();
protected:
    void GenRenderData();

    //
    // nLT : 0--------1  nRT
    //        |      |
    //        |      |
    // nLB : 3--------2  nRB
    // 添加一个矩形的索引
    void AddQuadIndex( int nQuadID, u16** ppData, int & nIndexPos );

    void AddBillboardVertex(CBillboard* pBillboard);
private:

    int				m_nMaxBillboardCountPerDP;		// 矩形缓冲区所能容纳的最大的矩形数
    int				m_nMaxVertexCountPerDP;		// 矩形缓冲区所能容纳的最大的顶点数
    int				m_nMaxIndexCountPerDP;		// 矩形缓冲区所能容纳的最大的索引数
    int				m_nCurTriangleCountToRender;// 当前要渲染的三角形数
    int				m_nCurIndexCountToRender;	// 当前要渲染的索引数
    int				m_nCurVertexCountToRender;	// 当前要渲染的顶点数
    IVertexBuffer*  m_pVB;
    IIndexBuffer*   m_pIB;
    IMaterial*      m_pMaterial;
    ITexture*       m_pCurTexture;
    LPVERTEX_STANDARD  m_pVertices;

    bool  m_bInitialize;
    Primitive*  m_pPrimitive;  
};

#define BILLBOARD_RENDERER BillboardRenderer::GetInstance()
