/********************************************************************
	created:	2009/12/08
	created:	8:12:2009   11:30
	filename: 	d:\My Codes\client\Current\Engine\Engine\Include\TerrainQuadTree.h
	file path:	d:\My Codes\client\Current\Engine\Engine\Include
	file base:	TerrainQuadTree
	file ext:	h
	author:		liangairan
	
	purpose:	ר����Ե��ε��Ĳ���
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

    //����Ĳ���
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
    // Qualifier: �����Ĳ���
    // Parameter: int nRootCellSize  ���ڵ�ÿ�еĸ���
    // Parameter: int nMinNodeCellSize  ��С���ÿ�еĸ���
    // Parameter: float fRootNodeWidth  Ҫ�ڵ�Ŀ��
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
    CTerrainQuadTree*     m_pChildren[4];   //˳�������ϣ����ϣ����£�����
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

    int m_nNodesCount;   //�������

public:
    int      m_nMaxDepth;   //���Ĳ���
};
