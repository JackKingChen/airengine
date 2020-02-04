#include "stdafx.h"
#include "DirectionRandomiserAffector.h"
#include "ParticleSystem.h"

CDirectionRandomiserAffector::CDirectionRandomiserAffector() : m_fRandomness(0),
m_fScope(0),
m_bKeepVelocity(false)
{

}

CDirectionRandomiserAffector::~CDirectionRandomiserAffector()
{

}

void CDirectionRandomiserAffector::AffectParticle(CParticleSystem* pParticleSystem, float fTimeElapsed)
{
    CParticleIterator pi = pParticleSystem->GetIterator();
    CParticle *p;
    float length;

    while (!pi.IsEnd())
    {
        p = pi.GetNext();
        if (m_fScope > AIR::UnitRandom())
        {
            if (!p->m_direction.IsZeroLength())
            {
                if (m_bKeepVelocity)
                {
                    length = p->m_direction.GetLength();
                }

                p->m_direction += Vector3Df(AIR::RangeRandom(-m_fRandomness, m_fRandomness) * fTimeElapsed,
                    AIR::RangeRandom(-m_fRandomness, m_fRandomness) * fTimeElapsed,
                    AIR::RangeRandom(-m_fRandomness, m_fRandomness) * fTimeElapsed);

                if (m_bKeepVelocity)
                {
                    p->m_direction *= length / p->m_direction.GetLength();
                }
            }
        }
    }
}

bool CDirectionRandomiserAffector::LoadFromXML(CMarkupSTL& xml)
{
    AIRString strValue = xml.GetAttrib("randomness").c_str();
    m_fRandomness = atof(strValue.c_str());

    strValue = xml.GetAttrib("scope").c_str();
    m_fScope = atof(strValue.c_str());

    strValue = xml.GetAttrib("keep_velocity").c_str();
    if (strValue == "TRUE")
    {
        m_bKeepVelocity = true;
    }
    else 
    {
        m_bKeepVelocity = false;
    }
    return true;
}

bool CDirectionRandomiserAffector::SaveToXMLNode(CMarkupSTL& xml)
{
    xml.AddAttrib("type", "DirectionRandomiser");
    xml.AddAttrib("randomness", GetString("%f", m_fRandomness));
    xml.AddAttrib("scope", GetString("%f", m_fScope));
    xml.AddAttrib("keep_velocity", m_bKeepVelocity ? "TRUE" : "FALSE");
    return true;
}

void CDirectionRandomiserAffector::SetRandomness(float fRandomness)
{
    m_fRandomness = fRandomness;
}

void CDirectionRandomiserAffector::SetScope(float fScope)
{
    m_fScope = fScope;
}

void CDirectionRandomiserAffector::SetKeepVelocity(bool bKeepVolicity)
{
    m_bKeepVelocity = bKeepVolicity;
}