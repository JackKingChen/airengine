// ***************************************************************
//  文件: TerrainDetailLayer.h
//  日期: 2010-5-7
//  作者: liangairan
//  说明: 地形纹理细节的渲染体
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
    //生成细节
    void GenerateDetailGrids();

    
private:
    CHeightMap*  m_pHeightMap;

    typedef AIRVector<TerrainDetailGrid> TerrainDetailGridArray;
    typedef TerrainDetailGridArray::iterator TerrainDetailGridArray_Iter;

    TerrainDetailGridArray m_vtDetailGrids;   //格
};
