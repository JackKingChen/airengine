#include "stdafx.h"
//#include "stdafx.h"
#include "TerrainBlock.h"

#include "Camera.h"
#include "Engine.h"
#include "HeightMap.h"
#include "Terrain.h"
#include "Primitive.h"
#include "ISceneManager.h"
#include "PageStaticGeometory.h"
#include "MaterialManager.h"
//#include "Technique.h"
//#include "Pass.h"
//#include "TextureUnit.h"
#include "GPUProgramManager.h"
//#include "TechniqueSelector.h"

CTerrainBlock::CTerrainBlock() : m_nRow(0),
m_nCol(0),
m_nIndex(0), 
m_dwVertexIndex(0),
m_wBlockSize(0), 
m_dwNumBlockIndices(0), 
m_dwNumBlockVertices(0), 
m_pUseLOD(NULL), 
m_pHeightMap(NULL)
, m_fSphereHalf(0)
{
    m_bTransformByParent = false;
	m_szType = "terrain";
    m_renderGroupType = eRGT_Terrain;
	m_strVertexDecl = "E_VB_NORMAL";
}

CTerrainBlock::CTerrainBlock(int nRow, int nCol, int nCenterX, int nCenterY, u16 wBlockSize, u32 dwTerrainSize, 
                             float fGridLength, CHeightMap* pHeightMap) : 
m_nRow(nRow),
m_nCol(nCol),
m_nCenterX(nCenterX),
m_nCenterY(nCenterY),
m_pHeightMap(pHeightMap)
, m_fSphereHalf(0)
{
    //m_nCenterY = nCenterY;
    //m_nCenterX = nCenterX;

    m_wBlockSize = wBlockSize;
    m_wGridsPerRow = m_wBlockSize - 1;
    m_dwTerrainSize = dwTerrainSize;

    m_fBlockWidth = (float)m_wGridsPerRow * fGridLength;

    m_nTopLeftX = m_nCenterX - (int)(wBlockSize - 1) / 2;
    m_nTopLeftY = m_nCenterY - (int)(wBlockSize - 1) / 2;

    m_dwVertexIndex   = m_nTopLeftX + m_nTopLeftY * dwTerrainSize;

    m_nIndex = nRow * pHeightMap->GetBlocksNumPerRow() + nCol;

    m_dwNumBlockVertices = m_wBlockSize * m_wBlockSize;
    m_dwNumBlockIndices = (m_wBlockSize - 1) * (m_wBlockSize - 1) * 6;

    m_dwTotalIndex = 0;

    m_nRealCell = 0;

    m_nLODLevel = 1;

    //m_nLeftIndexCount = 0;
    //m_nTopIndexCount = 0;
    //m_nRightIndexCount = 0;
    //m_nBottomIndexCount = 0;

    memset(m_nNeighbor, 0, sizeof(m_nNeighbor));

    m_pVertexBuffer = NULL;
    m_pNormalVB = NULL;

    m_wIndexStride = 0;

    m_bTransformByParent = false;

    m_szType = "terrain";

    /*
    if (ENGINE_INST->GetConfig()->m_bNewTerrain)
    {
        //根据blockindex创建material
        char szMaterial[256] = { 0 };
        sprintf(szMaterial, "%s/block_%d", m_pHeightMap->GetMaterialFile().c_str(), m_nIndex);
        m_pMaterial = MATERIAL_MANAGER->CreateMaterial(szMaterial);
        if (m_pMaterial == 0)
        {
            m_pMaterial = MATERIAL_MANAGER->CreateMaterial("TemplateMaterial.xml/Terrain");
        }
        
    }
    else
    {
        m_pMaterial = m_pHeightMap->GetMaterial();
        m_bShareParentMaterial = true;
    }
    */
    
    m_renderGroupType = eRGT_Terrain;
	m_strVertexDecl = "E_VB_NORMAL";
}

CTerrainBlock::~CTerrainBlock(void)
{
}

bool CTerrainBlock::LoadRenderData(const LPVERTEX_NORMAL pVertices)
{
    CreateVertexBuffer();
    ComputeVertices(pVertices);

    //CreateIndics();

	//m_bActive = true;
    return true;
}


bool CTerrainBlock::Create(CHeightMap* pHeightMap, const LPVERTEX_NORMAL pCusVertices)
{
    m_pHeightMap = pHeightMap;
    CreateVertexBuffer();
    ComputeVertices(pCusVertices);

    ComputeNeighbor();

    ComputeSphereHalf(pHeightMap);

    if (ENGINE_INST->GetConfig()->m_bNewTerrain)
    {
        return CreateMaterial();
    }
    else
    {
        m_pMaterial = m_pHeightMap->GetMaterial();
        m_bShareParentMaterial = true;
    }
    
    //m_bActive = true;
    return true;
}

