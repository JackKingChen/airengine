#include "stdafx.h"
#include "TerrainQuadTreeNode.h"
#include "HeightMap.h"

CTerrainQuadTreeNode::CTerrainQuadTreeNode(float fWidth, int nCenterX, int nCenterY, int nGridsPerRow, CHeightMap* pHeightMap)
{
    memset(m_pChildren, 0, sizeof(m_pChildren));

    m_pObject = NULL;

    m_fWidth = fWidth;

    m_fSphereHalf = sqrtf( 0.5 * m_fWidth * 0.5 * m_fWidth * 2);

    m_nCenterX = nCenterX;
    m_nCenterY = nCenterY;

    //CHeightMap* pTerrain = TERRAINSCENE->GetTerrain();

    int nCenterIndex = m_nCenterX + m_nCenterY * pHeightMap->GetDepthSize();
    m_CenterPos = pHeightMap->GetVertexPos(nCenterIndex); //->position;

    m_nGridsPerRow = nGridsPerRow;


    //×óÉÏ½Ç
    //m_vtCorner.push_back(dwTopLeft);
    int nTopLeft = m_nCenterX - m_nGridsPerRow / 2 + (m_nCenterY - m_nGridsPerRow / 2) * pHeightMap->GetSize();
    Vector3Df v = pHeightMap->GetVertexPos(nTopLeft); //->position;
    Vector3Df vCenter2Pos = m_CenterPos - v;
    m_fSphereHalf = AIR::Max<float>(m_fSphereHalf, vCenter2Pos.GetLength());
    //fMinH = Min<float>(fMinH, v.y);
    //fMaxH = Max<float>(fMaxH, v.y);

    //ÓÒÉÏ½Ç
    //m_vtCorner.push_back(dwTopLeft + nRow);
    int nTopRight = m_nCenterX + m_nGridsPerRow / 2 + (m_nCenterY - m_nGridsPerRow / 2) * pHeightMap->GetSize();
    v = pHeightMap->GetVertexPos(nTopRight); //->position;
    vCenter2Pos = m_CenterPos - v;
    m_fSphereHalf = AIR::Max<float>(m_fSphereHalf, vCenter2Pos.GetLength()); 
    //fMinH = Min<float>(fMinH, v.y);
    //fMaxH = Max<float>(fMaxH, v.y);

    //ÓÒÏÂ½Ç
    int nBottomRight = m_nCenterX + m_nGridsPerRow / 2 + (m_nCenterY + m_nGridsPerRow / 2) * pHeightMap->GetSize();
    v = pHeightMap->GetVertexPos(nBottomRight); //->position;
    vCenter2Pos = m_CenterPos - v;
    m_fSphereHalf = AIR::Max<float>(m_fSphereHalf, vCenter2Pos.GetLength());
    //fMinH = Min<float>(fMinH, v.y);
    //fMaxH = Max<float>(fMaxH, v.y);

    //×óÏÂ½Ç
    int nBottomLeft  = m_nCenterX - m_nGridsPerRow / 2 + (m_nCenterY + m_nGridsPerRow / 2) * pHeightMap->GetSize();
    v = pHeightMap->GetVertexPos(nBottomLeft); //->position;
    vCenter2Pos = m_CenterPos - v;
    m_fSphereHalf = AIR::Max<float>(m_fSphereHalf, vCenter2Pos.GetLength());

    m_nCullFlag = FLOW_SCENE | FLOW_REFLECTION | FLOW_RETRACTION;
}

CTerrainQuadTreeNode::~CTerrainQuadTreeNode()
{

}

void CTerrainQuadTreeNode::Attach(CTerrainBlock* pObject)
{
    m_pObject = pObject;
    m_fSphereHalf = pObject->GetSphereHalf();
}

bool CTerrainQuadTreeNode::IsVisable(const CFrustum* pFrustum)
{
    return pFrustum->IsSphereIn(m_CenterPos, m_fSphereHalf);
}

//bool CTerrainQuadTreeNode::IsInFrustum(const CFrustum* pFrustum)
//{
//    return pFrustum->IsSphereIn(m_CenterPos, m_fSphereHalf);
//}

CTerrainQuadTreeNode* CTerrainQuadTreeNode::FindAttachNode(CTerrainBlock* pObject)
{
    CTerrainQuadTreeNode* pNode = NULL;
    if (CanAttach(pObject))
    {
        return this;
    }
    else
    {
        for (int i = 0; i < 4; i++)
        {
            if (m_pChildren[i]->IsInside(pObject))
            {
                pNode = m_pChildren[i]->FindAttachNode(pObject);
                break;
            }
        }
    }

    return pNode;
}

bool CTerrainQuadTreeNode::IsInside(CTerrainBlock* pBlock)
{
    int nBlockGridsPerRow = pBlock->GetBlockSize() - 1;
    int nTopLeftX = pBlock->GetTopLeftX();
    int nTopLeftY = pBlock->GetTopLeftY();
    int nTopRightX = nTopLeftX + nBlockGridsPerRow;
    int nTopRightY = nTopLeftY;
    int nBottomRightX = nTopLeftX + nBlockGridsPerRow;
    int nBottomRightY = nTopLeftY + nBlockGridsPerRow;
    if (nTopLeftX >= m_nCenterX - m_nGridsPerRow * 0.5 && nTopLeftY >= m_nCenterY - m_nGridsPerRow * 0.5
        && nBottomRightX <= m_nCenterX + m_nGridsPerRow * 0.5 && nBottomRightY <= m_nCenterY + m_nGridsPerRow * 0.5)
    {
        return true;
    }


    return false;
}

bool CTerrainQuadTreeNode::CanAttach(CTerrainBlock* pBlock)
{
    if (pBlock->GetCenterX() == m_nCenterX && pBlock->GetCenterY() == m_nCenterY && pBlock->GetBlockSize() - 1 == m_nGridsPerRow)
    {
        return true;
    }
    return false;
}

void CTerrainQuadTreeNode::GetRenderObjects(AIRList<CTerrainBlock*>& lstRenderQueue, CCamera* pCamera)
{
    if (IsVisable(pCamera->GetFrustum()))
    {
        if (m_pObject)
        {
            if (m_pObject->IsInFrustum(pCamera->GetFrustum()))
            {
                //m_pObject->ComputeLOD(pCamera);
                lstRenderQueue.push_back(m_pObject);
            }
        }
        else
        {
            for (int i = 0; i < 4; i++)
            {
                m_pChildren[i]->GetRenderObjects(lstRenderQueue, pCamera);
            }
        }

    }
}

void CTerrainQuadTreeNode::SetQuadTree(CTerrainQuadTree* pTree)
{
    m_pQuadTree = pTree;
}

void CTerrainQuadTreeNode::GetVisableNodes(CCamera* pCamera, AIRList<CSceneNode*>& lstVisableNodes)
{
    if (IsVisable(pCamera->GetFrustum()))
    {
        if (m_pObject)
        {
            if (m_pObject->IsInFrustum(pCamera->GetFrustum()))
            {
                m_pObject->ComputeLOD(pCamera);
                lstVisableNodes.push_back(this);
            }
        }
        else
        {
            for (int i = 0; i < 4; i++)
            {
                m_pChildren[i]->GetVisableNodes(pCamera, lstVisableNodes);
            }
        }

    }
}

void CTerrainQuadTreeNode::FillRenderContext(CCamera* pCamera, RenderContextQueue* pQueue, eRenderFlow flow)
{
    if (m_pObject && m_pObject->IsVisable())
    {
        m_pObject->FillRenderContext(pCamera, pQueue, flow);
    }
}
