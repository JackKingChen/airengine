#include "stdafx.h"
#include "OctreeNode.h"
#include "OTSceneManager.h"

COctreeNode::COctreeNode(ISceneManager* pCreator, const AIRString strName, const Vector3Df& vPosition, const Quaternion& qRotation /* = Quaternion */, const Vector3Df& vScale) : 
CSceneNode(pCreator, strName, vPosition, qRotation, vScale),
m_pOctree(NULL)
{
	m_szType = "octreenode";
}

COctreeNode::~COctreeNode()
{

}

bool COctreeNode::IsIn(const AABBBox3Df& box) const
{
    return m_worldBox.IsFullInside(box);
    /*
    const Vector3Df& vCenter = m_worldBox.GetCenter();

    const Vector3Df& vBoxMin = box.MinEdge;
    const Vector3Df& vBoxMax = box.MaxEdge;

    bool centre = (vBoxMax > vCenter && vBoxMin < vCenter);
    if (!centre)
    {
        return false;
    }

    Vector3Df vBoxSize = vBoxMax - vBoxMin;
    Vector3Df vSelfSize = m_worldBox.GetExtent();

    return vSelfSize < vBoxSize;
    */
}

void COctreeNode::UpdateBounds()
{
    CSceneNode::UpdateBounds();

    static_cast<COTSceneManager*>(m_pCreator)->UpdateOctreeNode(this); 
}

