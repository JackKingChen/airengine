/*! \file Terrain
\brief  2008 �汾����.\n
��    ��: liangairan\n
����޸�ʱ��: 2009-01-24\n
��    ��: \n LOD���θ߶�ͼ�࣬�൱��һ�������ĵ���
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

    u32 nRow;   //��
    u32 nCol;   //��
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
    // Qualifier: �����߶�ͼ
    // Parameter: const char * szFilename       �߶�ͼ�ļ���
    // Parameter: const LPTERRAINCONFIG pTerrainConfig  �����������ݽṹ
    //************************************
    bool   CreateHeightMap(const char* szFilename, const LPTERRAINCONFIG pTerrainConfig, float fInitialHeight,
        float fYScale, const AIRString& strMaterial);

    //����һ��Ĭ�ϵĵ���
    bool   CreateHeightMap(int nTerrainSize, float fInitialHeight, float fYScale, const LPTERRAINCONFIG pTerrainConfig);

    //bool   LoadFromXML(TiXmlElement* pElement);
    
    void   AddBlocksToQuadTree(CTerrainQuadTree* pTree);

    u32  GetSize() { return m_nWidth; }
    u32  GetBlockSize() { return m_dwBlockSize; }

    virtual void   Clear();

    f32  GetWidth() { return m_fWidth; }

    f32  GetBlockWidth() const { return m_fGridLength * (m_dwBlockSize - 1); }

    //����X��Z��õ��ε�Yֵ
    f32  GetHeight(float fX, float fZ);

    
    //��ö�������
    LPVERTEX_NORMAL GetVertex(int nIndex) 
    { 
        return &m_vtVertices[nIndex]; 
    }

    //��øʸ����㣬Խ�緵��NULL
    LPVERTEX_NORMAL GetVertex(int nRow, int nCol);

    const Vector3Df& GetVertexPos(u32 nIndex)
    { 
        return m_vtVertices[nIndex].vPos;  //Vector3Df(m_vtVertices[nIndex].x, m_vtVertices[nIndex].y, m_vtVertices[nIndex].z); 
    }

    s32    GetID() { return m_nID; }

    void   GetRenderableObject(AIRList<CTerrainBlock*>& lstRender, CCamera* pCamera); 

    virtual void Render(CCamera* pCamera);

    //��ÿ��ӷ�Χ�ڵĵ���block
    virtual void GetVisableObjects(CCamera* pCamera, AIRList<CSceneNode*>& lstVisables, u32 nCullFlag);

    //virtual f32 GetMaxZ() { return 0.0; }

    u32  GetRenderableObjectsCount() { return (u32)m_lstRenderObject.size(); }

    u32  GetTrianglesCount() { return m_dwTrianglesCount; }

    //��XML��ȡ����
    virtual bool LoadFromXML(CMarkupSTL& xml);

    //���X����Ķ�����
    int GetWidthSize()
    {
        return m_nWidth;
    }

    //���Z����Ķ�����
    int GetDepthSize()
    {
        return m_nDepth;
    }

    void SetYScale(float fYscale);

    //ʰȡ���ε�ĳһ��
    bool GetRayPoint(const Vector3Df& vOrg, const Vector3Df& vEnd, Vector3Df& vPointOut);

    //���һ��Ĵ�С
    float GetGridWidth() const
    {
        return m_fGridLength;
    }

    float GetGridDepth() const
    {
        return m_fGridDepth;
    }

    //�������������ڵĸ�
    TerrainGrid GetTerrainGrid(float x, float z) const;

    //�������������ڵĸ�
    TerrainGrid GetTerrainGrid(float x, float z, bool& bLeftPass, bool& bTopPass, bool& bRightPass, bool& bBottomPass) const;

    //���һ����Χ�ڵĵ��ο�
    //bSquare �Ƿ�Ϊ����
    void GetBlocksInRange(AIRList<CTerrainBlock*>& lstBlocks, float x, float z, float fRadius);

    //��õ������ڵĿ�
    CTerrainBlock* GetTerrainBlock(float x, float z);

    //���ÿ�еĶ���
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

    //��ó�ʼ�߶�
    f32 GetInitHeight() const
    {
        return m_fInitHeight;
    }

    //���Y����
    f32 GetYScale() const
    {
        return m_fYScale;
    }

    /*ImageOperator**/void GenLightMap(const Vector3Df& vecSunLight, float fAmbient, int nLightMapSize, AIRImage& imgLightMap);

    //�����������ڿ������ڵĵ��ο�
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

    //�ѵ�����Ⱦ��GBuffer�ӳ���Ⱦ
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

    //ͨ���߶�������㶥������
    void ComputeVerticesByHeights(const AIRVector<float>& vtHeights);

    /*
    virtual void GenRenderToGBufferTechnique();

    CTechnique* GetTechniqueRenderToGBuffer() 
    {
        return m_pTechniqueRenderToGBuffer;
    }
    */

    virtual bool Prepare();

    //���decal�Ķ���
    void GetDecalVertices(AIRVector<Vector3Df>& vtVertices, const Vector3Df& centerPoint, f32 fWidth, f32 fHeight, u32 nRowStart, u32 nColStart, u32 nRowEnd, u32 nColEnd);

    /*
	//ɾ���ؿ��ϵ�ֲ��
	void DelAllPSG();

	//ȡ�õ����ϵ�ֲ��
	void GetAllPSG( AIRList<PageStaticGeometory*>& psgList );

	//��ʾ/���ص����ϵ�ֲ���İ�Χ��
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
    
    //���Բ���һ���Ƚ�С�Ķ�
    Line3Df LinearSearch(const Vector3Df& vOrg, const Vector3Df& vEnd);

    //���ֲ���
    bool BinarySearchRayPoint(Line3Df& line, Vector3Df& vPointOut);

    //�ж������Ƿ�������ཻ
    bool IsIntersectMap(const Vector3Df vertex, const Vector3Df& vOrig, const Vector3Df& vDirection);
private:
    //ע�⣬�����raw������ʵ��raw�ļ�������ŵĶ���float����
    bool   ReadRawFile(const char* szFilename);
    bool   ReadBMPFile(const char* szFilename);
    void   ComputeVertices();
    void   ComputeNormals();

    //���Ʒ���
    void   DrawNormals();

    //�����ֿ�
    void   CreateBlocks();

    float  GetHeightMapEntry(u32 nRow, u32 nCol);

    bool CacheTerrain();

    //����
    void   Sort();

private:
    AIRVector<VERTEX_NORMAL>         m_vtVertices;    //��������
    AIRVector<f32>                 m_vtHeights;     //�߶�ͼ�еĸ߶�ֵ����
    //MyVector<ITexture*>             m_vtTextures;
    AIRMap<int, CTerrainBlock*>        m_mapBlock;
    typedef AIRMap<int, CTerrainBlock*>::iterator MapBlockIter;

    //IMaterial*                m_pMaterial;           //���ò���

    u32   m_dwTrianglesCount;

    bool    m_bDrawNormal;    //�Ƿ���Ʒ���
    bool    m_bLastKeyDown;   //��һ֡�ļ��Ƿ���

    AIRString  m_strHeightMap;
    AIRString  m_strMaterial;

    //string  m_strName;

    //MyVector<CTerrainBlock*>          
    CTerrainQuadTree    m_quadTree;        //�Ĳ�����block

    u32                           m_dwSize;          //HeightMap Size ÿ�еĶ��������������к��ж�������ͬ
    u32                           m_dwBlockSize;     //Block�Ĵ�С����һ��block��ռ�Ķ�����
    u32                           m_dwVertices;
    u32                             m_nWidth;          //X�����Ķ�����
    u32                             m_nDepth;          //Z����Ķ�����

    f32                             m_fGridLength;     //ÿһ��ĳ���
    f32                             m_fGridDepth;      //ÿһ������
    f32                             m_fWidth;          //�������������������еĿ��
    f32                             m_fDepth;
    f32                             m_fBlockWidth;     //һ��block�Ŀ��
    f32                             m_fBlockDepth;

    Vector3Df                       m_vtCenter;         //�������ĵ�
    f32                             m_fYScale;         //Y��Ŵ����
    f32                             m_fInitHeight;     //��ʼ�߶�

    //Matrix4f                          m_matWorld;

    s32    m_nID;    //Heightmap id
    //TerrainDetailLayer*   m_pDetailLayer;        //ϸ�ڲ�
    //IMaterial*            m_pDetailMaterial;     //ϸ�ڲ�Ĳ���

    AIRList<CTerrainBlock*>  m_lstRenderObject;     //���ο���Ⱦ����

    typedef AIRList<CTerrainBlock*>::iterator  RenderObject_Iter;

    ITexture*  m_pHeightmapTex;   //�߶�ͼ������
	bool m_bHeightDataDirty;     //weather the heightmap data changed
    FileData*  m_pResource;
};
