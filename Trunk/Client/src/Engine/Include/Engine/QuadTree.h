#pragma once

#include "QuadTreeNode.h"
#include "RenderQueue.h"

class EngineExport CQuadTree //: public EngineAllocator(CQuadTree)
{
	EngineAllocator(CQuadTree)
public:
    int  m_nMaxDepth;
    //CQuadTree(void);
    CQuadTree(CQuadTree* pParent);
    ~CQuadTree(void);

    void AddNode(CQuadTreeNode* pNode, int nDepth = 0);

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
    //int Create(int nRootCellSize, int nMinNodeCellSize, float fRootNodeWidth, CHeightMap* pHeightMap);

    //void DivideTree(CQuadTreeNode* pParent, int nNodeCellSize, int nMinCellCount, int nCenterX, int nCenterY, float fNodeWidth, int nChildIndex, CHeightMap* pHeightMap);

    //void GetRenderableObject(list<CTerrainBlock*>& lstRenderQueue, CCamera* pCamera);

    // /void AddRenderObject(Renderable* pBlock);

    void GetVisableObjects(CCamera* pCamera, CRenderQueue* pRenderQueue);

    bool IsFitSize(const AABBBox3Df& box);

    bool CanAddNode(const AABBBox3Df& box) const;

    void GetChildIndex(const AABBBox3Df& box, int& index);

    int GetNodesNum() const
    {
        return m_nNodesCount;
    }

    //������ж���
    void Clear();

    //���ð�Χ��
    void SetBoundingBox(const AABBBox3Df& box);

    //void SetBoundingBox

    CQuadTree*  m_pChildren[4];   //��˳��Ϊ���ϡ����ϡ����¡�����

protected:
    typedef std::list<CQuadTreeNode*> QUADTREENODE_LIST;
    typedef QUADTREENODE_LIST::iterator QUADTREENODE_ITER;

    QUADTREENODE_LIST  m_lstNodes;

private:
    void _AddNode(CQuadTreeNode* pNode);
    CQuadTreeNode* FindAttachNode(CTerrainBlock* pBlock);

    inline void AddNodesNum()
    {
        m_nNodesCount++;
        if (m_pParent)
        {
            m_pParent->m_nNodesCount++;
        }
    }
    //CQuadTreeNode* m_pRoot;

    CQuadTree* m_pParent;

    int m_nNodesCount;   //�������
    AABBBox3Df  m_wordBox;
};