void CTerrainBlock::UnLoadRenderData()
{
    IRenderer* pRenderer = ENGINE_INST->GetRenderer();
    pRenderer->ClearVertexBuffer(m_pVertexBuffer);
    m_pVertexBuffer = NULL;
    //SafeDelete(m_pVertexBuffer);
    //SAFE_RELEASE(m_pIndexBuffer);

}

bool CTerrainBlock::ComputeVertices(const LPVERTEX_NORMAL pCusVertices)
{
    
    //LPVERTEX_NORMAL pVertices = NULL;
    // Fill vertex buffer
    LPVERTEX_NORMAL pVertices = (LPVERTEX_NORMAL)m_pVertexBuffer->Lock();

    if (pVertices == NULL)
    {
        m_pVertexBuffer->Unlock();
        return false;
    }

    float uCoordIncrementSize = 1.0f / (float)(m_wBlockSize - 1);
    float vCoordIncrementSize = 1.0f / (float)(m_wBlockSize - 1);
    

	u32 dwTerrainSize = m_dwTerrainSize;//TERRAINSCENE->GetTerrain()->GetTerrainSize();
    //m_vtCusVerts.resize(m_dwNumGroundVertices);
    m_WorldAABB.MinEdge.x = pCusVertices[0].vPos.x;
    m_WorldAABB.MaxEdge.z = pCusVertices[0].vPos.z;
    m_WorldAABB.MaxEdge.x = pCusVertices[(m_wBlockSize - 1) * (m_wBlockSize - 1)].vPos.x;
    m_WorldAABB.MinEdge.z = pCusVertices[(m_wBlockSize - 1) * (m_wBlockSize - 1)].vPos.z;

    for (u16 i = 0; i < m_wBlockSize; i++)          //行
    {
        for (u16 j = 0; j < m_wBlockSize; j++)       //列
        {
            u32 nIndex = m_nTopLeftX + j + (m_nTopLeftY + i) * dwTerrainSize;
            //pVertices->x = pCusVertices[nIndex].x;
            //pVertices->y = pCusVertices[nIndex].y;
            //pVertices->z = pCusVertices[nIndex].z;
            pVertices->vPos = pCusVertices[nIndex].vPos;
            m_WorldAABB.MaxEdge.y = Max(m_WorldAABB.MaxEdge.y, pVertices->vPos.y);
            m_WorldAABB.MinEdge.y = Min(m_WorldAABB.MinEdge.y, pVertices->vPos.y);
            pVertices->normal = pCusVertices[nIndex].normal;
            pVertices->color = SColor(255, 255, 255, 255);
            pVertices->tu = ENGINE_INST->GetConfig()->m_bNewTerrain ? (float)j * uCoordIncrementSize : pCusVertices[nIndex].tu;  //;
            pVertices->tv = ENGINE_INST->GetConfig()->m_bNewTerrain ? (float)i * vCoordIncrementSize : pCusVertices[nIndex].tv;   //;
            pVertices++;
        }
    }

    //pVB->Unlock();
    m_pVertexBuffer->Unlock();

    //以下是计算顶点法向量
    LPVERTEX_NOTEX pNormal = (LPVERTEX_NOTEX)m_pNormalVB->Lock();

    for (u16 i = 0; i < m_wBlockSize; i++)          //行
    {
        for (u16 j = 0; j < m_wBlockSize; j++)       //列
        {
            u32 nIndex = m_nTopLeftX + j + (m_nTopLeftY + i) * dwTerrainSize;
            pNormal->position = pCusVertices[nIndex].vPos;
            //pNormal->position.x = pCusVertices[nIndex].x;
            //pNormal->position.y = pCusVertices[nIndex].y;
            //pNormal->position.z = pCusVertices[nIndex].z;
            pNormal->color = 0xcccccccc;
            pNormal++;

            Vector3Df vPos = pCusVertices[nIndex].vPos + pCusVertices[nIndex].normal * 20.0f;

            //pNormal->position.x = vPos.x;
            //pNormal->position.y = vPos.y;
            //pNormal->position.z = vPos.z;
            pNormal->position = vPos;
            pNormal->color = 0xcccccccc;
            pNormal++;
        }
    }

    m_pNormalVB->Unlock();

    m_boundingBox = m_WorldAABB;

    m_fDeltaH = m_boundingBox.MaxEdge.y - m_boundingBox.MinEdge.y;

    return true;
}

bool CTerrainBlock::CreateVertexBuffer()
{
    IRenderer* pRenderer = ENGINE_INST->GetRenderer();
    //m_pVertexBuffer = pRenderer->CreateVertexBuffer(E_VB_NORMAL, m_dwNumBlockVertices/* * sizeof(VERTEX_NORMAL)*/);
    m_pVertexBuffer = pRenderer->CreateVertexBuffer(m_dwNumBlockVertices, sizeof(VERTEX_NORMAL));
    if (m_pVertexBuffer == NULL)
    {
        return false;
    }

    //m_pNormalVB = pRenderer->CreateVertexBuffer(E_VB_NOTEX, m_dwNumBlockVertices * 2/* * sizeof(VERTEX_NOTEX)*/);
    m_pNormalVB = pRenderer->CreateVertexBuffer(m_dwNumBlockVertices * 2, sizeof(VERTEX_NOTEX));

    return (m_pNormalVB != NULL);
    
}

