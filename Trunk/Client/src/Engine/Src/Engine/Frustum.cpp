#include "stdafx.h"
#include "Frustum.h"


void CFrustum::UpdateWorldCorners() const
{
    Matrix4f eyeToWorld;
    m_matView.GetInverse(eyeToWorld);

    // Note: Even though we can dealing with general projection matrix here,
    //       but because it's incompatibly with infinite far plane, thus, we
    //       still need to working with projection parameters.

    // Calc near plane corners
    float nearLeft, nearRight, nearBottom, nearTop;
    CalcProjectionParameters(nearLeft, nearRight, nearBottom, nearTop);

    // Treat infinite fardist as some arbitrary far value
    float farDist = (m_fFarDist == 0) ? 100000 : m_fFarDist;

    // Calc far palne corners
    float radio = m_bPerspertive ? farDist / m_fNearDist : 1;
    float farLeft = nearLeft * radio;
    float farRight = nearRight * radio;
    float farBottom = nearBottom * radio;
    float farTop = nearTop * radio;

    // near
    eyeToWorld.TransformCoord(Vector3Df(nearLeft, nearTop,    m_fNearDist), m_WorldSpaceCorners[0]);
    eyeToWorld.TransformCoord(Vector3Df(nearRight,  nearTop,    m_fNearDist), m_WorldSpaceCorners[1]);
    eyeToWorld.TransformCoord(Vector3Df(nearLeft,  nearBottom, m_fNearDist), m_WorldSpaceCorners[2]);
    eyeToWorld.TransformCoord(Vector3Df(nearRight, nearBottom, m_fNearDist), m_WorldSpaceCorners[3]);
    // far
    eyeToWorld.TransformCoord(Vector3Df(farLeft,  farTop,     farDist), m_WorldSpaceCorners[4]);
    eyeToWorld.TransformCoord(Vector3Df(farRight,   farTop,     farDist), m_WorldSpaceCorners[5]);
    eyeToWorld.TransformCoord(Vector3Df(farLeft,   farBottom,  farDist), m_WorldSpaceCorners[6]);
    eyeToWorld.TransformCoord(Vector3Df(farRight,  farBottom,  farDist), m_WorldSpaceCorners[7]);
}

void CFrustum::CalcProjectionParameters(float& left, float& right, float& bottom, float& top) const
{
    Matrix4f invProj; 
    m_ProjMatrix.GetInverse(invProj);
    Vector3Df topLeft(-0.5f, 0.5f, 0.0f);
    Vector3Df bottomRight(0.5f, -0.5f, 0.0f);

    //topLeft = topLeft * invProj;
    invProj.TransformCoord(topLeft, topLeft);
    //bottomRight = bottomRight * invProj;
    invProj.TransformCoord(bottomRight, bottomRight);

    left = topLeft.x;
    top = topLeft.y;
    right = bottomRight.x;
    bottom = bottomRight.y;
}

void CFrustum::GetViewSpaceCorners(Vector3Df* pCoords) const
{
    for (int i = 0; i < 8; i++)  // compute extrema
    {
        const Plane3Df& p0 = (i & 1) ? m_Planes[VF_NEAR_PLANE] : m_Planes[VF_FAR_PLANE];
        const Plane3Df& p1 = (i & 2) ? m_Planes[VF_TOP_PLANE] : m_Planes[VF_BOTTOM_PLANE];
        const Plane3Df& p2 = (i & 4) ? m_Planes[VF_LEFT_PLANE] : m_Planes[VF_RIGHT_PLANE];

        PlaneIntersection( pCoords[i], p0, p1, p2 );
    }
}
