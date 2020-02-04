/*! \file Terrain
    \brief  2008 版本所有.\n
 作    者: liangairan\n
 最后修改时间: 2008-08-19\n
 功    能: \n LOD地形类，管理所有的HeightMap
*/
#pragma once

#include "Common.h"
#include "EngineConfig.h"

#include "HeightMap.h"
//#include "IRenderObject.h"

#include "LODInfo.h"

//#include "ImageOperator.h"
#include "Singleton.h"

class EngineExport CTerrain : public Singleton<CTerrain>
{
	BE_SINGLETON(CTerrain);
public:
	
	virtual ~CTerrain(void);

    bool   Init(const LPTERRAINCONFIG pConfig);

    //读取地形配置文件
    bool   LoadConfigFile(const char* szCFGFile);

	//清空所有对象和释放所有内存
	void   Clear();

	//渲染函数
	//void   Render(CCamera* pCamera);

    //添加高度图
    void   AddHeightMap(CHeightMap* pHeightMap);

	CHeightMap* GetCurHeightmap() 
	{
		return m_pCurHeightMap;
	}

	void SetCurHeightmap(CHeightMap* pHeightmap)
	{
		m_pCurHeightMap = pHeightmap;
	}

    void   RemoveHeightMap(int nID);

	//根据X，Z坐标获取地形的高度
	float  GetHeight(float fX, float fZ) const;

	float  GetHeightMapEntry(int nRow, int nCol) const;

    float  GetBlockWidth()
    {
        return (m_config.dwBlockSize - 1) * m_config.fGridLength;
        //return m_pCurHeightMap->GetBlockWidth();
    }
	
    //获得地形大小，即地形每行的顶点数
    u32  GetTerrainSize()
    {
        return m_dwNumVertsPerRow;
    }

    u32  GetRenderObjectsCount()
    {
        return m_dwRenderObjects;
    }

    u32  GetTrianglesCount()
    {
        return m_dwTriangleNum;
    }

	//获得相邻两顶点的长度
	float GetGridLength()
	{
		return m_fGridLength;
	}

    float GetMinCellLength()
    {
        return m_fMinCellLength;
    }

    
	//根据索引得到顶点数据
	//const LPTERRAINVERTEX_TXT GetCusVerts(int nIndex);

	//绘制地形，把要渲染块加到渲染队列中
	//void  DrawTerrain();
    //CIndexBuffer* GetIndexBuffer() { return m_pIB; }

	//void  Update(CCamera* pCamera);


    //************************************
    // Method:    FindLODInfo
    // FullName:  CTerrain::FindLODInfo
    // Access:    public 
    // Returns:   CLODInfo* 
    // Qualifier: 根据LOD数据查找相应的LOD数据
    // Parameter: const CLODInfo & info LOD数据
    //************************************
    CLODInfo* FindLODInfo(const CLODInfo& info);


    CHeightMap*   GetHeightMap(int nID);

    LPTERRAINCONFIG GetConfig() { return &m_config; }

    bool IsInialize() const
    {
        return m_bInitialize;
    }

    //生成一张lightmap
    void GenLightmap(LPVERTEX_NORMAL pNormal, AIRImage* pHeightMap, float fYScale, const Vector3Df& sunLight, int nLightmapSize, float fAmbient, AIRImage& imgLightMapOut);

    //生成渲染数据
    void Cache();

protected:
	//更新shader,设置shader需要的变量
	//virtual void  UpdateShader(CCamera* pCamera);

    
    int IntersectMap(const Vector3Df& pos, const Vector3Df& vOrig, const Vector3Df& Direction, AIRImage& imgHeightMap, float fHeightScale);

private:
    //创建lodinfo的indexbuffer
	CLODInfo* CreateLODInfo(const CLODInfo& info, u16 nBlockSize);

    std::map<int, CHeightMap*>   m_mapHeightMap;

    typedef  std::map<int, CHeightMap*>::iterator HeightMap_Iter;


    CHeightMap*  m_pCurHeightMap;

    //CIndexBuffer*  m_pIB;

    int    m_nPass;    //Pass ID，即下标

	//void   BeginTerrainRenderState();
	//void   EndTerrainRenderState();

    //计算所有的索引
    int ComputeIndices(int nLODCount, u16 nBlockSize);

#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
    AIRHashMap<int, CLODInfo*> m_mapLODInfo;
    typedef AIRHashMap<int, CLODInfo*>::iterator LODInfo_Iter;
#else
	AIRMap<int, CLODInfo*> m_mapLODInfo;
	typedef AIRMap<int, CLODInfo*>::iterator LODInfo_Iter;
#endif

    

    int  m_nLODInfoCount;   //索引数量
    u32  m_dwNumBlockVertices;

    u32  m_dwTriangleNum;    //要渲染的三角形数
    u32  m_dwRenderObjects;  //渲染物数量

    TERRAINCONFIG   m_config;  //地形配置


    static CTerrain* m_pInstance;

protected:
	u32              m_dwNumGroundVertices;    //顶点总数
	u32              m_dwNumGroundIndices;     //索引总数

	u32   m_dwNumVertsPerRow;       //每行顶点数
	u32   m_dwNumMaxCellPerRow;     //每行最小单元格的数量
	//u32   m_dwTriangleNum;          
	u32   m_dwMaxTrangleCount;          //绘制的三角形数
	//u32   m_dwPrimityCount;       //渲染图元数
	//u32   m_dwTotalIndex;            //每次渲染的索引总数

	int     m_nCellPerBlock;           //每一块的单元格数，16、32或64

	u32   m_dwBlockGridSize;         //每个block的格数
    float   m_fHeightMapWidth;

	float   m_fGridLength;                           //相邻顶点间的距离，即单元格的长度
	float   m_fMinCellLength;                         //最小块的长度

    bool    m_bInitialize;

	//float   m_fWidth;            //整个地形的宽度
	//float   m_fDepth;            //整个地形的深度

    //CEffect* m_pEffect;
    //D3DXMATRIX m_matWorld;
};

#define TERRAIN CTerrain::GetInstance()