void CTerrainBlock::Clear()
{
    IRenderer* pRenderer = ENGINE_INST->GetRenderer();
    //for (int i = 0; i < m_vtTextures.Size(); i++)
    //{
    //    pRenderer->ClearTexture(m_vtTextures[i]);
    //}
    Renderable::Clear();
    
    pRenderer->ClearVertexBuffer(m_pVertexBuffer);
    m_pVertexBuffer = NULL;

    pRenderer->ClearVertexBuffer(m_pNormalVB);
    m_pNormalVB = NULL;

    RenderableVectorIter itEnd = m_vtRenderables.end();
    for (RenderableVectorIter it = m_vtRenderables.begin(); it != itEnd; ++it)
    {
        CSceneNode* pNode = (*it)->GetNodeParent();
        (*it)->Clear();
        SafeDelete(*it);

        if (pNode && pNode->GetAttachRenderablesNum() == 0 && pNode->GetChildrenNum() == 0)
        {
            pNode->GetCreator()->DestroySceneNode(pNode);
        }
    }

    m_vtRenderables.clear();

    //m_pEffect = NULL;
}

//计算LOD值
void CTerrainBlock::ComputeLOD(CCamera* pCamera)
{
    Vector3Df vCameraPos;
    //CAMERAINST->GetEvePos(&vCameraPos);
    pCamera->GetPosition(vCameraPos);
    Vector3Df vLength = m_vtCenter - vCameraPos;
    float fCamDis = vLength.GetLength();//D3DXVec3Length(&vLength);
    m_LOD.ComputeLOD(fCamDis, m_fBlockWidth, m_fDeltaH);
    
}

void CTerrainBlock::ComputeSphereHalf(CHeightMap* pTerrain)
{
    Vector3Df vCenter2Pos;

    //u32 dwGridCount = m_wBlockSize - 1;
    u32 dwCenterIndex = m_nCenterX + m_nCenterY * m_dwTerrainSize;
    m_vtCenter = pTerrain->GetVertexPos(dwCenterIndex); //->position;

    //float fMinH = m_vtCenter.y;
    //float fMaxH = fMinH;

    //左上角
    //m_vtCorner.push_back(dwTopLeft);
    Vector3Df v = pTerrain->GetVertexPos(m_dwVertexIndex); //->position;
    //D3DXVECTOR3 v = pTerrain->GetVertex(m_dwIndex)->position;
    vCenter2Pos = m_vtCenter - v;
    m_fSphereHalf = Max<float>(m_fSphereHalf, vCenter2Pos.GetLength());//D3DXVec3Length(&vCenter2Pos));
    //fMinH = Min<float>(fMinH, v.y);
    //fMaxH = Max<float>(fMaxH, v.y);

    //右上角
    //m_vtCorner.push_back(dwTopLeft + nRow);
    v = pTerrain->GetVertexPos(m_dwVertexIndex + m_wGridsPerRow); //->position;
    vCenter2Pos = m_vtCenter - v;
    m_fSphereHalf = Max<float>(m_fSphereHalf, vCenter2Pos.GetLength());//D3DXVec3Length(&vCenter2Pos));
    //fMinH = Min<float>(fMinH, v.y);
    //fMaxH = Max<float>(fMaxH, v.y);

    //右下角
    v = pTerrain->GetVertexPos(m_dwVertexIndex + m_wGridsPerRow + m_wGridsPerRow * m_dwTerrainSize); //->position;
    vCenter2Pos = m_vtCenter - v;
    m_fSphereHalf = Max<float>(m_fSphereHalf, vCenter2Pos.GetLength());//D3DXVec3Length(&vCenter2Pos));
    //fMinH = Min<float>(fMinH, v.y);
    //fMaxH = Max<float>(fMaxH, v.y);

    //左下角
    
    v = pTerrain->GetVertexPos(m_dwVertexIndex + m_wGridsPerRow * m_dwTerrainSize); //->position;
    vCenter2Pos = m_vtCenter - v;
    m_fSphereHalf = Max<float>(m_fSphereHalf, vCenter2Pos.GetLength());//D3DXVec3Length(&vCenter2Pos));
    //fMinH = Min<float>(fMinH, v.y);
    //fMaxH = Max<float>(fMaxH, v.y);

    //m_fDeltaH = fMaxH - fMinH;
}

