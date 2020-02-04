// ***************************************************************
//  �ļ�: QuadTreeSceneManager.h
//  ����: 2010-4-23
//  ����: liangairan
//  ˵��: �Ĳ������������������ڵ��Σ��Ժ�Ҫ���Ƿ��ڰ˲������бȽ�
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

    //�����Ĳ����ڵ�
    virtual void UpdateQuadTreeNode(CTerrainQuadTreeNode* pNode);

    virtual void DestroySceneNode(CSceneNode* pNode);

    //�����Ĳ�����boundingbox
    void SetSceneBoundingBox(const AABBBox3Df& box);

    //�������ĸ߶�
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
