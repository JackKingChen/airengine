// ***************************************************************
//  文件: TerrainLayer.h
//  日期: 2010-3-24
//  作者: liangairan
//  说明: 地形的一层，每层有一个材质，但一层只对应一个纹理
// ***************************************************************

#pragma once

#include "IMaterial.h"

class EngineExport CTerrainLayer
{
public:
    CTerrainLayer();
    virtual ~CTerrainLayer();

    //渲染层
    virtual void RenderLayer();

    IMaterial* GetMaterial()
    {
        return m_pMaterial;
    }

    //清理层
    virtual void Clear();

    void ScaleUV(float fX, float fY, float fZ);


protected:
    IMaterial* m_pMaterial;    //对应的材质
    int        m_nLayerIndex;  //层的编号

private:
};