void CTerrainBlock::FixCrack()
{
    
    //CHeightMap* pHeightMap = TERRAIN->GetHeightMap(m_nHeightMapID);
    //上方
    CTerrainBlock* pBlock = m_pHeightMap->GetBlock(m_nNeighbor[0]);

    if (pBlock)
    {
        m_LOD.m_nTopLODDelta = pBlock->GetLODLevel() - m_LOD.m_nLODLevel;
        m_LOD.m_nTopLODDelta = m_LOD.m_nTopLODDelta > 0 ? m_LOD.m_nTopLODDelta : 0;
    }

    //右方
    pBlock = m_pHeightMap->GetBlock(m_nNeighbor[1]);
    if (pBlock)
    {
        m_LOD.m_nRightLODDelta = pBlock->GetLODLevel() - m_LOD.m_nLODLevel;
        m_LOD.m_nRightLODDelta = m_LOD.m_nRightLODDelta > 0 ? m_LOD.m_nRightLODDelta : 0;
    }

    //下方
    pBlock = m_pHeightMap->GetBlock(m_nNeighbor[2]);
    if (pBlock)
    {
        m_LOD.m_nBottomLODDelta = pBlock->GetLODLevel() - m_LOD.m_nLODLevel;
        m_LOD.m_nBottomLODDelta = m_LOD.m_nBottomLODDelta > 0 ? m_LOD.m_nBottomLODDelta : 0;
    }

    //左方
    pBlock = m_pHeightMap->GetBlock(m_nNeighbor[3]);
    if (pBlock)
    {
        m_LOD.m_nLeftLODDelta = pBlock->GetLODLevel() - m_LOD.m_nLODLevel;
        m_LOD.m_nLeftLODDelta = m_LOD.m_nLeftLODDelta > 0 ? m_LOD.m_nLeftLODDelta : 0;
    }

    if (m_LOD.m_nLeftLODDelta == 1)
    {
        int nDebug = 0;
    }


    m_pUseLOD = TERRAIN->FindLODInfo(m_LOD);
}

void CTerrainBlock::ComputeNeighbor()
{
    m_nNeighbor[0] = (int)m_nIndex - m_pHeightMap->GetBlocksNumPerRow();  //m_nTopLeftX + (m_nTopLeftY - m_wGridsPerRow) * m_dwTerrainSize;   //上方
    m_nNeighbor[1] = (m_nCol > m_pHeightMap->GetBlocksNumPerRow() - 1) ? -1 : m_nIndex + 1;   //m_nTopLeftX + m_wGridsPerRow + m_nTopLeftY * m_dwTerrainSize;     //右方
    m_nNeighbor[2] = (int)m_nIndex + m_pHeightMap->GetBlocksNumPerRow();  //m_nTopLeftX + (m_nTopLeftY + m_wGridsPerRow) * m_dwTerrainSize;   //下方  
    m_nNeighbor[3] = m_nCol == 0 ? -1 : m_nIndex - 1;   //m_nTopLeftX - m_wGridsPerRow + m_nTopLeftY * m_dwTerrainSize;     //左方                
}

bool CTerrainBlock::IsInFrustum(const CFrustum* pFrustum) const
{
    if (pFrustum->IsSphereIn(m_vtCenter, m_fSphereHalf))
    {
        return true;
    }
    return false;
}

void CTerrainBlock::PreRender(CCamera* pCamera)
{
    FixCrack();
}

/*
void CTerrainBlock::Render(CCamera* pCamera)
{
    //更新块数据
    PreRender(pCamera);

    RENDERER->SetWorldTransform(&Matrix4f::IDENTITY);

    _GetPrimitive();
    
    m_pPrimitive->Draw(m_pMaterial->GetCurrentTechnique());
}
*/

