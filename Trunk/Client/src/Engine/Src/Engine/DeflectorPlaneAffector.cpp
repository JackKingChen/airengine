#include "stdafx.h"
#include "DeflectorPlaneAffector.h"
#include "ParticleSystem.h"

CDeflectorPlaneAffector::CDeflectorPlaneAffector() : m_vPlaneNormal(Vector3Df::UNIT_Y),
m_fBounce(0)
{

}

CDeflectorPlaneAffector::~CDeflectorPlaneAffector()
{

}

void CDeflectorPlaneAffector::AffectParticle(CParticleSystem* pParticleSystem, float fTimeElapsed)
{
    // precalculate distance of plane from origin
    float planeDistance = - m_vPlaneNormal.DotProduct(m_PlanePoint) / sqrt(m_vPlaneNormal.DotProduct(m_vPlaneNormal));
    Vector3Df directionPart;

    CParticleIterator pi = pParticleSystem->GetIterator();
    CParticle *p;

    while (!pi.IsEnd())
    {
        p = pi.GetNext();

        Vector3Df direction(p->m_direction * fTimeElapsed);
        if (m_vPlaneNormal.DotProduct(p->m_position + direction) + planeDistance <= 0.0)
        {
            float a = m_vPlaneNormal.DotProduct(p->m_position) + planeDistance;
            if (a > 0.0)
            {
                // for intersection point
                directionPart = direction * (- a / direction.DotProduct( m_vPlaneNormal ));
                // set new position
                p->m_position = (p->m_position + ( directionPart )) + (((directionPart) - direction) * m_fBounce);

                // reflect direction vector
                p->m_direction = (p->m_direction - (p->m_direction.DotProduct( m_vPlaneNormal ) * m_vPlaneNormal) * 2.0) * m_fBounce;
            }
        }
    }
}

void CDeflectorPlaneAffector::SetBounce(float fBounce)
{
    m_fBounce = fBounce;
}

void CDeflectorPlaneAffector::SetPlaneNormal(const Vector3Df& vNormal)
{
    m_vPlaneNormal = vNormal;
}

void CDeflectorPlaneAffector::SetPlanePoint(const Vector3Df& point)
{
    m_PlanePoint = point;
}

bool CDeflectorPlaneAffector::LoadFromXML(CMarkupSTL& xml)
{
    AIRString strValue = xml.GetAttrib("plane_point").c_str();
    sscanf(strValue.c_str(), "(%f, %f, %f)", &m_PlanePoint.x, &m_PlanePoint.y, &m_PlanePoint.z);

    strValue = xml.GetAttrib("plane_normal").c_str();
    sscanf(strValue.c_str(), "(%f, %f, %f)", &m_vPlaneNormal.x, &m_vPlaneNormal.y, &m_vPlaneNormal.z);

    strValue = xml.GetAttrib("bounce").c_str();
    m_fBounce = atof(strValue.c_str());
    return true;
}

bool CDeflectorPlaneAffector::SaveToXMLNode(CMarkupSTL& xml)
{
    xml.AddAttrib("type", "DeflectorPlane");
    char szTmp[256] = { 0 };
    sprintf(szTmp, "(%f, %f, %f)", m_PlanePoint.x, m_PlanePoint.y, m_PlanePoint.z);
    xml.AddAttrib("plane_point", szTmp);

    sprintf(szTmp, "(%f, %f, %f)", m_vPlaneNormal.x, m_vPlaneNormal.y, m_vPlaneNormal.z);
    xml.AddAttrib("plane_normal", szTmp);

    sprintf(szTmp, "%f", m_fBounce);
    xml.AddAttrib("bounce", szTmp);
    return true;
}
