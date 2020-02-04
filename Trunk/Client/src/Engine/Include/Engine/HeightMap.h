/*! \file Terrain
\brief  2008 版本所有.\n
作    者: liangairan\n
最后修改时间: 2009-01-24\n
功    能: \n LOD地形高度图类，相当于一个场景的地形
*/

#pragma once

//#include "Common.h"
#include "TerrainQuadTree.h"
#include "Renderable.h"
#include "MarkupSTL.h"
#include "VertexDef.h"
#include "TerrainLayer.h"
//#include "TerrainDetailLayer.h"
#include "IteratorWrappers.h"
#include "AIRImage.h"

struct TerrainGrid
{
	TerrainGrid():nRow(0),nCol(0){}
	TerrainGrid( const TerrainGrid& other ):nRow(other.nRow),nCol(other.nCol){}

    u32 nRow;   //行
    u32 nCol;   //列
};

class ImageOperator;
class FileData;

class EngineExport CHeightMap : public Renderable
{
public:
    typedef AIRMap<int, CTerrainBlock*> MapTerrainBlock;
public:
    CHeightMap();
    virtual ~CHeightMap(void);

    CHeightMap(int nSize, const LPTERRAINCONFIG pTerrainConfig, const char* szMaterial);
    //CHeightMap(const LPTERRAINCONFIG pTerrainConfig);

    CTerrainBlock*  GetBlock(int nIndex);

    //************************************
    // Method:    CreateHeightMap
    // FullName:  CHeightMap::CreateHeightMap
    // Access:    public 
    // Returns:   bool
    // Qualifier: 创建高度图
    // Parameter: const char * szFilename       高度图文件名
    // Parameter: const LPTERRAINCONFIG pTerrainConfig  地形配置数据结构
    //************************************
    bool   CreateHeightMap(const char* szFilename, const LPTERRAINCONFIG pTerrainConfig, float fInitialHeight,
        float fYScale, const AIRString& strMaterial);

    //创建一个默认的地形
    bool   CreateHeightMap(int nTerrainSize, float fInitialHeight, float fYScale, const LPTERRAINCONFIG pTerrainConfig);

    //bool   LoadFromXML(TiXmlElement* pElement);
    
    void   AddBlocksToQuadTree(CTerrainQuadTree* pTree);

    u32  GetSize() { return m_nWidth; }
    u32  GetBlockSize() { return m_dwBlockSize; }

    virtual void   Clear();

    f32  GetWidth() { return m_fWidth; }

    f32  GetBlockWidth() const { return m_fGridLength * (m_dwBlockSize - 1); }

    //根据X和Z获得地形的Y值
    f32  GetHeight(float fX, float fZ);

    
    //获得顶点数据
    LPVERTEX_NORMAL GetVertex(int nIndex) 
    { 
        return &m_vtVertices[nIndex]; 
    }

    //获得甘个顶点，越界返回NULL
    LPVERTEX_NORMAL GetVertex(int nRow, int nCol);

    const Vector3Df& GetVertexPos(u32 nIndex)
    { 
        return m_vtVertices[nIndex].vPos;  //Vector3Df(m_vtVertices[nIndex].x, m_vtVertices[nIndex].y, m_vtVertices[nIndex].z); 
    }

    s32    GetID() { return m_nID; }

    void   GetRenderableObject(AIRList<CTerrainBlock*>& lstRender, CCamera* pCamera); 

    virtual void Render(CCamera* pCamera);

    //获得可视范围内的地形block
    virtual void GetVisableObjects(CCamera* pCamera, AIRList<CSceneNode*>& lstVisables, u32 nCullFlag);

    //virtual f32 GetMaxZ() { return 0.0; }

    u32  GetRenderableObjectsCount() { return (u32)m_lstRenderObject.size(); }

    u32  GetTrianglesCount() { return m_dwTrianglesCount; }

    //从XML读取数据
    virtual bool LoadFromXML(CMarkupSTL& xml);