void CTerrainBlock::DrawNormals()
{
    //绘制法线
    IRenderer* pRenderer = ENGINE_INST->GetRenderer();
    pRenderer->SetVertexBuffer(0, m_pNormalVB);

    //pRenderer->DrawLineList(0, m_wBlockSize * m_wBlockSize);
}
/*
void CTerrainBlock::GenerateDetailData()
{
    IRenderer* pRenderer = ENGINE_INST->GetRenderer();
    if (m_pVBDetail == NULL)
    {
        m_pVBDetail = pRenderer->CreateVertexBuffer(E_VB_NORMAL, m_wGridsPerRow * m_wGridsPerRow * 4);
    }

    if (m_pIBDetail == NULL)
    {
        m_pIBDetail = pRenderer->CreateIndexBuffer(m_wGridsPerRow * m_wGridsPerRow * 6);
    }

    u16* pIndices = (u16*)m_pIBDetail->Lock(0, 0);

    int nGridsNum = m_wGridsPerRow * m_wGridsPerRow;
    for(size_t idx, idxOff, bboard = 0;
        bboard < nGridsNum;
        ++bboard )
    {
        // Do indexes
        idx    = bboard * 6;
        idxOff = bboard * 4;

        pIndices[idx] = static_cast<unsigned short>(idxOff); // + 0;, for clarity
        pIndices[idx + 1] = static_cast<unsigned short>(idxOff + 1);
        pIndices[idx + 2] = static_cast<unsigned short>(idxOff + 2);
        pIndices[idx + 3] = static_cast<unsigned short>(idxOff + 1);
        pIndices[idx + 4] = static_cast<unsigned short>(idxOff + 3);
        pIndices[idx + 5] = static_cast<unsigned short>(idxOff + 2);

    }
    m_pIBDetail->Unlock();
}

void CTerrainBlock::RenderDetail(CCamera* pCamera)
{
    if (IsDetailEnable())
    {
        IRenderer* pRenderer = ENGINE_INST->GetRenderer();

		pRenderer->SetVertexBuffer(0, m_pVBDetail);
        pRenderer->SetIndexBuffer(m_pIBDetail);
        
        pRenderer->DrawTriangleListByIndex(0, 0, m_pVBDetail->GetVertexCount(), 0, m_pIBDetail->GetTotalIndex() / 3);
    }
}


bool CTerrainBlock::CreateDetails()
{
    GenerateDetailData();
    return true;
}
*/
/*
void CTerrainBlock::UpdateDetailVertexBuffer()
{
    if (m_pVBDetail)
    {
        TerrainGrid topLeft;
        topLeft.nRow = m_nRow * m_wGridsPerRow;
        topLeft.nCol = m_nCol * m_wGridsPerRow;
        TerrainGrid bottomRight;
        bottomRight.nRow = (m_nRow + 1) * m_wGridsPerRow - 1;
        bottomRight.nCol = (m_nCol + 1) * m_wGridsPerRow - 1;

        TerrainDetailLayer* pDetailLayer = m_pHeightMap->GetDetailLayer();
        LPVERTEX_NORMAL pVertices = (LPVERTEX_NORMAL)m_pVBDetail->Lock();

        for (int nRow = topLeft.nRow, x = 0; nRow <= bottomRight.nRow; ++nRow, ++x)
        {
            for (int nCol = topLeft.nCol, y = 0; nCol <= bottomRight.nCol; ++nCol, ++y)
            {
                //一格一格的数据填充
                //左上角
                TerrainDetailGrid* pGrid = pDetailLayer->GetDetailGrid(nRow, nCol);
                LPVERTEX_NORMAL pTerrainVertex = m_pHeightMap->GetVertex(nRow * m_pHeightMap->GetWidthSize() + nCol);
                pVertices->color = 0xffffffff;
                pVertices->vPos = pTerrainVertex->vPos;
                //pVertices->position.y += 5.0f;
                pVertices->tu = pGrid->m_uv.left;  //y * m_pHeightMap->GetGridWidth() * invDecalX - fDecalOffsetX * invDecalX;
                pVertices->tv = pGrid->m_uv.top;   //x * m_pHeightMap->GetGridDepth() * invDecalY - fDecalOffsetY * invDecalY;
                pVertices++;

                //右上角
                int nRealCol = nCol + 1;
                if (nRealCol > m_pHeightMap->GetGridsNumPerCol() )
                {
                    nRealCol = m_pHeightMap->GetGridsNumPerCol();
                }
                pTerrainVertex = m_pHeightMap->GetVertex(nRow * m_pHeightMap->GetWidthSize() + nRealCol);
                pVertices->color = 0xffffffff;
                pVertices->vPos = pTerrainVertex->vPos;
                //pVertices->position.y += 5.0f;
                pVertices->tu = pGrid->m_uv.right;  //y * m_pHeightMap->GetGridWidth() * invDecalX - fDecalOffsetX * invDecalX + fUDelta;
                pVertices->tv = pGrid->m_uv.top;  //x * m_pHeightMap->GetGridDepth() * invDecalY - fDecalOffsetY * invDecalY;
                pVertices++;

                //左下角
                int nRealRow = nRow + 1;
                if (nRealRow > m_pHeightMap->GetGridsNumPerRow())
                {
                    nRealRow = m_pHeightMap->GetGridsNumPerRow();
                }
                pTerrainVertex = m_pHeightMap->GetVertex(nRealRow * m_pHeightMap->GetWidthSize() + nCol);
                pVertices->color = 0xffffffff;
                pVertices->vPos = pTerrainVertex->vPos;
                //pVertices->position.y += 5.0f;
                pVertices->tu = pGrid->m_uv.left;  //y * m_pHeightMap->GetGridWidth() * invDecalX - fDecalOffsetX * invDecalX;
                pVertices->tv = pGrid->m_uv.bottom;  //x * m_pHeightMap->GetGridDepth() * invDecalY - fDecalOffsetY * invDecalY + fVDelta;
                pVertices++;

                //右下角
                pTerrainVertex = m_pHeightMap->GetVertex(nRealRow * m_pHeightMap->GetWidthSize() + nRealCol);
                pVertices->color = 0xffffffff;
                pVertices->vPos = pTerrainVertex->vPos;
                //pVertices->position.y += 5.0f;
                pVertices->tu = pGrid->m_uv.right;  //y * m_pHeightMap->GetGridWidth() * invDecalX - fDecalOffsetX * invDecalX + fUDelta;
                pVertices->tv = pGrid->m_uv.bottom;  //x * m_pHeightMap->GetGridDepth() * invDecalY - fDecalOffsetY * invDecalY + fVDelta;
                pVertices++;
            }
        }
    }
}
*/
/*
void CTerrainBlock::RenderToGBuffer(CCamera* pCamera, CPass* pPass)
{
    PreRender(pCamera);
    IRenderer* pRenderer = ENGINE_INST->GetRenderer();
    pRenderer->SetVertexBuffer(0, m_pVertexBuffer);
    IIndexBuffer* pIndexBuffer = m_pUseLOD->GetIndexBuffer();
    m_dwTotalIndex = pIndexBuffer->GetTotalIndex();
    pRenderer->SetIndexBuffer(pIndexBuffer);
    IMaterial* pMaterial = m_pHeightMap->GetMaterial();
    CTextureUnit* pTextureUnit = pMaterial->GetTechnique(0)->GetPass(0)->GetTextureUnit(1);
    pPass->GetTextureUnit(0)->ApplyTextureUnit(pPass, pTextureUnit->GetTexture());
    pPass->ApplyShaders();
    pRenderer->DrawTriangleListByIndex(0, 0, m_dwNumBlockVertices, 0, m_dwTotalIndex / 3);
}

void CTerrainBlock::RenderToDepthMap(CCamera* pCamera, CPass* pPass)
{
	PreRender(pCamera);
	IRenderer* pRenderer = ENGINE_INST->GetRenderer();
	pRenderer->SetVertexBuffer(0, m_pVertexBuffer);
	IIndexBuffer* pIndexBuffer = m_pUseLOD->GetIndexBuffer();
	m_dwTotalIndex = pIndexBuffer->GetTotalIndex();
	pRenderer->SetIndexBuffer(pIndexBuffer);
	IMaterial* pMaterial = m_pHeightMap->GetMaterial();
	CTextureUnit* pTextureUnit = pMaterial->GetTechnique(0)->GetPass(0)->GetTextureUnit(1);
	pPass->GetTextureUnit(0)->ApplyTextureUnit(pPass, pTextureUnit->GetTexture());
	pPass->ApplyShaders();
	pRenderer->DrawTriangleListByIndex(0, 0, m_dwNumBlockVertices, 0, m_dwTotalIndex / 3);
}
*/

