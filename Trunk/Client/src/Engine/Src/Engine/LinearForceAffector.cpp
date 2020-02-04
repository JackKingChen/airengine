#include "stdafx.h"
#include "LinearForceAffector.h"
#include "ParticleSystem.h"

CLinearForceAffector::CLinearForceAffector() : m_forceType(Force_Add)
{

}

CLinearForceAffector::~CLinearForceAffector()
{

}

void CLinearForceAffector::SetForceType(ForceType type)
{
    m_forceType = type;
}

void CLinearForceAffector::SetForceVector(const Vector3Df& vec)
{
    m_vecForce = vec;
}

void CLinearForceAffector::AffectParticle(CParticleSystem* pParticleSystem, float fTimeElapsed)
{
    
    Vector3Df scaleVector;  //这样做性能会降低，因为每个粒子的scaleVector都是一样的
    if (m_forceType == Force_Add)
    {
        scaleVector = m_vecForce * fTimeElapsed;
    }

    CParticleIterator particleIter = pParticleSystem->GetIterator();

    CParticle* pParticle = NULL;
    while (!particleIter.IsEnd())
    {
        pParticle = particleIter.GetNext();
        if (m_forceType == Force_Add)
        {
            pParticle->m_direction += scaleVector;
        }
        else
        {
            pParticle->m_direction = (pParticle->m_direction + m_vecForce) / 2;
        }
    }
}

bool CLinearForceAffector::LoadFromXML(CMarkupSTL& xml)
{
    AIRString strValue = xml.GetAttrib("force_vector").c_str();

    sscanf(strValue.c_str(), "(%f,%f,%f)", &(m_vecForce.x), &(m_vecForce.y), &(m_vecForce.z));

    strValue = xml.GetAttrib("force_type").c_str();
    if (strValue == "Force_Add")
    {
        m_forceType = Force_Add;
    }
    else if (strValue == "Force_Average")
    {
        m_forceType = Force_Average;
    }
    return true;
}

bool CLinearForceAffector::SaveToXMLNode(CMarkupSTL& xml)
{
    xml.AddAttrib("type", "LinearForce");
    char szTmp[256] = { 0 };
  
    sprintf(szTmp, "(%f, %f, %f)", m_vecForce.x, m_vecForce.y, m_vecForce.z);
    xml.AddAttrib("force_vector", szTmp);

    if (m_forceType == Force_Add)
    {
        xml.AddAttrib("force_type", "Force_Add");
    }
    else if (m_forceType == Force_Average)
    {
        xml.AddAttrib("force_type", "Force_Average");
    }
    return true;
}
