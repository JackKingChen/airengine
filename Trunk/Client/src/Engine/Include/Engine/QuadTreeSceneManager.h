// ***************************************************************
//  文件: QuadTreeSceneManager.h
//  日期: 2010-4-23
//  作者: liangairan
//  说明: 四叉树场景管理器，用于地形，以后要考虑放在八叉树进行比较
// ***************************************************************

#pragma once 

#include "ISceneManager.h"
#include "TerrainQuadTree.h"
//class CTerrainQuadTreeNode;
//class CTerrainQuadTree;

class EngineExport QuadTreeSceneManager : public ISceneManager
{
public:
    QuadTreeSceneManager();
    virtual ~QuadTreeSceneManager();

    virtual bool ProcessInputEvent(const LPINPUTEVENT pEvent)
    {
        return true;
    }

    //更新四叉树节点
    virtual void UpdateQuadTreeNode(CTerrainQuadTreeNode* pNode);

    virtual void DestroySceneNode(CSceneNode* pNode);

    //设置四叉树的boundingbox
    void SetSceneBoundingBox(const AABBBox3Df& box);

    //设置树的高度
    void SetTreeMaxDepth(int nMaxDepth);

    CTerrainQuadTree* GetTerrainQuadTree()
    {
        return &m_quadTree;
    }


protected:
	virtual CSceneNode* CreateSceneNodeImp(const AIRString& strName, const Vector3Df& vPosition /* = Vector3Df::ZERO */, const Quaternion& qRotation /* = Quaternion */,
		const Vector3Df& vScale = Vector3Df::UNIT_SCALE);


    //virtual void UpdateBounds();

    //virtual CTerrainQuadTree* CreateQuadTree(const AABBBox3Df& box);
protected:
    //CTerrainQuadTree*  m_pQuadTree;
    CTerrainQuadTree   m_quadTree;
private:
};
