/********************************************************************
	created:	2009/11/18
	created:	18:11:2009   14:09
	filename: 	d:\My Codes\Engine\Engine\Include\Octree.h
	file path:	d:\My Codes\Engine\Engine\Include
	file base:	Octree
	file ext:	h
	author:		liangairan
	
	purpose:	�˲����࣬���ڳ�������
*********************************************************************/

#pragma once 
#include "EngineConfig.h"

#include "OctreeNode.h"
#include "RenderQueue.h"
#include "NavPathFinder.h"
#include "MemoryAlloter.h"
#include "RTTI.h"

struct NavIntersectInfo
{
	NavTriangle*	m_pIntersectTriangle;
	Vector3Df		m_vIntersectPoint;
};

class Entity;


class EngineExport COctree// : public EngineAllocator(COctree)
{
	EngineAllocator(COctree)
		DECLARE_ROOT_RTTI;
public:
    static   int      m_nMaxDepth;   //���Ĳ���
    COctree(COctree* pParent = NULL);
    virtual ~COctree();

    //��հ˲���
    virtual void Clear();

    //void AddTreeNode(COctreeNode* pNode, int nDepth);

    void AddNode(COctreeNode* pNode, int nDepth = 0);

    void RemoveNode(COctreeNode* pNode);

    int  GetNodesNum() const
    {
        return m_nNodesNum;
    }

    //��ÿ��ӷ�Χ�ڵ���Ⱦ��
    void GetVisableObjects(CCamera* pCamera, AIRList<CSceneNode*>& lstVisables, u32 nCullFlag);

    void GetVisableObjects(CCamera* pCamera, AIRList<CSceneNode*>& lstVisables, f32 fDistanceIn, u32 nCullFlag);

	void GetVisableEntities(CCamera* pCamera, AIRList<Entity*>& lstVisables, u32 nCullFlag);

	void GetVisableEntities(CCamera* pCamera, AIRList<Entity*>& lstVisables, f32 fDistanceIn, u32 nCullFlag);

	void AddEntity(Entity* pEntity, int nDepth = 0);

	void RemoveEntity(Entity* pEntity);

	void GetCollisionRenderables(const AABBBox3Df& aabbTest, AIRList<Renderable*>& renderList);

    inline void AddNodesNum()
    {
        m_nNodesNum++;
        if (m_pParent)
        {
            m_pParent->AddNodesNum();
        }
    }

    inline void DelNodesNum()
    {
        m_nNodesNum--;
        if (m_pParent)
        {
            m_pParent->DelNodesNum();
        }
    }

    //�ж��Ƿ��ʺ��ڸ��������
    bool IsFitSize(const AABBBox3Df& box);

    void GetChildIndex(const AABBBox3Df& box, int& x, int& y, int& z) const;

    void SetAABBBox(const AABBBox3Df& box);

    void UpdateOctreeNode(COctreeNode* pNode);

    const AABBBox3Df& GetBoundingbox() const
    {
        return m_wordBox;
    }

    void AddNavTriangle(int nDepth, NavTriangle* pNavTriangle);

    //��ñ���ʰȡ������octree
    void GetRayPickOctrees(const Line3Df& ray, AIRList<COctree*>& lstOut);

    void GetRayPickNavTriangles(const Line3Df& ray, AIRList<NavIntersectInfo>& lstTriangles);
	//void GetRayPickNavTriangles(const Line3Df& ray, AIRList<NavTriangle*>& lstTriangles);

    //�ж��Ƿ��������
    bool IsIntersectAlign(const AABBBox3Df& box);

	//void SetEntityTree(Entity* pEntity);

protected:

    void _AddNavTriangle(int nDepth, NavTriangle* pNAVTriangle, const AABBBox3Df& boundingBox);

    void _AddNode(COctreeNode* pNode);

    void _RemoveNode(COctreeNode* pNode);

	void _AddEntity(Entity* pEntity);

	void _RemoveEntity(Entity* pEntity);

    typedef AIRList<COctreeNode*> OCTREENODE_LIST;
    typedef OCTREENODE_LIST::iterator OCTREENODE_ITER;

    COctree* m_pChildren[2][2][2];

    typedef AIRList<NavTriangle*> NavTriangle_List;
    typedef NavTriangle_List::iterator NavTriangle_List_Iter;

    OCTREENODE_LIST m_lstNode;
    NavTriangle_List m_lstNavtriangles;

	typedef AIRList<Entity*> EntityList;
	EntityList m_lstEntities;
private:

    COctree* m_pParent;
    int      m_nNodesNum;   //����������������Ľ����
    AABBBox3Df  m_wordBox;  //��
};
