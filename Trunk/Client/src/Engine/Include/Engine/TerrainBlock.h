/*! \file Terrain
\brief  2008 �汾����.\n
��    ��: liangairan\n
����޸�ʱ��: 2009-02-19\n
��    ��: \n LOD���ο���
*/
#pragma once

#include "Common.h"

#include "ITexture.h"
#include "IEffect.h"

//#include "Frustum.h"
#include "Renderable.h"
#include "LODInfo.h"
//#include "MyVector.h"
#include "IVertexBuffer.h"
#include "VertexDef.h"
#include <list>
//#include "TerrainCell.h"

class CHeightMap;
class PageStaticGeometory;

enum BLOCKEDGE
{
    BLOCK_TOP = 0,
    BLOCK_RIGHT,
    BLOCK_BOTTOM,
    BLOCK_LEFT,
};

//����ȫ������
typedef struct tagTerrainConfig 
{
    //u32 dwTerrainSize;          //���δ�С
    u32 dwBlockSize;            //blockÿ�ж�����
    //u16  wTextureCount;          //������
    u16  wLODLevel;              //LOD����
    float fGridLength;            //ÿһ���������еĴ�С
    //float fYScale;
    //char  szVertexShader[256];
    //char  szPixelShader[256];
    //AIRString m_strMaterial;    //���εĲ���
    //AIRString strHeightmap;     //�߶�ͼ�ļ�
    //char  szEffect[256];          //effect�ļ�
    //char  szHeightMap[256];       //heightmap
    //vector<string>  m_vtTextures;

}TERRAINCONFIG, *LPTERRAINCONFIG;

class CTerrain;
class CHeightMap;

class EngineExport CTerrainBlock : public Renderable
{
public:
    typedef AIRVector<Renderable*> RenderableVector;
    typedef RenderableVector::iterator RenderableVectorIter;
public:
	CTerrainBlock();
    CTerrainBlock(int nRow, int nCol, int nCenterX, int nCenterY, u16 wBlockSize, u32 dwTerrainSize, float fGridLength, CHeightMap* pHeightMap);
    //CTerrainBlock(u32 x, u32 y, u32 dwIndex, u16 wBlockSize);

    bool      Create(CHeightMap* pHeightMap, const LPVERTEX_NORMAL pCusVertices);

    //�·������Եؿ�Ϊ������λ�ķ���
    bool Create(const f32* fHeights);

	virtual   ~CTerrainBlock(void);

	//virtual void      Render(CCamera* pCamera);

    //void RenderDetail(CCamera* pCamera);

    bool LoadFromXML(CMarkupSTL& xml) { return true; }

	//����Ҫ��Ⱦ������
	bool      LoadRenderData(const LPVERTEX_NORMAL pCusVertices);

    //�����Ⱦ����
    void      UnLoadRenderData();

	//�������
	virtual void      Clear();

    //�������뾶��dwTopLeft�ǿ�����Ͻ������������ţ�nRow��
    //void ComputeSphereHalf(u32 dwTopLeft, int nRow, CHeightMap* pTerrain);

    //����LOD�ȼ�
    void ComputeLOD(CCamera* pCamera);

    //�����ھ�
    void ComputeNeighbor();

    int  GetLODLevel() { return m_LOD.m_nLODLevel; }

    //�ѷ��޲�
    void FixCrack();
    //void FixCrackEdge(int nNeighborLevel, CELLEDGE edge);
    //void FixCrackEdge(int nNeighborLevel, BLOCKEDGE edge);

    //void FillIndices(CTerrain* pTerrain, u32 dwVertsPerRow);

	u32     GetBlockIndex() { return m_nIndex; }

    //����Ҫ�������ݣ����ػ������ݵĴ�С
    //u32     UpdateDrawNodes(CTerrain* pNode);

    //����block�İ뾶
    void      ComputeSphereHalf(CHeightMap* pTerrain);

    //��õ��ο�İ뾶
    f32       GetSphereHalf() { return m_fSphereHalf; }

    bool      IsInFrustum(const CFrustum* pFrustum) const;

    virtual u32 GetRenderTriangles() { return m_dwTotalIndex / 3; }

    virtual f32 GetMaxZ() { return m_boundingBox.MaxEdge.z; }

    void DrawNormals();

    virtual void SetMaterial(IMaterial* pMaterial) {}

    s32 GetRow() const
    {
        return m_nRow;
    }

    s32 GetCol() const
    {
        return m_nCol;
    }

    //���㶥��
    bool ComputeVertices(const LPVERTEX_NORMAL pCusVertices);

    //�·��������㶥��
    bool ComputeVertices(const f32* fHeights);
    //virtual Vector3Df* GetCenter() { return &m_vCenterPos; }

    //virtual void RenderToGBuffer(CCamera* pCamera, CPass* pPass);

	//virtual void RenderToDepthMap(CCamera* pCamera, CPass* pPass);

