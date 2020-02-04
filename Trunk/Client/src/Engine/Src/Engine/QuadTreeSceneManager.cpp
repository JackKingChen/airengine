#include "stdafx.h"
#include "QuadTreeSceneManager.h"


QuadTreeSceneManager::QuadTreeSceneManager()
{

}

QuadTreeSceneManager::~QuadTreeSceneManager()
{

}

void QuadTreeSceneManager::SetSceneBoundingBox(const AABBBox3Df& box)
{
    m_quadTree.SetAABBBox(box);
    //全部节点更新一次
    SCENENODE_ITER itEnd = m_mapSceneNodes.end();
    for (SCENENODE_ITER it = m_mapSceneNodes.begin(); it != itEnd; ++it)
    {
        UpdateQuadTreeNode(static_cast<CTerrainQuadTreeNode*>(it->second));
    }
}

void QuadTreeSceneManager::UpdateQuadTreeNode(CTerrainQuadTreeNode* pNode)
{
    m_quadTree.UpdateQuadNode(pNode);
}

CSceneNode* QuadTreeSceneManager::CreateSceneNodeImp(const AIRString& strName, const Vector3Df& vPosition , const Quaternion& qRotation , const Vector3Df& vScale /* = Vector3Df::UNIT_SCALE */)
{
    return NULL;//new CTerrainQuadTreeNode(this, strName, vPosition);
}

void QuadTreeSceneManager::DestroySceneNode(CSceneNode* pNode)
{
    m_quadTree.RemoveNode(static_cast<CTerrainQuadTreeNode*>(pNode));
    ISceneManager::DestroySceneNode(pNode);
}

void QuadTreeSceneManager::SetTreeMaxDepth(int nMaxDepth)
{
    m_quadTree.SetMaxDepth(nMaxDepth);
}
