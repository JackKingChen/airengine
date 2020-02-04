#include "stdafx.h"
#include "Obstruct.h"
#include "SceneNode.h"

Obstruct::Obstruct() : m_bCanWalk(false), m_pNode(NULL)
{

}

Obstruct::~Obstruct()
{

}

void Obstruct::SetCanWalkOn(bool bCanWalk)
{
    m_bCanWalk = bCanWalk;
}

bool Obstruct::IsIntersert(const Vector3Df& start, const Vector3Df& lineDirection, Vector3Df& positionOut)
{
    size_t nTrianglesNum = m_vtObstructFace.size();
    for (size_t i = 0; i < nTrianglesNum; ++i)
    {
        Triangle3Df tri1(m_vtObstructFace[i].topleft, m_vtObstructFace[i].topright, m_vtObstructFace[i].bottomright);
        Triangle3Df tri2(m_vtObstructFace[i].topright, m_vtObstructFace[i].bottomright, m_vtObstructFace[i].bottomleft);
        if (m_pNode)
        {
            const Matrix4f& matWorld = m_pNode->GetFullTransform();
            matWorld.TransformCoord(tri1.pointA, tri1.pointA);
            matWorld.TransformCoord(tri1.pointB, tri1.pointB);
            matWorld.TransformCoord(tri1.pointC, tri1.pointC);
            matWorld.TransformCoord(tri2.pointA, tri2.pointA);
            matWorld.TransformCoord(tri2.pointB, tri2.pointB);
            matWorld.TransformCoord(tri2.pointC, tri2.pointC);
        }
        if (tri1.GetIntersectionOfPlaneWithLine(start, lineDirection, positionOut))
        {
            return true;
        }
        if (tri2.GetIntersectionOfPlaneWithLine(start, lineDirection, positionOut))
        {
            return true;
        }
    }

    return false;
}

void Obstruct::AddObstructFace(const ObstructFace& face)
{
    m_vtObstructFace.push_back(face);
}

void Obstruct::RemoveObstructFace(size_t index)
{
    AIRVector<ObstructFace>::iterator it = m_vtObstructFace.begin();
    std::advance(it, index);
    if (it != m_vtObstructFace.end())
    {
        m_vtObstructFace.erase(it);
    }
}

