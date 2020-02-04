#include "stdafx.h"
#include "TerrainQuadTree.h"

CTerrainQuadTree::CTerrainQuadTree(CTerrainQuadTree* pParent) : m_pParent(pParent), m_nMaxDepth(1)
{
    m_nNodesCount = 0;
    m_pRoot = NULL;
    memset(m_pChildren, 0, sizeof(m_pChildren));
}

CTerrainQuadTree::~CTerrainQuadTree(void)
{
    SafeDelete(m_pRoot);

    for (int i = 0; i < 4; i++)
    {
        if (m_pChildren[i])
        {
            SafeDelete(m_pChildren[i]);
        }
    }
}

void CTerrainQuadTree::Clear()
{
    m_pParent = NULL;

    for (int i = 0; i < 4; i++)
    {
        if (m_pChildren[i])
        {
            m_pChildren[i]->Clear();
        }
    }
}

bool CTerrainQuadTree::IsFitSize(const AABBBox3Df& box) const
{
    Vector3Df vTreeBoxSize = m_worldBox.GetExtent() / 2;

    Vector3Df boxSize = box.GetExtent();

    return ((boxSize.x <= vTreeBoxSize.x) && (boxSize.z <= vTreeBoxSize.z));
}

void CTerrainQuadTree::AddNode(CTerrainQuadTreeNode* pNode, int nDepth /* = 0 */)
{
    if (pNode->GetQuadTree() != 0)
    {
        pNode->GetQuadTree()->RemoveNode(pNode);
    }

    const AABBBox3Df& box = pNode->GetWorldBoundingBox();
    if (nDepth < m_nMaxDepth && IsFitSize(pNode->GetWorldBoundingBox()))
    {
        int nIndex = 0;

        GetChildIndex(box, nIndex);

        if (m_pChildren[nIndex] == 0)
        {
            m_pChildren[nIndex] = new CTerrainQuadTree(this);

            Vector3Df min, max;
            min.y = m_worldBox.MinEdge.y;
            max.y = m_worldBox.MaxEdge.y;

            if (nIndex == 0)  //左上角
            {
                min.x = m_worldBox.MinEdge.x;
                min.z = (m_worldBox.MinEdge.z + m_worldBox.MaxEdge.z) / 2;
                max.x = (m_worldBox.MinEdge.x + m_worldBox.MaxEdge.x) / 2;
                max.z = m_worldBox.MaxEdge.z;
            }
            else if (nIndex == 1)   //右上角
            {
                min.x = (m_worldBox.MaxEdge.x + m_worldBox.MinEdge.x) / 2;
                min.z = (m_worldBox.MinEdge.z + m_worldBox.MaxEdge.z) / 2;
                max.x = m_worldBox.MaxEdge.x;
                max.z = m_worldBox.MaxEdge.z;
            }
            else if (nIndex == 2)   //右下角
            {
                min.x = (m_worldBox.MaxEdge.x + m_worldBox.MinEdge.x) / 2;
                min.z = m_worldBox.MinEdge.z;
                max.x = m_worldBox.MaxEdge.x;
                max.z = (m_worldBox.MinEdge.z + m_worldBox.MaxEdge.z) / 2;
            }
            else if (nIndex == 3)   //左下角
            {
                min.x = m_worldBox.MinEdge.x;
                min.z = m_worldBox.MinEdge.z;
                max.x = (m_worldBox.MinEdge.x + m_worldBox.MaxEdge.x) / 2;
                max.z = (m_worldBox.MinEdge.z + m_worldBox.MaxEdge.z) / 2;
            }

            m_pChildren[nIndex]->m_worldBox.MinEdge = min;
            m_pChildren[nIndex]->m_worldBox.MaxEdge = max;
        }

        m_pChildren[nIndex]->AddNode(pNode, ++nDepth);
    }
    else
    {
        _AddNode(pNode);
    }
}

void CTerrainQuadTree::RemoveNode(CTerrainQuadTreeNode* pNode)
{
    CTerrainQuadTree* pTree = pNode->GetQuadTree();
    if (pTree)
    {
        pTree->_RemoveNode(pNode);
    }
}

void CTerrainQuadTree::_AddNode(CTerrainQuadTreeNode* pNode)
{
    m_lstNodes.push_back(pNode);
    pNode->SetQuadTree(this);
}

void CTerrainQuadTree::_RemoveNode(CTerrainQuadTreeNode* pNode)
{
    m_lstNodes.erase( std::find( m_lstNodes.begin(), m_lstNodes.end(), pNode ) );
    pNode -> SetQuadTree( 0 );
}

