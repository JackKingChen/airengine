#include "stdafx.h"
//#include "stdafx.h"
#include "QuadTree.h"
#include "ISceneManager.h"
//int CQuadTree::m_nMaxDepth = 0;

CQuadTree::CQuadTree(CQuadTree* pParent) : m_pParent(pParent)
{
    m_nNodesCount = 0;
    //m_pRoot = NULL;
    if (m_pParent)
    {
        m_nMaxDepth = m_pParent->m_nMaxDepth;
    }

    memset(m_pChildren, 0, sizeof(m_pChildren));
}

CQuadTree::~CQuadTree(void)
{
    Clear();
    for (int i = 0; i < 4; i++)
    {
        SafeDelete(m_pChildren[i]);
    }
}

void CQuadTree::AddNode(CQuadTreeNode* pNode, int nDepth /* = 0 */)
{
    const AABBBox3Df& box = pNode->GetWorldBoundingBox();

    if (nDepth < m_nMaxDepth && CanAddNode(box))
    {
        int nIndex = 0;
        GetChildIndex( box, nIndex );
        //if ( IsFitSize(box) )
        //{
            if (m_pChildren[nIndex] == 0)
            {
                m_pChildren[nIndex] = new CQuadTree( this );
                //const Vector3Df& octantMin = m_wordBox.MinEdge;
                //const Vector3Df& octantMax = m_wordBox.MaxEdge;
                Vector3Df min, max;

                if ( nIndex == 0 )  //左上角
                {
                    min.x = m_wordBox.MinEdge.x;
                    max.x = ( m_wordBox.MinEdge.x + m_wordBox.MaxEdge.x ) / 2;
                    min.z = ( m_wordBox.MinEdge.z + m_wordBox.MaxEdge.z ) / 2;
                    max.z = m_wordBox.MaxEdge.z;
                    min.y = m_wordBox.MinEdge.y;
                    max.y = ( m_wordBox.MinEdge.y + m_wordBox.MaxEdge.y ) / 2;
                }

                else if (nIndex == 1)   //右上角
                {
                    min.x = ( m_wordBox.MinEdge.x + m_wordBox.MaxEdge.x ) / 2;
                    max.x = m_wordBox.MaxEdge.x;
                    min.z = ( m_wordBox.MinEdge.z + m_wordBox.MaxEdge.z ) / 2;
                    max.z = m_wordBox.MaxEdge.z;
                    min.y = m_wordBox.MinEdge.y;
                    max.y = ( m_wordBox.MinEdge.y + m_wordBox.MaxEdge.y ) / 2;
                }

                else if (nIndex == 2)   //右下角
                {
                    min.x = ( m_wordBox.MinEdge.x + m_wordBox.MaxEdge.x ) / 2;
                    max.x = m_wordBox.MaxEdge.x;
                    min.z = m_wordBox.MinEdge.z;
                    max.z = ( m_wordBox.MinEdge.z + m_wordBox.MaxEdge.z ) / 2;
                    min.y = ( m_wordBox.MinEdge.y + m_wordBox.MaxEdge.y ) / 2;
                    max.y = m_wordBox.MaxEdge.y;
                }

                else if (nIndex == 3)   //左下角
                {
                    min.x = m_wordBox.MinEdge.x;
                    max.x = ( m_wordBox.MinEdge.x + m_wordBox.MaxEdge.x ) / 2;
                    min.z = m_wordBox.MinEdge.z;
                    max.z = ( m_wordBox.MinEdge.z + m_wordBox.MaxEdge.z ) / 2;
                    min.y = ( m_wordBox.MinEdge.y + m_wordBox.MaxEdge.y ) / 2;
                    max.y = m_wordBox.MaxEdge.y;
                }

                //min.y = m_wordBox.MinEdge.y;
                //max.y = m_wordBox.MaxEdge.y;

                m_pChildren[ nIndex ] -> m_wordBox.MinEdge = min;
                m_pChildren[ nIndex ] -> m_wordBox.MaxEdge = max;

                if (IsFitSize(box))
                {
                    m_pChildren[ nIndex ]->AddNode(pNode, ++nDepth);
                }
                else
                {
                    m_pChildren[ nIndex ]->_AddNode(pNode);
                }
            }
            else
            {
                if (IsFitSize(box))
                {
                    m_pChildren[ nIndex ]->AddNode(pNode, ++nDepth);
                }
                else
                {
                    m_pChildren[ nIndex ]->_AddNode(pNode);
                }
            }
        //}
        //else
        //{
        //    m_pChildren[nIndex]->AddNode(pNode, ++nDepth);
        //}
    }
    else
    {
        _AddNode(pNode);
    }
}

bool CQuadTree::IsFitSize(const AABBBox3Df& box)
{
    /*
    Vector3Df vTreeBoxSize = m_wordBox.GetExtent() / 2;

    Vector3Df boxSize = box.GetExtent();

    return ((boxSize.x == vTreeBoxSize.x) && (boxSize.y == vTreeBoxSize.y) && (boxSize.z == vTreeBoxSize.z));
    */

    Vector3Df vTreeBoxMax = m_wordBox.MaxEdge / 2;
    Vector3Df vTreeBoxMin = m_wordBox.MinEdge / 2;

    //Vector3Df boxSize = box.GetExtent();

    return (box.MaxEdge == vTreeBoxMax && box.MinEdge == vTreeBoxMin);
}

