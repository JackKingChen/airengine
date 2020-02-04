#include "stdafx.h"
#include "RotationAffector.h"
#include "ParticleSystem.h"

CRotationAffector::CRotationAffector() : m_fRotationSpeedRangeStart(0), m_fRotationSpeedRangeEnd(0),
m_fRotationRangeStart(0), m_fRotationRangeEnd(AIR::PI * 2)
{
    //m_type = AT_Rotator;
}

CRotationAffector::~CRotationAffector()
{

}

void CRotationAffector::AffectParticle(CParticleSystem* pParticleSystem, float fTimeElapsed)
{
    CParticleIterator particleIter = pParticleSystem->GetIterator();

    CParticle* pParticle = NULL;
    while (!particleIter.IsEnd())
    {
        pParticle = particleIter.GetNext();
        pParticle->m_fRotationAngle += pParticle->m_fRotationSpeed * fTimeElapsed;
    }
    
}

void CRotationAffector::InitParticle(CParticle* pParticle)
{
    pParticle->m_fRotationAngle = m_fRotationRangeStart + AIR::UnitRandom() * (m_fRotationRangeEnd - m_fRotationRangeStart);
    pParticle->m_fRotationSpeed = m_fRotationSpeedRangeStart + AIR::UnitRandom() * (m_fRotationSpeedRangeEnd - m_fRotationSpeedRangeStart);
}

bool CRotationAffector::LoadFromXML(CMarkupSTL& xml)
{
    AIRString strValue = xml.GetAttrib("rotation_range_start").c_str();
    //文件里面的是角度，所有要转成弧度
    m_fRotationRangeStart = atof(strValue.c_str());
    AIR::DegreeToRadius(m_fRotationRangeStart);
    
    strValue = xml.GetAttrib("rotation_range_end").c_str();
    m_fRotationRangeEnd = atof(strValue.c_str());
    AIR::DegreeToRadius(m_fRotationRangeEnd);

    strValue = xml.GetAttrib("rotation_speed_range_start").c_str();
    m_fRotationSpeedRangeStart = atof(strValue.c_str());
    AIR::DegreeToRadius(m_fRotationSpeedRangeStart);

    strValue = xml.GetAttrib("rotation_speed_range_end").c_str();
    m_fRotationSpeedRangeEnd = atof(strValue.c_str());
    AIR::DegreeToRadius(m_fRotationSpeedRangeEnd);

    return true;
}

bool CRotationAffector::SaveToXMLNode(CMarkupSTL& xml)
{
    xml.AddAttrib("type", "Rotator");
    char szTmp[256] = { 0 };
    float fAngle = m_fRotationRangeStart;
    AIR::RadiusToDegree(fAngle);
    sprintf(szTmp, "%f", fAngle);
    xml.AddAttrib("rotation_range_start", szTmp);

    fAngle = m_fRotationRangeEnd;
    AIR::RadiusToDegree(fAngle);
    sprintf(szTmp, "%f", fAngle);
    xml.AddAttrib("rotation_range_end", szTmp);

    fAngle = m_fRotationSpeedRangeStart;
    AIR::RadiusToDegree(fAngle);
    sprintf(szTmp, "%f", fAngle);
    xml.AddAttrib("rotation_speed_range_start", szTmp);

    fAngle = m_fRotationSpeedRangeEnd;
    AIR::RadiusToDegree(fAngle);
    sprintf(szTmp, "%f", fAngle);
    xml.AddAttrib("rotation_speed_range_end", szTmp);

    return true;
}

void CRotationAffector::SetRotationSpeedRangeStart(float fSpeedRangeStart)
{
    m_fRotationSpeedRangeStart = fSpeedRangeStart;
}

void CRotationAffector::SetRotationSpeedRangeEnd(float fSpeedRangeEnd)
{
    m_fRotationSpeedRangeEnd = fSpeedRangeEnd;
}

void CRotationAffector::SetRotationRangeStart(float fRangeStart)
{
    m_fRotationRangeStart = fRangeStart;
}

void CRotationAffector::SetRotationRangeEnd(float fRangeEnd)
{
    m_fRotationRangeEnd = fRangeEnd;
}
