/********************************************************************
	created:	2009/11/18
	created:	18:11:2009   11:46
	filename: 	d:\My Codes\Engine\Engine\Include\OctreeNode.h
	file path:	d:\My Codes\Engine\Engine\Include
	file base:	OctreeNode
	file ext:	h
	author:		liangairan
	
	purpose:	八叉树场景的管理
*********************************************************************/
#pragma once

#include "SceneNode.h"

class COctree;

class EngineExport COctreeNode : public CSceneNode
{
public:
    COctreeNode(ISceneManager* pCreator, const AIRString strName, const Vector3Df& vPosition = Vector3Df::ZERO, const Quaternion& qRotation = Quaternion(0, 0, 0, 1), const Vector3Df& vScale = Vector3Df::UNIT_SCALE);
    virtual ~COctreeNode();

    //判断该node是否在box里面
    virtual bool IsIn(const AABBBox3Df& box) const;

    void SetOctree(COctree* pOct)
    {
        m_pOctree = pOct;
    }

    COctree* GetOctree()
    {
        return m_pOctree;
    }
protected:
    virtual void UpdateBounds();

    COctree*     m_pOctree;
private:
};
