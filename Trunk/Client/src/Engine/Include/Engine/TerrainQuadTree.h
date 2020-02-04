/********************************************************************
	created:	2009/12/08
	created:	8:12:2009   11:30
	filename: 	d:\My Codes\client\Current\Engine\Engine\Include\TerrainQuadTree.h
	file path:	d:\My Codes\client\Current\Engine\Engine\Include
	file base:	TerrainQuadTree
	file ext:	h
	author:		liangairan
	
	purpose:	专门针对地形的四叉树
*********************************************************************/
#pragma once
#include "TerrainQuadTreeNode.h"

class EngineExport CTerrainQuadTree //: public EngineAllocator(CTerrainQuadTree)
{
	EngineAllocator(CTerrainQuadTree)
public:
    //CTerrainQuadTree();
    CTerrainQuadTree(CTerrainQuadTree* pParent = NULL);
    virtual ~CTerrainQuadTree();

    //清空四叉树
    void Clear();

    //void AddTreeNode(COctreeNode* pNode, int nDepth);

    void AddNode(CTerrainQuadTreeNode* pNode, int nDepth = 0);

    void RemoveNode(CTerrainQuadTreeNode* pNode);

    void GetChildIndex(const AABBBox3Df& box, int& nIndex) const;

    bool IsFitSize(const AABBBox3Df& box) const;

    //************************************
    // Method:    Create
    // FullName:  CQuadTree::Create
    // Access:    public 
    // Returns:   int
    // Qualifier: 创建四叉树
    // Parameter: int nRootCellSize  根节点每行的格数
    // Parameter: int nMinNodeCellSize  最小结点每行的格数
    // Parameter: float fRootNodeWidth  要节点的宽度
    //************************************
    int Create(int nRootCellSize, int nMinNodeCellSize, float fRootNodeWidth, CHeightMap* pHeightMap);

    void DivideTree(CTerrainQuadTreeNode* pParent, int nNodeCellSize, int nMinCellCount, int nCenterX, int nCenterY, float fNodeWidth, int nChildIndex, CHeightMap* pHeightMap);

    void GetRenderableObject(AIRList<CTerrainBlock*>& lstRenderQueue, CCamera* pCamera);

    void AddRenderObject(CTerrainBlock* pBlock);

    void GetVisableObjects(CCamera* pCamera, AIRList<CSceneNode*>& lstVisables, u32 nCullFlag);

    void SetAABBBox(const AABBBox3Df& box);

    void UpdateQuadNode(CTerrainQuadTreeNode* pNode);

    void SetMaxDepth(int nMaxDepth)
    {
        m_nMaxDepth = nMaxDepth;
    }

    //void Cull(CCamera* pCamera, AIRList<CSceneNode*>& lstVisableNodes);
protected:
    CTerrainQuadTree*     m_pChildren[4];   //顺序是左上，右上，右下，左下
    CTerrainQuadTree*     m_pParent;
    AABBBox3Df            m_worldBox;

    void _AddNode(CTerrainQuadTreeNode* pNode);

    void _RemoveNode(CTerrainQuadTreeNode* pNode);

    typedef AIRList<CTerrainQuadTreeNode*> QUADTREENODE_LIST;
    typedef QUADTREENODE_LIST::iterator QUADTREENODE_ITER;

    QUADTREENODE_LIST  m_lstNodes;
private:
    CTerrainQuadTreeNode* FindAttachNode(CTerrainBlock* pBlock);
    CTerrainQuadTreeNode* m_pRoot;

    int m_nNodesCount;   //结点数量

public:
    int      m_nMaxDepth;   //最大的层数
};