bool CQuadTree::CanAddNode(const AABBBox3Df& box) const
{
    Vector3Df vTreeBoxSize = m_wordBox.GetExtent() / 2;
    //Vector3Df vTreeBoxMax = m_wordBox.MaxEdge / 2;
    //Vector3Df vTreeBoxMin = m_wordBox.MinEdge / 2;

    Vector3Df boxSize = box.GetExtent();

    return ((boxSize.x <= vTreeBoxSize.x) && (boxSize.y <= vTreeBoxSize.y) && (boxSize.z <= vTreeBoxSize.z));
}

void CQuadTree::_AddNode(CQuadTreeNode* pNode)
{
    m_lstNodes.push_back(pNode);
    
    AddNodesNum();
}

/*
int CQuadTree::Create(int nRootCellSize, int nMinNodeCellSize, float fRootNodeWidth, CHeightMap* pHeightMap)
{

    if (m_pRoot)
    {
        SafeDelete(m_pRoot);
        m_nNodesCount = 0;
    }
    int nCenterX = (int)(nRootCellSize * 0.5f);
    int nCenterY = nCenterX;
    m_pRoot = AURO_NEW_VERBOSE CQuadTreeNode(fRootNodeWidth, nRootCellSize / 2, nRootCellSize / 2, nRootCellSize, pHeightMap);
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

void CQuadTree::DivideTree(CQuadTreeNode* pParent, int nNodeCellSize, int nMinCellCount, int nCenterX, int nCenterY, float fNodeWidth, int nChildIndex, CHeightMap* pHeightMap)
{
    if (nNodeCellSize >= nMinCellCount)
    {
        CQuadTreeNode* pNode = AURO_NEW_VERBOSE CQuadTreeNode(fNodeWidth, nCenterX, nCenterY, nNodeCellSize, pHeightMap);
        pParent->m_pChilds[nChildIndex] = pNode;
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
*/

/*
void CQuadTree::GetRenderableObject(list<CTerrainBlock*>& lstRenderQueue, CCamera* pCamera)
{
    if (m_pRoot->IsInFrustum(pCamera->GetFrustum()))
    {
        m_pRoot->GetRenderObjects(lstRenderQueue, pCamera);
    }
}

void CQuadTree::AddRenderObject(CTerrainBlock* pBlock)
{
    CQuadTreeNode* pNode = m_pRoot->FindAttachNode(pBlock);

    if (pNode)
    {
        pNode->Attach(pBlock);
    }
}

CQuadTreeNode* CQuadTree::FindAttachNode(CTerrainBlock* pBlock)
{
    CQuadTreeNode* pNode = NULL;
    return NULL;
}

*/

void CQuadTree::GetChildIndex(const AABBBox3Df& box, int& index)
{
    Vector3Df max = m_wordBox.MaxEdge;
    Vector3Df min = box.MinEdge;

    Vector3Df center = m_wordBox.MaxEdge.GetMidPoint( m_wordBox.MinEdge );

    Vector3Df ncenter = box.MaxEdge.GetMidPoint( box.MinEdge );

    Vector3Df vTopLeftCenter = center.GetMidPoint(m_wordBox.MinEdge);
    Vector3Df vTopRightCenter = center.GetMidPoint(Vector3Df(m_wordBox.MaxEdge.x, m_wordBox.MinEdge.y, m_wordBox.MinEdge.z));
    Vector3Df vBottomRightCenter = center.GetMidPoint(m_wordBox.MaxEdge);
    Vector3Df vBottomLeftCenter = center.GetMidPoint(Vector3Df(m_wordBox.MinEdge.x, m_wordBox.MaxEdge.y, m_wordBox.MinEdge.z));
    //float fRaduis = vTopLeftCenter - center.


    if ( ncenter.x > center.x && ncenter.z >= center.z && ncenter.y < center.y)  //右上角
    {
        index = 1;
    }

    if ( ncenter.x < center.x && ncenter.z >= center.z && ncenter.y < center.y)  //左上角
    {
        index = 0;
    }

    if (ncenter.x > center.x && ncenter.z <= center.z && ncenter.y >center.y)  //右下角
    {
        index = 2;
    }

    if (ncenter.x < center.x && ncenter.z <= center.z && ncenter.y > center.y)   //左下角
    {
        index = 3;
    }
}

void CQuadTree::Clear()
{
    ISceneManager* pScene = ENGINE_INST->GetSceneMgr();
    QUADTREENODE_ITER it = m_lstNodes.begin();
    while (it != m_lstNodes.end())
    {
        CQuadTreeNode* pNode = *it;
        pNode->DetachAllRenderable();
        //Renderable* pRenderable = pNode->GetAttachRenderable();
        //if (pRenderable)
        //{
        //    pScene->DestroyRenderable(pRenderable);
        //}
        SafeDelete(pNode);
        ++it;
    }

    m_lstNodes.clear();

    for (int i = 0; i < 4; i++)
    {
        if (m_pChildren[i] != 0)
        {
            m_pChildren[i]->Clear();
        }
    }
}

void CQuadTree::GetVisableObjects(CCamera* pCamera, CRenderQueue* pRenderQueue)
{
    if (m_nNodesCount == 0)
    {
        return;
    }

    //首先获取当前
    QUADTREENODE_ITER it = m_lstNodes.begin();
    while (it != m_lstNodes.end())
    {
        CQuadTreeNode* pNode = *it;
        if (pCamera->IsVisable(pNode->GetWorldBoundingBox()))
        {
            //pRenderQueue->AddRenderable(pNode->GetAttachRenderable());
        }
        ++it;
    }

    for (int i = 0; i < 4; i++)
    {
        if (m_pChildren[i] != 0)
        {
            m_pChildren[i]->GetVisableObjects(pCamera, pRenderQueue);
        }
    }
}

void CQuadTree::SetBoundingBox(const AABBBox3Df& box)
{
    m_wordBox = box;
}