void CTerrainBlock::_GetPrimitive()
{
    if (m_pPrimitive == NULL)
    {
        m_pPrimitive = new Primitive(Primitive::Primitive_TriangleList/*, m_pVertexBuffer->GetVertexDeclaration()*/);
        m_pPrimitive->m_pSrcRenderable = this;
    }

    m_pPrimitive->m_pVertexBuffer = m_pVertexBuffer;
    m_pPrimitive->m_pIndexBuffer = m_pUseLOD->GetIndexBuffer();
}
/*
void CTerrainBlock::GenRenderToGBufferTechnique()
{
    if (ENGINE_INST->GetConfig()->m_bNewTerrain)
    {
        m_pTechniqueRenderToGBuffer = m_pMaterial->GetTechnique("TerrainRenderToGBuffer");
        if (m_pTechniqueRenderToGBuffer == NULL)
        {
            m_pTechniqueRenderToGBuffer = new CTechnique(m_pMaterial);
            *m_pTechniqueRenderToGBuffer = *(m_pMaterial->GetTechnique(0));
            m_pTechniqueRenderToGBuffer->SetName("TerrainRenderToGBuffer");
            //m_pTechniqueRenderToGBuffer->SetVSVersion("vs_2_0");
            //m_pTechniqueRenderToGBuffer->SetPSVersion("ps_2_0");

            CPass* pPass = m_pTechniqueRenderToGBuffer->GetPass(0);
            CTextureUnit* pUnit = pPass->GetTextureUnit(0);
            CTextureUnit* pUnit2 = pPass->GetTextureUnit(1);
            pUnit->SetTexture(pUnit2->GetTexture());
            pUnit2->GetTexture()->AddRef();
            //把第1阶段以后的纹理都清除
            for (size_t i = 1; i < 8; ++i)
            {
                pPass->RemoveTextureUnit(i);
            }
            //IShader* pShader = pPass->GetShader();
            //pShader->Clear();
            pPass->SetFogEnable(false);
            pPass->SetAlphaBlend(false);

            AIRString strPixelShader = "PSLPPRenderToGBuffer_3_0";
            AIRString strVertexShader = "VSTerrainRenderToGBuffer";

            IRenderer* pRenderer = ENGINE_INST->GetRenderer();
            IGPUProgram* pVertexShader = GPUPROGRAM_MANAGER->GetGPUProgram(strVertexShader); //pRenderer->CreateVertexShader(strVertexShader.c_str(), m_pTechniqueRenderToGBuffer->GetVSVersion().c_str());
            IGPUProgram* pPixelShader = GPUPROGRAM_MANAGER->GetGPUProgram(strPixelShader); //pRenderer->CreatePixelShader(strPixelShader.c_str(), m_pTechniqueRenderToGBuffer->GetPSVersion().c_str());
            //pPass->SetVertexShader(pVertexShader);
            //pPass->SetPixelShader(pPixelShader);
			IGPUProgram* pGpuPrograms[4] = { 0 };
			pGpuPrograms[0] = pVertexShader;
			pGpuPrograms[1] = pPixelShader;
			ShaderObject* pShaderObject = pRenderer->CreateShaderObject(pGpuPrograms, 2);
			pPass->SetShaderObject(pShaderObject);

            m_pMaterial->AddTechnique(m_pTechniqueRenderToGBuffer);
        }
    }
    else
    {
        if (m_pTechniqueRenderToGBuffer == 0)
        {
            m_pHeightMap->GenRenderToGBufferTechnique();
        }
        m_pTechniqueRenderToGBuffer = m_pHeightMap->GetTechniqueRenderToGBuffer();
    }
    
}
*/
bool CTerrainBlock::Prepare()
{
    if (!m_bPrepared)
    {
        m_bPrepared = Renderable::Prepare();
		if (m_bPrepared)
		{
			//TECHNIQUE_SELECTOR->SelectTechnique(this);
		}
    }
    return m_bPrepared;
}

