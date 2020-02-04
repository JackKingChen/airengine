/*! \file Terrain
\brief  2008 版本所有.\n
作    者: liangairan\n
最后修改时间: 2009-02-19\n
功    能: \n LOD地形块类
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

//地形全局配置
typedef struct tagTerrainConfig 
{
    //u32 dwTerrainSize;          //地形大小
    u32 dwBlockSize;            //block每行顶点数
    //u16  wTextureCount;          //纹理数
    u16  wLODLevel;              //LOD层数
    float fGridLength;            //每一格在世界中的大小
    //float fYScale;
    //char  szVertexShader[256];
    //char  szPixelShader[256];
    //AIRString m_strMaterial;    //地形的材质
    //AIRString strHeightmap;     //高度图文件
    //char  szEffect[256];          //effect文件
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

    //新方法，以地块为独立单位的方法
    bool Create(const f32* fHeights);

	virtual   ~CTerrainBlock(void);

	//virtual void      Render(CCamera* pCamera);

    //void RenderDetail(CCamera* pCamera);

    bool LoadFromXML(CMarkupSTL& xml) { return true; }

	//加载要渲染的数据
	bool      LoadRenderData(const LPVERTEX_NORMAL pCusVertices);

    //清空渲染数据
    void      UnLoadRenderData();

	//清空数据
	virtual void      Clear();

    //计算出球半径，dwTopLeft是块的左上角所在数组的序号，nRow是
    //void ComputeSphereHalf(u32 dwTopLeft, int nRow, CHeightMap* pTerrain);

    //计算LOD等级
    void ComputeLOD(CCamera* pCamera);

    //计算邻居
    void ComputeNeighbor();

    int  GetLODLevel() { return m_LOD.m_nLODLevel; }

    //裂缝修补
    void FixCrack();
    //void FixCrackEdge(int nNeighborLevel, CELLEDGE edge);
    //void FixCrackEdge(int nNeighborLevel, BLOCKEDGE edge);

    //void FillIndices(CTerrain* pTerrain, u32 dwVertsPerRow);

	u32     GetBlockIndex() { return m_nIndex; }

    //更新要画的数据，返回绘制数据的大小
    //u32     UpdateDrawNodes(CTerrain* pNode);

    //计算block的半径
    void      ComputeSphereHalf(CHeightMap* pTerrain);

    //获得地形块的半径
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

    //计算顶点
    bool ComputeVertices(const LPVERTEX_NORMAL pCusVertices);

    //新方法，计算顶点
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

    //添加地块的渲染物
    void AddRenderable(Renderable* pRenderable);

    //添加在可视范围内的渲染物进渲染队列
    void GetVisibleRenderables(CCamera* pCamera, CRenderQueue* pQueue);

    /*
	//删除地块上的植被
	void DelAllPSG();

	//取得地形上的植被
	void GetAllPSG( std::list<PageStaticGeometory*>& psgList );

	//显示/隐藏地形上的植被的包围盒
	void SetShowAllPSGBoundingBox( bool b );
    */

    //新方法，取得该地块的纹理索引
    ITexture* GetIndexTexture();

    virtual const AABBBox3Df&   GetWorldBoundingBox(bool bTransform)
    {
        return m_boundingBox;
    }

    //获得对应的高度
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
    
    //渲染前的更新，要更新其世界矩阵，和shader，并修补裂缝
    virtual void  PreRender(CCamera* pCamera);

    //创建细节的东西
    //void GenerateDetailData();

    //更新细节的VB
    //void UpdateDetailVertexBuffer();

    virtual void _GetPrimitive();

//逻辑数据的处理
public:
    s32 GetCenterX() { return m_nCenterX; }
    s32 GetCenterY() { return m_nCenterY; }
    s32 GetTopLeftX() { return m_nTopLeftX; }
    s32 GetTopLeftY() { return m_nTopLeftY; }
    u16 GetBlockSize() { return m_wBlockSize; }

//逻辑数据
private:
    
    s32   m_nRow;          //所在地形上的行
    s32   m_nCol;          //所在地形上的列
    s32   m_nCenterX;             //block在高度图的中点X坐标
    s32   m_nCenterY;             //block在高度图的中点Y坐标
    s32   m_nTopLeftX;            //在高度图的左上角X坐标
    s32   m_nTopLeftY;            //在高度图的左上解Y坐标
    u32   m_dwVertexIndex;       //block的左上角顶点索引号
    u32   m_nIndex;              //block的索引号
    u16  m_wBlockSize;           //block的大小，即block每行的顶点数
    u16  m_wGridsPerRow;          //每行的格数，m_wBlockSize - 1
    u32 m_dwTerrainSize;          //所在地形的大小，即地形每行的顶点数
    u32 m_dwNumBlockIndices;
    u32 m_dwNumBlockVertices;

    f32   m_fSphereHalf;      //block的半径，用于裁剪
    f32   m_fDeltaH;          //block的高度差。
    f32   m_fBlockWidth;     //block在世界里的宽度

    u32 m_dwTotalIndex;     //索引总数

    u32 m_dwTerrainVertsPerRow;

    s32   m_nLODLevel;   //LOD层号

    s32   m_nRealCell;     //实际渲染时每行的格数


    u16             m_wIndexStride;             //渲染索引的顶点步长，分别为1, 2, 4, 8

    s32              m_nNeighbor[4];             //邻居的索引号

    //Vector3Df      m_vCenterPos;          //中心点

    //IEffect*         m_pEffect;

//渲染数据
private:
    IVertexBuffer*               m_pVertexBuffer;
    IVertexBuffer*               m_pNormalVB;

    //s32    m_nHeightMapID;     //高度图的ID

    void SetupMatrix();

	//载入纹理
	bool LoadTexture(const char* szFilename);

	//bool CreateEffect(const char* szFilename);

    //
    bool CreateVertexBuffer();

    //创建材质
    bool CreateMaterial();

    CLODInfo  m_LOD;
    CLODInfo* m_pUseLOD;

    CHeightMap*   m_pHeightMap;

    
    RenderableVector m_vtRenderables;

    AIRVector<VERTEX_NORMAL> m_vtVertices;   //block顶点数据

    //IVertexBuffer*  m_pVBDetail;  //细节层的VB
    //IIndexBuffer*   m_pIBDetail;  //细节层的IB

    //AABBBox3Df m_aabb;
};