    //获得X方面的顶点数
    int GetWidthSize()
    {
        return m_nWidth;
    }

    //获得Z方向的顶点数
    int GetDepthSize()
    {
        return m_nDepth;
    }

    void SetYScale(float fYscale);

    //拾取地形的某一点
    bool GetRayPoint(const Vector3Df& vOrg, const Vector3Df& vEnd, Vector3Df& vPointOut);

    //获得一格的大小
    float GetGridWidth() const
    {
        return m_fGridLength;
    }

    float GetGridDepth() const
    {
        return m_fGridDepth;
    }

    //根据坐标获得所在的格
    TerrainGrid GetTerrainGrid(float x, float z) const;

    //根据坐标获得所在的格
    TerrainGrid GetTerrainGrid(float x, float z, bool& bLeftPass, bool& bTopPass, bool& bRightPass, bool& bBottomPass) const;

    //获得一定范围内的地形块
    //bSquare 是否为方形
    void GetBlocksInRange(AIRList<CTerrainBlock*>& lstBlocks, float x, float z, float fRadius);

    //获得地形所在的块
    CTerrainBlock* GetTerrainBlock(float x, float z);

    //获得每行的顶点
    int GetBlocksNumPerRow() const
    {
        return (m_nWidth - 1) / (m_dwBlockSize - 1);
    }

    int GetGridsNumPerRow() const
    {
        return m_nDepth - 1;
    }

    int GetGridsNumPerCol() const
    {
        return m_nWidth - 1;
    }

    int GetBlocksNum() const
    {
        return GetBlocksNumPerRow() * GetBlocksNumPerRow();
    }

    //TerrainDetailLayer* GetDetailLayer() 
    //{
    //    return m_pDetailLayer;
    //}

    //bool CreateDetailLayer(const char* szMaterial);

    //获得初始高度
    f32 GetInitHeight() const
    {
        return m_fInitHeight;
    }

    //获得Y缩放
    f32 GetYScale() const
    {
        return m_fYScale;
    }

    /*ImageOperator**/void GenLightMap(const Vector3Df& vecSunLight, float fAmbient, int nLightMapSize, AIRImage& imgLightMap);

    //根据摄像获得在可视区内的地形块
    void   GetRenderObject(CCamera* pCamera, CRenderQueue* pQueue);
    
    const AIRString& GetHeightMapFile() const
    {
        return m_strHeightMap;
    }

    virtual void AcceptShadowmap(bool bAccept);

    //ListIterator<ListTerrainBlock> GetListTerrainBlockIter()
    //{
    //    return ListIterator<ListTerrainBlock>(m_lstRenderObject);
    //}

    //把地形渲染到GBuffer延迟渲染
    //virtual void RenderToGBuffer(CCamera* pCamera);

	//virtual void RenderToDepthMap(CCamera* pCamera, CPass* pPass);

    virtual void SetAcceptLighting(bool bAccept);

    //virtual void ResetCurrentTechnique();

    ITexture* GetHeightmapTexture();

	inline void SetHeightmapDataDirty(bool bDirty)
	{
		m_bHeightDataDirty = bDirty;
	}

	virtual void RetriveHeightMapIndexMapData( void );

    //通过高度数组计算顶点数据
    void ComputeVerticesByHeights(const AIRVector<float>& vtHeights);

    /*
    virtual void GenRenderToGBufferTechnique();

    CTechnique* GetTechniqueRenderToGBuffer() 
    {
        return m_pTechniqueRenderToGBuffer;
    }
    */

    virtual bool Prepare();

    //获得decal的顶点
    void GetDecalVertices(AIRVector<Vector3Df>& vtVertices, const Vector3Df& centerPoint, f32 fWidth, f32 fHeight, u32 nRowStart, u32 nColStart, u32 nRowEnd, u32 nColEnd);

