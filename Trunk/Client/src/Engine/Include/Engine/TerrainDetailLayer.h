// ***************************************************************
//  �ļ�: TerrainDetailLayer.h
//  ����: 2010-5-7
//  ����: liangairan
//  ˵��: ��������ϸ�ڵ���Ⱦ��
// ***************************************************************

#pragma once
#include "renderable.h"

class CHeightMap;

struct TerrainDetailGrid
{
    int nRow;
    int nCol;
    Rectf m_uv;
};

class EngineExport TerrainDetailLayer :
    public Renderable
{
public:
    TerrainDetailLayer(CHeightMap* pHeightMap);
    virtual ~TerrainDetailLayer(void);

    TerrainDetailGrid* GetDetailGrid(float x, float z);

    TerrainDetailGrid* GetDetailGrid(int nRow, int nCol);

    void GetDetailGridsInRange(float centerX, float centerZ, float fRadius, std::list<TerrainDetailGrid*>& lstGrids);
private:
    //����ϸ��
    void GenerateDetailGrids();

    
private:
    CHeightMap*  m_pHeightMap;

    typedef AIRVector<TerrainDetailGrid> TerrainDetailGridArray;
    typedef TerrainDetailGridArray::iterator TerrainDetailGridArray_Iter;

    TerrainDetailGridArray m_vtDetailGrids;   //��
};
