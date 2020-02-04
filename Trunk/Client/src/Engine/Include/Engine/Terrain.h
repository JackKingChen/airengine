/*! \file Terrain
    \brief  2008 �汾����.\n
 ��    ��: liangairan\n
 ����޸�ʱ��: 2008-08-19\n
 ��    ��: \n LOD�����࣬�������е�HeightMap
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

    //��ȡ���������ļ�
    bool   LoadConfigFile(const char* szCFGFile);

	//������ж�����ͷ������ڴ�
	void   Clear();

	//��Ⱦ����
	//void   Render(CCamera* pCamera);

    //��Ӹ߶�ͼ
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

	//����X��Z�����ȡ���εĸ߶�
	float  GetHeight(float fX, float fZ) const;

	float  GetHeightMapEntry(int nRow, int nCol) const;

    float  GetBlockWidth()
    {
        return (m_config.dwBlockSize - 1) * m_config.fGridLength;
        //return m_pCurHeightMap->GetBlockWidth();
    }
	
    //��õ��δ�С��������ÿ�еĶ�����
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

	//�������������ĳ���
	float GetGridLength()
	{
		return m_fGridLength;
	}

    float GetMinCellLength()
    {
        return m_fMinCellLength;
    }

    
	//���������õ���������
	//const LPTERRAINVERTEX_TXT GetCusVerts(int nIndex);

	//���Ƶ��Σ���Ҫ��Ⱦ��ӵ���Ⱦ������
	//void  DrawTerrain();
    //CIndexBuffer* GetIndexBuffer() { return m_pIB; }

	//void  Update(CCamera* pCamera);


    //************************************
    // Method:    FindLODInfo
    // FullName:  CTerrain::FindLODInfo
    // Access:    public 
    // Returns:   CLODInfo* 
    // Qualifier: ����LOD���ݲ�����Ӧ��LOD����
    // Parameter: const CLODInfo & info LOD����
    //************************************
    CLODInfo* FindLODInfo(const CLODInfo& info);


    CHeightMap*   GetHeightMap(int nID);

    LPTERRAINCONFIG GetConfig() { return &m_config; }

    bool IsInialize() const
    {
        return m_bInitialize;
    }

    //����һ��lightmap
    void GenLightmap(LPVERTEX_NORMAL pNormal, AIRImage* pHeightMap, float fYScale, const Vector3Df& sunLight, int nLightmapSize, float fAmbient, AIRImage& imgLightMapOut);

    //������Ⱦ����
    void Cache();

protected:
	//����shader,����shader��Ҫ�ı���
	//virtual void  UpdateShader(CCamera* pCamera);

    
    int IntersectMap(const Vector3Df& pos, const Vector3Df& vOrig, const Vector3Df& Direction, AIRImage& imgHeightMap, float fHeightScale);

private:
    //����lodinfo��indexbuffer
	CLODInfo* CreateLODInfo(const CLODInfo& info, u16 nBlockSize);

    std::map<int, CHeightMap*>   m_mapHeightMap;

    typedef  std::map<int, CHeightMap*>::iterator HeightMap_Iter;


    CHeightMap*  m_pCurHeightMap;

    //CIndexBuffer*  m_pIB;

    int    m_nPass;    //Pass ID�����±�

	//void   BeginTerrainRenderState();
	//void   EndTerrainRenderState();

    //�������е�����
    int ComputeIndices(int nLODCount, u16 nBlockSize);

#if AIR_TARGET_PLATFORM == AIR_PLATFORM_WIN32
    AIRHashMap<int, CLODInfo*> m_mapLODInfo;
    typedef AIRHashMap<int, CLODInfo*>::iterator LODInfo_Iter;
#else
	AIRMap<int, CLODInfo*> m_mapLODInfo;
	typedef AIRMap<int, CLODInfo*>::iterator LODInfo_Iter;
#endif

    

    int  m_nLODInfoCount;   //��������
    u32  m_dwNumBlockVertices;

    u32  m_dwTriangleNum;    //Ҫ��Ⱦ����������
    u32  m_dwRenderObjects;  //��Ⱦ������

    TERRAINCONFIG   m_config;  //��������


    static CTerrain* m_pInstance;

protected:
	u32              m_dwNumGroundVertices;    //��������
	u32              m_dwNumGroundIndices;     //��������

	u32   m_dwNumVertsPerRow;       //ÿ�ж�����
	u32   m_dwNumMaxCellPerRow;     //ÿ����С��Ԫ�������
	//u32   m_dwTriangleNum;          
	u32   m_dwMaxTrangleCount;          //���Ƶ���������
	//u32   m_dwPrimityCount;       //��ȾͼԪ��
	//u32   m_dwTotalIndex;            //ÿ����Ⱦ����������

	int     m_nCellPerBlock;           //ÿһ��ĵ�Ԫ������16��32��64

	u32   m_dwBlockGridSize;         //ÿ��block�ĸ���
    float   m_fHeightMapWidth;

	float   m_fGridLength;                           //���ڶ����ľ��룬����Ԫ��ĳ���
	float   m_fMinCellLength;                         //��С��ĳ���

    bool    m_bInitialize;

	//float   m_fWidth;            //�������εĿ��
	//float   m_fDepth;            //�������ε����

    //CEffect* m_pEffect;
    //D3DXMATRIX m_matWorld;
};

#define TERRAIN CTerrain::GetInstance()
