// ***************************************************************
//  �ļ�: TerrainLayer.h
//  ����: 2010-3-24
//  ����: liangairan
//  ˵��: ���ε�һ�㣬ÿ����һ�����ʣ���һ��ֻ��Ӧһ������
// ***************************************************************

#pragma once

#include "IMaterial.h"

class EngineExport CTerrainLayer
{
public:
    CTerrainLayer();
    virtual ~CTerrainLayer();

    //��Ⱦ��
    virtual void RenderLayer();

    IMaterial* GetMaterial()
    {
        return m_pMaterial;
    }

    //�����
    virtual void Clear();

    void ScaleUV(float fX, float fY, float fZ);


protected:
    IMaterial* m_pMaterial;    //��Ӧ�Ĳ���
    int        m_nLayerIndex;  //��ı��

private:
};