void CTerrainBlock::AddRenderable(Renderable* pRenderable)
{
    RenderableVectorIter it = std::find(m_vtRenderables.begin(), m_vtRenderables.end(), pRenderable);

    if (it == m_vtRenderables.end())
    {
        m_vtRenderables.push_back(pRenderable);
    }
}

void CTerrainBlock::GetVisibleRenderables(CCamera* pCamera, CRenderQueue* pQueue)
{
    RenderableVectorIter itEnd = m_vtRenderables.end();
    for (RenderableVectorIter it = m_vtRenderables.begin(); it != itEnd; ++it)
    {
        if (pCamera->IsVisable((*it)->GetWorldBoundingBox(false)))
        {
            pQueue->AddRenderable(*it);
        }
    }
}

//新方法，创建地形块
bool CTerrainBlock::Create(const f32* fHeights)
{
    CreateVertexBuffer();

    //填充顶点数据
    ComputeVertices(fHeights);

    return true;
}

bool CTerrainBlock::ComputeVertices(const f32* fHeights)
{
    LPVERTEX_NORMAL pVertices = (LPVERTEX_NORMAL)m_pVertexBuffer->Lock();

    if (pVertices == NULL)
    {
        m_pVertexBuffer->Unlock();
        return false;
    }

    f32 fGridLength = m_pHeightMap->GetGridWidth();
    int nBlocksPerCol = m_pHeightMap->GetBlocksNumPerRow();
    int nBlocksPerRow = nBlocksPerCol;
    int nGridsPerCol = m_pHeightMap->GetWidthSize() - 1;   //每列格数
    int nGridsPerRow = m_pHeightMap->GetDepthSize() - 1;   //每行格数

    const Vector3Df& vtCenter = m_pHeightMap->GetCenter();

    int startX = fGridLength * (m_nTopLeftX * m_wGridsPerRow - nGridsPerCol / 2) + (int)vtCenter.x;
    int startZ = fGridLength * (m_nTopLeftY * m_wGridsPerRow - nGridsPerRow / 2) + (int)vtCenter.z;

    // coordinates to end generating vertices at
    int endX = startX + fGridLength * m_wBlockSize;
#ifdef LEFT_HAND_COORDINATE
    int endZ = startZ - fGridLength * m_wBlockSize;
#else
    int endZ = startZ + fGridLength * m_wBlockSize;
#endif
    float uCoordIncrementSize = 1.0f / (float)m_pHeightMap->GetWidthSize();
    float vCoordIncrementSize = 1.0f / (float)m_pHeightMap->GetDepthSize();
    //f32 uCoordIncrementSize = 2.0f / (float)(m_wBlockSize - 1);
    //f32 vCoordIncrementSize = 2.0f / (float)(m_wBlockSize - 1);


    u32 dwTerrainSize = m_dwTerrainSize;//TERRAINSCENE->GetTerrain()->GetTerrainSize();
    //m_vtCusVerts.resize(m_dwNumGroundVertices);

    u32 nIndex = 0;
    int i = m_nTopLeftY * m_wGridsPerRow;
#ifdef LEFT_HAND_COORDINATE
    for (int z = startZ; z >= endZ; z -= fGridLength)          //行
#else
    for (int z = startZ; z >= endZ; z += fGridLength)          //行
#endif
    {
        int j = m_nTopLeftX * m_wGridsPerRow;
        for (int x = startX; x <= endX; x += fGridLength)       //列
        {
            pVertices->vPos.y = fHeights[nIndex];
            pVertices->vPos.x = x;
            pVertices->vPos.z = z;
            m_WorldAABB.MaxEdge.y = Max(m_WorldAABB.MaxEdge.y, pVertices->vPos.y);
            m_WorldAABB.MinEdge.y = Min(m_WorldAABB.MinEdge.y, pVertices->vPos.y);
            //pVertices->normal = pCusVertices[nIndex].normal;
            pVertices->color = SColor(255, 255, 255, 255);
            pVertices->tu = (float)j * uCoordIncrementSize;
            pVertices->tv = (float)i * vCoordIncrementSize;
            pVertices++;
            nIndex++;
            j++;
        }
        i++;
    }

    //pVB->Unlock();
    m_pVertexBuffer->Unlock();

    //以下是计算顶点法向量
    LPVERTEX_NOTEX pNormal = (LPVERTEX_NOTEX)m_pNormalVB->Lock();

    for (u16 i = 0; i < m_wBlockSize; i++)          //行
    {
        for (u16 j = 0; j < m_wBlockSize; j++)       //列
        {
            u32 nIndex = m_nTopLeftX + j + (m_nTopLeftY + i) * dwTerrainSize;
            pNormal->position = m_vtVertices[nIndex].vPos;
            //pNormal->position.x = pCusVertices[nIndex].x;
            //pNormal->position.y = pCusVertices[nIndex].y;
            //pNormal->position.z = pCusVertices[nIndex].z;
            pNormal->color = 0xffffffff;
            pNormal++;

            Vector3Df vPos = m_vtVertices[nIndex].vPos + m_vtVertices[nIndex].normal * 20.0f;

            //pNormal->position.x = vPos.x;
            //pNormal->position.y = vPos.y;
            //pNormal->position.z = vPos.z;
            pNormal->position = vPos;
            pNormal->color = 0xffffffff;
            pNormal++;
        }
    }

    m_pNormalVB->Unlock();

    m_boundingBox = m_WorldAABB;

    m_fDeltaH = m_boundingBox.MaxEdge.y - m_boundingBox.MinEdge.y;
    return true;
}

