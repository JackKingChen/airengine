#include "stdafx.h"
#include "TerrainDetailLayer.h"
#include "HeightMap.h"

TerrainDetailLayer::TerrainDetailLayer(CHeightMap* pHeightMap) : m_pHeightMap(pHeightMap)
{
    GenerateDetailGrids();
}

TerrainDetailLayer::~TerrainDetailLayer(void)
{
}

void TerrainDetailLayer::GenerateDetailGrids()
{
    m_vtDetailGrids.resize( m_pHeightMap->GetGridsNumPerRow() * m_pHeightMap->GetGridsNumPerCol() );
    for (int nRow = 0; nRow < m_pHeightMap->GetGridsNumPerRow(); ++nRow)
    {
        for (int nCol = 0; nCol < m_pHeightMap->GetGridsNumPerCol(); ++nCol)
        {
            TerrainDetailGrid grid;
            grid.nRow = nRow;
            grid.nCol = nCol;
            m_vtDetailGrids[nRow * m_pHeightMap->GetGridsNumPerCol() + nCol] = grid;
        }
    }
}

TerrainDetailGrid* TerrainDetailLayer::GetDetailGrid(float x, float z)
{
    TerrainGrid grid = m_pHeightMap->GetTerrainGrid(x, z);
    return GetDetailGrid((int)grid.nRow, (int)grid.nCol);
}

TerrainDetailGrid* TerrainDetailLayer::GetDetailGrid(int nRow, int nCol)
{
    return &m_vtDetailGrids[nRow * m_pHeightMap->GetGridsNumPerCol() + nCol];
}

