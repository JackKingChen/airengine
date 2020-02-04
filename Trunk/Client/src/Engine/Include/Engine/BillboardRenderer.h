/********************************************************************
	created:	2010/10/16
	created:	16:10:2010   11:24
	filename: 	BillboardRenderer.h
	author:		liangairan
	
	purpose:	Billboard��Ⱦ��������������Ⱦbillboard
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

    //������Ⱦ
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
    // ���һ�����ε�����
    void AddQuadIndex( int nQuadID, u16** ppData, int & nIndexPos );

    void AddBillboardVertex(CBillboard* pBillboard);
private:

    int				m_nMaxBillboardCountPerDP;		// ���λ������������ɵ����ľ�����
    int				m_nMaxVertexCountPerDP;		// ���λ������������ɵ����Ķ�����
    int				m_nMaxIndexCountPerDP;		// ���λ������������ɵ�����������
    int				m_nCurTriangleCountToRender;// ��ǰҪ��Ⱦ����������
    int				m_nCurIndexCountToRender;	// ��ǰҪ��Ⱦ��������
    int				m_nCurVertexCountToRender;	// ��ǰҪ��Ⱦ�Ķ�����
    IVertexBuffer*  m_pVB;
    IIndexBuffer*   m_pIB;
    IMaterial*      m_pMaterial;
    ITexture*       m_pCurTexture;
    LPVERTEX_STANDARD  m_pVertices;

    bool  m_bInitialize;
    Primitive*  m_pPrimitive;  
};

#define BILLBOARD_RENDERER BillboardRenderer::GetInstance()
