#include "stdafx.h"
//#include "stdafx.h"
#include "QuadTreeNode.h"

//#include "TerrainSceneManager.h"
#include "MyMath.h"
#include "HeightMap.h"
using namespace AIR;

CQuadTreeNode::CQuadTreeNode(void)
{
    m_szType = "quadtreenode";
    m_fSphereHalf = 0.0;

    m_pObject = NULL;

    m_nCenterX = 0;
    m_nCenterY = 0;
}

CQuadTreeNode::CQuadTreeNode(ISceneManager* pCreator, const AIRString& strName, const AIR::Vector3Df &vPosition, const AIR::Quaternion &qRotation, const AIR::Vector3Df &vScale) : CSceneNode(pCreator, strName, vPosition, qRotation, vScale)
{

}

/*
CQuadTreeNode::CQuadTreeNode(float fWidth, int nCenterX, int nCenterY, int nGridsPerRow, int nVerticesPerRow, const Vector3Df& vCenter)
{
    memset(m_pChilds, 0, sizeof(m_pChilds));

    m_pObject = NULL;

    m_fWidth = fWidth;

    m_fSphereHalf = sqrtf( 0.5 * m_fWidth * 0.5 * m_fWidth * 2);

    m_nCenterX = nCenterX;
    m_nCenterY = nCenterY;

    //CHeightMap* pTerrain = TERRAINSCENE->GetTerrain();

    int nCenterIndex = m_nCenterX + m_nCenterY * pHeightMap->GetSize();
    m_CenterPos = pHeightMap->GetVertexPos(nCenterIndex); //->position;

    m_nGridsPerRow = nGridsPerRow;


    //×óÉÏ½Ç
    //m_vtCorner.push_back(dwTopLeft);
    int nTopLeft = m_nCenterX - m_nGridsPerRow / 2 + (m_nCenterY - m_nGridsPerRow / 2) * pHeightMap->GetSize();
    Vector3Df v = pHeightMap->GetVertexPos(nTopLeft); //->position;
    Vector3Df vCenter2Pos = m_CenterPos - v;
    m_fSphereHalf = AIR::Max<float>(m_fSphereHalf, vCenter2Pos.GetLength());//D3DXVec3Length(&vCenter2Pos));
    //fMinH = Min<float>(fMinH, v.y);
    //fMaxH = Max<float>(fMaxH, v.y);

    //ÓÒÉÏ½Ç
    //m_vtCorner.push_back(dwTopLeft + nRow);
    int nTopRight = m_nCenterX + m_nGridsPerRow / 2 + (m_nCenterY - m_nGridsPerRow / 2) * pHeightMap->GetSize();
    v = pHeightMap->GetVertexPos(nTopRight); //->position;
    vCenter2Pos = m_CenterPos - v;
    m_fSphereHalf = AIR::Max<float>(m_fSphereHalf, vCenter2Pos.GetLength()); //D3DXVec3Length(&vCenter2Pos));
    //fMinH = Min<float>(fMinH, v.y);
    //fMaxH = Max<float>(fMaxH, v.y);

    //ÓÒÏÂ½Ç
    int nBottomRight = m_nCenterX + m_nGridsPerRow / 2 + (m_nCenterY + m_nGridsPerRow / 2) * pHeightMap->GetSize();
    v = pHeightMap->GetVertexPos(nBottomRight); //->position;
    vCenter2Pos = m_CenterPos - v;
    m_fSphereHalf = AIR::Max<float>(m_fSphereHalf, vCenter2Pos.GetLength());//D3DXVec3Length(&vCenter2Pos));
    //fMinH = Min<float>(fMinH, v.y);
    //fMaxH = Max<float>(fMaxH, v.y);

    //×óÏÂ½Ç
    int nBottomLeft  = m_nCenterX - m_nGridsPerRow / 2 + (m_nCenterY + m_nGridsPerRow / 2) * pHeightMap->GetSize();
    v = pHeightMap->GetVertexPos(nBottomLeft); //->position;
    vCenter2Pos = m_CenterPos - v;
    m_fSphereHalf = AIR::Max<float>(m_fSphereHalf, vCenter2Pos.GetLength());//D3DXVec3Length(&vCenter2Pos));
    
}
*/

//CQuadTreeNode::CQuadTreeNode(float fWidth, float fCenterX, float fCenterY, int nGridsPerRow)
//{
//
//}

CQuadTreeNode::~CQuadTreeNode(void)
{

}


/*
void CQuadTreeNode::Attach(CTerrainBlock* pObject)
{
    m_pObject = pObject;
    m_fSphereHalf = pObject->GetSphereHalf();
}

bool CQuadTreeNode::IsInFrustum(const CFrustum* pFrustum)
{
    return pFrustum->IsSphereIn(m_CenterPos, m_fSphereHalf);
}

CQuadTreeNode* CQuadTreeNode::FindAttachNode(CTerrainBlock* pObject)
{
    CQuadTreeNode* pNode = NULL;
    if (CanAttach(pObject))
    {
        return this;
    }
    else
    {
        for (int i = 0; i < 4; i++)
        {
            if (m_pChilds[i]->IsInside(pObject))
            {
                pNode = m_pChilds[i]->FindAttachNode(pObject);
                break;
            }
        }
    }

    return pNode;
}

bool CQuadTreeNode::IsInside(CTerrainBlock* pBlock)
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

bool CQuadTreeNode::CanAttach(CTerrainBlock* pBlock)
{
    if (pBlock->GetCenterX() == m_nCenterX && pBlock->GetCenterY() == m_nCenterY && pBlock->GetBlockSize() - 1 == m_nGridsPerRow)
    {
        return true;
    }
    return false;
}

void CQuadTreeNode::GetRenderObjects(std::list<CTerrainBlock*>& lstRenderQueue, CCamera* pCamera)
{
    if (IsInFrustum(pCamera->GetFrustum()))
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
                m_pChilds[i]->GetRenderObjects(lstRenderQueue, pCamera);
            }
        }
        
    }
}
*/