    /*
	//删除地块上的植被
	void DelAllPSG();

	//取得地形上的植被
	void GetAllPSG( AIRList<PageStaticGeometory*>& psgList );

	//显示/隐藏地形上的植被的包围盒
	void SetShowAllPSGBoundingBox( bool b );
    */

    const AIRString& GetMaterialFile() const
    {
        return m_strMaterial;
    }

    MapIterator<MapTerrainBlock> GetTerrainBlockIter()
    {
        return MapIterator<MapTerrainBlock>(m_mapBlock);
    }

    void RebuildHeightmapTexture();

    void Cull(CCamera* pCamera, AIRList<CSceneNode*>& lstVisableNodes, u32 nCullFlag);

protected:
    virtual void PreRender(CCamera* pCamera);
    
    //线性查找一个比较小的段
    Line3Df LinearSearch(const Vector3Df& vOrg, const Vector3Df& vEnd);

    //二分查找
    bool BinarySearchRayPoint(Line3Df& line, Vector3Df& vPointOut);

    //判断射线是否与地形相交
    bool IsIntersectMap(const Vector3Df vertex, const Vector3Df& vOrig, const Vector3Df& vDirection);
private:
    //注意，这里的raw不是真实的raw文件，里面放的都是float数据
    bool   ReadRawFile(const char* szFilename);
    bool   ReadBMPFile(const char* szFilename);
    void   ComputeVertices();
    void   ComputeNormals();

    //绘制法线
    void   DrawNormals();

    //创建分块
    void   CreateBlocks();

    float  GetHeightMapEntry(u32 nRow, u32 nCol);

    bool CacheTerrain();

    //排序
    void   Sort();

private:
    AIRVector<VERTEX_NORMAL>         m_vtVertices;    //顶点数组
    AIRVector<f32>                 m_vtHeights;     //高度图中的高度值数组
    //MyVector<ITexture*>             m_vtTextures;
    AIRMap<int, CTerrainBlock*>        m_mapBlock;
    typedef AIRMap<int, CTerrainBlock*>::iterator MapBlockIter;

    //IMaterial*                m_pMaterial;           //所用材质

    u32   m_dwTrianglesCount;

    bool    m_bDrawNormal;    //是否绘制法线
    bool    m_bLastKeyDown;   //上一帧的键是否按下

    AIRString  m_strHeightMap;
    AIRString  m_strMaterial;

    //string  m_strName;

    //MyVector<CTerrainBlock*>          
    CTerrainQuadTree    m_quadTree;        //四叉树放block

    u32                           m_dwSize;          //HeightMap Size 每行的顶点数，这里是行和列顶点数相同
    u32                           m_dwBlockSize;     //Block的大小，即一个block所占的顶点数
    u32                           m_dwVertices;
    u32                             m_nWidth;          //X方向宽的顶点数
    u32                             m_nDepth;          //Z方向的顶点数

    f32                             m_fGridLength;     //每一格的长度
    f32                             m_fGridDepth;      //每一格的深度
    f32                             m_fWidth;          //整个地形在世界坐标中的宽度
    f32                             m_fDepth;
    f32                             m_fBlockWidth;     //一个block的宽度
    f32                             m_fBlockDepth;

    Vector3Df                       m_vtCenter;         //地形中心点
    f32                             m_fYScale;         //Y轴放大变量
    f32                             m_fInitHeight;     //初始高度

    //Matrix4f                          m_matWorld;

    s32    m_nID;    //Heightmap id
    //TerrainDetailLayer*   m_pDetailLayer;        //细节层
    //IMaterial*            m_pDetailMaterial;     //细节层的材质

    AIRList<CTerrainBlock*>  m_lstRenderObject;     //地形块渲染队列

    typedef AIRList<CTerrainBlock*>::iterator  RenderObject_Iter;

    ITexture*  m_pHeightmapTex;   //高度图的纹理
	bool m_bHeightDataDirty;     //weather the heightmap data changed
    FileData*  m_pResource;
};