void CTerrainQuadTree::GetChildIndex(const AABBBox3Df& box, int& nIndex) const
{
    Vector3Df max = m_worldBox.MaxEdge;
    Vector3Df min = box.MinEdge;

    Vector3Df center = m_worldBox.MaxEdge.GetMidPoint( m_worldBox.MinEdge );

    Vector3Df ncenter = box.MaxEdge.GetMidPoint( box.MinEdge );

    if (ncenter.x > center.x)
    {
        if (ncenter.z > center.z)   //右上方
        {
            nIndex = 1;
        }
        else                        //右下方
            nIndex = 2;
    }
    else 
    {
        if (ncenter.z > center.z)   //左上方
        {
            nIndex = 0;
        }
        else
            nIndex = 3;
    }
}

int CTerrainQuadTree::Create(int nRootCellSize, int nMinNodeCellSize, float fRootNodeWidth, CHeightMap* pHeightMap)
{

    if (m_pRoot)
    {
        SafeDelete(m_pRoot);
        m_nNodesCount = 0;
    }
    int nCenterX = (int)(nRootCellSize * 0.5f);
    int nCenterY = nCenterX;
    m_pRoot = new CTerrainQuadTreeNode(fRootNodeWidth, nRootCellSize / 2, nRootCellSize / 2, nRootCellSize, pHeightMap);
    m_nNodesCount = 1;
    //左上角
    int nStride = nRootCellSize / 4; //* 0.25;
    DivideTree(m_pRoot, nRootCellSize / 2, nMinNodeCellSize, nCenterX - nStride, nCenterY - nStride, fRootNodeWidth * 0.5f, 0, pHeightMap);
    //右上角
    DivideTree(m_pRoot, nRootCellSize / 2, nMinNodeCellSize, nCenterX + nStride, nCenterY - nStride, fRootNodeWidth * 0.5f, 1, pHeightMap);
    //右下角
    DivideTree(m_pRoot, nRootCellSize / 2, nMinNodeCellSize, nCenterX + nStride, nCenterY + nStride, fRootNodeWidth * 0.5f, 2, pHeightMap);
    //左下角
    DivideTree(m_pRoot, nRootCellSize / 2, nMinNodeCellSize, nCenterX - nStride, nCenterY + nStride, fRootNodeWidth * 0.5f, 3, pHeightMap);

    return m_nNodesCount;
}

void CTerrainQuadTree::DivideTree(CTerrainQuadTreeNode* pParent, int nNodeCellSize, int nMinCellCount, int nCenterX, int nCenterY, float fNodeWidth, int nChildIndex, CHeightMap* pHeightMap)
{
    if (nNodeCellSize >= nMinCellCount)
    {
        CTerrainQuadTreeNode* pNode = new CTerrainQuadTreeNode(fNodeWidth, nCenterX, nCenterY, nNodeCellSize, pHeightMap);
        pParent->m_pChildren[nChildIndex] = pNode;
        m_nNodesCount++;

        int nStride = nNodeCellSize / 4;
        //左上角
        DivideTree(pNode, nNodeCellSize / 2, nMinCellCount, nCenterX - nStride, nCenterY - nStride, fNodeWidth * 0.5f, 0, pHeightMap);
        //右上角
        DivideTree(pNode, nNodeCellSize / 2, nMinCellCount, nCenterX + nStride, nCenterY - nStride, fNodeWidth * 0.5f, 1, pHeightMap);
        //右下角
        DivideTree(pNode, nNodeCellSize / 2, nMinCellCount, nCenterX + nStride, nCenterY + nStride, fNodeWidth * 0.5f, 2, pHeightMap);
        //左下角
        DivideTree(pNode, nNodeCellSize / 2, nMinCellCount, nCenterX - nStride, nCenterY + nStride, fNodeWidth * 0.5f, 3, pHeightMap);
    }

}

void CTerrainQuadTree::GetRenderableObject(AIRList<CTerrainBlock*>& lstRenderQueue, CCamera* pCamera)
{
    if (m_pRoot->IsVisable(pCamera->GetFrustum()))
    {
        m_pRoot->GetRenderObjects(lstRenderQueue, pCamera);
    }
}

void CTerrainQuadTree::AddRenderObject(CTerrainBlock* pBlock)
{
    CTerrainQuadTreeNode* pNode = m_pRoot->FindAttachNode(pBlock);

    if (pNode)
    {
        pNode->Attach(pBlock);
    }
}

CTerrainQuadTreeNode* CTerrainQuadTree::FindAttachNode(CTerrainBlock* pBlock)
{
    CTerrainQuadTreeNode* pNode = NULL;
    return NULL;
}

void CTerrainQuadTree::GetVisableObjects(CCamera* pCamera, AIRList<CSceneNode*>& lstVisables, u32 nCullFlag)
{
    if ((m_pRoot->GetCullFlag() & nCullFlag) && m_pRoot->IsVisable(pCamera->GetFrustum()))
    {
        //m_pRoot->GetVisableObjects(pCamera, lstVisables);
        m_pRoot->GetVisableNodes(pCamera, lstVisables);
    }
}

void CTerrainQuadTree::SetAABBBox(const AABBBox3Df& box)
{
    m_worldBox = box;
}

void CTerrainQuadTree::UpdateQuadNode(CTerrainQuadTreeNode* pNode)
{

}