bool CTerrainBlock::CreateMaterial()
{
    char szMaterial[256] = { 0 };
    sprintf(szMaterial, "%s/block_%d", m_pHeightMap->GetMaterialFile().c_str(), m_nIndex);
    m_pMaterial = MATERIAL_MANAGER->CreateMaterial(szMaterial);

    if (m_pMaterial == 0)
    {
        m_pMaterial = MATERIAL_MANAGER->CreateMaterial("TemplateMaterial.xml/Terrain");
        //m_pMaterial->SetMaterialName(GetString("block_%d", m_nIndex));
    }

    if (m_pMaterial && !m_pMaterial->IsReady())
    {
        m_pMaterial->Cache();
    }

    return m_pMaterial != 0;
}

ITexture* CTerrainBlock::GetIndexTexture()
{
    /*
    if (m_pMaterial)
    {
        CTextureUnit* pTextureUnit = m_pMaterial->GetTechnique(0)->GetPass(0)->GetTextureUnit(0);
        if (pTextureUnit)
        {
            return pTextureUnit->GetTexture();
        }
    }
    */

    return NULL;
}

void CTerrainBlock::GetVertices(LPVERTEX_NORMAL pVertices)
{
    LPVERTEX_NORMAL pCurVertices = (LPVERTEX_NORMAL)m_pVertexBuffer->Lock();

    if (pCurVertices == NULL)
    {
        m_pVertexBuffer->Unlock();
        return;
    }

    u32 dwTerrainSize = m_dwTerrainSize;//TERRAINSCENE->GetTerrain()->GetTerrainSize();


    for (u16 i = 0; i < m_wBlockSize; i++)          //行
    {
        for (u16 j = 0; j < m_wBlockSize; j++)       //列
        {
            u32 nIndex = m_nTopLeftX + j + (m_nTopLeftY + i) * dwTerrainSize;
            //pVertices->x = pCusVertices[nIndex].x;
            //pVertices->y = pCusVertices[nIndex].y;
            //pVertices->z = pCusVertices[nIndex].z;
            pVertices[nIndex].vPos = pCurVertices->vPos;
            pVertices[nIndex].normal = pCurVertices->normal;
            pVertices[nIndex].color = pCurVertices->color;
            pVertices[nIndex].tu = pCurVertices->tu;  //;
            pVertices[nIndex].tv = pCurVertices->tv;   //;
            pCurVertices++;
        }
    }

    m_pVertexBuffer->Unlock();
}

void CTerrainBlock::SetAcceptLighting(bool bAccept)
{
    Renderable::SetAcceptLighting(bAccept);
    //m_pMaterial->SetCurrentTechnique(TECHNIQUE_SELECTOR->SelectTechnique(this));
}

void CTerrainBlock::AcceptShadowmap(bool bAccept)
{
    Renderable::AcceptShadowmap(bAccept);
    //m_pMaterial->SetCurrentTechnique(TECHNIQUE_SELECTOR->SelectTechnique(this));
}