    IVertexBuffer* GetVertexBuffer()
    {
        return m_pVertexBuffer;
    }

    //virtual void GenRenderToGBufferTechnique();

    virtual bool Prepare();

    //��ӵؿ����Ⱦ��
    void AddRenderable(Renderable* pRenderable);

    //����ڿ��ӷ�Χ�ڵ���Ⱦ�����Ⱦ����
    void GetVisibleRenderables(CCamera* pCamera, CRenderQueue* pQueue);

    /*
	//ɾ���ؿ��ϵ�ֲ��
	void DelAllPSG();

	//ȡ�õ����ϵ�ֲ��
	void GetAllPSG( std::list<PageStaticGeometory*>& psgList );

	//��ʾ/���ص����ϵ�ֲ���İ�Χ��
	void SetShowAllPSGBoundingBox( bool b );
    */

    //�·�����ȡ�øõؿ����������
    ITexture* GetIndexTexture();

    virtual const AABBBox3Df&   GetWorldBoundingBox(bool bTransform)
    {
        return m_boundingBox;
    }

    //��ö�Ӧ�ĸ߶�
    void GetVertices(LPVERTEX_NORMAL pVertices);

    CHeightMap* GetOwnerTerrain()
    {
        return m_pHeightMap;
    }

    virtual void SetAcceptLighting(bool bAccept);

    virtual void AcceptShadowmap(bool bAccept);

protected:
    void  UpdateShader();
    //u32 GetDrawNodesCount() { return m_dwDrawNodesCount; }
    
    //��Ⱦǰ�ĸ��£�Ҫ������������󣬺�shader�����޲��ѷ�
    virtual void  PreRender(CCamera* pCamera);

    //����ϸ�ڵĶ���
    //void GenerateDetailData();

    //����ϸ�ڵ�VB
    //void UpdateDetailVertexBuffer();

    virtual void _GetPrimitive();

//�߼����ݵĴ���
public:
    s32 GetCenterX() { return m_nCenterX; }
    s32 GetCenterY() { return m_nCenterY; }
    s32 GetTopLeftX() { return m_nTopLeftX; }
    s32 GetTopLeftY() { return m_nTopLeftY; }
    u16 GetBlockSize() { return m_wBlockSize; }

//�߼�����
private:
    
    s32   m_nRow;          //���ڵ����ϵ���
    s32   m_nCol;          //���ڵ����ϵ���
    s32   m_nCenterX;             //block�ڸ߶�ͼ���е�X����
    s32   m_nCenterY;             //block�ڸ߶�ͼ���е�Y����
    s32   m_nTopLeftX;            //�ڸ߶�ͼ�����Ͻ�X����
    s32   m_nTopLeftY;            //�ڸ߶�ͼ�����Ͻ�Y����
    u32   m_dwVertexIndex;       //block�����ϽǶ���������
    u32   m_nIndex;              //block��������
    u16  m_wBlockSize;           //block�Ĵ�С����blockÿ�еĶ�����
    u16  m_wGridsPerRow;          //ÿ�еĸ�����m_wBlockSize - 1
    u32 m_dwTerrainSize;          //���ڵ��εĴ�С��������ÿ�еĶ�����
    u32 m_dwNumBlockIndices;
    u32 m_dwNumBlockVertices;

    f32   m_fSphereHalf;      //block�İ뾶�����ڲü�
    f32   m_fDeltaH;          //block�ĸ߶Ȳ
    f32   m_fBlockWidth;     //block��������Ŀ��

    u32 m_dwTotalIndex;     //��������

    u32 m_dwTerrainVertsPerRow;

    s32   m_nLODLevel;   //LOD���

    s32   m_nRealCell;     //ʵ����Ⱦʱÿ�еĸ���


    u16             m_wIndexStride;             //��Ⱦ�����Ķ��㲽�����ֱ�Ϊ1, 2, 4, 8

    s32              m_nNeighbor[4];             //�ھӵ�������

    //Vector3Df      m_vCenterPos;          //���ĵ�

    //IEffect*         m_pEffect;

//��Ⱦ����
private:
    IVertexBuffer*               m_pVertexBuffer;
    IVertexBuffer*               m_pNormalVB;

    //s32    m_nHeightMapID;     //�߶�ͼ��ID

    void SetupMatrix();

	//��������
	bool LoadTexture(const char* szFilename);

	//bool CreateEffect(const char* szFilename);

    //
    bool CreateVertexBuffer();

    //��������
    bool CreateMaterial();

    CLODInfo  m_LOD;
    CLODInfo* m_pUseLOD;

    CHeightMap*   m_pHeightMap;

    
    RenderableVector m_vtRenderables;

    AIRVector<VERTEX_NORMAL> m_vtVertices;   //block��������

    //IVertexBuffer*  m_pVBDetail;  //ϸ�ڲ��VB
    //IIndexBuffer*   m_pIBDetail;  //ϸ�ڲ��IB

    //AABBBox3Df m_aabb;
};
