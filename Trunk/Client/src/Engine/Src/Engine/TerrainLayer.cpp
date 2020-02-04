#include "stdafx.h"
#include "TerrainLayer.h"
#include "Engine.h"
#include "MaterialManager.h"

CTerrainLayer::CTerrainLayer() : m_pMaterial(NULL), m_nLayerIndex(0)
{

}

CTerrainLayer::~CTerrainLayer()
{

}

void CTerrainLayer::Clear()
{
    if (m_pMaterial)
    {
        MATERIAL_MANAGER->ClearMaterial(m_pMaterial);
    }
}

void CTerrainLayer::RenderLayer()
{
    IRenderer* pRenderer = ENGINE_INST->GetRenderer();
    if (m_pMaterial)
    {
        //if (!m_pMaterial->HasVertexShader())
        //{
        //    //ÉèÖÃÎÆÀíUV±ä»»
        //    pRenderer->SetTextureTransform(m_nLayerIndex, m_matUV);
        //}
        //m_pMaterial->ApplyMaterial(pRenderer);
    }
}

void CTerrainLayer::ScaleUV(float fX, float fY, float fZ)
{
    if (m_pMaterial)
    {
        //CTextureUnit* pTextureUnit = m_pMaterial->GetTextureUnit(0);
        //if (pTextureUnit)
        //{
        //    Matrix4f matTransform = pTextureUnit->GetTextureMatrix();
        //    matTransform.SetScale(Vector3Df(fX, fY, fZ));
        //}
    }
    
}
